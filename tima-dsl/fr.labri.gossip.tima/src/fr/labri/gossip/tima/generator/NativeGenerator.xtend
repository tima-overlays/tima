package fr.labri.gossip.tima.generator

import fr.labri.Utils
import fr.labri.gossip.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

class NativeGenerator extends NamedNodeGenerator {
	
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
		val names = newHashMap(automata.automata.values.map[it.name -> it.names])
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
		«automaton.name»_«names.get(automaton.name).get(node)»_do(std::string& name, tima::TimaNativeContext* ctx)
		{
			tima::TimaNativeContext* ctx2;
			«FOR act : node.actions»
			«act.IR2Target»;
			«ENDFOR»
		}
		
		«FOR t: node.transtions SEPARATOR '\n'»
		static void
		«automaton.name»_«names.get(automaton.name).get(node)»_«counter++»_do() {
			
		}
		«ENDFOR»
		
		«{counter = 0; null}»
		static struct tima::Transition «automaton.name»_«names.get(automaton.name).get(node)»[] = {
		«FOR t: node.transtions SEPARATOR ','»
			{
				.dst = «Utils.indexOf(t.target, automaton.nodes)»,
				.action = «automaton.name»_«names.get(automaton.name).get(node)»_«counter++»_do
				.guard = «t.guard.IR2Target»,
«««				.msg_id = MESSAGES_ID::«(a.value.getPredicate(state, follower) as TimaGuard<?>).messageID»_MSG_ID,
«««				.src_id = AUTOMATA_ID::«(a.value.getPredicate(state, follower) as TimaGuard<?>).sourceID»_AUTOMATON_ID
			}
		«ENDFOR»
		};
		
	«ENDFOR»
	
	
	static struct tima::State «automaton.name»_states[] = {
	«FOR node: automaton.nodes SEPARATOR ','»
		{
		.name = "«names.get(automaton.name).get(node)»",
«««		.urgent = «IF (MicroUtil.isUrgent(state))»true«ELSE»false«ENDIF»,
		.timeout = «IF node.timeout ==-1»tima::never_timeout«ELSE»«node.timeout»«ENDIF», // milliseconds
		.timeout_destination = «IF node.timeout==-1»tima::null_destination«ELSE»«Utils.indexOf(node.timeoutTarget, automaton.nodes) »«ENDIF»,
		.nr_transitions = «node.transtions.size», // without taking into account the default transition
		.transitions = «automaton.name»_«names.get(automaton.name).get(node)»,
		.action = «automaton.name»_«names.get(automaton.name).get(node)»_do,
		}
	«ENDFOR»
	};
	
	static struct tima::Automaton «automaton.name» = {
		.name = "«automaton.name»",
		.initial = «Utils.indexOf(automaton.entryPoint, automaton.nodes)»,
		.nr_states = «automaton.nodes.length»,
		.states = «automaton.name»_states
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
	
	dispatch def void IR2Target(IRAutomata.BuiltinAction action) {
		
	}
	
	dispatch def void IR2Target(IRAutomata.ExternalAction action) {
		
	}
	
	dispatch def void IR2Target(IRAutomata.MessageAction action) {
		
	}
	
	dispatch def void IR2Target(IRAutomata.MessageGuard action) {
		
	}
	
	dispatch def void IR2Target(IRAutomata.ExternalGuard action) {
		
	}
	
	dispatch def void IR2Target(IRAutomata.BuiltinGuard action) {
		
	}
}