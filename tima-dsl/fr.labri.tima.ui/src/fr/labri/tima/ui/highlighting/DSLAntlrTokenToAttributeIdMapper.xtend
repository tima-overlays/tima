package fr.labri.tima.ui.highlighting

import org.eclipse.xtext.ui.editor.syntaxcoloring.DefaultAntlrTokenToAttributeIdMapper

class DSLAntlrTokenToAttributeIdMapper extends DefaultAntlrTokenToAttributeIdMapper {
	
	override protected calculateId(String tokenName, int tokenType) {
		return switch (tokenName) {
			case "RULE_TK_ACTION",
			case "RULE_TK_AUTOMATON",
			case "RULE_TK_GUARD",
			case "RULE_TK_MODULE",
			case "RULE_TK_STATE",
			case "RULE_TK_MESSAGES",
			case "RULE_TK_PROTOCOLS",
			case "RULE_TK_SUMMARY": DSLHighlightingConfigurator.KEYWORD_ID
			default: super.calculateId(tokenName, tokenType)
		}
	}
}
