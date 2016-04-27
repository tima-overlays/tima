package fr.labri.gossip.tima.generator;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Map;
import fr.labri.gossip.tima.ir.IRAutomata;

public class Generators {
	@SuppressWarnings("serial")
	public static final Map<String, Class<? extends AutomataGenerator>> generators =
			new HashMap<String, Class<? extends AutomataGenerator>>() {{
    	put("dot", DotGenerator.class);
    	put("omnet", OmnetGenerator.class);
    	put("c++", NativeGenerator.class);
	}};

	public static AutomataGenerator getGenerator(String name, IRAutomata automata) throws NoSuchMethodException, SecurityException, InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
		Class<? extends AutomataGenerator> clazz = generators.get(name);
		if (clazz == null)
			throw new UnsupportedOperationException("I do not know this generator "+ name);
		Constructor<? extends AutomataGenerator> ctor = clazz.getConstructor(IRAutomata.class);
		return ctor.newInstance(automata);
	}
}
