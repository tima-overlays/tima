package java.util.concurrent;

/**
 * Created by morandat on 22/12/2015.
 */
public class ConcurrentSearchableLinkedDeque<E> extends ConcurrentLinkedDeque<E> {
    public E removeFirstMatching(Object o) {
        checkNotNull(o);
        for (Node<E> p = first(); p != null; p = succ(p)) {
            E item = p.item;
            if (item != null && o.equals(item) && p.casItem(item, null)) {
                unlink(p);
                return item;
            }
        }
        return null;
    }

    public E removeFirstIf(Object o) {
        checkNotNull(o);
        E item;
        do {
            Node<E> p = first();
            item = p.item;
            boolean equal = o.equals(item);
            if (item != null && !equal) {
                return null;
            } else if (item != null && p.casItem(item, null)) {
                unlink(p);
                return item;
            }
        } while (item != null);
        return null;
    }

    /**
     * Throws NullPointerException if argument is null.
     *
     * @param v the element
     */
    private static void checkNotNull(Object v) {
        if (v == null)
            throw new NullPointerException();
    }

}
