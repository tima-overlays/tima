package fr.labri.gossip.tima;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;

import org.eclipse.core.resources.IFile;


public class Util {
	
	public static String unMangleString(String str) {
		 String result = str.substring(1,str.length()-1);
		 return result.replaceAll("\\\\", ""); // FIXME that not the right way of changing codepoints
	}
	
	public static void copy(URL src, File dst, boolean overwrite) {;
		if (!overwrite && dst.exists()) return;
		
		try {
			InputStream in = src.openStream();
			OutputStream out = new FileOutputStream(dst);
			copy(in, out);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static void copy(IFile src, File dst, boolean overwrite) {
		if (!overwrite && dst.exists()) return;

		File source = new File(src.getLocationURI());
		try {
			InputStream in = new FileInputStream(source);
			OutputStream out = new FileOutputStream(dst);
			copy(in, out);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static void copy(InputStream in, OutputStream out) throws IOException {
		byte[] buffer = new byte[1024];
		int len;
		while ((len = in.read(buffer)) != -1) {
			out.write(buffer, 0, len);
		}
		in.close();
		out.close();
	}
}
