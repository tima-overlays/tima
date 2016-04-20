package fr.labri.gossip.tima.generator

import fr.labri.gossip.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

class DotGenerator extends AutomataGenerator {
	
	override generateFiles(IRAutomata automata, String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		fsa.generateFile('''«s»/«s»_real.dot''',
		'''
			«FOR a : automatons.entrySet SEPARATOR '\n' »
				«DotRenderer.toDot(a.value, a.key)»
			«ENDFOR»
		''')
		for (e : automatons.entrySet) {
			e.value = e.value.compile
		}
		fsa.generateFile('''«s»/«s»_compiled.dot''',
		'''
			«FOR a : automatons.entrySet SEPARATOR '\n' »
				«DotRenderer.toDot(a.value, a.key)»
			«ENDFOR»
		''')
	}
	
}
