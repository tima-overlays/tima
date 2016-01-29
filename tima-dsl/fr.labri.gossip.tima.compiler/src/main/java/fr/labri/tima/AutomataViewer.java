package fr.labri.tima;

import java.awt.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;

import javax.swing.*;

import edu.uci.ics.jung.algorithms.layout.FRLayout;
import edu.uci.ics.jung.algorithms.layout.Layout;
import edu.uci.ics.jung.graph.DirectedGraph;
import edu.uci.ics.jung.graph.DirectedSparseGraph;
import edu.uci.ics.jung.visualization.VisualizationViewer;
import fr.labri.tima.ITimedAutomata.Cursor;
import fr.labri.tima.ITimedAutomata.Executor;
import fr.labri.tima.ITimedAutomata.Predicate;
import fr.labri.tima.ITimedAutomata.State;
import org.apache.commons.collections15.Transformer;

public class AutomataViewer {
	private static final int GAP = 50;

	private static JFrame createFrame(JComponent panel) {
		JFrame frame = new JFrame("Simple Graph View");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.getContentPane().add(panel);
		frame.pack();
		frame.setVisible(true);
		return frame;
	}

	public static JFrame viewAsFrame(ITimedAutomata<?> automata) {
		return createFrame(createPanel(automata));
	}

	public static <C> JFrame viewAsFrame(Collection<? extends ITimedAutomata<C>> autos) {
		return createFrame(createPanel(autos));
	}

	public static <C> VisualizationViewer<State<C>, Predicate<C>> createPanel(Executor<C> executor) {
		return createPanel(executor, new Dimension(300, 300));
	}

	public static <C> VisualizationViewer<State<C>, Predicate<C>> createPanel(ITimedAutomata<C> auto) {
		return createPanel(auto, new Dimension(300, 300));
	}

	public static <C> VisualizationViewer<State<C>, Predicate<C>> createPanel(Collection<? extends ITimedAutomata<C>> autos) {
		return createPanel(autos, new Dimension(300, 300));
	}

	public static <C> VisualizationViewer<State<C>, Predicate<C>> createPanel(ITimedAutomata<C> auto, Dimension dimension) {
		return createPanel(dimension, createGraph(auto));
	}

	public static <C> VisualizationViewer<State<C>, Predicate<C>> createPanel(Executor<C> executor, Dimension dimension) {
		ArrayList<ITimedAutomata<C>> lst = new ArrayList<>();
		for(Cursor<C> cursor: executor.getCursors())
			lst.add(cursor.getAutomata());
		return createPanel(dimension, createGraph(lst));
	}

	public static <C> VisualizationViewer<State<C>, Predicate<C>> createPanel(Collection<? extends ITimedAutomata<C>> autos, Dimension dimension) {
		return createPanel(dimension, createGraph(autos));
	}
	
	public static <C> VisualizationViewer<State<C>, Predicate<C>> createPanel(Dimension dimension, AutomataGraph<C> g) {
		Layout<State<C>, Predicate<C>> layout = new FRLayout<>(g, dimension);
		VisualizationViewer<State<C>, Predicate<C>> vv = new VisualizationViewer<State<C>, Predicate<C>>(
				layout);
		vv.setAutoscrolls(true);
		vv.setPreferredSize(new Dimension(dimension.width + GAP, dimension.height + GAP)); // Sets the viewing area size

        vv.getRenderContext().setVertexFillPaintTransformer(state -> g.getActiveStates().contains(state) ?  Color.GREEN :
                g.getInitialStates().contains(state) ? Color.YELLOW : Color.RED);
        vv.getRenderContext().setVertexLabelTransformer(state -> state.getName());

        vv.getRenderContext().setEdgeLabelTransformer(pred -> pred instanceof DefaultTransition ? null : pred.getType());

        final Stroke plainLines = new BasicStroke(1.0f);
        final Stroke dashedLines = new BasicStroke(1.0f, BasicStroke.CAP_BUTT,
                BasicStroke.JOIN_MITER, 10.0f, new float[]{10.0f}, 0.0f);

        vv.getRenderContext().setEdgeStrokeTransformer(pred -> pred instanceof DefaultTransition ? dashedLines : plainLines);

        return vv;
	}

    interface AutomataGraph<C> extends DirectedGraph<State<C>, Predicate<C>> {
        Collection<? extends ITimedAutomata<C>> getAutomatas();
        Collection<? extends State<C>> getInitialStates();
        Collection<? extends State<C>> getActiveStates();
    }

	public static <C> AutomataGraph<C> createGraph(final Collection<? extends ITimedAutomata<C>> autos) {

        AutomataGraph<C> graph = new SimpleAutomataGraph<C>(autos);

		for(ITimedAutomata<C> auto: autos)
			createGraph(auto, graph);
		return graph;
	}

	public static <C> AutomataGraph<C> createGraph(ITimedAutomata<C> autos[]) {
		return createGraph(Arrays.asList(autos));
	}

	public static <C> AutomataGraph<C> createGraph(ITimedAutomata<C> auto) {

        AutomataGraph<C> graph = new SimpleAutomataGraph<C>(auto);
        createGraph(auto, graph);
		return graph;
	}

    private static class SimpleAutomataGraph<C> extends DirectedSparseGraph<State<C>, Predicate<C>> implements AutomataGraph<C> {
        final ArrayList<ITimedAutomata<C>> autos = new ArrayList<>();
        final ArrayList<State<C>> initialStates = new ArrayList<>();
        final ArrayList<State<C>> activeStates = new ArrayList<>();

        public SimpleAutomataGraph(ITimedAutomata<C> auto) {
            autos.add(auto);
            initialStates.add(auto.getInitialState());
        }

        public SimpleAutomataGraph(Collection<? extends ITimedAutomata<C>> autos) {
            this.autos.addAll(autos);
            for (ITimedAutomata<C> auto : autos) {
                initialStates.add(auto.getInitialState());
            }
        }

        @Override
        public Collection<? extends ITimedAutomata<C>> getAutomatas() {
            return autos;
        }

        @Override
        public Collection<? extends State<C>> getInitialStates() {
            return initialStates;
        }

        @Override
        public Collection<? extends State<C>> getActiveStates() {
            return activeStates;
        }
    }
	
	private static <C> AutomataGraph<C> createGraph(ITimedAutomata<C> auto, AutomataGraph<C> sgv) {
		for (State<C> state : auto.getStates()) {
            sgv.addVertex(state);
        }

		for (State<C> state : auto.getStates())
			for (State<C> dst : auto.getFollowers(state)) {
				final Predicate<C> pred = auto.getPredicate(state, dst);
				int timeout = auto.getTimeout(state, dst);
				if(timeout != TimedAutomata.TIMEOUT)
					sgv.addEdge(new Predicate<C>() {
						@Override
						public boolean isValid(C context) {
							return pred.isValid(context);
						}

						@Override
						public String getType() {
							return pred.getType();
						}
					}, state, dst);
				else
					sgv.addEdge(new DefaultTransition<C>(pred), state, dst);

			}
		return sgv;
	}

	private static class DefaultTransition<C> implements Predicate<C> {
		final Predicate<C> _pred;
		public DefaultTransition(Predicate<C> pred) {
			_pred = pred;
		}
		@Override
		public boolean isValid(C context) {
			return _pred.isValid(context);
		}

		@Override
		public String getType() {
			return _pred.getType();
		}
	}
}
