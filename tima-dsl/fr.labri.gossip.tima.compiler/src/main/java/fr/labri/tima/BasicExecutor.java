package fr.labri.tima;

import java.util.*;

import javax.swing.AbstractListModel;
import javax.swing.JList;

import fr.labri.tima.ITimedAutomata.ContextProvider;
import fr.labri.tima.ITimedAutomata.Cursor;
import fr.labri.tima.ITimedAutomata.Executor;

public class BasicExecutor<C> implements Executor<C> {
	final Collection<ITimedAutomata<C>> _automatas;
 	final Cursor<C> _cursors[];
	final ContextProvider<C> _context;
	int _alive;

	TAViewer _viewer;

	public BasicExecutor(ContextProvider<C> context, Collection<ITimedAutomata<C>> automatas) {
		_context = context;
		_automatas = automatas;
		_cursors = new Cursor[automatas.size()];
		_alive = 0;
	}
	
	@Override
	public BasicExecutor<C> start() {
		for (ITimedAutomata<C> automata : _automatas) {
			_cursors[_alive ++] = automata.start(_context);
		}
		if(_viewer != null)
			_viewer.update();
		return this;
	}

	@Override
	public boolean next() {
		int alive = _alive;
		for (int i = 0; i < _alive;) {
			Cursor<C> cursor = _cursors[i];
			if (cursor.next(_context)) {
				_alive --;
				_cursors[i] = _cursors[alive];
			} else
				i ++;
		}
		if (alive != _alive)
			_viewer.update();
		return _alive > 0;
	}

	@Override
	public Collection<Cursor<C>> getCursors() {
		return Collections.unmodifiableCollection(Arrays.asList(_cursors)); // should trim to alive
	}
	
	public JList<String> getViewer() {
		_viewer = new TAViewer();
		return new JList<>(_viewer);
	}
	
	@SuppressWarnings("serial")
	class TAViewer extends AbstractListModel<String> {
        public int getSize() { return _alive; }
        public String getElementAt(int i) { return _cursors[i].toString(); }
        public void update() { fireContentsChanged(this, 0, _alive - 1); }
    }
}
