package fr.labri.gossip.tima.ui.wizards;

import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;

import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.List;
import org.osgi.framework.Bundle;
import org.osgi.framework.FrameworkUtil;

public class WizardSelectTemplate extends WizardPage {
	
	int previousIdx = 0;
	Button cb;
	List single;
	ArrayList<URL> items = new ArrayList<>();
	

	private class SimpleSelectionListener extends SelectionAdapter{

		@Override
		public void widgetSelected(SelectionEvent e) {
			if (!cb.getSelection()) {
				single.setSelection(previousIdx);
			}
			else {
				previousIdx = single.getSelectionIndex();
				single.setSelection(-1);
			}
			single.setEnabled(!cb.getSelection());
		}
	}
	
	protected WizardSelectTemplate(String pageName) {
		super(pageName);
	}

	@Override
	public void createControl(Composite parent) {
		Composite composite = new Composite(parent, SWT.NULL); 
		composite.setLayout(new GridLayout(3, false));

		cb = new Button(composite, SWT.CHECK);
		cb.setSelection(true);
		cb.setText("Empty project");
		GridData data = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		data.horizontalSpan = 3;
		cb.setLayoutData(data);
		cb.addSelectionListener(new SimpleSelectionListener());
		
		single = new List(composite, SWT.BORDER | SWT.SINGLE);
		data = new GridData(SWT.FILL, SWT.FILL, true, true);
		data.horizontalSpan = 3;
		single.setLayoutData(data);
		
		Bundle bundle = FrameworkUtil.getBundle(getClass());
		String pathToResources = "resources/templates";
		Enumeration<URL> l = bundle.findEntries(pathToResources,"*", false);
		while (l!=null && l.hasMoreElements()) {
			URL e = l.nextElement();
			single.add("Example - " + e.getFile().replace(pathToResources, "").replace("/", ""));
			items.add(e);
//			Enumeration<URL> l2 = bundle.findEntries(e.getPath(), "*", false);
//			while (l2!=null && l2.hasMoreElements()) {
//				URL e2 = l2.nextElement();
//				single.add(e2.getFile());
//			}
		}
		
		single.setEnabled(false);
		setPageComplete(true);
		setControl(composite);
	}
	
	public boolean useTemplate() {
		return !cb.getSelection();
	}
	
	public URL getTemplate() {
		if (useTemplate()) {
			return items.get(single.getSelectionIndex());
		}
		else return null;
	}
}
