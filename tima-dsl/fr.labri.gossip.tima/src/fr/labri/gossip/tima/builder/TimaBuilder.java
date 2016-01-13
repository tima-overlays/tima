package fr.labri.gossip.tima.builder;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Map;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.eclipse.core.commands.operations.OperationStatus;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.osgi.framework.Bundle;
import org.osgi.framework.FrameworkUtil;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.helpers.DefaultHandler;

public class TimaBuilder extends IncrementalProjectBuilder {

	
	public static final String BUILDER_ID = "fr.labri.gossip.tima.timabuilder";

	@Override
	protected IProject[] build(int kind, Map<String, String> args,
			IProgressMonitor monitor) throws CoreException {
		
		String destination = getProject().getPersistentProperty(TimaNature.KEY_PATH_OMNET);
		System.out.println(destination);
		if (!new File(destination).exists()) {
			throw new CoreException(new OperationStatus(IStatus.ERROR, "", 0, "Cannot create an INET applications without defined INET path. Add it to the properties (Tima Property Page).", null));
		}
		IFolder folder = getProject().getFolder("src-gen");
		if (!folder.exists()) {
			throw new CoreException(new OperationStatus(IStatus.ERROR, "", 0, "Cannot create an INET applications without defined protocols", null));
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
			File app = new File(destination, name);
			app.mkdir();
			// copy all files under src-gen/${app}
			f.accept(new IResourceVisitor() {
				@Override
				public boolean visit(IResource resource) throws CoreException {
					if (resource.getType() == IResource.FILE) {
						// copy file from the project to the external location
						copy(f.getFile(resource.getName()), new File(app.getAbsolutePath(), resource.getName()));
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
				copy(e, new File(app.getAbsolutePath(),  
						e.getFile().replace(pathToResources, "")));
			}
		}
		return null;
	}
	
	protected void copy(URL src, File dst) {;
		if (dst.exists()) return;
		try {
			InputStream in = src.openStream();
			OutputStream out = new FileOutputStream(dst);
			byte[] buffer = new byte[1024];
			int len;
			while ((len = in.read(buffer)) != -1) {
			    out.write(buffer, 0, len);
			}
			in.close();
			out.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	protected void copy(IFile src, File dst) {
		File source = new File(src.getLocationURI());
		try {
			InputStream in = new FileInputStream(source);
			OutputStream out = new FileOutputStream(dst);
			byte[] buffer = new byte[1024];
			int len;
			while ((len = in.read(buffer)) != -1) {
			    out.write(buffer, 0, len);
			}
			in.close();
			out.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
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
