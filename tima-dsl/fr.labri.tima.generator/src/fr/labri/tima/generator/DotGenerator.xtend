package fr.labri.tima.generator

import fr.labri.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext
import java.util.HashMap
import fr.labri.tima.ir.IRAutomata.Node

class DotGenerator extends NamedNodeGenerator {
	val names = new HashMap<IRAutomata.Node, String>()
	var cmpt = 0
	
	new(IRAutomata a) {
		super(a)
		generateNames(a)
	}
	
	override generateFiles( String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		fsa.generateFile('''«name»/«name».dot''',
		'''
			digraph «name» {
				node [margin=0 shape=box height=0 width=0];
			«FOR a : automata.automata.entrySet SEPARATOR '\n' »
				«a.value.toDot()»
			«ENDFOR»
			}
		''')
	}
	
	def newID(String prefix) {
		'''«prefix»«cmpt++»'''.toString
	}
	
	private def generateNames(IRAutomata a) {
		for (automaton: a.automata.values)
			for(state: automaton.nodes)
				names.put(state, newID("s"))
	}
	
	dispatch def String toDot(IRAutomata.Automaton a) {
		'''
		subgraph cluster_«a.name» {
			label="«a.name»";
			«FOR state : a.nodes»
				«state.toDot»
			«ENDFOR»
			
			«FOR state : a.nodes»
				«FOR t : state.transtions»
					«t.toDotFrom(state)»
				«ENDFOR»
				«IF state.timeoutTarget != null »
					«state.internalName» -> «state.timeoutTarget.internalName» [style=dashed];
				«ENDIF»
			«ENDFOR»
		 }
		'''
	}
	
	def getInternalName(Node node) {
		names.get(node)
	}
	
	dispatch def getLabel(IRAutomata.NamedNode n) {
		n.name
	}
	
	
	dispatch def getLabel(IRAutomata.DerivedNode n) {
		'''<I>«n.referenceNode.name»</I>'''
	}
	
	dispatch def String toDot(IRAutomata.Node n) {
		'''«n.internalName» [«IF n.timeoutTarget != null»shape=folder «ENDIF»label=<<table border="0"><tr><td>«n.label»«IF n.timeoutTarget != null» <font point-size="10pt"><sup>«n.timeout»</sup></font>«ENDIF»</td></tr>''' +
		'''«IF ! n.actions.empty»«ENDIF»'''+
		'''«FOR action: n.actions»
		<tr><td>«action.toDot»</td></tr>
		«ENDFOR»</table>>];
		'''
	}
	
	def toDotFrom(IRAutomata.Transition t, IRAutomata.Node from) {
		if (t.actions.empty)
			'''«from.internalName» -> «t.target.internalName» [label=<«t.guard.toDot»>];'''
		else {
			val temp = newID("t") 
			'''«temp» [style="dot" shape="cds" label=<<table border="0">'''+
			'''«FOR action: t.actions»<TR><TD>«action.toDot»</TD></TR>«ENDFOR»</table>>];
			«from.internalName» -> «temp» [label=<«t.guard.toDot»> arrowhead=dot];
			«temp» -> «t.target.internalName»;
			'''
		}
	}
	
	dispatch def String toDot(IRAutomata.BuiltinGuard g) {
		'''«g.builtinName»'''
	}
	
	dispatch def String toDot(IRAutomata.ExternalGuard g) {
		'''«g.externalName.substring(1, g.externalName.length - 1)»'''
	}
	
	dispatch def String toDot(IRAutomata.MessageGuard g) {
		'''«g.messageType.name»'''
	}
	
	dispatch def String toDot(IRAutomata.BuiltinAction a) {
		'''<B>«a.builtinName»</B>'''
	}
		
	dispatch def String toDot(IRAutomata.ExternalAction a) {
		'''<I>«a.externalName.substring(1, a.externalName.length - 1)»</I>'''
	}
		
	dispatch def String toDot(IRAutomata.MessageAction a) {
		'''!«a.message.name»@«a.target.toDot»'''
	}
	
	dispatch def String toDot(IRAutomata.MessageTarget.Broadcast t) {
		"*"
	}
	
	dispatch def String toDot(IRAutomata.MessageTarget.Unicast t) {
		t.target.toString;
	}
	
	dispatch def String toDot(IRAutomata.MessageTarget.Internal t) {
		'''<b>«t.target.name»</b>''';
	}
}
