/*
 * generated by Xtext 2.9.0
 */
package fr.labri.tima.ui.labeling

import com.google.inject.Inject
import fr.labri.tima.dSL.BuiltInGuard
import fr.labri.tima.dSL.BuiltinAction
import fr.labri.tima.dSL.ExternalAction
import fr.labri.tima.dSL.ExternalGuard
import fr.labri.tima.dSL.GuardedTransition
import fr.labri.tima.dSL.MessageAction
import fr.labri.tima.dSL.MessageGuard
import fr.labri.tima.dSL.TimeoutTransition
import fr.labri.tima.dSL.Transition
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider
import org.eclipse.xtext.ui.IImageHelper
import org.eclipse.xtext.ui.label.DefaultEObjectLabelProvider
import fr.labri.tima.dSL.Automaton
import fr.labri.tima.dSL.MessageType

/**
 * Provides labels for EObjects.
 * 
 * See https://www.eclipse.org/Xtext/documentation/304_ide_concepts.html#label-provider
 */
class DSLLabelProvider extends DefaultEObjectLabelProvider {

	@Inject
    private IImageHelper imageHelper;

	@Inject
	new(AdapterFactoryLabelProvider delegate) {
		super(delegate);
	}

	// Labels and icons can be computed like this:
	def text(MessageAction action) '''
	Send «action.type.name»
	'''
	
	def text(BuiltinAction action ){
		action.name
	}
	
	def text(ExternalAction action ){
		action.name
	}
	

	def image(ExternalAction a) {
		imageHelper.getImage("action.png")
    }
    
    def image(BuiltinAction a) {
		imageHelper.getImage("action.png")
    }
    
    def image(MessageAction a) {
		imageHelper.getImage("send.png")
    }
    
    def text(Transition t){
		'''transition to «t.target.name»'''	
	}
	
	def text(MessageType msg) {
		'''Msg: «msg.name»'''
	}
	
	def image(TimeoutTransition t) {
		imageHelper.getImage("clock.png")
	}
    
    def image(GuardedTransition t) {
		imageHelper.getImage(t.guard.iconName)
    }
	
	dispatch def String iconName(MessageGuard guard) {
		"receive.png"
	}
	
	dispatch def String iconName(BuiltInGuard guard) {
		"action.png"
	}
	
	dispatch def String iconName(ExternalGuard guard) {
		"action.png"
	}
    
    def image(Automaton s) {
		imageHelper.getImage('automata.png')
    }
}