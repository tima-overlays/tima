package fr.labri;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Map;


public class DotViewer {
	public final static String DOTTY_PATH = System.getProperty("dotty.path", "/usr/local/bin/dotty");
	public final static String ENV_PATH = System.getProperty("system.path", "PATH");
	
	public static Process view(String dot) {
		try {
			ProcessBuilder pb = new ProcessBuilder(DOTTY_PATH, "-").redirectErrorStream(true);
			Map<String, String> env = pb.environment();
			String path = env.get(ENV_PATH);
			String dottyPath =  new File(DOTTY_PATH).getParent().toString();
			env.put(ENV_PATH, (path == null) ? dottyPath : path.concat(File.pathSeparator + dottyPath));
			Process p = pb.start();
			OutputStream s = p.getOutputStream();
			s.write(dot.getBytes());
			s.flush();
			s.close();
			return p;
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}
}
