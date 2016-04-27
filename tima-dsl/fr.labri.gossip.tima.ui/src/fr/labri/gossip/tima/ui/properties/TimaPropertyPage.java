package fr.labri.gossip.tima.ui.properties;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;

import fr.labri.gossip.tima.Util;
import fr.labri.gossip.tima.generator.Generators;
import fr.labri.gossip.tima.ui.builder.TimaNature;

public class TimaPropertyPage extends PropertyPage {

	private static final String PATH_TITLE = "Path to OMNet++/INET application folders";
	public static final String TARGETS_TITLE = "Targets language to generate";

	private Text ownerText;
	
	private String path;
	private Set<String> targets = new HashSet<>();
	private Map<String, Button> targetsButton = new HashMap<>();

	/**
	 * Constructor for SamplePropertyPage.
	 */
	public TimaPropertyPage() {
		super();
	}

	private void addFirstSection(Composite parent) {
		Composite composite = createDefaultComposite(parent);

		//Label for path field
		Label pathLabel = new Label(composite, SWT.NONE);
		pathLabel.setText(PATH_TITLE);

		// Path text field
		Text pathValueText = ownerText = new Text(composite, SWT.NONE | SWT.READ_ONLY);
		try {
			path = getElement().getAdapter(org.eclipse.core.resources.IProject.class).getPersistentProperty(TimaNature.KEY_PATH_OMNET);
			pathValueText.setText(path);
		} catch (CoreException e1) {
			e1.printStackTrace();
		}
		
		Button btn = new Button(composite, SWT.PUSH);
		btn.setText("Browse");
		btn.addSelectionListener(new SelectionListener() {
			
			@Override
			public void widgetSelected(SelectionEvent e) {
				DirectoryDialog dialog = new DirectoryDialog(getShell(), SWT.OPEN);
				dialog.setFilterPath(path);
				path = dialog.open();
				if (path != null)
					ownerText.setText(path);
			}
			
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
			}
		});
	}

	private void addSecondSection(Composite parent) {
		Composite composite = createDefaultComposite(parent);

		//Label for path field
		Label targetsLabel = new Label(composite, SWT.NONE);
		targetsLabel.setText(TARGETS_TITLE);

		try {
			Util.unserialize(getElement().getAdapter(org.eclipse.core.resources.IProject.class).getPersistentProperty(TimaNature.KEY_TARGETS), targets);
		} catch (CoreException e1) {
		}

		for (String generator: Generators.generators.keySet()) {
			Button btn = new Button(composite, SWT.CHECK);
			btn.setText(generator);
			btn.setSelection(targets.contains(generator));
			btn.pack();
			targetsButton.put(generator, btn);
		}
	}
	
	/**
	 * @see PreferencePage#createContents(Composite)
	 */
	protected Control createContents(Composite parent) {
		Composite composite = new Composite(parent, SWT.NONE);
		
		GridLayout layout = new GridLayout();
		composite.setLayout(layout);
		GridData data = new GridData(GridData.FILL);
		data.grabExcessHorizontalSpace = true;
		composite.setLayoutData(data);

		addFirstSection(composite);
//		addSeparator(composite);
		addSecondSection(composite);
		return composite;
	}

	private Composite createDefaultComposite(Composite parent) {
		Composite composite = new Composite(parent, SWT.NULL);
		GridLayout layout = new GridLayout();
		layout.numColumns = 1;
		composite.setLayout(layout);

		GridData data = new GridData();
		data.verticalAlignment = GridData.FILL;
		data.horizontalAlignment = GridData.FILL;
		composite.setLayoutData(data);

		return composite;
	}

	protected void performDefaults() {
		super.performDefaults();
		// Populate the owner text field with the default value
		path = TimaNature.DEFAULT_PATH_TO_OMNET;
		ownerText.setText(TimaNature.DEFAULT_PATH_TO_OMNET);
		
		Util.unserialize(TimaNature.DEFAULT_TARGETS, targets);
		for (Entry<String, Button> entry: targetsButton.entrySet()) {
			entry.getValue().setSelection(targets.contains(entry.getKey()));
		}
	}
	
	public boolean performOk() {
		try {
			getElement().getAdapter(org.eclipse.core.resources.IProject.class).
				setPersistentProperty(TimaNature.KEY_PATH_OMNET, path);
			targets.clear();
			for (Entry<String, Button> entry: targetsButton.entrySet()) {
				if(entry.getValue().getSelection())
					targets.add(entry.getKey());
			}
			getElement().getAdapter(org.eclipse.core.resources.IProject.class).
				setPersistentProperty(TimaNature.KEY_TARGETS, Util.serialize(targets));
		} catch (CoreException e) {
			return false;
		}
		return true;
	}

}