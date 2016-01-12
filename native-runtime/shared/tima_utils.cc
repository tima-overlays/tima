
#include "tima_utils.h"

/** For accessing automata */
struct tima::Automata& get_automata(uint32_t idx);
uint32_t get_nr_automatas();


void
tima::TimaMethods::print_automata(std::vector<tima::Automata*>& automata)
{
  // THIS IS DEBUG
  for (auto it = automata.begin(), it_end = automata.end() ; it != it_end ; ++it) {
    struct tima::Automata* x = *it;
    std::cout << x->name << std::endl;
    for (int i = 0 ; i < x->nr_states; i++) {
      if (x->initial == i)
        std::cout << "\033[1;31m";
      std::cout << "\tState: " << x->states[i].name << std::endl;
      if (x->initial == i)
        std::cout << "\033[0m";
      for (int j = 0 ; j < x->states[i].nr_transitions ; j++) {
        std::cout << "\t\t=> " << x->states[x->states[i].transitions[j].dst].name << std::endl;
      }
      if (x->states[i].timeout_destination != tima::null_destination) {
        std::cout << "\033[1;35m";
        std::cout << "\t\t=>*" << x->states[x->states[i].timeout_destination].name << std::endl;
        std::cout << "\033[0m";
      }
    }
  }
}

std::vector<tima::Automata*>
tima::TimaMethods::build_stl_version()
{
  std::vector<tima::Automata*> automatas;
  uint32_t n = get_nr_automatas();
  for (size_t i = 0; i < n; i++) {
    /* code */
    struct tima::Automata* x = &get_automata(i);
    automatas.push_back(x);
  }
  return automatas;
}
