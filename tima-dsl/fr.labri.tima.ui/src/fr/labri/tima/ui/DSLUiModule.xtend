/*
 * generated by Xtext 2.9.0
 */
package fr.labri.tima.ui

import fr.labri.tima.ui.highlighting.DSLHighlightingConfigurator
import fr.labri.tima.ui.highlighting.DSLSemanticHighlightingConfigurator
import org.eclipse.xtend.lib.annotations.FinalFieldsConstructor
import org.eclipse.xtext.ui.resource.SimpleResourceSetProvider
import com.google.inject.Binder
import org.eclipse.xtext.ui.editor.syntaxcoloring.IHighlightingConfiguration

import fr.labri.tima.ui.highlighting.DSLAntlrTokenToAttributeIdMapper
import org.eclipse.xtext.ui.editor.syntaxcoloring.AbstractAntlrTokenToAttributeIdMapper
import org.eclipse.xtext.ui.editor.model.ResourceForIEditorInputFactory
import org.eclipse.xtext.ide.editor.syntaxcoloring.ISemanticHighlightingCalculator

/**
 * Use this class to register components to be used within the Eclipse IDE.
 */
@FinalFieldsConstructor
class DSLUiModule extends AbstractDSLUiModule {
	
	override configure(Binder binder) {
		super.configure(binder)
		binder.bind(IHighlightingConfiguration).to(DSLHighlightingConfigurator)
		binder.bind(ISemanticHighlightingCalculator).to(DSLSemanticHighlightingConfigurator)
		binder.bind(AbstractAntlrTokenToAttributeIdMapper).to(DSLAntlrTokenToAttributeIdMapper)
	}
	
	override bindIResourceSetProvider() {
    	SimpleResourceSetProvider
  	}
  	
    override bindIResourceForEditorInputFactory() {
    	ResourceForIEditorInputFactory
	}
}
