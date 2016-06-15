package fr.labri.tima.ui.highlighting

import org.eclipse.xtext.ide.editor.syntaxcoloring.DefaultSemanticHighlightingCalculator
import org.eclipse.xtext.resource.XtextResource
import org.eclipse.xtext.ide.editor.syntaxcoloring.IHighlightedPositionAcceptor
import org.eclipse.xtext.nodemodel.INode
import fr.labri.tima.dSL.KeyInStore
import org.eclipse.xtext.util.CancelIndicator
import fr.labri.tima.dSL.StoredValueSection

class DSLSemanticHighlightingConfigurator extends DefaultSemanticHighlightingCalculator {
	
	override protected doProvideHighlightingFor(XtextResource resource, IHighlightedPositionAcceptor acceptor, CancelIndicator cancelIndicator) {
//		super.doProvideHighlightingFor(resource, acceptor, cancelIndicator)
		  if (resource == null || resource.getParseResult() == null)
	        return;
	        
	      val root = resource.getParseResult().getRootNode()
	      
	      for (INode node : root.getAsTreeIterable()) {
//	      	val ge = node.getGrammarElement()
	      	
//	      	println(ge.class.name + " " + node.semanticElement)
	        if (node.semanticElement instanceof StoredValueSection) {
	        	
	          acceptor.addPosition(node.getOffset(), node.leafNodes.get(0).length, 
	            DSLHighlightingConfigurator.KEY_PAIR_ID);
	        }
	      }
	}
				
}
