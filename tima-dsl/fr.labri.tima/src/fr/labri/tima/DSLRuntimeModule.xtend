/*
 * generated by Xtext 2.9.0
 */
package fr.labri.tima

import org.eclipse.xtext.naming.SimpleNameProvider

/**
 * Use this class to register components to be used at runtime / without the Equinox extension registry.
 */
class DSLRuntimeModule extends fr.labri.tima.AbstractDSLRuntimeModule {
	
	// contributed by org.eclipse.xtext.xtext.generator.exporting.QualifiedNamesFragment2
	override def bindIQualifiedNameProvider() {
		SimpleNameProvider
	}
}
