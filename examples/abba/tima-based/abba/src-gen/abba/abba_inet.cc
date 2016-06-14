#include "abba_inet.h"

namespace abba {

uint32_t get_nr_automaton_in_abba();	
struct tima::Automaton& get_automaton_abba(uint32_t idx);

void
init_device_data_abba(
	std::string& device_name,
	std::map<std::string, std::string>& options,
	std::shared_ptr<tima::GlobalStorage> st);

}

namespace inet {

Define_Module(abba_inet);

std::vector<tima::Automaton*>
abba_inet::build_stl_version()
{
  std::vector<tima::Automaton*> automatas;
  uint32_t n = abba::get_nr_automaton_in_abba();
  for (size_t i = 0; i < n; i++) {
    /* code */
    struct tima::Automaton* x = &abba::get_automaton_abba(i);
    automatas.push_back(x);
  }
  return automatas;
}

tima::device_initialization_t
abba_inet::get_device_initialization_routine()
{
    return &abba::init_device_data_abba;
}

}
