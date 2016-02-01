

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include "automata.h"
#include "executor.h"
#include "unix_tima_nature.h"

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory>


void
execute_protocol(std::shared_ptr<tima::AbstractTimaNature> nature, int port, std::map< std::string, std::string >& options)
{

  // printing the automatas
  // debug
  std::vector<tima::Automata*> automatas = nature->build_stl_version();
  std::cout << "Printing the state machine" << std::endl;
  nature->print_automata(automatas);

  nature->initialize();

  nature->configure_communication(port);

  // create executor
  tima::Executor executor(nature, options);

  bool  was_tick = true;

  while (true) {
     int msec = 1;
     // SET TIMER
     if (was_tick) {
       msec = executor.global_deadline();
       std::cout << "next deadline is in " << msec << " milliseconds" << std::endl;
       if (msec == tima::never_timeout) {
         msec = 100;
       }
       nature->configure_timer((uint64_t)(msec) * 1000000);
     }

     nature->schedule_events();
     // SCHEDULE HANLDERS FOR EVENTS

     // IDENTIFY EVENT
	 int msg_id;
	 char* payload;
     tima::AbstractTimaNature::EventType event_type = nature->identify_event(&msg_id, &payload);

     // CALL EXECUTOR WHEN YOU RECEIVE A TICK
     was_tick = false;
     if (event_type == tima::AbstractTimaNature::TICK) {
       executor.tick(msec);
       was_tick = true;
     }
	 else if (event_type == tima::AbstractTimaNature::NETWORK_MESSAGE) {
     	executor.add_received_network_message(msg_id, payload);
		delete [] payload;
     }
 }
}

int
main(int argc, char* const *argv)
{

  int port = 1567;
  std::map< std::string, std::string > options;
  char c;
  while ((c = getopt (argc, argv, "p:")) != -1) {
    std::stringstream str;
    switch (c) {
      case 'p':
		 options.emplace("p", optarg);
         str << optarg;
         str >> port;
      break;
    }
  }

  std::cout << "The port is " << port << std::endl;

  std::shared_ptr<tima::AbstractTimaNature> nature (new UnixTimaNature("node 0"));
  execute_protocol(nature, port, options);

  return 0;
}
