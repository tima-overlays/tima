package fr.labri.gossip.tima.semantic;

import com.google.common.base.Objects;
import fr.labri.gossip.tima.dSL.Automata;
import fr.labri.gossip.tima.dSL.ExternalAction;
import fr.labri.gossip.tima.dSL.Guard;
import fr.labri.gossip.tima.dSL.Message;
import fr.labri.gossip.tima.dSL.MessageType;
import fr.labri.tima.ITimedAutomata;
import org.eclipse.xtend2.lib.StringConcatenation;

@SuppressWarnings("all")
public class TimaGuard<C extends Object> implements ITimedAutomata.Predicate<C> {
  public enum GuardType {
    LOCAL_MESSAGE_GUARD,
    
    NETWORK_MESSAGE_GUARD,
    
    GENERIC_GUARD,
    
    TIMEOUT_GUARD;
  }
  
  private final String type;
  
  public String messageID = "ANY";
  
  public String sourceID = "ANY";
  
  public TimaGuard.GuardType guardType;
  
  public TimaGuard(final String automatonName, final Guard g) {
    CharSequence _dsl2cpp = this.dsl2cpp(automatonName, g);
    String _string = _dsl2cpp.toString();
    this.type = _string;
  }
  
  /**
   * Create a c++ representation of a guard
   */
  private CharSequence dsl2cpp(final String automatonName, final Guard g) {
    CharSequence _xifexpression = null;
    MessageType _msg = g.getMsg();
    boolean _notEquals = (!Objects.equal(_msg, null));
    if (_notEquals) {
      CharSequence _xblockexpression = null;
      {
        MessageType _msg_1 = g.getMsg();
        Message _msg_2 = _msg_1.getMsg();
        String _name = _msg_2.getName();
        this.messageID = _name;
        CharSequence _xifexpression_1 = null;
        Automata _source = g.getSource();
        boolean _notEquals_1 = (!Objects.equal(_source, null));
        if (_notEquals_1) {
          CharSequence _xblockexpression_1 = null;
          {
            Automata _source_1 = g.getSource();
            String _name_1 = _source_1.getName();
            this.sourceID = _name_1;
            this.guardType = TimaGuard.GuardType.LOCAL_MESSAGE_GUARD;
            StringConcatenation _builder = new StringConcatenation();
            _builder.append("tima::Mailbox::exists");
            _xblockexpression_1 = _builder;
          }
          _xifexpression_1 = _xblockexpression_1;
        } else {
          CharSequence _xblockexpression_2 = null;
          {
            this.guardType = TimaGuard.GuardType.NETWORK_MESSAGE_GUARD;
            StringConcatenation _builder = new StringConcatenation();
            _builder.append("tima::Mailbox::exists_network_message");
            _xblockexpression_2 = _builder;
          }
          _xifexpression_1 = _xblockexpression_2;
        }
        _xblockexpression = _xifexpression_1;
      }
      _xifexpression = _xblockexpression;
    } else {
      CharSequence _xifexpression_1 = null;
      ExternalAction _externalAction = g.getExternalAction();
      boolean _notEquals_1 = (!Objects.equal(_externalAction, null));
      if (_notEquals_1) {
        CharSequence _xblockexpression_1 = null;
        {
          this.guardType = TimaGuard.GuardType.GENERIC_GUARD;
          StringConcatenation _builder = new StringConcatenation();
          ExternalAction _externalAction_1 = g.getExternalAction();
          String _name = _externalAction_1.getName();
          _builder.append(_name, "");
          _builder.append("::");
          ExternalAction _externalAction_2 = g.getExternalAction();
          String _method = _externalAction_2.getMethod();
          _builder.append(_method, "");
          _xblockexpression_1 = _builder;
        }
        _xifexpression_1 = _xblockexpression_1;
      } else {
        CharSequence _xblockexpression_2 = null;
        {
          this.guardType = TimaGuard.GuardType.TIMEOUT_GUARD;
          StringConcatenation _builder = new StringConcatenation();
          _builder.append("timeout");
          _xblockexpression_2 = _builder;
        }
        _xifexpression_1 = _xblockexpression_2;
      }
      _xifexpression = _xifexpression_1;
    }
    return _xifexpression;
  }
  
  @Override
  public String getType() {
    return this.type;
  }
  
  @Override
  public boolean isValid(final C context) {
    return true;
  }
  
  @Override
  public String toString() {
    return this.type;
  }
  
  @Override
  public boolean equals(final Object obj) {
    boolean _and = false;
    Class<?> _class = obj.getClass();
    boolean _equals = Objects.equal(_class, TimaGuard.class);
    if (!_equals) {
      _and = false;
    } else {
      boolean _equals_1 = Objects.equal(((TimaGuard<C>) obj).type, this.type);
      _and = _equals_1;
    }
    return _and;
  }
  
  @Override
  public int hashCode() {
    return this.type.hashCode();
  }
}
