package fr.labri.tima.ui.wizards;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

import fr.labri.tima.generator.Generators;
import fr.labri.tima.ui.builder.TimaNature;
import fr.labri.tima.ui.properties.TimaPropertyPage;
import fr.labri.tima.Util;

public class WizardSelectTargets extends WizardPage  {

	private Map<String, Button> targetsButton = new HashMap<>();

	protected WizardSelectTargets(String pageName) {
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
		setControl(composite);
	}

	private void addFirstSection(Composite parent) {
		Composite comp_path = createDefaultComposite(parent);
		RowLayout rl = new RowLayout();
		comp_path.setLayout(rl);

		Composite composite = createDefaultComposite(parent);

		//Label for path field
		Label targetsLabel = new Label(composite, SWT.NONE);
		targetsLabel.setText(TimaPropertyPage.TARGETS_TITLE);
		Set<String> targets = Util.unserialize(TimaNature.DEFAULT_TARGETS, new HashSet<>());
		
		for (String generator: Generators.generators.keySet()) {
			Button btn = new Button(composite, SWT.CHECK);
			btn.setText(generator);
			btn.setSelection(targets.contains(generator));
			btn.pack();
			targetsButton.put(generator, btn);
		}
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

	public Set<String> getTargets() {
		Set<String> targets = new HashSet<>();
		for (Entry<String, Button> entry: targetsButton.entrySet())
			if (entry.getValue().getSelection())
				targets.add(entry.getKey());
		return targets;
	}
}
