package fr.labri.tima;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.URL;
import java.util.Collection;

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

	public static String serialize(Collection<String> collection) {
		StringBuffer buffer = new StringBuffer();
		boolean first = true;
		
		for(String value: collection) {
			if (first) {
				first = false;
			} else {
				buffer.append(", ");
			}
			buffer.append(value);
		}
		return buffer.toString();
	}
	
	public static <C extends Collection<String>> C unserialize(String serialized, C collection) {
		collection.clear();
		if (serialized == null) return collection;
		for(String s: serialized.split(", "))
			collection.add(s);
		return collection;
	}

	public static final long nthBit(long number, int nth) {
	    while(nth-- > 0) {
	        number &= ~(number & -number);
	    }
	    return number &= -number;
	}
	
	public static final long nthLowerBit(long number, int nth) {
	    long res = 0;
	    while(nth-- > 0) {
	        long out = number & -number;
	        res |= out;
	        number &= ~(out);
	    }
	    return res;
	}
	
	public static <T> int indexOf(T needle, T[] haystack) {
	    for (int i=0; i<haystack.length; i++) {
	        if (haystack[i] != null && haystack[i].equals(needle)
	            || needle == null && haystack[i] == null) return i;
	    }
	    return -1;
	}

	public static String readStream(InputStream stream) throws IOException {
		BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
		StringBuilder out = new StringBuilder();
		String newLine = System.getProperty("line.separator");
		String line;
		while ((line = reader.readLine()) != null) {
			out.append(line);
			out.append(newLine);
		}
		return out.toString();
	}

}
