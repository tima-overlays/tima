#include "gbb_inet.h"

namespace gbb {

uint32_t get_nr_automaton_in_gbb();	
struct tima::Automaton& get_automaton_gbb(uint32_t idx);

void
init_device_data_gbb(
	std::string& device_name,
	std::map<std::string, std::string>& options,
	std::shared_ptr<tima::GlobalStorage> st);

}

namespace inet {

Define_Module(gbb_inet);

std::vector<tima::Automaton*>
gbb_inet::build_stl_version()
{
  std::vector<tima::Automaton*> automatas;
  uint32_t n = gbb::get_nr_automaton_in_gbb();
  for (size_t i = 0; i < n; i++) {
    /* code */
    struct tima::Automaton* x = &gbb::get_automaton_gbb(i);
    automatas.push_back(x);
  }
  return automatas;
}

tima::device_initialization_t
gbb_inet::get_device_initialization_routine()
{
    return &gbb::init_device_data_gbb;
}

}
