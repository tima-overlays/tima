package fr.labri.gossip.tima.semantic

import fr.labri.tima.ITimedAutomata.ActionAdapter
import java.util.LinkedList
import fr.labri.gossip.tima.dSL.Action

class TimaAction<C> extends ActionAdapter<C> {
	
	static class SimpleTimaAction<C> extends ActionAdapter<C> {
		
		val String type = '';
		
		public boolean isMessage = false;
		
		public String msg_id
		
		public String automaton_dst 
		
		public 	String src_id
		
		new(String auto_name, Action g) {
//			type = dsl2cpp(auto_name, g).toString
		}
		
		override getType() {
			type
		}
		
//		private def dsl2cpp(String auto_name, Action g) {
//			isMessage = false
//			if (g.msgAction != null) {
//				msg_id = g.msgAction.type + "_MSG_ID"
//				automaton_dst = g.msgAction.target.name // FIXME switch case if broadcast, local or remote
//				src_id = auto_name + "_AUTOMATON_ID"
//				isMessage = true
//				'''tima::Mailbox::send''' //
//			}
//			else if (g.externalAction != null) {
//				'''«unMangleString(g.externalAction.name)»'''
//			}
//			else { // TODO builtin
//				'''timeout'''
//			}
//		}
		
	}
	
	public val LinkedList<SimpleTimaAction<C>> pre_actions = newLinkedList()
	public val LinkedList<SimpleTimaAction<C>> post_actions = newLinkedList()
	public val LinkedList<SimpleTimaAction<C>> each_actions = newLinkedList()
	
	
		
}