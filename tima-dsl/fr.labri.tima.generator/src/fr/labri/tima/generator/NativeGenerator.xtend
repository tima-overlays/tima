package fr.labri.tima.generator

import fr.labri.tima.Util
import fr.labri.tima.ir.IRAutomata
import fr.labri.tima.ir.IRAutomata.Automaton
import fr.labri.tima.ir.IRAutomata.ExternalAction
import fr.labri.tima.ir.IRAutomata.Message
import fr.labri.tima.ir.IRAutomata.MessageGuard
import fr.labri.tima.ir.IRAutomata.MessageTarget
import fr.labri.tima.ir.IRAutomata.Node
import fr.labri.tima.ir.IRAutomata.RemoteMessage
import java.util.Map
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext
import java.util.List
import java.util.LinkedList
import java.util.HashMap

class NativeGenerator extends NamedNodeGenerator {
	
	
	static class GenerationContext {
		var last_tmp = 0
		val symbols = new LinkedList<Map<String, String>>
		
		def lastTmp() {
			'''tmp«last_tmp»'''
		}
		
		def nextTmp() {
			last_tmp ++
			'''tmp«last_tmp»'''
		}
		
		def enterScope(Map<String, String> symbols) {
			last_tmp = 0;
			this.symbols.addFirst(symbols)
		}
		
		def leaveScope() {
			last_tmp = 0;
			symbols.removeFirst
		}
		
		def String symbol2Target(String symbol) {
			val t = symbols.findFirst[ it.containsKey(symbol) ]
			if (t != null) {
				t.get(symbol)
			}
			else {
				'''GET_GLOBAL(ctx, "«symbol»")'''
			}
		}
	}
	
	val context = new GenerationContext()
	
	new(IRAutomata a) {
		super(a)
	}
	
	override def void generateFiles(String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		fsa.generateFile('''«name»/«name».cc''', native_version(name))
		fsa.generateFile('''«name»/«name».h''', native_version_header(name))
	}
	
	def native_version_header(String name) {
		'''
		#ifndef __«name»__
		#define __«name»__
		
		#include "automata.h"
		#include "tima.h"
		#include <string>
		
		int	get_msg_id_from_name(const char* name);
		
		/** ID for each automaton */
		enum AUTOMATA_ID {
			ANY_AUTOMATON_ID,
			«FOR a: automata.automata.values SEPARATOR ','»
				«a.name»_AUTOMATON_ID
			«ENDFOR»
		};
		
		enum MESSAGES_ID {
			ANY_MSG_ID,
			«FOR m : automata.messages.values SEPARATOR ','»
				«get_message_id(m)»
			«ENDFOR»
		};
		
		«FOR m : automata.messages.values SEPARATOR '\n'»
		class «get_message_name(m)» : public tima::Message {
		public:
			«get_message_name(m)»(): Message(«get_message_id(m)») {}
			
			«FOR field : m.fields»
				std::string «field»() {
					return fields["«field»"];
				}
				
				void «field»(const std::string& v) {
					fields["«field»"] = v;
				}
			«ENDFOR»
		};		
		«ENDFOR»
		
		#endif
		'''
	}
	
	
	var Map<String, Map<IRAutomata.Node, String>> names = null
	
