
#include "executor.h"
#include <algorithm>
#include <iostream>

#include "mailbox.hpp"


tima::Executor::Executor(std::shared_ptr<tima::AbstractTimaNature> nature)
  :nature(nature)
{

  this->automatas = nature->build_stl_version();

  for (auto it = this->automatas.begin(),
            end=this->automatas.end(); it != end ; ++it) {
    Mailbox::get_instance()->add_automaton((*it)->name);
    this->current_states.push_back((*it)->initial);
    this->timeouts.push_back(deadline(*it, (*it)->initial));
  }

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
  for (auto idx = 0 ; idx < current_states.size(); ++idx) {
    auto a = automatas[idx];
    auto current_state = current_states[idx];
    auto state = &a->states[current_state];
    if (only_urgents && !state->urgent) {
      continue;
    }
    bool message_received = false;
    Message _the_message;
    // find valid transition
    int i = 0 ;
    bool found = false;
    while (i < state->nr_transitions && !found) {
      if (state->transitions[i].guard != tima::Mailbox::exists && state->transitions[i].guard != tima::Mailbox::exists_network_message) {
        found = state->transitions[i].guard(a->name, nullptr);
      }
      else {
        message_received = true;
        auto ctx = new MailboxContext(state->transitions[i].msg_id, state->transitions[i].src_id);
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
      must_execute_action = true;
      moved = true;
      current_states[idx] = state->transitions[i].dst; // new state
    }
    if (must_execute_action) {
      timeouts[idx] = deadline(a, current_states[idx]);
      auto ctx = new GenericActionContext(Message(state->transitions[i].msg_id, state->transitions[i].src_id), message_received, nature);
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
