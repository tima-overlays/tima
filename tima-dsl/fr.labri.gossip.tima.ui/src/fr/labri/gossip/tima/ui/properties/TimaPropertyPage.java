package fr.labri.gossip.tima.ui.properties;

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

import fr.labri.gossip.tima.ui.builder.TimaNature;

public class TimaPropertyPage extends PropertyPage {

	private static final String PATH_TITLE = "Path to OMNet++/INET application folders";
//	private static final String OWNER_TITLE = "&Owner:";
//	private static final String OWNER_PROPERTY = "OWNER";
//	private static final String DEFAULT_OWNER = "John Doe";

	private Text ownerText;
	
	private String path;

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
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		Button btn = new Button(composite, SWT.PUSH);
		btn.setText("Browse");
		btn.addSelectionListener(new SelectionListener() {
			
			@Override
			public void widgetSelected(SelectionEvent e) {
				// TODO Auto-generated method stub
				DirectoryDialog dialog = new DirectoryDialog(getShell(), SWT.OPEN);
				dialog.setFilterPath(path);
				path = dialog.open();
				if (path != null)
					ownerText.setText(path);
			}
			
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
				// TODO Auto-generated method stub
				
			}
		});
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
//		addSecondSection(composite);
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
	}
	
	public boolean performOk() {
		// store the value in the owner text field
		try {
			getElement().getAdapter(org.eclipse.core.resources.IProject.class).setPersistentProperty(
				TimaNature.KEY_PATH_OMNET,
				path);
		} catch (CoreException e) {
			return false;
		}
		return true;
	}

}