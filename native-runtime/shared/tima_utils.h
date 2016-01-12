#ifndef __TIMA_UTILS__
#define __TIMA_UTILS__ value

#include "automata.h"
#include "executor.h"
#include <vector>
#include <iostream>

namespace tima {

class TimaMethods {
public:
 enum EventType {
   TICK, NETWORK_MESSAGE
 };

 void print_automata(std::vector<tima::Automata*>& automata);
 std::vector<tima::Automata*> build_stl_version();
};


}

int	get_msg_id_from_name(const char* name);

#endif
