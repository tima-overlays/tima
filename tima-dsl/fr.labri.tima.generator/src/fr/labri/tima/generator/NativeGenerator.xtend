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

class NativeGenerator extends NamedNodeGenerator {
	
	new(IRAutomata a) {
		super(a)
	}
	
	override def void generateFiles(String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		fsa.generateFile('''«name»/«name».cc''', native_version(name))
		fsa.generateFile('''«name»/«name».h''', native_version_header(name))
	}
	
	def native_version_header(String name) {
		/*
		 *  This code is commented because it is not portable. It implies that a developer
		 *  has to learn specific details to implement a protocol for a specific target.
		 */
//		val classes = new LinkedHashMap<String,HashMap<String, Boolean>>
//		for (automaton : automata.automata.values) {
//			for (node : automaton.nodes) {
//				// guard
//				for (t : node.transtions.filter[it != node.timeoutTarget && it instanceof IRAutomata.ExternalGuard]) {
//					val guard = (t.guard as IRAutomata.ExternalGuard)
//					val members = guard.externalName.split("::") // FIXME: this absolutely non-portable 
//					if (members.get(0) != "tima") {
//						if (!classes.containsKey(members.get(0))) {
//							classes.put(members.get(0), new HashMap<String, Boolean>)
//						} 
//						classes.get(members.get(0)).put(members.get(1), false)
//					}
//				}
//				// action
//				for (act : state.actions) {
//					for (aaa : (act as TimaAction<String>).pre_actions) {
//						val members = aaa.type.split("::")
//						if (members.get(0) != "tima") {
//							if (!classes.containsKey(members.get(0))) {
//								classes.put(members.get(0), new HashMap<String, Boolean>)
//							} 
//							classes.get(members.get(0)).put(members.get(1), true)
//						}	
//					}
//					for (aaa : (act as TimaAction<String>).post_actions) {
//						val members = aaa.type.split("::")
//						if (members.get(0) != "tima") {
//							if (!classes.containsKey(members.get(0))) {
//								classes.put(members.get(0), new HashMap<String, Boolean>)
//							} 
//							classes.get(members.get(0)).put(members.get(1), true)
//						}	
//					}
//					for (aaa : (act as TimaAction<String>).each_actions) {
//						val members = aaa.type.split("::")
//						if (members.get(0) != "tima") {
//							if (!classes.containsKey(members.get(0))) {
//								classes.put(members.get(0), new HashMap<String, Boolean>)
//							} 
//							classes.get(members.get(0)).put(members.get(1), true)
//						}	
//					}
//				}
//			}
//		
//		}
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
		
«««		«FOR clazz: classes.keySet SEPARATOR '\n'»
«««			class «clazz» {
«««			public:
«««				«FOR method: classes.get(clazz).entrySet»
«««				«IF !method.value»
«««				static bool «method.key»(std::string& name, tima::TimaNativeContext* context);
«««				«ELSE»
«««				static void «method.key»(std::string& name, tima::TimaNativeContext* context);
«««				«ENDIF»
«««				«ENDFOR»
«««			};
«««		«ENDFOR»
		
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
			«FOR act : node.actions»
			«act.IR2Target»;
			«ENDFOR»
		}
		
		«FOR t: node.transtions SEPARATOR '\n'»
		
«««		/* forward declarations */
		«t.actions.get_forward_declarations»
		
		static void
		«get_transition_action_name(automaton, node, counter)»(
					const std::string& name,
					tima::TimaNativeContext* ctx)
		{
			/*transition from «names.get(automaton.name).get(node)» to «names.get(automaton.name).get(t.target)» */
			«IF t.actions.length > 0»
			«ENDIF»
			«FOR act : t.actions»
			«act.IR2Target»
			«ENDFOR»
		}
		
		static bool
		«get_transition_guard_name(automaton, node, counter++)»(
					const std::string& name,
					tima::TimaNativeContext* ctx)
		{
«««			return «t.guard.IR2Target»;
			return false;
		}
		
		«ENDFOR»
		
		«IF node.timeoutTarget != null»
		
«««		/* forward declarations */
		«node.timeoutActions.get_forward_declarations»
		
		static void
		«get_timeout_action_name(automaton, node)»(
					const std::string& name,
					tima::TimaNativeContext* ctx)
		{
		«FOR act : node.timeoutActions»
			«act.IR2Target»
			«ENDFOR»
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
			timeout : «IF node.timeout ==-1»tima::never_timeout«ELSE»«node.timeout»«ENDIF», // milliseconds
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
					«FOR p : act.arguments.entrySet SEPARATOR ','»
						std::string
					«ENDFOR»
					);
		«ENDFOR»
		'''
		
	}
	
	private def get_timeout_action_name(Automaton automaton, Node node)
		'''«automaton.name»_«names.get(automaton.name).get(node)»_timeout_do'''
	
	
	dispatch def IR2Target(IRAutomata.BuiltinAction action) {
		throw new UnsupportedOperationException("Really? Do we have some built-in action?");
	}
	
	dispatch def IR2Target(IRAutomata.ExternalAction action) {
		/* steps:
		 *  - build parameters
		 *  - call operation
		 */
		'''
		«action.externalName.substring(1, action.externalName.length-1)»(
			name, ctx,
			«FOR p : action.arguments.entrySet SEPARATOR ','»
				"«p.value»"
			«ENDFOR»
		);
		'''
		
	}
	
	dispatch def IR2Target(IRAutomata.MessageAction action) {
		/* steps:
		 * 1 - create message
		 * 2 - build parameters
		 * 3 - build target
		 * 4 - Send message
		 */
		var counter = 0
		'''
		«action.message.get_message_name» «get_temp(counter)»;
		«FOR f : action.arguments.entrySet»
		«get_temp(counter)».«f.key»("«f.value»");
		«ENDFOR»
		«IF action.message instanceof RemoteMessage»
		((tima::ActionContext*)ctx)->broadcast(1000, «get_temp(counter)»);
		«ELSE»
		tima::Mailbox::send(«get_temp(counter)», «(action.target as MessageTarget.Unicast).target»)
		«ENDIF»
		'''
	}
	
	def get_temp(int i) {
		'''tmp«i»'''
	}
	
	dispatch def IR2Target(IRAutomata.MessageGuard g) {
		'''message'''
	}
	
	dispatch def IR2Target(IRAutomata.ExternalGuard g)
		'''«g.externalName.simple_name»'''
	
	dispatch def IR2Target(IRAutomata.BuiltinGuard g) {
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
