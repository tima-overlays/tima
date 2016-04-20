package fr.labri.gossip.tima.ir;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import fr.labri.gossip.tima.ir.IRAutomata;

public class IRAutomata {
	
	public Map<String, Message> messages = new HashMap<>();
	public Map<String, Automaton> automata = new HashMap<>();
	
	public interface Action {}
	public interface Guard {}
	public interface MessageTarget {
		public static class Broadcast implements MessageTarget {
		}
		
		public static class Unicast implements MessageTarget {
			String target;
			public Unicast(String t) {
				target = t;
			}
		}
		public static class Internal implements MessageTarget {
			Automaton target;
			public Internal(Automaton t) {
				target = t;
			}
		}

	}

	public static class Automaton {
		String name;
		
		public Automaton(String name) {
			this.name = name;
		}
		
		Node entryPoint;
		List<Node> nodes;

		public void add(Node node) {
			nodes.add(node);
		}
	}
	
	public static abstract class Node {
		public int timeout;
		public Node timeoutTarget;
		public List<Transition> transtions;
		
		public List<Action> actions;

		abstract public NamedNode getReferenceNode() ;

		public void addTransition(Transition transition) {
			transtions.add(transition);
		}
	}
	
	public static class NamedNode extends Node {
		String name;
		public NamedNode(String name) {
			this.name = name;
		}

		@Override
		public NamedNode getReferenceNode() {
			return this;
		}
	}
	
	public static class DerivedNode extends Node {
		NamedNode reference;
		
		public DerivedNode(Node node) {
			reference = node.getReferenceNode();
		}
		
		@Override
		public NamedNode getReferenceNode() {
			return reference;
		}
	}
	
	public static class Transition {
		Node target;
		Guard guard;
		List<Action> actions;
		
		public Transition(Node t, Guard g, List<Action> acts) {
			target = t;
			guard = g;
			actions = acts;
		}
	}
	
	public static class Message {
		String name;
		List<String> fields;
		public Message(String n, List<String> f) {
			name = n;
			fields = f;
		}
	}
	
	public static class RemoteMessage extends Message {
		Map<String, String> options;
		public RemoteMessage(String name, List<String> fields, Map<String, String> opts) {
			super(name, fields);
			options = opts;
		}
	}
	
	public static class Pattern {
		String operator;
		List<Operand> operands;
		
		public Pattern(String op, List<Operand> ops) {
			operator = op;
			operands = ops;
		}
	}
	
	public static class Operand {}
	public static class MessageFieldOperand extends Operand {
		String field;
		public MessageFieldOperand(String field) {
			this.field = field;
		}
	}
	public static class StringOperand extends Operand {
		String value;
		public StringOperand(String value) {
			this.value = value;
		}
	}
	
	abstract static class BuiltinElement {
		String builtinName;
		Map<String, String> arguments;
		
		protected BuiltinElement(String name, Map<String, String> args) {
			builtinName = name;
			arguments = args;
		}
	}
	abstract static private class ExternalElement {
		public ExternalElement(String name) {
			externalName = name;
		}

		String externalName;
	}
	public static class BuiltinGuard extends BuiltinElement implements Guard {
		public BuiltinGuard(String name, Map<String, String> args) {
			super(name, args);
		}
	}
	public static class BuiltinAction extends BuiltinElement implements Action {
		public BuiltinAction(String name, Map<String, String> args) {
			super(name, args);
		}
	}
	public static class ExternalGuard extends ExternalElement implements Guard {
		public ExternalGuard(String name) {
			super(name);
		}
	}
	public static class ExternalAction extends ExternalElement implements Action {
		public ExternalAction(String name) {
			super(name);
		}
	}
	public static class MessageAction implements Action {
		MessageTarget target;
		Message message;
		Map<String, String> arguments;
		
		public MessageAction(MessageTarget t, Message msg, Map<String, String> args) {
			target = t;
			message = msg;
			arguments = args;
		}
	}
	public static class MessageGuard implements Guard {
		Message message;
		List<Pattern> arguments;
		
		public MessageGuard(Message msg, List<Pattern> args) {
			message = msg;
			arguments = args;
		}
	}
	
	public void add(Automaton auto) {
		automata.put(auto.name, auto);
	}
}
