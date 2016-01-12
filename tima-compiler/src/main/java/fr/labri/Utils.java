package fr.labri;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class Utils {
	public static final void debug(Object recv, Object... strs) {
		StringBuilder builder = new StringBuilder("[").append(recv.getClass())
				.append("]\t");
		for(Object str: strs)
			builder.append(str);
		System.out.println(builder);
	}

	public static final void pause(long wait) {
		try {
			Thread.sleep(wait);
		} catch (InterruptedException e) {
		}
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
