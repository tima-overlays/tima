package fr.labri.tima.ir;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import fr.labri.tima.ir.IRAutomata;

public class IRAutomata {
	
	public Map<String, Message> messages = new HashMap<>();
	public Map<String, Automaton> automata = new HashMap<>();
	
	public interface Action {}
	public interface Guard {}
	public interface MessageTarget {
		public static class Broadcast implements MessageTarget {
		}
		
		public static class Unicast implements MessageTarget {
			public Expression target;
			public Unicast(Expression t) {
				target = t;
			}
		}
		public static class Internal implements MessageTarget {
			public Automaton target;
			public Internal(Automaton t) {
				target = t;
			}
		}

	}
	
	public interface Expression {
		public static class BinaryExpression implements Expression {
			public Expression left;
			public Expression right;
			public String operator;
			public BinaryExpression(Expression l, Expression r, String op) {
				left = l;
				right = r;
				operator = op;
			}
		}
		
		public static class Constant  implements Expression {
			public String value;
			public Constant(String v) {
				value = v;
			}
		}
		
		public static class Identifier  implements Expression {
			public String name;
			public Identifier(String n) {
				name = n;
			}
		}
		
		public static class KeyValuePair  implements Expression {
			public List<String> key;
			public KeyValuePair(List<String> key) {
				this.key = key;
			}
		}
	}

	public static class Automaton {
		public String name;
		
		public Automaton(String name) {
			this.name = name;
		}
		
		public Node entryPoint;
		List<Node> nodes = new ArrayList<>();

		public void add(Node node) {
			nodes.add(node);
		}

		public List<Node> getNodes() {
		  return nodes;
		}
	}
	
	public static abstract class Node {
		
		public int timeout;
		public Node timeoutTarget;
		public List<Action> timeoutActions = new ArrayList<>();
		
		public List<Transition> transtions = new ArrayList<>();
		
		public List<Action> actions  = new ArrayList<>();

		abstract public NamedNode getReferenceNode() ;

		public void addTransition(Transition transition) { 
			transtions.add(transition);
		}
		
		public void addTimeoutActions(List<Action> a) {
			timeoutActions.addAll(a);
		}
		
		public List<Action> getTimeoutActions() {
			return timeoutActions;
		}
		
		public abstract boolean isUrgent();
	}
	
	public static class NamedNode extends Node {
		private boolean urg;
		public String name;
		public NamedNode(String name, boolean u) {
			this.name = name;
			this.urg = u;
		}

		@Override
		public NamedNode getReferenceNode() {
			return this;
		}

		@Override
		public boolean isUrgent() {
			return urg;
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
		
		@Override
		public boolean isUrgent() {
			return true;
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
		
		public Node getTarget() {
			return target;
		}
		
		public Guard getGuard() {
			return guard;
		}
		
		public List<Action> getActions() {
			return actions;
		}
	}
	
	public static class Message {
		String name;
		List<String> fields;
		public Message(String n, List<String> f) {
			name = n;
			fields = f;
		}
		
		public String getName() {
			return name;
		}
		
		public List<String> getFields() {
			return fields;
		}
	}
	
	public static class RemoteMessage extends Message {
		public RemoteMessage(String name, List<String> fields) {
			super(name, fields);
		}
	}
	
	public static class Pattern {
		String operator;
		List<Expression> operands;
		
		public Pattern(String op, List<Expression> ops) {
			operator = op;
			operands = ops;
		}
		
		public String getOperator() {
			return operator;
		}
		
		public List<Expression> getOperands() {
			return operands;
		}
	}

	
	abstract static class BuiltinElement {
		String builtinName;
		List<Expression> arguments;
		
		protected BuiltinElement(String name, List<Expression> args) {
			builtinName = name;
			arguments = args;
		}
		public String getBuiltinName() {
			return builtinName;
		}
		
		public List<Expression> getArguments() {
			return arguments;
		}
	}
	
	abstract static private class ExternalElement {
		
		List<Expression> arguments;
		
		public ExternalElement(String name, List<Expression> args) {
			externalName = name;
			arguments = args;
		}

		String externalName;
		
		public String getExternalName() {
			return externalName;
		}
		
		public List<Expression> getArguments() {
			return arguments;
		}
	}
	public static class BuiltinGuard extends BuiltinElement implements Guard {
		public BuiltinGuard(String name, List<Expression> args) {
			super(name, args);
		}
	}
	public static class BuiltinAction extends BuiltinElement implements Action {
		public BuiltinAction(String name, List<Expression> args) {
			super(name, args);
		}
	}
	public static class ExternalGuard extends ExternalElement implements Guard {
		public ExternalGuard(String name, List<Expression> args) {
			super(name, args);
		}
	}
	public static class ExternalAction extends ExternalElement implements Action {
		public ExternalAction(String name, List<Expression> args) {
			super(name, args);
		}
	}
	public static class MessageAction implements Action {
		public MessageTarget target;
		public Message message;
		public Map<String, Expression> arguments;
		
		public MessageAction(MessageTarget t, Message msg, Map<String, Expression> args) {
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
		
		public Message getMessageType() {
			return message;
		}
		
		public List<Pattern> getPatterns() {
			return arguments;
		}
	}
	
	public void add(Automaton auto) {
		automata.put(auto.name, auto);
	}
}
