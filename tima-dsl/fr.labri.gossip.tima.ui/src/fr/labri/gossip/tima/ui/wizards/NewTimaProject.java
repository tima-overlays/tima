package fr.labri.gossip.tima.ui.wizards;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URL;
import java.util.Enumeration;

import org.eclipse.core.internal.resources.ProjectDescription;
import org.eclipse.core.resources.IBuildConfiguration;
import org.eclipse.core.resources.ICommand;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.wizard.ProgressMonitorPart;
import org.eclipse.ui.dialogs.WizardNewLinkPage;
import org.eclipse.ui.dialogs.WizardNewProjectCreationPage;
import org.eclipse.ui.wizards.newresource.BasicNewResourceWizard;
import org.eclipse.xtext.util.Arrays;
import org.osgi.framework.Bundle;
import org.osgi.framework.FrameworkUtil;

import fr.labri.gossip.tima.ui.builder.TimaNature;


public class NewTimaProject extends BasicNewResourceWizard {
	
	private WizardNewProjectCreationPage _pageOne;
	private WizardSelectFolder _pageTwo;
	private WizardSelectTemplate _pageThree;

	@Override
	public void addPages() {
		// TODO Auto-generated method stub
		_pageOne = new WizardNewProjectCreationPage("Tima Project Wizard");
	    _pageOne.setTitle("Tima Project");
	    _pageOne.setDescription("Create a project to define network protocols using TIMA.");
	    addPage(_pageOne);
	    
	    _pageTwo = new WizardSelectFolder("Path to OMNET++/Inet");
	    _pageTwo.setTitle("Path to OMNET++/Inet");
	    _pageTwo.setDescription("You must have a working Omnet/inet installation. This path must point to 'applications' in INET's source");
	    _pageTwo.setInitialPath(TimaNature.DEFAULT_PATH_TO_OMNET);
	    addPage(_pageTwo);
	    
	    _pageThree = new WizardSelectTemplate("Select template");
	    _pageThree.setTitle("Select template to use");
	    addPage(_pageThree);
	} 

	@Override
	public boolean canFinish() {
		return _pageTwo.isPageComplete() && _pageOne.canFlipToNextPage();
	}

	@Override
	public boolean performFinish() {
		IProject project = _pageOne.getProjectHandle();
		try {
			IProjectDescription pd =  project.getWorkspace().newProjectDescription(_pageOne.getName());
			project.create(pd, new NullProgressMonitor());
			// open project
			project.open(new NullProgressMonitor());
			// set path to omnetpp
			project.setPersistentProperty(TimaNature.KEY_PATH_OMNET, _pageTwo.getPath());
			// add natures
			pd = project.getDescription();
			pd.setNatureIds(new String[] { "org.eclipse.xtext.ui.shared.xtextNature", TimaNature.NATURE_ID });
			project.setDescription(pd, new NullProgressMonitor());
			// create structure
			IFolder folder = project.getFolder("src");
			folder.create(true, true, new NullProgressMonitor());
			if (_pageThree.useTemplate()) {
				URL e = _pageThree.getTemplate();
				Bundle bundle = FrameworkUtil.getBundle(getClass());
				Enumeration<URL> l2 = bundle.findEntries(e.getPath(), "*", false);
				while (l2!=null && l2.hasMoreElements()) {
					URL e2 = l2.nextElement();
					IFile dst = folder.getFile(e2.getFile().replace(e.getPath(), ""));
					try {
						InputStream stream = e2.openStream();
						dst.create(stream, true, new NullProgressMonitor());
						stream.close();
					} catch (IOException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
				}
			}
			else {
				folder.getFile(String.format("%s.%s", project.getName(), "cc")).create(new ByteArrayInputStream(new byte[]{}), true, null);
				folder.getFile(String.format("%s.%s", project.getName(), "gossip")).create(new ByteArrayInputStream(new byte[]{}), true, null);
			}
		} catch (CoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return true;
	}
}
