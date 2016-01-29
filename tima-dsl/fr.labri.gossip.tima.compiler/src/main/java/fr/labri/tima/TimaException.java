package fr.labri.tima;

public class TimaException extends RuntimeException {
    public static class InitialStateException extends RuntimeException {
        public InitialStateException(String s) {
            super(s);
        }
    }
}
