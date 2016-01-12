package fr.labri.gossip.tima.ui.highlighting

import org.eclipse.swt.SWT
import org.eclipse.xtext.ui.editor.syntaxcoloring.DefaultHighlightingConfiguration
import org.eclipse.xtext.ui.editor.syntaxcoloring.IHighlightingConfigurationAcceptor
import org.eclipse.swt.graphics.RGB

class DSLHighlightingConfigurator extends DefaultHighlightingConfiguration {
    public static val String TIME_TYPE_ID = "TIME_ID";
 
    override configure(IHighlightingConfigurationAcceptor acceptor) {
    	super.configure(acceptor);
        acceptor.acceptDefaultHighlighting(TIME_TYPE_ID, "Time Type", dataTypeTextStyle())
    }
 
    def dataTypeTextStyle() {
        val textStyle = defaultTextStyle().copy()
        textStyle.setStyle(SWT.ITALIC)
        textStyle.color = new RGB(20, 20, 200)
        textStyle
    }
}
