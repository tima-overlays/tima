package fr.labri.gossip.tima.semantic;

import com.google.common.base.Objects;
import fr.labri.gossip.tima.dSL.Action;
import fr.labri.gossip.tima.dSL.Automata;
import fr.labri.gossip.tima.dSL.ExternalAction;
import fr.labri.gossip.tima.dSL.Message;
import fr.labri.gossip.tima.dSL.MessageType;
import fr.labri.tima.ITimedAutomata;
import java.util.LinkedList;
import org.eclipse.xtend2.lib.StringConcatenation;
import org.eclipse.xtext.xbase.lib.CollectionLiterals;

@SuppressWarnings("all")
public class TimaAction<C extends Object> extends ITimedAutomata.ActionAdapter<C> {
  public static class SimpleTimaAction<C extends Object> extends ITimedAutomata.ActionAdapter<C> {
    private final String type;
    
    public boolean isMessage = false;
    
    public String msg_id;
    
    public String automaton_dst;
    
    public String src_id;
    
    public SimpleTimaAction(final String auto_name, final Action g) {
      CharSequence _dsl2cpp = this.dsl2cpp(auto_name, g);
      String _string = _dsl2cpp.toString();
      this.type = _string;
    }
    
    @Override
    public String getType() {
      return this.type;
    }
    
    private CharSequence dsl2cpp(final String auto_name, final Action g) {
      CharSequence _xblockexpression = null;
      {
        this.isMessage = false;
        CharSequence _xifexpression = null;
        MessageType _msg = g.getMsg();
        boolean _notEquals = (!Objects.equal(_msg, null));
        if (_notEquals) {
          CharSequence _xblockexpression_1 = null;
          {
            MessageType _msg_1 = g.getMsg();
            Message _msg_2 = _msg_1.getMsg();
            String _name = _msg_2.getName();
            String _plus = (_name + "_MSG_ID");
            this.msg_id = _plus;
            Automata _target = g.getTarget();
            String _name_1 = _target.getName();
            this.automaton_dst = _name_1;
            this.src_id = (auto_name + "_AUTOMATON_ID");
            this.isMessage = true;
            StringConcatenation _builder = new StringConcatenation();
            _builder.append("tima::Mailbox::send");
            _xblockexpression_1 = _builder;
          }
          _xifexpression = _xblockexpression_1;
        } else {
          CharSequence _xifexpression_1 = null;
          ExternalAction _externalAction = g.getExternalAction();
          boolean _notEquals_1 = (!Objects.equal(_externalAction, null));
          if (_notEquals_1) {
            StringConcatenation _builder = new StringConcatenation();
            ExternalAction _externalAction_1 = g.getExternalAction();
            String _name = _externalAction_1.getName();
            _builder.append(_name, "");
            _builder.append("::");
            ExternalAction _externalAction_2 = g.getExternalAction();
            String _method = _externalAction_2.getMethod();
            _builder.append(_method, "");
            _xifexpression_1 = _builder;
          } else {
            StringConcatenation _builder_1 = new StringConcatenation();
            _builder_1.append("timeout");
            _xifexpression_1 = _builder_1;
          }
          _xifexpression = _xifexpression_1;
        }
        _xblockexpression = _xifexpression;
      }
      return _xblockexpression;
    }
  }
  
  public final LinkedList<TimaAction.SimpleTimaAction<C>> pre_actions = CollectionLiterals.<TimaAction.SimpleTimaAction<C>>newLinkedList();
  
  public final LinkedList<TimaAction.SimpleTimaAction<C>> post_actions = CollectionLiterals.<TimaAction.SimpleTimaAction<C>>newLinkedList();
  
  public final LinkedList<TimaAction.SimpleTimaAction<C>> each_actions = CollectionLiterals.<TimaAction.SimpleTimaAction<C>>newLinkedList();
}
