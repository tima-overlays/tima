package fr.labri;

public class AutoQualifiedClassLoader extends ClassLoader {
	final String _prefix;

	public AutoQualifiedClassLoader(String prefix) {
		this(prefix, getSystemClassLoader());
	}

	public AutoQualifiedClassLoader(String prefix, ClassLoader parent) {
		super(parent);
		_prefix = prefix.endsWith(".") ? prefix : prefix + ".";
	}

	@Override
	protected Class<?> findClass(String name) throws ClassNotFoundException {
		ClassLoader parent = getParent();
		try {
			Class<?> clazz = parent.loadClass(_prefix + name);
			return clazz;
		} catch (ClassNotFoundException e) {
			return parent.loadClass(name);
		}
	}
}
