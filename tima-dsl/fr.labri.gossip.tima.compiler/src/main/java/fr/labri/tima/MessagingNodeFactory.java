package fr.labri.tima;

import fr.labri.Pair;
import fr.labri.tima.ITimedAutomata.*;
import fr.labri.tima.MessagingNodeFactory.MessageExecutor;

import java.util.*;
import java.util.concurrent.ConcurrentSearchableLinkedDeque;
import java.util.function.Function;

public class MessagingNodeFactory<C> implements ITimedAutomata.NodeFactory<MessageExecutor<C>.MessageContext> {
    final NodeFactory<C> _factory;

    MessagingNodeFactory(NodeFactory<C> factory) {
        _factory = factory;
    }

    @Override
    public Predicate<MessageExecutor<C>.MessageContext> newPredicate(String type, String attr) {
        if (type.startsWith("?"))
            return filterMessageFactory(type, attr);
        final Predicate<C> pred = _factory.newPredicate(type, attr);

        return new Predicate<MessageExecutor<C>.MessageContext>() {
            @Override
            public boolean isValid(MessageExecutor<C>.MessageContext context) {
                return pred.isValid(context.getContext());
            }

            @Override
            public String getType() {
                return pred.getType();
            }
        };
    }

    private Predicate<MessageExecutor<C>.MessageContext> filterMessageFactory(String type, String attr) {
        return new MessageFilter();
    }

    @Override
    public Action<MessageExecutor<C>.MessageContext> newAction(String type, String attr) {
        if (type.startsWith("!"))
            return sendMessageFactory(type, attr);
        final Action<C> action = _factory.newAction(type, attr);
        return new Action<MessageExecutor<C>.MessageContext>() {
            @Override
            public void preAction(MessageExecutor<C>.MessageContext context) {
                action.preAction(context.getContext());
            }

            @Override
            public void eachAction(MessageExecutor<C>.MessageContext context) {
                action.eachAction(context.getContext());
            }

            @Override
            public void postAction(MessageExecutor<C>.MessageContext context) {
                action.postAction(context.getContext());
            }

            @Override
            public String getType() {
                return action.getType();
            }
        };
    }

    private Action<MessageExecutor<C>.MessageContext> sendMessageFactory(String type, String attr) {
        return new SendMessage("someautomata", new Message("hello"));
    }

    class MessageFilter extends ITimedAutomata.PredicateAdapter<MessageExecutor<C>.MessageContext>{
        @Override
        public boolean isValid(MessageExecutor<C>.MessageContext context) {
            return context.select(new MessagePattern() {
                @Override
                public boolean match(Message other) {
                    return other._type == "hello";
                }
            });
        }
    }

    class SendMessage extends ITimedAutomata.ActionAdapter<MessageExecutor<C>.MessageContext> {
        private final Message _prototype;
        final String _automaton;
        int address;

        final Pair<String, Function<Integer, Integer>> _transformations[];

        public SendMessage(String to, Message prototype) {
            _automaton = to;
            _prototype = prototype;
            _transformations = null;
        }

        public SendMessage(String to, Message prototype, Pair<String, Function<Integer, Integer>>... transformations) {
            _automaton = to;
            _prototype = prototype;
            _transformations = transformations;
        }

        public SendMessage(String to, Message prototype, Map<String, Function<Integer, Integer>> transformations) {
            _automaton = to;
            _prototype = prototype;
            _transformations = new Pair[transformations.size()];
            int i = 0;
            for (Map.Entry<String, Function<Integer, Integer>> trans : transformations.entrySet()) {
                _transformations[i++] = Pair.of(trans.getKey(), trans.getValue());
            }
        }

        int resolveAddress(MessageExecutor<C>.MessageContext context) {
            if (address == -1)
                address = 1;
            return address;
        }

        @Override
        public void preAction(MessageExecutor<C>.MessageContext context) {
            Message m = new Message(_prototype);
            Message other = context.currentMessage();
            for (int i = 0; i < _transformations.length; i++) {
                Pair<String, Function<Integer, Integer>> transformation = _transformations[i];
                m.merge(other, transformation.snd, transformation.fst);
            }
            context.sendMessage(resolveAddress(context), m);
        }
    }

    static class MessageExecutor<C> implements Executor<MessageExecutor<C>.MessageContext> {
        private final ContextProvider<C> _context;
        private final Collection<ITimedAutomata<MessageContext>> _automatons;
        Cursor<MessageContext> _cursors[];
        Mailbox _mailboxes[];
        private MessageContextProvider[] _contexts;

