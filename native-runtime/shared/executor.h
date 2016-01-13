#ifndef __TIMA_EXECUTOR__
#define __TIMA_EXECUTOR__

#include <vector>
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

  uint32_t deadline(struct tima::Automata* a, int state_idx);

  bool step(uint32_t milliseconds, bool only_urgents);

public:
  Executor(std::shared_ptr<tima::AbstractTimaNature> nature);

  // return tima::never_timeout if all the states can only wait forever
  int tick(uint32_t milliseconds);

  // return the next global deadline
  int global_deadline();
};

}

#endif
