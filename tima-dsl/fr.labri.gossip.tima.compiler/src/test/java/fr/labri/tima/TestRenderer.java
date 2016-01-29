package fr.labri.tima;

import fr.labri.Utils;
import org.jdom2.JDOMException;
import org.junit.Test;

import java.io.*;
import java.util.List;

import static org.junit.Assert.*;

public class TestRenderer {

    @Test
    public void testEx1() throws Exception {
        assertDotRepresentation("/ex1.dot", "/ex1.xml", 1);
    }
    @Test
    public void testEx2() throws Exception {
        assertDotRepresentation("/ex2.dot", "/ex2.xml", 1);
    }

    private void assertDotRepresentation(String expectedFile, String actualFile, int numberOfAutomatas) throws IOException, JDOMException {
        InputStream stream = TestUtils.getInputStream(actualFile);
        String expectedDot = Utils.readStream(TestUtils.getInputStream(expectedFile));

        TimedAutomataFactory loader = new TimedAutomataFactory(new SimpleNodeFactory());
        List<TimedAutomata<Void>> automatas = loader.loadXML(stream, false); // FIXME why validate true do not work
        assertEquals(numberOfAutomatas, automatas.size());

        String actualDot = DotRenderer.toDot(automatas, "test");
        assertEquals(expectedDot, actualDot);
    }
}