        @SuppressWarnings("unchecked")
        MessageExecutor(ContextProvider<C> context, Collection<ITimedAutomata<C>> automatas) {
            _context = context;
            // FIXME this cast scares me ... but actually not I will one day explain why
            _automatons = (Collection<ITimedAutomata<MessageContext>>)(Collection<?>) automatas;
        }

        @Override
        @SuppressWarnings("unchecked")
        public Executor<MessageExecutor<C>.MessageContext> start() {
            _mailboxes = new Mailbox[_automatons.size()];
            _cursors = new Cursor[_automatons.size()];
            _contexts = new MessageExecutor.MessageContextProvider[_automatons.size()];

            int i = 0;
            for (ITimedAutomata<MessageExecutor<C>.MessageContext> automata : _automatons) {
                _mailboxes[i] = new Mailbox();
                _contexts[i] = new MessageContextProvider(i);
                _cursors[i] = automata.start(null);//new MessageContextProvider<C>.MessageContext(_context, this)); // FIXME
                i ++;
            }
            return this;
        }

        @Override
        public boolean next() {
            int running = 0;
            for (int i = 0; i < _cursors.length; i++) {
                Cursor<MessageContext> cursor = _cursors[i];
                if (cursor != null && !cursor.next(_contexts[i]))
                    running ++;
            }
            return running > 0;
        }

        @Override
        public Collection<Cursor<MessageContext>> getCursors() {
            return Collections.unmodifiableCollection(Arrays.asList(_cursors)); // should trim to alive
        }

        class MessageContextProvider implements ContextProvider<MessageExecutor<C>.MessageContext> {
            final int _automataIndex;

            MessageContextProvider(int index) {
                _automataIndex = index;
            }

            @Override
            public MessageExecutor<C>.MessageContext getContext() {
                return _contexts[_automataIndex].getContext();
            }
        }

        class MessageContext {
            final int _automataIndex;
            private Message _currentMessage;

            MessageContext(int index) {
                _automataIndex = index;
            }

            C getContext() {
                return _context.getContext();
            }

            public void sendMessage(int address, Message msg) {
                _mailboxes[address].receive(msg);
            }

            public Message currentMessage() {
                return _currentMessage;
            }

            final public boolean select(MessagePattern pattern) {
                return select(pattern, false);
            }

            public boolean select(MessagePattern pattern, boolean first) {
                Message m = mailbox().extractFrom(pattern, first);
                if (m == null)
                    return false;
                _currentMessage = m;
                return true;
            }

            final private Mailbox mailbox() {
                return _mailboxes[_automataIndex];
            }
        }
    }

    static class Message {
        final String _type;
        final Map<String, Integer> _data;

        Message(String type) {
            this(type, new HashMap<>());
        }

        Message(Message other) {
            this(other._type, other);
        }

        Message(String type, Message other) {
            this(type, new HashMap<>(other.data()));
        }

        private Message(String type, Map<String, Integer> data) {
            _type = type.intern();
            _data = data;
        }

        public Integer get(String key) {
            return _data.get(key);
        }

        public Map<String, Integer> data() {
            return Collections.unmodifiableMap(_data);
        }

        public Message merge(Message other, Function<Integer, Integer> transform, String key) {
            _data.put(key, transform.apply(other.get(key)));
            return this;
        }

        public Message merge(Message other, Function<Integer, Integer> transform, String... keys) {
            for (String key: keys) {
                _data.put(key, transform.apply(other.get(key)));
            }
            return this;
        }

        static class Factory {
            Message newSimpleMessage(String type, Map.Entry<String, Integer>... values) {
                HashMap data = new HashMap();
                for (Map.Entry<String, Integer> value: values) {
                    data.put(value.getKey(), value.getValue());
                }
                return new Message(type, data);
            }
        }
    }

    static class Mailbox {
        ConcurrentSearchableLinkedDeque<Message> queue = new ConcurrentSearchableLinkedDeque<>();

        public void receive(Message msg) {
            queue.addLast(msg);
        }

        public Message extractFrom(MessagePattern pattern, boolean first) {
            return first ? queue.removeFirstIf(pattern) : queue.removeFirstMatching(pattern);
        }
    }

    abstract static class MessagePattern {
        abstract public boolean match(Message other);

        @Override
        final public boolean equals(Object obj) {
            Message other = (Message) obj;
            if (other == null)
                return false;
            return match(other);
        }
    }
}