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
import fr.labri.gossip.tima.ir.IRAutomata
import java.util.Map
import fr.labri.gossip.tima.dSL.GuardedTransition
import fr.labri.gossip.tima.dSL.ExternalGuard
import fr.labri.gossip.tima.dSL.BuiltInGuard
import fr.labri.gossip.tima.dSL.MessageGuard
import fr.labri.gossip.tima.dSL.FieldExpression
import fr.labri.gossip.tima.ir.IRAutomata.MessageFieldOperand
import fr.labri.gossip.tima.dSL.StringExpression
import fr.labri.gossip.tima.ir.IRAutomata.StringOperand
import fr.labri.gossip.tima.dSL.Action
import fr.labri.gossip.tima.dSL.ExternalAction
import fr.labri.gossip.tima.dSL.BuiltinAction
import fr.labri.gossip.tima.dSL.MessageAction
import fr.labri.gossip.tima.dSL.Message
import fr.labri.gossip.tima.dSL.BroadcastTarget
import fr.labri.gossip.tima.dSL.UnicastTarget
import fr.labri.gossip.tima.dSL.InternalTarget
import fr.labri.gossip.tima.ir.IRAutomata.Automaton
import java.util.Iterator
import fr.labri.gossip.tima.dSL.TimeoutTransition

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
//				for (t : s.transitions) {
//					val target = hm.get(t.target.name)
//					 FIXME deal with actions in transition
//					if (t.gguards.msgGuard != null || t.guards.externalGuard != null) {
//						val timeout = t.guards.toMilliseconds
//						System.out.println(source + " " + target + " " + dsl2cpp(t.guards).toString);
//						automata.addTransition(source, timeout, new TimaGuard<String>(a.name, t.guards), target)
//					} else{
//						automata.addDefaultTransition(source, target)
//					}
//				}
			}
			automatons.put(a.name, automata)
		}
		automatons
	}
	
	def toIR(Resource resource) {
		val builder = new IRBuilder
		builder.createMessages(resource.allContents.filter(Message))
		builder.createAutomata(resource.allContents.filter(Automata))
		builder.automata
	}
	
	static class IRBuilder {
		val automata = new IRAutomata

		def createMessages(Iterator<Message> msgs) {
			msgs.forEach[automata.messages.put(it.name, newMessage(it))]
		}
		
		def createAutomata(Iterator<Automata> autos) {
			autos.forEach[createAutomaton(it)]
		}
		
		def createAutomaton(Automata a) {
			val builder = new IRAutomatonBuilder(this, a)
			builder.createAutomata(a)
			automata.add(builder.automaton)
		}
		
		def IRAutomata.Message getMessage(Message msg) {
			automata.messages.get(msg.name)
		}
		
		def IRAutomata.Automaton getAutomaton(Automata a) {
			automata.automata.get(a.name)
		}
		
		def newMessage(Message msg) {
			if (msg.fields == null) // FIXME ugly
				new IRAutomata.Message(msg.name, msg.declaredFields.map[it])
			else
				new IRAutomata.RemoteMessage(msg.name, msg.declaredFields.map[it], newHashMap(msg.fields.map[it.name -> it.value]))			
		}
	}
	
	static class IRAutomatonBuilder {
		val IRAutomata.Automaton automaton
		val IRBuilder autoBuilder
		val Map<State, IRAutomata.Node> states = newHashMap()

		new(IRBuilder builder, Automata auto) {
			autoBuilder = builder
			automaton = new IRAutomata.Automaton(auto.name)
		}
		
		def createAutomata(Automata a) {
			a.states.forEach[states.put(it, new IRAutomata.NamedNode(it.name))]
			states.forEach[x, y| buildTransitions(x)]
		}
		
		def buildTransitions(State state) {
			val node = states.get(state)
			val newTimeout = minAbove(-1, state.transitions.filter(GuardedTransition))
			if (newTimeout == -1) {
				state.transitions.filter(GuardedTransition).forEach [node.addTransition(newTransition(it))]
			} else {
				buildTransitions(state, node, -1) 
			}
		}
		
		def void buildTransitions(State state, IRAutomata.Node node, int timeout) {
			val newTimeout = minAbove(timeout, state.transitions.filter(GuardedTransition))
			if (newTimeout != -1) {
				node.timeout = newTimeout
				val more = state.transitions.filter(GuardedTransition).map [
					if (it.guard.value > newTimeout)
						node.addTransition(newTransition(it))
					Math.max(0, it.guard.value - newTimeout)].reduce[s, v| s + v]
				if (more > 0) {
					val newNode = new IRAutomata.DerivedNode(node)
					node.timeoutTarget = newNode
					buildTransitions(state, newNode, newTimeout)
				} else {
					val timeoutTarget = state.transitions.findFirst[it instanceof TimeoutTransition]
					node.timeoutTarget = states.get(timeoutTarget)
				} 
			}
		}

		def IRAutomata.Transition newTransition(GuardedTransition t) {
			val g = t.guard
			val guard = switch g {
				ExternalGuard: new IRAutomata.ExternalGuard(g.name)
				BuiltInGuard: new IRAutomata.BuiltinGuard(g.name, newHashMap(g.fields.map[it.name -> it.value]))
				MessageGuard: new IRAutomata.MessageGuard(autoBuilder.getMessage(g.msgPattern.type), g.msgPattern.patterns.map[
					new IRAutomata.Pattern(it.operator, it.operands.map[
						switch it {
							FieldExpression: new MessageFieldOperand(it.field)
							StringExpression: new StringOperand(it.value)
						}
					])
				])
			}
			new IRAutomata.Transition(states.get(t.target), guard, t.actions.map[newAction(it)])
		}

		def IRAutomata.Action newAction(Action action) {
			switch action {
				ExternalAction:	new IRAutomata.ExternalAction(action.name)
				BuiltinAction: new IRAutomata.BuiltinAction(action.name, newHashMap(action.fields.map[it.name -> it.value]))
				MessageAction: new IRAutomata.MessageAction(
					newMessageTarget(action),
					autoBuilder.automata.messages.get(action.type.name),
					newHashMap(action.fields.map[it.name -> it.value])
				)
			}
		}
		
		def IRAutomata.MessageTarget newMessageTarget(MessageAction action) {
			switch action {
				BroadcastTarget: new IRAutomata.MessageTarget.Broadcast
				UnicastTarget: new IRAutomata.MessageTarget.Unicast(action.address)
				InternalTarget: new IRAutomata.MessageTarget.Internal(autoBuilder.getAutomaton(action.automata))
			}
		}
		
		def minAbove(int value, Iterable<GuardedTransition> trans) {
			var min = -1;
			for (t : trans) {
				if (t.guard.within) {
					val v = t.guard.value
					if (v > value && v < min) // FIXME unit
						min = v
				}
			}
			min
		}
	}
}
					