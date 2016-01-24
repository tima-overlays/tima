package fr.labri.gossip.tima.semantic

import fr.labri.tima.ITimedAutomata.Predicate
import fr.labri.gossip.tima.dSL.Guard

class TimaGuard<C> implements Predicate<C> {
	
	val String type
	
	public var String messageID = "ANY"
	public var String sourceID = "ANY"
	
	public enum GuardType {
		LOCAL_MESSAGE_GUARD,
		NETWORK_MESSAGE_GUARD,
		GENERIC_GUARD,
		TIMEOUT_GUARD
	}
	
	public GuardType guardType
	
	new(String automatonName, Guard g) {
		this.type = dsl2cpp(automatonName, g).toString
	}
	
	/** Create a c++ representation of a guard */
	private def dsl2cpp(String automatonName, Guard g) {
		if (g.msg != null) {
			messageID = g.msg.msg.name
//			if (g.source != null) {
//				sourceID = g.source.name
				guardType = GuardType.LOCAL_MESSAGE_GUARD
				'''tima::Mailbox::exists'''
//			}
//			else {
//				guardType = GuardType.NETWORK_MESSAGE_GUARD
//				'''tima::Mailbox::exists_network_message'''
//			}
		}
		else if (g.externalAction != null) {
			guardType = GuardType.GENERIC_GUARD
			'''«g.externalAction.name»::«g.externalAction.method»'''
		}
		else {
			guardType = GuardType.TIMEOUT_GUARD
			'''timeout'''
		}
	}
	
	override getType() {
		type
	}
	
	override isValid(C context) {
		true
	}
	
	override toString() {
		type
	}
	
	override equals(Object obj) {
		(obj.class == TimaGuard) && ((obj as TimaGuard<C>).type == type) && (messageID == (obj as TimaGuard<C>).messageID) && ((obj as TimaGuard<C>).sourceID == sourceID)
	}
	
	override hashCode() {
		type.hashCode + messageID.hashCode + sourceID.hashCode
	}
	
}
