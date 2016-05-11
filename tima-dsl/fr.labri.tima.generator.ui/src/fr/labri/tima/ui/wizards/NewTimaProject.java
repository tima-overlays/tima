package fr.labri.tima.ui.wizards;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Enumeration;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.ui.dialogs.WizardNewProjectCreationPage;
import org.eclipse.ui.wizards.newresource.BasicNewResourceWizard;
import org.osgi.framework.Bundle;
import org.osgi.framework.FrameworkUtil;

import fr.labri.tima.ui.builder.TimaNature;
import fr.labri.tima.Util;


public class NewTimaProject extends BasicNewResourceWizard {
	
	private WizardNewProjectCreationPage _projectNamePage;
	private WizardSelectTargets _targetsPage;
	private WizardSelectFolder _omnetPage;
	private WizardSelectTemplate _templatePage;

	@Override
	public void addPages() {
		_projectNamePage = new WizardNewProjectCreationPage("Tima Project Wizard");
	    _projectNamePage.setTitle("Tima Project");
	    _projectNamePage.setDescription("Create a project to define network protocols using TIMA.");
	    addPage(_projectNamePage);
	    
	    _targetsPage = new WizardSelectTargets("Target languages");
	    _targetsPage.setTitle("Select target languages");
	    _targetsPage.setDescription("Select target languages that will be generated for this project.");
	    addPage(_targetsPage);
	    
	    _omnetPage = new WizardSelectFolder("Path to OMNET++/Inet");
	    _omnetPage.setTitle("Path to OMNET++/Inet");
	    _omnetPage.setDescription("You must have a working Omnet/inet installation. This path must point to 'applications' in INET's source");
	    _omnetPage.setInitialPath(TimaNature.DEFAULT_PATH_TO_OMNET);
	    addPage(_omnetPage);
	    
	    _templatePage = new WizardSelectTemplate("Select template");
	    _templatePage.setTitle("Select template to use");
	    addPage(_templatePage);
	} 

	@Override
	public boolean canFinish() {
		return _omnetPage.isPageComplete() && _projectNamePage.canFlipToNextPage();
	}

	@Override
	public boolean performFinish() {
		IProject project = _projectNamePage.getProjectHandle();
		try {
			IProjectDescription pd =  project.getWorkspace().newProjectDescription(_projectNamePage.getName());
			project.create(pd, new NullProgressMonitor());
			// open project
			project.open(new NullProgressMonitor());
			// set path to omnetpp
			project.setPersistentProperty(TimaNature.KEY_PATH_OMNET, _omnetPage.getPath());
			project.setPersistentProperty(TimaNature.KEY_TARGETS, Util.serialize(_targetsPage.getTargets()));

			// add nature
			pd = project.getDescription();
			
			// validate the natures
			IWorkspace workspace = ResourcesPlugin.getWorkspace();
			String[] newNatures = new String[] { "org.eclipse.xtext.ui.shared.xtextNature", TimaNature.NATURE_ID };
			IStatus status = workspace.validateNatureSet(newNatures);
			// only apply new nature, if the status is ok
			if (status.getCode() == IStatus.OK) {
			  pd.setNatureIds(newNatures);
			  project.setDescription(pd, new NullProgressMonitor());
			} 
			else {
				return false;
			}

			// create structure
			IFolder folder = project.getFolder("src");
			folder.create(true, true, new NullProgressMonitor());
			if (_templatePage.useTemplate()) {
				URL e = _templatePage.getTemplate();
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
						e1.printStackTrace();
					}
				}
			}
			else {
				folder.getFile(String.format("%s_semantic.%s", project.getName(), "cc")).create(new ByteArrayInputStream(new byte[]{}), true, null);
				folder.getFile(String.format("%s.%s", project.getName(), "tima")).create(new ByteArrayInputStream(new byte[]{}), true, null);
			}
		} catch (CoreException e) {
			e.printStackTrace();
		}
		return true;
	}
	
	@Override
	public IWizardPage getNextPage(IWizardPage currentPage) {
		if (currentPage == _targetsPage && !((WizardSelectTargets)currentPage).getTargets().contains("omnet")) // FIXME should be constant
			return _templatePage;
		return super.getNextPage(currentPage);
	}
	
	@Override
	public IWizardPage getPreviousPage(IWizardPage currentPage) {
		if (currentPage == _templatePage && !((WizardSelectTargets)currentPage).getTargets().contains("omnet")) // FIXME should be constant
			return _targetsPage;
		return super.getPreviousPage(currentPage);
	}
}
