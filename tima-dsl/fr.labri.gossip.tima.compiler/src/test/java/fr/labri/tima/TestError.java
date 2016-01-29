package fr.labri.tima;

import org.junit.Test;

public class TestError {

    @Test(expected = TimaException.InitialStateException.class)
    public void testIntialState() throws Exception {
        new TimedAutomataFactory(new SimpleNodeFactory<>()).loadXML(TestUtils.getInputStream("/errors/intial.xml"));
    }
}
