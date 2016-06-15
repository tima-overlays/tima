package fr.labri.tima.semantic

import fr.labri.tima.dSL.Action
import fr.labri.tima.dSL.Automaton
import fr.labri.tima.dSL.BroadcastTarget
import fr.labri.tima.dSL.BuiltinAction
import fr.labri.tima.dSL.ExternalAction
import fr.labri.tima.dSL.ExternalGuard
import fr.labri.tima.dSL.FieldExpression
import fr.labri.tima.dSL.Guard
import fr.labri.tima.dSL.GuardedTransition
//import fr.labri.tima.dSL.InternalTarget
import fr.labri.tima.dSL.MessageAction
import fr.labri.tima.dSL.MessageGuard
import fr.labri.tima.dSL.State
import fr.labri.tima.dSL.StringExpression
import fr.labri.tima.dSL.TimeUnit
import fr.labri.tima.dSL.TimeoutTransition
import fr.labri.tima.dSL.UnicastTarget
import fr.labri.tima.ir.IRAutomata
import java.util.Iterator
import java.util.Map
import org.eclipse.emf.ecore.resource.Resource
import fr.labri.tima.dSL.MessageType
import fr.labri.tima.dSL.Expression
import java.util.LinkedList
import fr.labri.tima.dSL.MessageSection
import fr.labri.tima.dSL.KeyInStore
import fr.labri.tima.dSL.SimpleAutomaton

public class DSLSemantic {

	def toIR(Resource resource) {
		val builder = new IRBuilder
		resource.allContents.filter(MessageSection).forEach[
			val isLocal = it.name == "local" // FIXME
			builder.createMessages(it.messages.iterator, isLocal)
		]
		
		builder.createAutomata(resource.allContents.filter(Automaton))
		builder.automata
	}

	static class IRBuilder {
		val automata = new IRAutomata
		
		var Iterator<Automaton> front_end_automata;

		def createMessages(Iterator<MessageType> msgs, boolean local) {
			msgs.forEach[automata.messages.put(it.name, newMessage(it, local))]
		}

		def createAutomata(Iterator<Automaton> autos) {
			front_end_automata = autos
			autos.forEach[createAutomaton(it)]
		}

		def createAutomaton(Automaton a) {
			switch (a) {
				SimpleAutomaton: {
					val builder = new IRAutomatonBuilder(this, a)
					// the order here is important, remember, recursive definitions
					automata.add(builder.automaton) // line 1
					builder.createAutomata(a) // line 2	
				}
			}
		}

		def IRAutomata.Message getMessage(MessageType msg) {
			automata.messages.get(msg.name)
		}

		def IRAutomata.Automaton getAutomaton(Automaton a) {
			if (automata.automata.containsKey(a.name)) {
				automata.automata.get(a.name)
			}
			else {
				createAutomaton(a as SimpleAutomaton)
				getAutomaton(a)
			}
		}
		
		def IRAutomata.Automaton getAutomaton(String name) {
			if (automata.automata.containsKey(name)) {
				automata.automata.get(name)
			}
			else {
				if (front_end_automata.exists[it.name == name]) {
					createAutomaton(front_end_automata.findFirst[it.name == name] as SimpleAutomaton)
					getAutomaton(front_end_automata.findFirst[it.name == name])
				}
				else null
			}
		}

		def newMessage(MessageType msg, boolean local) {
			val r = if (local)
				new IRAutomata.Message(msg.name, msg.declaredFields.map[it])
			else
				
				new IRAutomata.RemoteMessage(msg.name, msg.declaredFields.map[it])
//			println(msg.name + " " + msg.fields + " " + r.class.simpleName)	
			r
		}
	}

	static class IRAutomatonBuilder {
		val IRAutomata.Automaton automaton
		val IRBuilder autoBuilder
		val Map<State, IRAutomata.Node> states = newHashMap()

		new(IRBuilder builder, Automaton auto) {
			autoBuilder = builder
			automaton = new IRAutomata.Automaton(auto.name)
		}

