package fr.labri.gossip.tima.generator.ui;

import org.apache.log4j.Logger;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.xtext.ui.shared.SharedStateModule;
import org.eclipse.xtext.util.Modules2;
import org.osgi.framework.BundleContext;

import com.google.inject.Guice;
import com.google.inject.Injector;

import fr.labri.gossip.tima.DSLRuntimeModule;
import fr.labri.gossip.tima.generator.TimaGeneratorModule;
import fr.labri.gossip.tima.ui.DSLUiModule;

public class Activator extends AbstractUIPlugin {
    private Injector injector;
    private static Activator INSTANCE;
 
    public Injector getInjector() {
        return injector;
    }
     
    @Override
    public void start(BundleContext context) throws Exception {
        super.start(context);
        INSTANCE = this;
        try {
            injector = Guice.createInjector(Modules2.mixin(new DSLRuntimeModule(), new SharedStateModule(), new DSLUiModule(this), new TimaGeneratorModule(), new TimaGeneratorUIModule(this)));
        } catch (Exception e) {
            Logger.getLogger(getClass()).error(e.getMessage(), e);
            throw e;
        }
    }
     
    @Override
    public void stop(BundleContext context) throws Exception {
        injector = null;
        super.stop(context);
    }
     
    public static Activator getInstance() {
        return INSTANCE;
    }
     
}