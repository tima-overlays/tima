package fr.labri.gossip.tima.generator;

import org.eclipse.xtext.generator.IGenerator2;
import org.eclipse.xtext.service.AbstractGenericModule;

import fr.labri.gossip.tima.generator.DSLGenerator;

public class TimaGeneratorModule extends AbstractGenericModule {
	public Class<? extends IGenerator2> bindIGenerator2() {
		return DSLGenerator.class;
	}
}
