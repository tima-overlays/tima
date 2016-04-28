package fr.labri.tima;

import java.util.Collection;
import java.util.List;

public interface ITimedAutomata<C> {
	int INITIAL = 1 << 0;
	int URGENT = 1 << 1;
	int TERMINATE = 1 << 2;

	State<C> getInitialState();
	void setInitialState(State<C> initial);
	State<C>[] getStates();
	State<C>[] getFollowers(State<C> src);
	int getTimeout(State<C> src, State<C> dst);
	Predicate<C> getPredicate(State<C> src, State<C> dst);

	ITimedAutomata<C> compile();
	
	Cursor<C> start(ContextProvider<C> context);

	interface NodeFactory<C> {
		Predicate<C> newPredicate(String type, String attr);
		Action<C> newAction(String type, String attr);
	}

	interface Predicate<C> {
		boolean isValid(C context);
		String getType();
	}
	
	interface Action<C> {
		void preAction(C context);
		void eachAction(C context);
		void postAction(C context);
		
		String getType();
	}

	interface State<C> {
		String getName();
		
		List<Action<C>> getActions();
		int getModifier();
		
		void preAction(C context);
		void eachAction(C context);
		void postAction(C context);
	}
	
	interface Executor<C> {
		/*
			return this executor
		 */
		Executor<C> start();
		boolean next();
		
		Collection<Cursor<C>> getCursors();
	}
	
	interface Cursor<C> {
		/*
			return true if this cursor is terminated
		*/
		boolean next(ContextProvider<C> provider);

		Predicate<C> getLastValidPredicate();
		ITimedAutomata<C> getAutomata();
	}
	
	interface ContextProvider<C> {
		C getContext();
	}
	
	class ActionAdapter<C> implements Action<C> {
		public String getType() {
			return getClass().getCanonicalName();
		}

		@Override
		public void preAction(C context) {
		}

		@Override
		public void eachAction(C context) {
		}

		@Override
		public void postAction(C context) {
		}
		
		public String toString() {
			return getType();
		}
	}

	class PredicateAdapter<C> implements Predicate<C> {
		public boolean isValid(C context) {
			return false;
		}
	
		@Override
		public String getType() {
			return getClass().getCanonicalName();
		}
		
		public String toString() {
			return getType();
		}
	}
}
