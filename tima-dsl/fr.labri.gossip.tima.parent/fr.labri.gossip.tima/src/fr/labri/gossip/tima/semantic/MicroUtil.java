package fr.labri.gossip.tima.semantic;

import fr.labri.tima.ITimedAutomata.State;
import fr.labri.tima.TimedAutomata;

public class MicroUtil {
	
	// xtend doesn't have bitwise operators
	public static <C> boolean isUrgent(State<C> s) {
		return (s.getModifier() & TimedAutomata.URGENT) != 0;
	}
}
