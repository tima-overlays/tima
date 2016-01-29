package fr.labri.tima;

import java.io.InputStream;

import static org.junit.Assert.fail;

/**
 * Created by morandat on 30/10/2015.
 */
public class TestUtils {
    public static InputStream getInputStream(String resource) {
        InputStream stream = TestRenderer.class.getResourceAsStream(resource);
        if (stream == null)
            fail("Resource do not exists: " + resource);
        return stream;
    }
}
