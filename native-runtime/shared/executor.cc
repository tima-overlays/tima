
#include "executor.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <map>

#include "mailbox.h"
#include "tima.h"


void init_device_data(std::string& device_name,std::map<std::string, std::string>& options, void** ptr);

namespace tima {
	struct InnerGenericActionContext : public ActionContext {
	  InnerGenericActionContext(std::string device_name,void* user_data, Message msg, bool msg_received, std::shared_ptr<tima::AbstractTimaNature> nature);

	  virtual void send_to(const std::string& dst, int port, const Message& msg);
	  virtual void broadcast(int port, const Message& msg);
    virtual void print_trace(const std::string& msg);
    virtual ~InnerGenericActionContext();
	private:
	  std::shared_ptr<tima::AbstractTimaNature> nature;
	};
}


tima::ActionContext::ActionContext(
        std::string device_name,void* user_data,
        tima::Message msg, bool msg_received
      ):
        TimaNativeContext(device_name, user_data),
        msg_received(msg_received),
        msg(msg) {}

tima::InnerGenericActionContext::InnerGenericActionContext(std::string device_name,void* user_data, Message msg, bool msg_received, std::shared_ptr<tima::AbstractTimaNature> nature):
		tima::ActionContext(device_name, user_data, msg, msg_received), nature(nature) {}

tima::ActionContext::~ActionContext()
{
}

tima::InnerGenericActionContext::~InnerGenericActionContext()
{
}

void
tima::InnerGenericActionContext::send_to(const std::string& dst, int port, const Message& msg) {
  nature->send_network_message(dst, port, nature->serialize(msg));
}

void
tima::InnerGenericActionContext::broadcast(int port, const Message& msg) {
	// serialize the message
  nature->broadcast(port, nature->serialize(msg));
}

void
tima::InnerGenericActionContext::print_trace(const std::string& msg)
{
	nature->print_trace(msg);
}

tima::Executor::Executor(std::shared_ptr<tima::AbstractTimaNature> nature, std::map<std::string, std::string>& options)
  :nature(nature)
{

  this->automatas = nature->build_stl_version();

  for (auto it = this->automatas.begin(),
            end=this->automatas.end(); it != end ; ++it) {
    Mailbox::get_instance(nature->device_name)->add_automaton((*it)->name);
    this->current_states.push_back((*it)->initial);
    this->timeouts.push_back(deadline(*it, (*it)->initial));
  }

  init_device_data(nature->device_name, options, &user_data);

}

int
tima::Executor::tick(uint32_t milliseconds)
{

  bool b = step(milliseconds, false);

  while (step(0, true));

  return global_deadline();
}

bool
tima::Executor::step(uint32_t milliseconds, bool only_urgents)
{
  bool moved = false;
  for (auto idx = 0U ; idx < current_states.size(); ++idx) {
    auto a = automatas[idx];
    auto current_state = current_states[idx];
    auto state = &a->states[current_state];
    if (only_urgents && !state->urgent) {
      continue;
    }
    bool message_received = false;
    Message _the_message;
    // find valid transition
    uint i = 0 ;
    bool found = false;
    while (i < state->nr_transitions && !found) {
      if (state->transitions[i].guard != tima::Mailbox::exists && state->transitions[i].guard != tima::Mailbox::exists_network_message) {
        auto ctx = new TimaNativeContext(nature->device_name, user_data);
        found = state->transitions[i].guard(a->name, ctx);
        delete ctx;
      }
      else {
        message_received = true;
        auto ctx = new MailboxContext(state->transitions[i].msg_id, state->transitions[i].src_id, nature->device_name, user_data);
        found = state->transitions[i].guard(a->name, ctx);
        _the_message = ctx->read_message;
        delete ctx;
      }
      if (!found) {
        i++;
      }
    }
    // check if we found a valid transition
    bool must_execute_action = false;
    if (!found) {
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
      timeouts[idx] = deadline(a, current_states[idx]);
      auto ctx = new InnerGenericActionContext(nature->device_name, user_data, Message(state->transitions[i].msg_id, state->transitions[i].src_id), message_received, nature);
      ctx->msg = _the_message;
      a->states[current_states[idx]].each_action(a->name, ctx);
      delete ctx;
    }
  }

  return moved;
}


int
tima::Executor::global_deadline()
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
tima::Executor::deadline(struct tima::Automata* a, int state_idx)
{
  return a->states[state_idx].timeout;
}

void
tima::Executor::add_received_network_message(int msg_id, const char* payload)
{
    auto ctx = new TemporaryActionContext(nature->device_name, user_data, nature);
    Mailbox::add_received_network_message(msg_id, payload, ctx);
    delete ctx;
}
