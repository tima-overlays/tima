#ifndef __TIMA_EXECUTOR__
#define __TIMA_EXECUTOR__

#include <vector>
#include <map>
#include <stdint.h>
#include <memory>
#include <functional>

#include "automata.h"
#include "tima_nature.h"


namespace tima {

typedef std::function<void(std::string&,std::map<std::string, std::string>& , std::shared_ptr<tima::GlobalStorage>) > device_initialization_t;

class Executor {
private:
  std::shared_ptr< tima::AbstractTimaNature > nature;
  std::vector<struct tima::Automaton*> automatas;
  std::vector<int> current_states;
  std::vector<int> timeouts;

  std::shared_ptr<tima::GlobalStorage> storage;

  uint32_t deadline(struct tima::Automaton* a, int state_idx);

  bool step(uint32_t milliseconds, bool only_urgents);

public:
  Executor(std::vector<tima::Automaton*> a,
          std::shared_ptr<tima::AbstractTimaNature> nature,
          std::map<std::string, std::string>& options, device_initialization_t callback);

  // return tima::never_timeout if all the states can only wait forever
  int tick(uint32_t milliseconds);

  // return the next global deadline
  int global_deadline();

  void add_received_network_message(int msg_id, const char* payload);


};

}

#endif
