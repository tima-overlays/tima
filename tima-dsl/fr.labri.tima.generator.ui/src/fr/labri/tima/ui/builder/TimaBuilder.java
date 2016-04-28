package fr.labri.tima.ui.builder;

import java.io.File;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.osgi.framework.Bundle;
import org.osgi.framework.FrameworkUtil;

import fr.labri.tima.Util;

public class TimaBuilder extends IncrementalProjectBuilder {
	
	public static final String BUILDER_ID = "fr.labri.tima.ui.timabuilder";
	private static final String WRONG_OMNETPP_PATH = "Wrong Path to OMNETPP/INET";

	@Override
	protected IProject[] build(int kind, Map<String, String> args,
			IProgressMonitor monitor) throws CoreException {
		
		IFolder src_folder = getProject().getFolder("src");
		IFolder folder = getProject().getFolder("src-gen");
		if (!folder.exists()) {
			return null;
		}
		
		// FIXME check the properties to do things related for a backend
		String destination = getProject().getPersistentProperty(TimaNature.KEY_PATH_OMNET);
		if (!new File(destination).exists()) {
			IMarker marker = getProject().createMarker(WRONG_OMNETPP_PATH);
			marker.setAttribute(IMarker.MESSAGE, WRONG_OMNETPP_PATH);
			marker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_WARNING);
			return null;
		}

		final ArrayList<IFolder> apps = new ArrayList<IFolder>();
		folder.accept(new IResourceVisitor() {
			@Override
			public boolean visit(IResource resource) throws CoreException {
				boolean isFolder = resource.getType() == IResource.FOLDER;
				if (isFolder && resource != folder)
					apps.add(folder.getFolder(resource.getName()));
				return true;
			}
		}, 1, false);
		
		for (IFolder f: apps) {
			String name = f.getName();
			IFile semantic_file =  src_folder.getFile(name + "_semantic.cc");
			File app = new File(destination, name);
			app.mkdir();
			
			// copy semantic
			Util.copy(semantic_file, new File(app.getAbsoluteFile(), semantic_file.getName()), false);
			
			// copy all files under src-gen/${app}
			f.accept(new IResourceVisitor() {
				@Override
				public boolean visit(IResource resource) throws CoreException {
					if (resource.getType() == IResource.FILE) {
						// copy file from the project to the external location
						Util.copy(f.getFile(resource.getName()), new File(app.getAbsolutePath(), resource.getName()), true);
					}
					return true;
				}
			}, 1, false);
			
			// copy Tima runtime
			Bundle bundle = FrameworkUtil.getBundle(getClass());
			String pathToResources = "resources/code";
			Enumeration<URL> l = bundle.findEntries(pathToResources,"*", true);
			while (l!=null && l.hasMoreElements()) {
				URL e = l.nextElement();
				Util.copy(e, new File(app.getAbsolutePath(),  
						e.getFile().replace(pathToResources, "")), true);
			}
		}
		return null;
	}
}
