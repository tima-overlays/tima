package fr.labri.tima;

import com.sun.java.swing.SwingUtilities3;
import org.junit.Test;

import javax.swing.*;
import java.io.InputStream;
import java.util.List;

/**
 * Created by morandat on 30/10/2015.
 */
public class TestViewer {
    @Test
    public void testEx1() throws Exception {
        InputStream ex1 = TestUtils.getInputStream("/msg1.xml");
        List<ITimedAutomata<Object>> autos = new TimedAutomataFactory<>(new SimpleNodeFactory<>()).loadXML(ex1, false);

        JFrame frame = AutomataViewer.viewAsFrame(autos);

        Thread.currentThread().join();
    }
}
