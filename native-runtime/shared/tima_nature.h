#ifndef __TIMA_UTILS__
#define __TIMA_UTILS__

#include "automata.h"
#include <vector>
#include <string>
#include <iostream>

namespace tima {

class AbstractTimaNature {
public:
 enum EventType {
   NONE, TICK, NETWORK_MESSAGE
 };

 virtual void initialize() = 0;
 virtual void configure_communication(int port) = 0;
 virtual void configure_timer(uint64_t nanoseconds) = 0;
 virtual EventType identify_event(int* msg_id, char** payload) = 0;
 virtual void schedule_events() = 0;
 virtual void handle_error(const char * msg) = 0;
 virtual void send_network_message(const std::string& dst, int port, const std::string& msg) = 0;
 virtual void broadcast(int port, const std::string& msg) = 0;
 virtual void print_trace(const std::string& msg) = 0;

 void print_automata(std::vector<tima::Automata*>& automata);
 std::vector<tima::Automata*> build_stl_version();

 AbstractTimaNature(std::string device_name) : device_name(device_name) {}

public:
 std::string device_name;
};


}

int	get_msg_id_from_name(const char* name);

#endif
