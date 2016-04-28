package fr.labri.tima.ui.wizards;

import java.io.File;

import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

public class WizardSelectFolder extends WizardPage {
	
	private static final String PATH_TITLE = "Path to OMNet++/INET application folders";
	private Text ownerText;
	private String path;

	protected WizardSelectFolder(String pageName) {
		super(pageName);
	}

	@Override
	public void createControl(Composite parent) {
		Composite composite = new Composite(parent, SWT.NONE);
		
		GridLayout layout = new GridLayout();
		composite.setLayout(layout);
		GridData data = new GridData(GridData.FILL);
		data.grabExcessHorizontalSpace = true;
		composite.setLayoutData(data);

		addFirstSection(composite);
		addSeparator(composite);
		addSecondSection(composite);
		setControl(composite);
	}
	
	private void addSecondSection(Composite composite) {
				
	}

	private void addSeparator(Composite composite) {
		// TODO Auto-generated method stub
		
	}

	private void addFirstSection(Composite parent) {
		
		Composite comp_label = createDefaultComposite(parent);
		
		Composite comp_path = createDefaultComposite(parent);
		RowLayout rl = new RowLayout();
		comp_path.setLayout(rl);

		//Label for path field
		Label pathLabel = new Label(comp_label, SWT.NONE);
		pathLabel.setText(PATH_TITLE);

		// Path text field
		Text pathValueText = ownerText = new Text(comp_path, SWT.NONE | SWT.READ_ONLY);	
		pathValueText.setText(path);
		
		File f = new File(path);
		setPageComplete(f.exists() && f.isDirectory());
		if (!f.exists())
			setErrorMessage("The path doesn't exist");
		else if (!f.isDirectory()) 
			setErrorMessage("The path isn't a directory");
		
		Button btn = new Button(comp_path, SWT.PUSH);
		btn.setText("Browse");
		btn.addSelectionListener(new SelectionListener() {
			
			@Override
			public void widgetSelected(SelectionEvent e) {
				// TODO Auto-generated method stub
				DirectoryDialog dialog = new DirectoryDialog(getShell(), SWT.OPEN);
				dialog.setFilterPath(path);
				path = dialog.open();
				if (path != null) {
					ownerText.setText(path);
					File f = new File(path);
					setPageComplete(f.exists() && f.isDirectory());
					if (!f.exists())
						setErrorMessage("The path doesn't exist");
					else if (!f.isDirectory()) 
						setErrorMessage("The path isn't a directory");
				}
			}
			
			@Override
			public void widgetDefaultSelected(SelectionEvent e) {
			}
		});
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

	public void setInitialPath(String defaultPathToOmnet) {
		path = defaultPathToOmnet;
	}
	
	public String getPath() {
		return path;
	}
}
