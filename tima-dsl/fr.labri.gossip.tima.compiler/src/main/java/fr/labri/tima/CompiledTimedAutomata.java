package fr.labri.tima;

import fr.labri.Utils;

public class CompiledTimedAutomata<C> implements ITimedAutomata<C> {
	final State<C>[] _states;
	final Predicate<C>[] _predicates;
	
	int _initial;
	
	final int[][] _transitionsPredicates;
	final int[][] _transitionsTarget;
	final int[] _timeouts;
	final int[] _timeoutsTarget;
	
	public CompiledTimedAutomata(State<C>[] states, Predicate<C>[] predicates, int initial, int[][] transitionsPredicates, int[] timeouts, int[][] transitionsTarget, int[] timeoutsTarget) {
		_states = states;
		_predicates = predicates;
		_transitionsPredicates = transitionsPredicates;
		_timeouts = timeouts;
		_transitionsTarget = transitionsTarget;
		_timeoutsTarget = timeoutsTarget;
		
		_initial = initial;
		
		int l = states.length ;
		if(l != transitionsPredicates.length || l != timeouts.length || l != transitionsTarget.length || l != timeoutsTarget.length)
			throw new RuntimeException("Automaton is not well formed !");
	}
	
	public CompiledTimedAutomata(State<C>[] states, Predicate<C>[] predicates, Action<C> initial, int[][] transitionsPredicates, int[] timeouts, int[][] transitionsTarget, int[] timeoutsTarget) {
		this(states, predicates, Utils.indexOf(initial, states), transitionsPredicates, timeouts, transitionsTarget, timeoutsTarget);
	}
	
	@Override
	public Cursor<C> start(final ContextProvider<C> context) {
		return new Cursor<C>() {
			int _current = _initial;
			int _currentTimeout = _timeouts[_initial];
            int _lastPredicate;
            int _lastState;


			@Override
			final public boolean next(ContextProvider<C> provider) {
				boolean urgent = false, terminal = false; 
				do {
					int current = _current;
					int target = -1;
					C ctx = context.getContext();

					if(_currentTimeout > 0 && -- _currentTimeout == 0) 
						target = _timeoutsTarget[current];
					else {
						int[] trans = _transitionsPredicates[current];
						int len = trans.length;
						for(int i = 0; i < len; i ++)
							if(_predicates[trans[i]].isValid(ctx)) {
                                _lastPredicate = i;
                                _lastState = current;
								target = _transitionsTarget[current][i];
								break;
							}
					}
					if(target == -1) {
						_states[current].eachAction(ctx);
					} else {
						State<C> state = setState(target, provider, ctx);
						urgent = (state.getModifier() & URGENT) > 0;
						terminal = (state.getModifier() & TERMINATE) > 0;
					}
				} while(urgent);
				return terminal;
			}

            @Override
            public Predicate<C> getLastValidPredicate() {
                return _predicates[_transitionsPredicates[_lastState][_lastPredicate]];
            }

            final private State<C> setState(int target, ContextProvider<C> provider, C context) {
				_states[_current].postAction(context);
				_current = target;
				_currentTimeout = _timeouts[target];
				State<C> newState = _states[target];
				newState.preAction(context);
				return newState;
			}

			@Override
			public ITimedAutomata<C> getAutomata() {
				return CompiledTimedAutomata.this;
			}
		};
	}
	
	@Override
	final public State<C> getInitialState() {
		return _states[_initial];
	}
	
	@Override
	final public void setInitialState(State<C> initial) {
		_initial = Utils.indexOf(initial, _states);
	}

	@Override
	final public State<C>[] getStates() {
		return _states;
	}
	
	@Override
	public State<C>[] getFollowers(State<C> src) {
		int id = Utils.indexOf(src, _states);
		int[] line = _transitionsTarget[id];
		int l = line.length;
		int size = ((_timeoutsTarget[id] == -1) ? 0 : 1) + l;
		@SuppressWarnings("unchecked")
		State<C>[] states = new State[size];
		for(int i = 0; i < l; i++) {
			int t = line[i];
			states[i] = _states[t];
		}
		
		if(l != size)
			states[l] = _states[_timeoutsTarget[id]];
		
		return states;
	}

	@Override
	public int getTimeout(State<C> src, State<C> dst) {
		int idSrc = Utils.indexOf(src, _states);
		int idDst = Utils.indexOf(dst, _states);

		if(_timeoutsTarget[idSrc] == idDst)
			return TimedAutomata.TIMEOUT;
		else
			return _timeouts[idSrc];
	}
	
	/**
	 * In a compiled automata, every states must have a single timeout transition and non-timeout transitions must have the same expiration value.
	 * In other words, the timeout belongs now to the node  
	 * @return
	 */
	public int getStateTimeOut(State<C> s) {
		int idSrc = Utils.indexOf(s, _states);
		return _timeouts[idSrc];
	}
	
	/**
	 * In a compiled automata, every states must have a single timeout transition and non-timeout transitions must have the same expiration value.
	 * Hence, this method returns the destination
	 * @return
	 */
	public int getTimeoutDestination(State<C> s) {
		int idSrc = Utils.indexOf(s, _states);
		return _timeoutsTarget[idSrc];
	}

	@Override
	public Predicate<C> getPredicate(State<C> src, State<C> dst) {
		int idSrc = Utils.indexOf(src, _states);
		int idDst = Utils.indexOf(dst, _states);
		int[] line = _transitionsTarget[idSrc];
		for(int i = 0; i < line.length; i ++)
			if(line[i] == idDst)
				return _predicates[_transitionsPredicates[idSrc][i]];
		
		return null;
	}
	
	public String toString() {
		return DotRenderer.toDot(this);
	}

	@Override
	public ITimedAutomata<C> compile() {
		return this;
	}
}
