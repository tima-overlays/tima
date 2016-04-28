package fr.labri.tima.generator

import fr.labri.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

abstract class AutomataGenerator {
	protected IRAutomata automata
	new (IRAutomata a) {
		automata = a
	}
	abstract def void generateFiles(String name, IFileSystemAccess2 fsa, IGeneratorContext context)
}
