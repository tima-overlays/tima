package fr.labri.gossip.tima.semantic

import fr.labri.gossip.tima.dSL.State
import fr.labri.gossip.tima.dSL.Guard
import fr.labri.tima.TimedAutomata
import org.eclipse.emf.ecore.resource.Resource
import fr.labri.gossip.tima.dSL.Automata
import java.util.LinkedHashMap
import fr.labri.tima.ITimedAutomata
import fr.labri.gossip.tima.semantic.TimaAction.SimpleTimaAction
import fr.labri.gossip.tima.dSL.TimeUnit

class DSLSemantic {

	/** Convert the value of timeout to milliseconds */
	public def toMilliseconds(Guard g) {
		// FIXME: avoid using these horrible constants
		if (g.value == 0)
			TimedAutomata.INFINITY
		else if (g.unit == TimeUnit.SEC)
			g.value*1000
		else if (g.unit == TimeUnit.MSEC)
			g.value
		else TimedAutomata.INFINITY
	}
	
	/** Modifiers of a state */
	def getModifiers(State s) {
		var m = 0
		m = m + (if (s.initial) TimedAutomata.INITIAL else 0)
		m = m + (if (s.urgent) TimedAutomata.URGENT else 0)
	}
	
	/** transforms a model in DSL representation to TIMA representation */
	def toTima(Resource resource) {
		val automatons = new LinkedHashMap<String, ITimedAutomata<String>>()
		for (a: resource.allContents.filter(Automata).toIterable) {
			val automata = new TimedAutomata<String>() // add name
			val hm = newLinkedHashMap()
			for (s : a.states) {
				// actions
				val aaaa = new TimaAction<String>
				if (s.actions != null && s.actions.size > 0) {
					s.actions.forEach[aaaa.pre_actions.add(new SimpleTimaAction<String>(a.name, it))]
				}
				// add the state
				val state = new TimaState<String>(s.name, getModifiers(s), newLinkedList(aaaa))
				hm.put(s.name, state)
				if (s.isInitial) {
					automata.initial = state
				}
			}
			for (s : a.states) {
				val source = hm.get(s.name)
				for (t : s.transitions) {
					val target = hm.get(t.target.name)
					// FIXME deal with actions in transition
					if (t.guards.msgGuard != null || t.guards.externalGuard != null) {
						val timeout = t.guards.toMilliseconds
//						System.out.println(source + " " + target + " " + dsl2cpp(t.guards).toString);
						automata.addTransition(source, timeout, new TimaGuard<String>(a.name, t.guards), target)
					} else{
						automata.addDefaultTransition(source, target)
					}
				}
			}
			automatons.put(a.name, automata)
		}
		automatons
	}
}
					