	def native_version(String project_name) {
		names = newHashMap(automata.automata.values.map[it.name -> it.names])
		var counter = 0;
	'''
	#include "automata.h"
	#include "tima.h"
	#include "mailbox.h"
	#include "«project_name».h"
	#include <cstring>
	
	int
	get_msg_id_from_name(const char* name)
	{
		«FOR m : automata.messages.values»
			if (!strcmp(name, "«m.name»"))
				return MESSAGES_ID::«get_message_id(m)»;
		«ENDFOR»
		return -1;
	}
	
	«FOR automaton: automata.automata.values SEPARATOR '\n'»
	/** Automaton «automaton.name» */
	
	«FOR node: automaton.nodes SEPARATOR '\n'»
		«{counter = 0; null}»
		
«««		/* forward declarations */
		«node.actions.get_forward_declarations»
		
		static void
		«node_action_name(automaton, node)»(
					const std::string& name,
					tima::TimaNativeContext* ctx)
		{
			«{context.enterScope(new HashMap<String, String>); null}»
			«FOR act : node.actions»
			«act.IR2Target»;
			«ENDFOR»
			«{context.leaveScope; null}»
		}
		
		«FOR t: node.transtions SEPARATOR '\n'»
		«t.IR2Target(automaton, node)»
		«ENDFOR»
		
		«IF node.timeoutTarget != null»
		
«««		/* forward declarations */
		«node.timeoutActions.get_forward_declarations»
		
		static void
		«get_timeout_action_name(automaton, node)»(
					const std::string& name,
					tima::TimaNativeContext* ctx)
		{
			«{context.enterScope(new HashMap<String, String>); null}»
			«FOR act : node.timeoutActions»
			«act.IR2Target»
			«ENDFOR»
			«{context.leaveScope; null}»
		}
		«ENDIF»
		
		«{counter = 0; null}»
		static const struct tima::Transition «get_name_for_transitions(automaton, node)»[] = {
			«FOR t: node.transtions SEPARATOR ','»
			{
				dst : «Util.indexOf(t.target, automaton.nodes)»,
				guard : «get_transition_guard_name(automaton, node, counter)»,
				action : «get_transition_action_name(automaton, node, counter++)»,
				«IF t.guard instanceof MessageGuard»
				msg_id : «(t.guard as MessageGuard).messageType.get_message_id»
				«ELSE»
				msg_id : ANY_MSG_ID
				«ENDIF»
			}
			«ENDFOR»
		};
		
	«ENDFOR»
	
	
	static struct tima::State «get_automaton_structure_name(automaton)»[] = {
		«FOR node: automaton.nodes SEPARATOR ','»
		{
			name : "«names.get(automaton.name).get(node)»",
			urgent : false,
			timeout : «IF node.timeoutTarget==null»tima::never_timeout«ELSE»«node.timeout»«ENDIF», // milliseconds
			timeout_destination : «IF node.timeoutTarget==null»tima::null_destination«ELSE»«Util.indexOf(node.timeoutTarget, automaton.nodes) »«ENDIF»,
			timeout_action : «IF node.timeoutTarget==null»nullptr«ELSE»«get_timeout_action_name(automaton, node)»«ENDIF»,
			nr_transitions : «node.transtions.size», // without taking into account the default transition
			transitions : (struct tima::Transition*)&«get_name_for_transitions(automaton, node)»,
			action : «node_action_name(automaton, node)»,
		}
		«ENDFOR»
	};
	
	static struct tima::Automaton «automaton.name» = {
		name : "«automaton.name»",
		initial : «Util.indexOf(automaton.entryPoint, automaton.nodes)»,
		nr_states : «automaton.nodes.length»,
		states : (struct tima::State*)&«get_automaton_structure_name(automaton)»
	};
	
	«ENDFOR»
	
	static const uint32_t nr_automaton = «automata.automata.size»;
	
	static struct tima::Automaton* automatons [] = {
		«FOR name : automata.automata.keySet SEPARATOR ','»
		&«name»
		«ENDFOR»
	};
	
	uint32_t
	get_nr_automaton()
	{
		return nr_automaton;
	}
	
	struct tima::Automaton&
	get_automaton(uint32_t idx)
	{
		return *automatons[idx];
	}
	
	'''
	}
	
	private def get_forward_declarations(List<IRAutomata.Action> actions) {
		'''
		«FOR act : actions.filter(ExternalAction)»
		void «(act as fr.labri.tima.ir.IRAutomata.ExternalAction).externalName.simple_name»(
					const std::string& name,
					tima::TimaNativeContext* ctx,
					«FOR p : act.arguments SEPARATOR ','»
						std::string
					«ENDFOR»
					);
		«ENDFOR»
		'''
		
	}
	
	private def String get_timeout_action_name(Automaton automaton, Node node)
		'''«automaton.name»_«names.get(automaton.name).get(node)»_timeout_do'''
	
	
	def String IR2Target(IRAutomata.Transition t, IRAutomata.Automaton automaton, IRAutomata.Node node) {
		var counter = 0;
		'''
«««		/* forward declarations */
		«t.actions.get_forward_declarations»
		
		static void
		«get_transition_action_name(automaton, node, counter)»(
					const std::string& name,
					tima::TimaNativeContext* ctx)
		{
			/*transition from «names.get(automaton.name).get(node)» to «names.get(automaton.name).get(t.target)» */
			«{context.enterScope(
				switch t.guard {
					IRAutomata.MessageGuard: getMessageSymbolTable((t.guard as IRAutomata.MessageGuard).messageType)
					default: new HashMap<String, String>
				}
			); null}»
			«FOR act : t.actions»
			«act.IR2Target»
			«ENDFOR»
			«{context.leaveScope; null}»
		}
		
		static bool
		«get_transition_guard_name(automaton, node, counter++)»(
					const std::string& name,
					tima::TimaNativeContext* ctx)
		{
«««			return «t.guard.IR2Target»;
			return false;
		}
		'''
	}
	
