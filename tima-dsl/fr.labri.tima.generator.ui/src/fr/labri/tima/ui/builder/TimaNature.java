package fr.labri.tima.ui.builder;

import org.eclipse.core.resources.ICommand;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IProjectNature;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.QualifiedName;

public class TimaNature implements IProjectNature {

	/**
	 * ID of this project nature
	 */
	public static final String NATURE_ID = "fr.labri.tima.generator.ui.timaNature";

	private IProject project;
	
	public static final String DEFAULT_PATH_TO_OMNET = "/home/inti/work/app/omnetpp-4.6/samples/inet/src/inet/applications";
	public static final QualifiedName KEY_PATH_OMNET = new QualifiedName("Tima DSL", "Path To OMNet++/Inet");

	public static final String DEFAULT_TARGETS = "";
	public static final QualifiedName KEY_TARGETS = new QualifiedName("Tima DSL", "Generated Target");

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#configure()
	 */
	public void configure() throws CoreException {
		IProjectDescription desc = project.getDescription();
		ICommand[] commands = desc.getBuildSpec();

		for (int i = 0; i < commands.length; ++i) {
			if (commands[i].getBuilderName().equals(TimaBuilder.BUILDER_ID)) {
				return;
			}
		}

		ICommand[] newCommands = new ICommand[commands.length + 1];
		System.arraycopy(commands, 0, newCommands, 0, commands.length);
		ICommand command = desc.newCommand();
		command.setBuilderName(TimaBuilder.BUILDER_ID);
		newCommands[newCommands.length - 1] = command;
		desc.setBuildSpec(newCommands);
		project.setDescription(desc, null);
		if (project.getPersistentProperty(KEY_PATH_OMNET) == null) {
			project.setPersistentProperty(KEY_PATH_OMNET, DEFAULT_PATH_TO_OMNET);
		}
		if (project.getPersistentProperty(KEY_TARGETS) == null) {
			project.setPersistentProperty(KEY_TARGETS, DEFAULT_TARGETS);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#deconfigure()
	 */
	public void deconfigure() throws CoreException {
		project.setPersistentProperty(KEY_PATH_OMNET, null);
		project.setPersistentProperty(KEY_TARGETS, null);
		
		IProjectDescription description = getProject().getDescription();
		ICommand[] commands = description.getBuildSpec();
		for (int i = 0; i < commands.length; ++i) {
			if (commands[i].getBuilderName().equals(TimaBuilder.BUILDER_ID)) {
				ICommand[] newCommands = new ICommand[commands.length - 1];
				System.arraycopy(commands, 0, newCommands, 0, i);
				System.arraycopy(commands, i + 1, newCommands, i,
						commands.length - i - 1);
				description.setBuildSpec(newCommands);
				project.setDescription(description, null);			
				return;
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#getProject()
	 */
	public IProject getProject() {
		return project;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.resources.IProjectNature#setProject(org.eclipse.core.resources.IProject)
	 */
	public void setProject(IProject project) {
		this.project = project;
	}

}
