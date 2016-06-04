#include "dist2mean_inet.h"

namespace dist2mean {

uint32_t get_nr_automaton_in_dist2mean();	
struct tima::Automaton& get_automaton_dist2mean(uint32_t idx);

void
init_device_data_dist2mean(
	std::string& device_name,
	std::map<std::string, std::string>& options,
	std::shared_ptr<tima::GlobalStorage> st);

}

namespace inet {

Define_Module(dist2mean_inet);

std::vector<tima::Automaton*>
dist2mean_inet::build_stl_version()
{
  std::vector<tima::Automaton*> automatas;
  uint32_t n = dist2mean::get_nr_automaton_in_dist2mean();
  for (size_t i = 0; i < n; i++) {
    /* code */
    struct tima::Automaton* x = &dist2mean::get_automaton_dist2mean(i);
    automatas.push_back(x);
  }
  return automatas;
}

tima::device_initialization_t
dist2mean_inet::get_device_initialization_routine()
{
    return &dist2mean::init_device_data_dist2mean;
}

}
