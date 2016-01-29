package fr.labri.tima;

import fr.labri.tima.ITimedAutomata.Action;
import fr.labri.tima.ITimedAutomata.ActionAdapter;
import fr.labri.tima.ITimedAutomata.NodeFactory;
import fr.labri.tima.ITimedAutomata.Predicate;
import fr.labri.tima.ITimedAutomata.PredicateAdapter;

public class SimpleNodeFactory<C> implements NodeFactory<C> {

    public Predicate<C> newPredicate(final String name, String attr) {
        return new PredicateAdapter<C>() {
            public String getType() {
                return name;
            }
        };
    }

    @Override
    public Action<C> newAction(final String type, final String attr) {
        return new ActionAdapter<C>() {
            public String getType() {
                return type + ":" + attr;
            }
        };
    }
}
