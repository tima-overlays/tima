package fr.labri.gossip.tima.generator

import fr.labri.gossip.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

abstract class AutomataGenerator {
	abstract def void generateFiles(IRAutomata automata, String name, IFileSystemAccess2 fsa, IGeneratorContext context)
}