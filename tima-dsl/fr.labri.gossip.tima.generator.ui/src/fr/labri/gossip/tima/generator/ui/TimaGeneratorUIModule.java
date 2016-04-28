package fr.labri.gossip.tima.generator.ui;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.xtext.service.AbstractGenericModule;

import com.google.inject.Binder;

public class TimaGeneratorUIModule extends AbstractGenericModule {
	private final AbstractUIPlugin plugin;
    
    public TimaGeneratorUIModule(AbstractUIPlugin plugin) {
        this.plugin = plugin;
    }
 
    @Override
    public void configure(Binder binder) {
        super.configure(binder);
        binder.bind(AbstractUIPlugin.class).toInstance(plugin);
        binder.bind(IDialogSettings.class).toInstance(plugin.getDialogSettings());
    }
    
    public Class<? extends org.eclipse.xtext.builder.IXtextBuilderParticipant> bindIXtextBuilderParticipant() {
        return org.eclipse.xtext.builder.BuilderParticipant.class;
    }
    
    public org.eclipse.core.resources.IWorkspaceRoot bindIWorkspaceRootToInstance() {
        return org.eclipse.core.resources.ResourcesPlugin.getWorkspace().getRoot();
    }
}
