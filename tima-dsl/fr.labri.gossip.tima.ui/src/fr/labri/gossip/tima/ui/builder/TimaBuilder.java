package fr.labri.gossip.tima.ui.builder;

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

import fr.labri.gossip.tima.Util;

public class TimaBuilder extends IncrementalProjectBuilder {

	
	public static final String BUILDER_ID = "fr.labri.gossip.tima.ui.timabuilder";
	private static final String WRONG_OMNETPP_PATH = "Wrong Path to OMNETPP/INET";

	@Override
	protected IProject[] build(int kind, Map<String, String> args,
			IProgressMonitor monitor) throws CoreException {
		
		String destination = getProject().getPersistentProperty(TimaNature.KEY_PATH_OMNET);
		if (!new File(destination).exists()) {
			IMarker marker = getProject().createMarker(WRONG_OMNETPP_PATH);
			marker.setAttribute(IMarker.MESSAGE, WRONG_OMNETPP_PATH);
			marker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
			return null;
//			throw new CoreException(new OperationStatus(IStatus.ERROR, "", 0, "Cannot create an INET applications without defined INET path. Add it to the properties (Tima Property Page).", null));
		}
		IFolder src_folder = getProject().getFolder("src");
		IFolder folder = getProject().getFolder("src-gen");
		if (!folder.exists()) {
//			IMarker marker = getProject().createMarker(UNDEFINED_OMNETPP_PATH);
//			marker.setAttribute(IMarker.MESSAGE, WRONG_OMNETPP_PATH);
//			marker.setAttribute(IMarker.SEVERITY, IMarker.SEVERITY_ERROR);
			return null;
//			throw new CoreException(new OperationStatus(IStatus.ERROR, "", 0, "Cannot create an INET applications without defined protocols", null));
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
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.internal.events.InternalBuilder#build(int,
	 *      java.util.Map, org.eclipse.core.runtime.IProgressMonitor)
	 */
//	protected IProject[] build(int kind, Map args, IProgressMonitor monitor)
//			throws CoreException {
//		if (kind == FULL_BUILD) {
//			fullBuild(monitor);
//		} else {
//			IResourceDelta delta = getDelta(getProject());
//			if (delta == null) {
//				fullBuild(monitor);
//			} else {
//				incrementalBuild(delta, monitor);
//			}
//		}
//		return null;
//	}

	@Override
	protected void clean(IProgressMonitor monitor) throws CoreException {
		
		super.clean(monitor);
	}
	
//	protected void clean(IProgressMonitor monitor) throws CoreException {
//		// delete markers set and files created
////		getProject().deleteMarkers(MARKER_TYPE, true, IResource.DEPTH_INFINITE);
//	}

	
//	protected void fullBuild(final IProgressMonitor monitor)
//			throws CoreException {
//		try {
//			getProject().accept(new SampleResourceVisitor());
//		} catch (CoreException e) {
//		}
//	}
//
//	protected void incrementalBuild(IResourceDelta delta,
//			IProgressMonitor monitor) throws CoreException {
//		// the visitor does the work.
//		delta.accept(new SampleDeltaVisitor());
//	}
}
