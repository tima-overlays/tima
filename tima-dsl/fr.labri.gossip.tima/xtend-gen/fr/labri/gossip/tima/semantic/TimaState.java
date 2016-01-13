package fr.labri.gossip.tima.semantic;

import com.google.common.base.Objects;
import fr.labri.tima.ITimedAutomata;
import java.util.List;
import org.eclipse.xtend2.lib.StringConcatenation;

@SuppressWarnings("all")
public class TimaState<C extends Object> implements ITimedAutomata.State<C> {
  private final String name;
  
  private final int modifiers;
  
  private final List<ITimedAutomata.Action<C>> actions;
  
  public TimaState(final String name, final int modifiers, final List<ITimedAutomata.Action<C>> actions) {
    this.name = name;
    this.modifiers = modifiers;
    this.actions = actions;
  }
  
  @Override
  public List<ITimedAutomata.Action<C>> getActions() {
    return this.actions;
  }
  
  @Override
  public int getModifier() {
    return this.modifiers;
  }
  
  @Override
  public String getName() {
    return this.name;
  }
  
  @Override
  public void eachAction(final C context) {
    List<ITimedAutomata.Action<C>> _actions = this.getActions();
    for (final ITimedAutomata.Action<C> action : _actions) {
      action.eachAction(context);
    }
  }
  
  @Override
  public void postAction(final C context) {
    List<ITimedAutomata.Action<C>> _actions = this.getActions();
    for (final ITimedAutomata.Action<C> action : _actions) {
      action.postAction(context);
    }
  }
  
  @Override
  public void preAction(final C context) {
    List<ITimedAutomata.Action<C>> _actions = this.getActions();
    for (final ITimedAutomata.Action<C> action : _actions) {
      action.preAction(context);
    }
  }
  
  @Override
  public int hashCode() {
    return this.name.hashCode();
  }
  
  @Override
  public String toString() {
    StringConcatenation _builder = new StringConcatenation();
    _builder.append("TimaState: ");
    _builder.append(this.name, "");
    return _builder.toString();
  }
  
  @Override
  public boolean equals(final Object obj) {
    boolean _and = false;
    boolean _and_1 = false;
    Class<?> _class = obj.getClass();
    boolean _equals = Objects.equal(_class, TimaState.class);
    if (!_equals) {
      _and_1 = false;
    } else {
      _and_1 = (((TimaState<C>) obj).modifiers == this.modifiers);
    }
    if (!_and_1) {
      _and = false;
    } else {
      boolean _equals_1 = Objects.equal(((TimaState<C>) obj).name, this.name);
      _and = _equals_1;
    }
    return _and;
  }
}
