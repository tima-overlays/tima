package fr.labri.gossip.tima.generator

import java.util.HashMap
import fr.labri.gossip.tima.ir.IRAutomata
import java.util.Map

/**
 * This class provides operations to name Derived Nodes
 */
abstract class NamedNodeGenerator extends AutomataGenerator {

	new(IRAutomata a) {
		super(a)
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
	
	protected def Map<IRAutomata.Node, String> getNames(IRAutomata.Automaton a) {
		val m = new HashMap<IRAutomata.Node, String>()
		
		// add named nodes
		a.nodes.filter(IRAutomata.NamedNode).forEach[ 
			m.put(it, it.name)
		]
		
		// add derived nodes
		a.nodes.filter(IRAutomata.DerivedNode).forEach[it, idx|
			val s = it.baseName
			m.put(it, '''«s»_«idx»''')
		]
		
		return m
	}
	
}