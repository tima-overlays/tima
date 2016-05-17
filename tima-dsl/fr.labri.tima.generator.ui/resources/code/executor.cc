
#include "executor.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <map>

#include "mailbox.h"
#include "tima.h"


using std::string;
using std::shared_ptr;

void init_device_data(string& device_name,std::map<string, string>& options, shared_ptr<tima::GlobalStorage> st);

namespace tima {

class InnerGenericActionContext : public ActionContext {
public:
  InnerGenericActionContext(string device_name,shared_ptr<GlobalStorage> st, Message msg, bool msg_received, shared_ptr<AbstractTimaNature> nature);

  virtual void send_to(const string& dst, int port, const Message& msg);
  virtual void broadcast(int port, const Message& msg);
  virtual void print_trace(const string& msg);
  virtual ~InnerGenericActionContext();

private:
  shared_ptr<tima::AbstractTimaNature> nature;
};



ActionContext::ActionContext(
        string device_name,shared_ptr<tima::GlobalStorage> st,
        tima::Message msg, bool msg_received
      ):
        TimaNativeContext(device_name, st),
        msg_received(msg_received),
        msg(msg)
{
}


InnerGenericActionContext::InnerGenericActionContext(string device_name,shared_ptr<GlobalStorage> st, Message msg, bool msg_received, shared_ptr<AbstractTimaNature> nature):
		tima::ActionContext(device_name, st, msg, msg_received), nature(nature)

{}


ActionContext::~ActionContext()
{
}


InnerGenericActionContext::~InnerGenericActionContext()
{
}


void
InnerGenericActionContext::send_to(const string& dst, int port, const Message& msg) {
  nature->send_network_message(dst, port, nature->serialize(msg));
}


void
InnerGenericActionContext::broadcast(int port, const Message& msg) {
	// serialize the message
  nature->broadcast(port, nature->serialize(msg));
}


void
InnerGenericActionContext::print_trace(const string& msg)
{
	nature->print_trace(msg);
}


Executor::Executor(shared_ptr<tima::AbstractTimaNature> nature, std::map<string, string>& options)
  :nature(nature)
{

  automatas = nature->build_stl_version();

  auto myMailbox = Mailbox::get_instance(nature->device_name);

  for (auto it : automatas) {
      myMailbox->add_automaton(it->name);

      current_states.push_back(it->initial);

      timeouts.push_back(deadline(it, it->initial));
  }

  storage = std::make_shared<tima::GlobalStorage>(nullptr);

  init_device_data(nature->device_name, options, storage);

}

int
Executor::tick(uint32_t milliseconds)
{

  bool b = step(milliseconds, false);

  while (step(0, true));

  return global_deadline();
}

bool
Executor::step(uint32_t milliseconds, bool only_urgents)
{
  bool moved = false;
  for (auto idx = 0U ; idx < current_states.size(); ++idx) {
    auto a = automatas[idx];
    auto current_state = current_states[idx];
    auto state = &a->states[current_state];

    if (only_urgents && !state->urgent) {
      continue;
    }

    Message _the_message;

    uint i = 0;

    // find valid transition
    for (i = 0 ; i < state->nr_transitions ; i++) {
        bool found = false;
        TimaNativeContext* ctx;

        if (state->transitions[i].is_msg_transition == false) {

            /* external and built-in actions  */

            ctx = new TimaNativeContext(nature->device_name, storage);
            found = state->transitions[i].guard(a->name, ctx);

        }
        else {

            /* message pattern */

            ctx = new MailboxContext(
                        nature->device_name,
                        storage);

            found = state->transitions[i].guard(a->name, ctx);

            _the_message = ((MailboxContext*)ctx)->read_message;

        }

        delete ctx;

        if (found) {
            break;
        }
    }

    // check if we found a valid transition
    bool must_execute_action = false;
    if (i == state->nr_transitions) {
      timeouts[idx] -= (!only_urgents && timeouts[idx] != tima::never_timeout)?milliseconds:0;
      if (timeouts[idx] == 0) {
        must_execute_action = true;
        // FIXME: I wonder if after a timeout we should execute urgent states
        // moved = true;
        // go to the default state
        current_states[idx] = state->timeout_destination;

      }
    }
    else {
//      std::cout << "Found in automaton - state : " << a->name << " - " << state->name << std::endl;
      must_execute_action = true;
      moved = true;
      current_states[idx] = state->transitions[i].dst; // new state
    }

    if (must_execute_action) {
      if (i < state->nr_transitions) {

          /* a transition due to a guard being true */

          timeouts[idx] = deadline(a, current_states[idx]);
          auto ctx = new InnerGenericActionContext(
                                  nature->device_name,
                                  storage,
                                  _the_message,
                                  state->transitions[i].is_msg_transition,
                                  nature);

          /* execute action in transition */
          state->transitions[i].action(a->name, ctx);

          /* execute action in state */
          a->states[current_states[idx]].action(a->name, ctx);

          /* free context */
          delete ctx;
      }
      else {

        /* a time out transition */

        timeouts[idx] = deadline(a, current_states[idx]);
        auto ctx = new InnerGenericActionContext(
                                nature->device_name,
                                storage,
                                _the_message,
                                false,
                                nature);

        /* execute action in transition */
        state->timeout_action(a->name, ctx);

        /* execute action in state */
        a->states[current_states[idx]].action(a->name, ctx);

        /* free context */
        delete ctx;
      }
    }
  }

  return moved;
}


int
Executor::global_deadline()
{
  return *std::min_element(timeouts.begin(), timeouts.end(), [] (int i, int j) {
      if (i == tima::never_timeout)
        return false;
      if (j == tima::never_timeout)
        return true;
      return i < j;
    });
}

uint32_t
Executor::deadline(struct tima::Automaton* a, int state_idx)
{
  return a->states[state_idx].timeout;
}

void
Executor::add_received_network_message(int msg_id, const char* payload)
{
    auto ctx = new TemporaryActionContext(nature->device_name, storage, nature);
    Mailbox::add_received_network_message(msg_id, payload, ctx);
    delete ctx;
}

}
