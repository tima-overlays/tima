#ifndef __TIMA_AUTOMATA__
#define __TIMA_AUTOMATA__

#include <string>
#include <stdint.h>

#include "tima.h"

namespace tima {

const int32_t null_destination = -1;
const int32_t never_timeout = -1;

struct Transition;

struct State {
  std::string name;
  bool urgent;
  int32_t timeout;
  int32_t timeout_destination; // default transition
  uint32_t nr_transitions;
  struct Transition* transitions;

  void (*pre_action)(std::string&, TimaNativeContext*);
  void (*post_action)(std::string&, TimaNativeContext*);
  void (*each_action)(std::string&, TimaNativeContext*);
};

struct Transition {
  uint32_t dst; // destination state
  bool (*guard)(std::string&, TimaNativeContext*);
  int msg_id;
  int src_id;
};

struct Automata {
  std::string name;

  uint32_t initial;

  uint32_t nr_states;
  struct State* states;
};

}

#endif