#ifndef __TIMA_EXECUTOR__
#define __TIMA_EXECUTOR__

#include <vector>
#include <map>
#include <stdint.h>
#include <memory>

#include "automata.h"
#include "tima_nature.h"


namespace tima {

class Executor {
private:
  std::shared_ptr< tima::AbstractTimaNature > nature;
  std::vector<struct tima::Automata*> automatas;
  std::vector<int> current_states;
  std::vector<int> timeouts;

  void* user_data;

  uint32_t deadline(struct tima::Automata* a, int state_idx);

  bool step(uint32_t milliseconds, bool only_urgents);

public:
  Executor(std::shared_ptr<tima::AbstractTimaNature> nature, std::map<std::string, std::string>& options);

  // return tima::never_timeout if all the states can only wait forever
  int tick(uint32_t milliseconds);

  // return the next global deadline
  int global_deadline();

  void add_received_network_message(int msg_id, const char* payload);
};

}

#endif