	dispatch def String IR2Target(IRAutomata.BuiltinAction action) {
		throw new UnsupportedOperationException("Really? Do we have some built-in action?");
	}
	
	dispatch def String IR2Target(IRAutomata.ExternalAction action) {
		/* steps:
		 *  - build parameters
		 *  - call operation
		 */
		val v = newLinkedList()
		val i = newLinkedList()
		for (a : action.arguments) {
			v.add(a.IR2Target)
			i.add(context.lastTmp)
		}
		'''
		«FOR e : v»
		«e»
		«ENDFOR»
		«action.externalName.substring(1, action.externalName.length-1)»(
			name, ctx,
			«FOR tmp : i SEPARATOR ','»
				«tmp»
			«ENDFOR»
		);
		'''
		
	}
	
	dispatch def String IR2Target(IRAutomata.MessageAction action) {
		/* steps:
		 * 1 - create message
		 * 2 - build parameters
		 * 3 - build target
		 * 4 - Send message
		 */
		val v = newHashMap
		val i = newHashMap
		for (a : action.arguments.entrySet) {
			v.put(a.key, a.value.IR2Target)
			i.put(a.key, context.lastTmp)
		}
		val msg = context.nextTmp
		'''
		«FOR e : v.entrySet»
		«e.value»
		«ENDFOR»
		«action.message.get_message_name» «msg»;
		«FOR f : action.arguments.entrySet»
		«msg».«f.key»(«i.get(f.key)»);
		«ENDFOR»
		«IF action.message instanceof RemoteMessage»
		((tima::ActionContext*)ctx)->broadcast(1000, «msg»);
		«ELSE»
		tima::Mailbox::send(«msg», «(action.target as MessageTarget.Unicast).target»)
		«ENDIF»
		'''
	}
	
	dispatch def String IR2Target(IRAutomata.Expression.Constant c) {
		'''std::string «context.nextTmp» = "«c.value»";'''
	}
	
	dispatch def String IR2Target(IRAutomata.Expression.Identifier i) {
		'''std::string «context.nextTmp» = «context.symbol2Target(i.name)»;'''
	}
	
	dispatch def String IR2Target(IRAutomata.MessageGuard g) {
		'''message'''
	}
	
	dispatch def String IR2Target(IRAutomata.ExternalGuard g)
		'''«g.externalName.simple_name»'''
	
	dispatch def String IR2Target(IRAutomata.BuiltinGuard g) {
		switch (g.builtinName) {
			case "true": {
				'true'
			}
			case "false": {
				'false'
			}
			default: {
				throw new UnsupportedOperationException("Really? Do we have some builtin action?");
			}
		}
	}
	
	/* A bunch of methods to keep consistent the definition of identifies  */
	
	private def Map<String, String> getMessageSymbolTable(IRAutomata.Message m) {
		val t = new HashMap<String, String>
		m.fields.forEach[
			t.put(it, '''GET_FIELD(ctx, "«it»")''')
		]
		t
	}
	
	private def simple_name(String s) {
		s.substring(1, s.length-1)
	}
	
	
	private def get_message_name(Message m)
		'''Message«m.name.toFirstUpper»'''
	
	def get_message_id(Message m)
		'''«m.name»_MSG_ID'''
	
	def get_transition_guard_name(Automaton automaton, Node node, int i) {
		'''«automaton.name»_«names.get(automaton.name).get(node)»_transition_guard_«i»'''
	}
	
	def get_name_for_transitions(Automaton automaton, Node node) {
		'''«automaton.name»_transitions_for_«names.get(automaton.name).get(node)»'''
	}
	
	def get_automaton_structure_name(Automaton automaton) {
		'''«automaton.name»_states'''
	}
	
	def node_action_name(Automaton automaton, Node node) {
		'''«automaton.name»_«names.get(automaton.name).get(node)»_do'''
	}
	
	def get_transition_action_name(Automaton automaton, Node node, int i) {
		'''«automaton.name»_«names.get(automaton.name).get(node)»_transition_«i»_do'''
	}
}
