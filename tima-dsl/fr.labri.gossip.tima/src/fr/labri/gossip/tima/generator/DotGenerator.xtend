package fr.labri.gossip.tima.generator

import fr.labri.gossip.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext
import java.util.HashMap

class DotGenerator extends AutomataGenerator {
	
	val generatedNames = new HashMap<String, Integer>
	
	override generateFiles(IRAutomata automata, String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		fsa.generateFile('''«name»/«name».dot''',
		'''
			«FOR a : automata.automata.entrySet SEPARATOR '\n' »
				«a.value.toDot()»
			«ENDFOR»
		''')
	}
	
	dispatch def String toDot(IRAutomata.Automaton a) {
		'''
		digraph «a.name» {
			«FOR state : a.nodes»
				«FOR t : state.transtions»
					«state.generatedUniqueName» -> «t.target.generatedUniqueName» [label=«t.guard.toDot»];
				«ENDFOR»
				
			«ENDFOR»
		 }
		'''
	}
	
	dispatch def toDot(IRAutomata.BuiltinGuard g) {
		'''
			«g.builtinName»
		'''
	}
	
	dispatch def toDot(IRAutomata.ExternalGuard g) {
		'''
			«g.externalName»
		'''
	}
	
	dispatch def toDot(IRAutomata.MessageGuard g) {
		'''
			«g.messageType.name»
		'''
	}
	
	/**
	 * Obtain name of referenced node
	 */
	dispatch def baseName(IRAutomata.NamedNode n) {
		n.name
	} 
	
	
	/**
	 * Obtain name of referenced node
	 */
	dispatch def String baseName(IRAutomata.DerivedNode n) {
		n.referenceNode.baseName
	} 
	
	/**
	 * Generate unique name
	 */
	dispatch def generatedUniqueName(IRAutomata.NamedNode n) {
		generatedNames.putIfAbsent(n.name, 0)
		n.name
	} 
	
	/**
	 * Generate unique name
	 */
	dispatch def generatedUniqueName(IRAutomata.DerivedNode n) {
		val base = n.baseName
		val i = generatedNames.get(base) + 1
		generatedNames.put(base, i)
		'''«base»_«i-1»'''
	} 
}
