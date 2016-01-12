package fr.labri.gossip.tima.semantic

import fr.labri.gossip.tima.dSL.State
import fr.labri.gossip.tima.dSL.Modifier
import fr.labri.gossip.tima.dSL.Guard
import fr.labri.tima.TimedAutomata
import org.eclipse.emf.ecore.resource.Resource
import fr.labri.gossip.tima.dSL.Automata
import java.util.ArrayList
import fr.labri.tima.ITimedAutomata.Action
import java.util.LinkedHashMap
import fr.labri.tima.ITimedAutomata
import java.util.LinkedList
import fr.labri.gossip.tima.semantic.TimaAction.SimpleTimaAction

class DSLSemantic {
	/** Is the state initial? */
	public def isInitial(State s) {
		s.modifiers.exists[it.initial!=null]
	}
	
	/** Is the state urgent? */
	public def isUrgent(State s) {
		s.modifiers.exists[it.urgent!=null]
	}
	
	/** Is the state terminate ? */
	public def isTerminate(State s) {
		s.modifiers.exists[it.term!=null]
	}
	
	/** Are the same modifier */
	public def sameModifier(Modifier m1, Modifier m2) {
		(m1.urgent != null && m1.urgent == m2.urgent) || (m1.initial != null && m1.initial == m2.initial) || (m1.term != null && m1.term == m2.term)
	}
	
	/** Convert the value of timeout to milliseconds */
	public def toMilliseconds(Guard g) {
		// FIXME: avoid using these horrible constants
		if (g.value == 0)
			TimedAutomata.INFINITY
		else if (g.unit == "sec")
			g.value*1000
		else if (g.unit == "msec")
			g.value
		else TimedAutomata.INFINITY
	}
	
	/** Modifiers of a state */
	def getModifiers(State s) {
		var m = 0
		m = m + (if (isInitial(s)) TimedAutomata.INITIAL else 0)
		m = m + (if (isUrgent(s)) TimedAutomata.URGENT else 0)
		m = m + (if (s.isTerminate) TimedAutomata.TERMINATE else 0)
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
					for (action : s.actions) {
						switch (action.name) {
							case "pre-action": {
								action.actions.forEach[aaaa.pre_actions.add(new SimpleTimaAction<String>(a.name, it))]
							}
							case "post-action": {
								action.actions.forEach[aaaa.post_actions.add(new SimpleTimaAction<String>(a.name, it))]
							}
							case "action": {
								action.actions.forEach[aaaa.each_actions.add(new SimpleTimaAction<String>(a.name, it))]
							}
						}
					}
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
					if (t.guards.msg != null || t.guards.externalAction != null) {
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
					