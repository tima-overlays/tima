package fr.labri.tima;

import fr.labri.AutoQualifiedClassLoader;

import java.lang.reflect.InvocationTargetException;

/**
 * Created by morandat on 23/12/2015.
 */
public class ReflectNodeFactory<C> implements ITimedAutomata.NodeFactory<C> {

    final ClassLoader _loader;

    public ReflectNodeFactory() {
        this(ReflectNodeFactory.class.getClassLoader());
    }
    public ReflectNodeFactory(String searchPrefix) {
        this(new AutoQualifiedClassLoader(searchPrefix));
    }

    public ReflectNodeFactory(ClassLoader loader) {
        _loader = loader;
    }

    @Override
    public ITimedAutomata.Action<C> newAction(String type, String attr) {
        return newInstance(type, attr);
    }

    @Override
    public ITimedAutomata.Predicate<C> newPredicate(String type, String attr) {
        return newInstance(type, attr);
    }

    @SuppressWarnings("unchecked")
    private <T> T newInstance(String type, String attr) {
        try {
            Class<?> clz = _loader.loadClass(type);
            T state = null;
            if(type == null)
                return null;
            try {
                state = (T) clz.getConstructor(String.class).newInstance(attr);
            } catch (NoSuchMethodException e) {
                state = (T) clz.getConstructor().newInstance();
            }
            return state;
        } catch (NoSuchMethodException | SecurityException
                | ClassNotFoundException | InstantiationException
                | IllegalAccessException | IllegalArgumentException | InvocationTargetException e) {
//					e.printStackTrace();
        }
        return null;
    }
}
