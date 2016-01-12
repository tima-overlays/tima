package fr.labri.gossip.tima.ui.highlighting

import org.eclipse.xtext.ui.editor.syntaxcoloring.DefaultAntlrTokenToAttributeIdMapper

class DSLAntlrTokenToAttributeIdMapper extends DefaultAntlrTokenToAttributeIdMapper {
	
	override protected calculateId(String tokenName, int tokenType) {
		if(tokenName.equals("RULE_UNIT")) {
            DSLHighlightingConfigurator.TIME_TYPE_ID 
        }
        else
        	super.calculateId(tokenName, tokenType)
	}
	
}