		def createAutomata(SimpleAutomaton a) {
			a.states.forEach[states.put(it, newNamedNode(it))]
			states.forEach[x, y|
				automaton.add(y)
				buildTransitions(x)
			]
			automaton.entryPoint = states.get(a.states.findFirst[ it.initial ])
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
				node.timeout = newTimeout - Math.max(0, timeout); // remove previous timeout (or elapsed time)
				val more = state.transitions.filter(GuardedTransition).map [
					if (it.guard.milliseconds >= newTimeout)
						node.addTransition(newTransition(it))
					Math.max(0, it.guard.milliseconds - newTimeout)
				].reduce[s, v| s + v]

				if (more > 0) {
					val newNode = new IRAutomata.DerivedNode(node)
					automaton.add(newNode)
					node.timeoutTarget = newNode
					buildTransitions(state, newNode, newTimeout)
				} else {
					val timeoutTarget = state.transitions.findFirst[it instanceof TimeoutTransition].target
					// FIXME, timeout should have associated actions
					node.addTimeoutActions(state.transitions.findFirst[it instanceof TimeoutTransition].actions.map[newAction(it)]);
					node.timeoutTarget = states.get(timeoutTarget)
				}
			}
		}
		
		def IRAutomata.NamedNode newNamedNode(State state) {
			val r = new IRAutomata.NamedNode(state.name, state.urgent)
			if (state.actions != null) {
				r.actions.addAll(state.actions.map[newAction(it)])
			}
			r
		}

		def IRAutomata.Transition newTransition(GuardedTransition t) {
			val g = t.guard
			val guard = switch g {
				ExternalGuard: {
					new IRAutomata.ExternalGuard(g.name, g.operands.map[
						newExpression(it)
					])

				}
//				BuiltInGuard: {
//					new IRAutomata.BuiltinGuard(g.name,  g.operands.map[
//						newExpression(it)
//					])
//				}
				MessageGuard:  {
					new IRAutomata.MessageGuard(autoBuilder.getMessage(g.msgPattern.type), newExpression(g.msgPattern.predicate))

				}
			}
			new IRAutomata.Transition(states.get(t.target), guard, t.actions.map[newAction(it)])
		}
	
		def newExpression(Expression e) {
			switch e {
				FieldExpression: new IRAutomata.Expression.Identifier(e.field)
				StringExpression: new IRAutomata.Expression.Constant(e.value)
				KeyInStore: new IRAutomata.Expression.KeyValuePair(e.key.sections.map[it.key].toList)
			}
		}

		def IRAutomata.Action newAction(Action action) {
			switch action {
				ExternalAction:	new IRAutomata.ExternalAction(action.name, action.operands.map[
						newExpression(it)
					])
				BuiltinAction: new IRAutomata.BuiltinAction(action.name, action.operands.map[
						newExpression(it)
					])
				MessageAction:
						new IRAutomata.MessageAction(
						newMessageTarget(action),
						autoBuilder.automata.messages.get(action.type.name),
						action.fields.toMap[it.name].mapValues[newExpression(it.value)]
	
					)
			}
		}

		def IRAutomata.MessageTarget newMessageTarget(MessageAction action) {
			val target = action.target
			switch target {
				BroadcastTarget: new IRAutomata.MessageTarget.Broadcast
				UnicastTarget: {
					val v = target.address as FieldExpression
					val a = autoBuilder.getAutomaton(v.field)
					if (a == null) 
						new IRAutomata.MessageTarget.Unicast(newExpression(target.address))
					else {
						new IRAutomata.MessageTarget.Internal(a)					
					}
				}

			}
		}

		def minAbove(int value, Iterable<GuardedTransition> trans) {
			var min = Integer.MAX_VALUE;
			for (t : trans) {
				if (t.guard.within) {
					val v = t.guard.milliseconds
					if (v > value && v < min)
						min = v
				}
			}
			if (min == Integer.MAX_VALUE) - 1 else min
		}

		def milliseconds(Guard g) {
			(if (g.unit == TimeUnit.SEC) 1000 else 1) * g.value
		}
	}
}

