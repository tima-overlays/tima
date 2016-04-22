package fr.labri.gossip.tima.generator

import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext
import fr.labri.gossip.tima.dSL.Message
import fr.labri.gossip.tima.ir.IRAutomata
import java.util.LinkedHashMap
import java.util.HashMap
import fr.labri.Utils
import java.util.List

class NativeGenerator extends AutomataGenerator {
	
	override def void generateFiles(IRAutomata automata, String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		fsa.generateFile('''«name»/«name».cc''', native_version(name, automata))
		fsa.generateFile('''«name»/«name».h''', native_version_header(name, automata))
	}
	
	def native_version_header(String name, IRAutomata automata) {
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
	
	def native_version(String project_name, IRAutomata automata) {
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
	
	«FOR a: automata.automata.values SEPARATOR '\n'»
	/** Automaton «a.name» */
	
	«FOR node: a.nodes SEPARATOR '\n'»
		«FOR act : node.actions»
		static void
		«a.name»_«node.name»_do(std::string& name, tima::TimaNativeContext* ctx)
		{
			tima::TimaNativeContext* ctx2;
			«FOR act_simple : (act as TimaAction<String>).pre_actions»
				«actionStep(act_simple)»
			«ENDFOR»
		}
		«ENDFOR»
	«ENDFOR»
	
	«FOR state: a.value.states SEPARATOR '\n'»
		static struct tima::Transition «a.key»_«state.name»[] = {
		«FOR follower : a.value.getFollowers(state).filter[Utils.indexOf(it, a.value.states) != a.value.asCompiled.getTimeoutDestination(state)] SEPARATOR ','»
			{
				.dst = «Utils.indexOf(follower, a.value.states)»,
				.guard = «a.value.getPredicate(state, follower).type»,
				.msg_id = MESSAGES_ID::«(a.value.getPredicate(state, follower) as TimaGuard<?>).messageID»_MSG_ID,
				.src_id = AUTOMATA_ID::«(a.value.getPredicate(state, follower) as TimaGuard<?>).sourceID»_AUTOMATON_ID
			}
		«ENDFOR»
		};
	«ENDFOR»
	
	static struct tima::State «a.key»_states[] = {
	«FOR state: a.value.states SEPARATOR ','»
		{
		.name = "«state.name»",
		.urgent = «IF (MicroUtil.isUrgent(state))»true«ELSE»false«ENDIF»,
		.timeout = «IF (a.value.asCompiled).getStateTimeOut(state)==-1»tima::never_timeout«ELSE»«(a.value.asCompiled).getStateTimeOut(state)»«ENDIF», // milliseconds
		.timeout_destination = «IF (a.value.asCompiled).getTimeoutDestination(state)==-1»tima::null_destination«ELSE»«(a.value.asCompiled).getTimeoutDestination(state)»«ENDIF»,
		.nr_transitions = «numberOfFollowers(a.value, state)», // without taking into account the default transition
		.transitions = «a.key»_«state.name»,
		.pre_action = «a.key»_«state.name»_pre_action,
		.post_action = «a.key»_«state.name»_post_action,
		.each_action = «a.key»_«state.name»_each_action
		}
	«ENDFOR»
	};
	
	static struct tima::Automata «a.key» = {
		.name = "«a.key»",
		.initial = «Utils.indexOf(a.value.initialState, a.value.states)»,
		.nr_states = «a.value.states.length»,
		.states = «a.key»_states
	};
	
	«ENDFOR»
	
	static const uint32_t nr_automatas = «map.size»;
	
	static struct tima::Automata* automatons [] = {
		«FOR name : map.keySet SEPARATOR ','»
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
	
}