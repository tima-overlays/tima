package fr.labri.tima.semantic

import fr.labri.tima.dSL.Action
import fr.labri.tima.dSL.Automaton
import fr.labri.tima.dSL.BroadcastTarget
import fr.labri.tima.dSL.BuiltInGuard
import fr.labri.tima.dSL.BuiltinAction
import fr.labri.tima.dSL.ExternalAction
import fr.labri.tima.dSL.ExternalGuard
import fr.labri.tima.dSL.FieldExpression
import fr.labri.tima.dSL.Guard
import fr.labri.tima.dSL.GuardedTransition
import fr.labri.tima.dSL.InternalTarget
import fr.labri.tima.dSL.MessageAction
import fr.labri.tima.dSL.MessageGuard
import fr.labri.tima.dSL.State
import fr.labri.tima.dSL.StringExpression
import fr.labri.tima.dSL.TimeUnit
import fr.labri.tima.dSL.TimeoutTransition
import fr.labri.tima.dSL.UnicastTarget
import fr.labri.tima.ir.IRAutomata
import fr.labri.tima.ir.IRAutomata.MessageFieldOperand
import fr.labri.tima.ir.IRAutomata.StringOperand
import java.util.Iterator
import java.util.Map
import org.eclipse.emf.ecore.resource.Resource
import fr.labri.tima.dSL.MessageType

class DSLSemantic {

	def toIR(Resource resource) {
		val builder = new IRBuilder
		builder.createMessages(resource.allContents.filter(MessageType))
		builder.createAutomata(resource.allContents.filter(Automaton))
		builder.automata
	}

	static class IRBuilder {
		val automata = new IRAutomata

		def createMessages(Iterator<MessageType> msgs) {
			msgs.forEach[automata.messages.put(it.name, newMessage(it))]
		}

		def createAutomata(Iterator<Automaton> autos) {
			autos.forEach[createAutomaton(it)]
		}

		def createAutomaton(Automaton a) {
			val builder = new IRAutomatonBuilder(this, a)
			builder.createAutomata(a)
			automata.add(builder.automaton)
		}

		def IRAutomata.Message getMessage(MessageType msg) {
			automata.messages.get(msg.name)
		}

		def IRAutomata.Automaton getAutomaton(Automaton a) {
			automata.automata.get(a.name)
		}

		def newMessage(MessageType msg) {
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

		new(IRBuilder builder, Automaton auto) {
			autoBuilder = builder
			automaton = new IRAutomata.Automaton(auto.name)
		}

		def createAutomata(Automaton a) {
			a.states.forEach[states.put(it, new IRAutomata.NamedNode(it.name))]
			states.forEach[x, y|
				automaton.add(y)
				buildTransitions(x)
			]
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
					node.timeoutTarget = states.get(timeoutTarget)
				}
			}
		}

		def IRAutomata.Transition newTransition(GuardedTransition t) {
			val g = t.guard
			val guard = switch g {
				ExternalGuard: {
					new IRAutomata.ExternalGuard(g.name)

				}
				BuiltInGuard: {
					new IRAutomata.BuiltinGuard(g.name, newHashMap(g.fields.map[it.name -> it.value]))
				}
				MessageGuard:  {
					new IRAutomata.MessageGuard(autoBuilder.getMessage(g.msgPattern.type), g.msgPattern.patterns.map[
						new IRAutomata.Pattern(it.operator, it.operands.map[
							switch it {
								FieldExpression: new MessageFieldOperand(it.field)
								StringExpression: new StringOperand(it.value)
							}
							])
					])

				}
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
			val target = action.target
			switch target {
				BroadcastTarget: new IRAutomata.MessageTarget.Broadcast
				UnicastTarget: new IRAutomata.MessageTarget.Unicast(target.address)
				InternalTarget: new IRAutomata.MessageTarget.Internal(autoBuilder.getAutomaton(target.automata))
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

