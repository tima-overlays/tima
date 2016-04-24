package fr.labri.gossip.tima.generator

import fr.labri.gossip.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

class DotGenerator extends NamedNodeGenerator {
	
	
	override generateFiles(IRAutomata automata, String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		fsa.generateFile('''«name»/«name».dot''',
		'''
			«FOR a : automata.automata.entrySet SEPARATOR '\n' »
				«a.value.toDot()»
			«ENDFOR»
		''')
	}
	
	dispatch def String toDot(IRAutomata.Automaton a) {
		val generatedNames = a.getNames()
		'''
		digraph «a.name» {
			«FOR state : a.nodes»
				«FOR t : state.transtions»
					«generatedNames.get(state)» -> «generatedNames.get(t.target)» [label="«t.guard.toDot»/«state.timeout»"];
				«ENDFOR»
				«IF state.timeoutTarget != null »
				«generatedNames.get(state)» -> «generatedNames.get(state.timeoutTarget)» [style=dotted];
				«ENDIF»
			«ENDFOR»
		 }
		'''
	}
	
	dispatch def toDot(IRAutomata.BuiltinGuard g) {
		'''«g.builtinName»'''
	}
	
	dispatch def toDot(IRAutomata.ExternalGuard g) {
		'''«g.externalName.substring(1, g.externalName.length - 1)»'''
	}
	
	dispatch def toDot(IRAutomata.MessageGuard g) {
		'''«g.messageType.name»'''
	}
	
}
