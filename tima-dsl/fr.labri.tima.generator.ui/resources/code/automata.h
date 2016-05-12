#ifndef __TIMA_AUTOMATA__
#define __TIMA_AUTOMATA__

#include <string>
#include <vector>
#include <stdint.h>

#include "tima.h"

namespace tima {

const int32_t null_destination = -1;
const int32_t never_timeout = -1;

struct Transition;

typedef void (*StateAction_t)(const std::string&, TimaNativeContext*);
typedef bool (*TransitionGuard_t)(const std::string&, tima::TimaNativeContext*);
typedef void (*TransitionAction_t)(const std::string&, tima::TimaNativeContext*);

struct State {
  std::string name;
  bool urgent;
  int32_t timeout;
  int32_t timeout_destination; // default transition
  TransitionAction_t timeout_action;
  uint32_t nr_transitions;
  struct Transition* transitions;

  StateAction_t action;
};



struct Transition {
  uint32_t dst; // destination state
  TransitionGuard_t guard;
  TransitionAction_t action;
  int msg_id;
};

struct Automaton {
  std::string name;

  uint32_t initial;

  uint32_t nr_states;
  struct State* states;
};

}

#endif
