package fr.labri.gossip.tima.generator

import fr.labri.gossip.tima.Util
import fr.labri.gossip.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext
import fr.labri.gossip.tima.ir.IRAutomata.Automaton
import fr.labri.gossip.tima.ir.IRAutomata.Node
import java.util.Map

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
//				// guards
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
//				// actions
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
				«m.name»_MSG_ID
			«ENDFOR»
		};
		
		«FOR m : automata.messages.values SEPARATOR '\n'»
			struct Message«m.name.toFirstUpper» : public tima::Message {
				Message«m.name.toFirstUpper»(): Message(«m.name»_MSG_ID, 0) {}
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
				return MESSAGES_ID::«m.name»_MSG_ID;
		«ENDFOR»
		return -1;
	}
	
	«FOR automaton: automata.automata.values SEPARATOR '\n'»
	/** Automaton «automaton.name» */
	
	«FOR node: automaton.nodes SEPARATOR '\n'»
		
		static void
		«node_action_name(automaton, node)»(std::string& name, tima::TimaNativeContext* ctx)
		{
			tima::TimaNativeContext* ctx2;
			«FOR act : node.actions»
			«act.IR2Target»;
			«ENDFOR»
		}
		
		«FOR t: node.transtions SEPARATOR '\n'»
		static void
		«get_transition_action_name(automaton, node, counter)»()
		{
			/*transition from «names.get(automaton.name).get(node)» to «names.get(automaton.name).get(t.target)» */
			tima::TimaNativeContext* ctx2;
			«FOR act : t.getActions()»
			«act.IR2Target»;
			«ENDFOR»
		}
		
		static bool
		«get_transition_guard_name(automaton, node, counter++)»()
		{
			tima::TimaNativeContext* ctx2;
			return «t.guard.IR2Target»;
		}
		
		«ENDFOR»
		
		«IF node.timeoutTarget != null»
«««		static void
«««		«automaton.name»_«names.get(automaton.name).get(node)»_timeout_do() {
«««			tima::TimaNativeContext* ctx2;
«««			«FOR act : t.getActio»
«««			«act.IR2Target»;
«««			«ENDFOR»
«««		}
		«ENDIF»
		
		«{counter = 0; null}»
		static struct tima::Transition «get_name_for_transitions(automaton, node)»[] = {
			«FOR t: node.transtions SEPARATOR ','»
			{
				.dst = «Util.indexOf(t.target, automaton.nodes)»,
				.action = «get_transition_action_name(automaton, node, counter)»,
				.guard = «get_transition_guard_name(automaton, node, counter++)»
«««				.msg_id = MESSAGES_ID::«(a.value.getPredicate(state, follower) as TimaGuard<?>).messageID»_MSG_ID,
«««				.src_id = AUTOMATA_ID::«(a.value.getPredicate(state, follower) as TimaGuard<?>).sourceID»_AUTOMATON_ID
			}
			«ENDFOR»
		};
		
	«ENDFOR»
	
	
	static struct tima::State «get_automaton_structure_name(automaton)»[] = {
		«FOR node: automaton.nodes SEPARATOR ','»
		{
		.name = "«names.get(automaton.name).get(node)»",
«««		.urgent = «IF (MicroUtil.isUrgent(state))»true«ELSE»false«ENDIF»,
		.timeout = «IF node.timeout ==-1»tima::never_timeout«ELSE»«node.timeout»«ENDIF», // milliseconds
		.timeout_destination = «IF node.timeout==-1»tima::null_destination«ELSE»«Util.indexOf(node.timeoutTarget, automaton.nodes) »«ENDIF»,
		.nr_transitions = «node.transtions.size», // without taking into account the default transition
		.transitions = &«get_name_for_transitions(automaton, node)»,
		.action = «node_action_name(automaton, node)»,
		}
		«ENDFOR»
	};
	
	static struct tima::Automaton «automaton.name» = {
		.name = "«automaton.name»",
		.initial = «Util.indexOf(automaton.entryPoint, automaton.nodes)»,
		.nr_states = «automaton.nodes.length»,
		.states = &«get_automaton_structure_name(automaton)»
	};
	
	«ENDFOR»
	
	static const uint32_t nr_automaton = «automata.automata.size»;
	
	static struct tima::Automata* automatons [] = {
		«FOR name : automata.automata.keySet SEPARATOR ','»
		&«name»
		«ENDFOR»
	};
	
	uint32_t
	get_nr_automatas()
	{
		return nr_automatas;
	}
	
	struct tima::Automata&
	get_automata(uint32_t idx)
	{
		return *automatons[idx];
	}
	
	'''
	}
	
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
	
	dispatch def IR2Target(IRAutomata.BuiltinAction action) {
		throw new UnsupportedOperationException("Really? Do we have some builtin action?");
	}
	
	dispatch def IR2Target(IRAutomata.ExternalAction action)
		'''«action.externalName.substring(1, action.externalName.length-1)»'''
	
	dispatch def IR2Target(IRAutomata.MessageAction action) {
		
	}
	
	dispatch def IR2Target(IRAutomata.MessageGuard g) {
		'''message'''
	}
	
	dispatch def IR2Target(IRAutomata.ExternalGuard g)
		'''«g.externalName.substring(1, g.externalName.length-1)»'''
	
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
}