package fr.labri.tima;

import java.io.IOException;
import java.util.List;

import org.jdom2.JDOMException;

import fr.labri.AutoQualifiedClassLoader;
import fr.labri.DotViewer;
import fr.labri.tima.ITimedAutomata.Action;
import fr.labri.tima.ITimedAutomata.NodeFactory;
import fr.labri.tima.ITimedAutomata.Predicate;

public class TimaTester {
	final private AutoQualifiedClassLoader _classLoader;
	private boolean _render;

	final public static boolean RENDER = Boolean.parseBoolean(System.getProperty("tima.test.render", "true"));
	final public static boolean COMPILED = Boolean.parseBoolean(System.getProperty("tima.test.compile", "false"));
	final public static boolean VERBOSE = Boolean.parseBoolean(System.getProperty("tima.test.verbose", "true"));
	
	public TimaTester(String namespace) {
		this(namespace, RENDER);
	}
	
	public TimaTester(String namespace, boolean render) {
		_classLoader = new AutoQualifiedClassLoader(namespace);
		_render = render;
	}
	
	void setRender(boolean render) {
		_render = render;
	}

	<C>void test(String name, String namespace, Class<C> dummy) throws JDOMException, IOException {
		String fname = "/" + name.replaceAll("\\.", "/") + ".xml";
		List<ITimedAutomata<C>> b = new TimedAutomataFactory<>(getSimpleNodeBuilder(namespace, dummy)).loadXML(
				getClass().getResourceAsStream(fname)
		);
		
		//ITimedAutomata<Object> auto = COMPILED ? b.compile() : b;
		String dot = DotRenderer.toDot(b, name.substring(name.lastIndexOf(".") + 1));
		if(_render)
			DotViewer.view(dot);
		else
			System.out.println(dot);
	}
	
	<C> NodeFactory<C> getSimpleNodeBuilder(final String namespace, Class<C> dummy) {
		final NodeFactory<C> factory = new ReflectNodeFactory<>(new AutoQualifiedClassLoader(namespace, _classLoader));
		return new SimpleNodeFactory<C>() {
			public Predicate<C> newPredicate(String type, String attr) {
				if(factory.newPredicate(type, attr) == null) error(type);  else ok(type);
				return super.newPredicate(type, attr);
			}

			@Override
			public Action<C> newAction(String type, String attr) {
				if(factory.newAction(type, attr) == null) error(type); else ok(type);
				return super.newAction(type, attr);
			}

			private void ok(String name) {
				if(VERBOSE) System.out.printf("Class '%s' loaded\n", name);
			}
			
			private void error(String name) {
				System.err.printf("Class '%s' not found in '%s'\n", name, namespace);
			}
		};
	}
}
