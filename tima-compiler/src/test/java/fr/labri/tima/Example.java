package fr.labri.tima;

import org.jdom2.JDOMException;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Collection;

public class Example {

    static class CharPredicate implements ITimedAutomata.Predicate<Character> {
        final char letter;

        CharPredicate(char c) {
            letter = c;
        }

        @Override
        public boolean isValid(Character c) {
            return c == letter;
        }

        @Override
        public String getType() {
            return Character.toString(letter);
        }
    }

    static class PrintAction implements ITimedAutomata.Action<Character> {
        final String _name;

        PrintAction(String name) {
            _name = name;
        }

        @Override
        public void preAction(Character c) {
            System.out.println("enter " + _name + " on " + c);
        }

        @Override
        public void eachAction(Character c) {
            System.out.println("each " + _name + " on " + c);
        }

        @Override
        public void postAction(Character c) {
            System.out.println("leave " + _name + " on " + c);
        }

        @Override
        public String getType() {
            return _name;
        }
    }

    public static void main(String[] args) throws IOException, JDOMException {
        TimedAutomataFactory<Character> factory = new TimedAutomataFactory<Character>(new ITimedAutomata.NodeFactory<Character>() {
            @Override
            public ITimedAutomata.Predicate<Character> newPredicate(String type, String attr) {
                return new CharPredicate(type.charAt(0));
            }

            @Override
            public ITimedAutomata.Action<Character> newAction(String type, String attr) {
                return new PrintAction(type);
            }
        });

        Collection<ITimedAutomata<Character>> autos = factory.loadXML(new FileInputStream("hello_world.xml"));
        StringIterator it = new StringIterator("Hello World!");
        recognizeString(it, autos);
    }

    static class StringIterator implements ITimedAutomata.ContextProvider<Character> {
        final String _word;
        int _pos = 0;

        StringIterator(String str) {
            _word = str;
        }

        @Override
        public Character getContext() {
            return _word.charAt(_pos);
        }
    }

    static ITimedAutomata.Executor<Character> recognizeString(ITimedAutomata.ContextProvider<Character> provider, Collection<ITimedAutomata<Character>> autos) {
        return new BasicExecutor<>(provider, autos);
    }
}
