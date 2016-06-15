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
			case "RULE_TK_PROTOCOL",
			case "RULE_TK_DEFINED",
			case "RULE_TK_UNDEFINED",
			case "RULE_TK_IF",
			case "RULE_TK_EXTERNAL_ACTIONS",
			case "RULE_TK_TEMPLATE",
			case "RULE_TK_SUMMARY": DSLHighlightingConfigurator.KEYWORD_ID
			case "RULE_TK_TRUE",
			case "RULE_TK_FALSE": DSLHighlightingConfigurator.BOOLEAN_VALUE_ID 
			default: super.calculateId(tokenName, tokenType)
		}
	}
}
