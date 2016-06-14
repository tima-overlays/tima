package fr.labri.tima.ui.highlighting

import org.eclipse.swt.SWT
import org.eclipse.xtext.ui.editor.syntaxcoloring.DefaultHighlightingConfiguration
import org.eclipse.xtext.ui.editor.syntaxcoloring.IHighlightingConfigurationAcceptor
import org.eclipse.swt.graphics.RGB

class DSLHighlightingConfigurator extends DefaultHighlightingConfiguration {
    public static val String TIME_TYPE_ID = "TIME_ID";
    public static val String KEY_PAIR_ID = "KEY_PAIR_IN_STORE_ID";
 
    override configure(IHighlightingConfigurationAcceptor acceptor) {
    	super.configure(acceptor);
        acceptor.acceptDefaultHighlighting(TIME_TYPE_ID, "Time Type", timeTypeTextStyle())
        acceptor.acceptDefaultHighlighting(KEY_PAIR_ID, "Key Value Pair", keyPairTypeTextStyle())
    }
 
    def timeTypeTextStyle() {
        val textStyle = defaultTextStyle().copy()
        textStyle.setStyle(SWT.ITALIC)
        textStyle.color = new RGB(20, 20, 200)
        textStyle
    }
    
    def keyPairTypeTextStyle() {
        val textStyle = defaultTextStyle().copy()
        textStyle.setStyle(SWT.BOLD + SWT.ITALIC)
        textStyle.color = new RGB(20, 120, 120)
        textStyle
    }
				
}
