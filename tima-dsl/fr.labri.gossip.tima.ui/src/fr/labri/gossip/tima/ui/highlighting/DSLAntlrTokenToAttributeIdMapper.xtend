package fr.labri.gossip.tima.ui.highlighting

import org.eclipse.xtext.ui.editor.syntaxcoloring.DefaultAntlrTokenToAttributeIdMapper
import static fr.labri.gossip.tima.ide.contentassist.antlr.internal.InternalDSLLexer.*

class DSLAntlrTokenToAttributeIdMapper extends DefaultAntlrTokenToAttributeIdMapper {
	
	override protected calculateId(String tokenName, int tokenType) {
		println(tokenType + " " + tokenName + " " + RULE_TK_AUTOMATON)
		return switch (tokenName) {
			case "RULE_TK_ACTION",
			case "RULE_TK_AUTOMATON",
			case "RULE_TK_GUARD",
			case "RULE_TK_MODULE",
			case "RULE_TK_STATE",
			case "RULE_TK_SUMMARY": DSLHighlightingConfigurator.KEYWORD_ID
			default: super.calculateId(tokenName, tokenType)
		}
	}
}