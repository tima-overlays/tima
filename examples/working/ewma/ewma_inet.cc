#include "ewma_inet.h"

namespace ewma {

uint32_t get_nr_automaton_in_ewma();	
struct tima::Automaton& get_automaton_ewma(uint32_t idx);

void
init_device_data_ewma(
	std::string& device_name,
	std::map<std::string, std::string>& options,
	std::shared_ptr<tima::GlobalStorage> st);

}

namespace inet {

Define_Module(ewma_inet);

std::vector<tima::Automaton*>
ewma_inet::build_stl_version()
{
  std::vector<tima::Automaton*> automatas;
  uint32_t n = ewma::get_nr_automaton_in_ewma();
  for (size_t i = 0; i < n; i++) {
    /* code */
    struct tima::Automaton* x = &ewma::get_automaton_ewma(i);
    automatas.push_back(x);
  }
  return automatas;
}

tima::device_initialization_t
ewma_inet::get_device_initialization_routine()
{
    return &ewma::init_device_data_ewma;
}

}
