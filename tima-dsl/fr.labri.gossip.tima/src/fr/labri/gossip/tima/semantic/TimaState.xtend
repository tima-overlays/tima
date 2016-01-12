package fr.labri.gossip.tima.semantic

import fr.labri.tima.ITimedAutomata.State
import java.util.List
import fr.labri.tima.ITimedAutomata.Action
import fr.labri.tima.ITimedAutomata.State

class TimaState<C> implements State<C> {
	
	val String name
	val int modifiers
	val List<Action<C>> actions 
	
	new(String name, int modifiers, List<Action<C>> actions ) {
		this.name = name
		this.modifiers = modifiers
		this.actions = actions
	}
	
	override getActions() {
		actions
	}
	
	override getModifier() {
		modifiers
	}
	
	override getName() {
		name
	}
	
	override eachAction(C context) {
		for (action : getActions())
             action.eachAction(context);
	}
	
	override postAction(C context) {
		for (action : getActions())
             action.postAction(context);
	}
	
	override preAction(C context) {
		for (action : getActions())
             action.preAction(context);
	}
	
	override hashCode() {
		name.hashCode
	}
	
	override toString() {
		'''TimaState: «name»'''
	}
	
	override equals(Object obj) {
		(obj.class == TimaState) && (obj as TimaState<C>).modifiers == modifiers && (obj as TimaState<C>).name == name
	}
	
}
