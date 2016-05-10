
#include "tima_nature.h"

#include <sstream>

/** For accessing automata */
struct tima::Automata& get_automata(uint32_t idx);
uint32_t get_nr_automatas();

std::string
tima::AbstractTimaNature::serialize(const tima::Message& msg)
{
  std::stringstream ss;
	ss << msg.msg_id;
	ss << ";";
	for (auto it = msg.fields.begin(),
						itEnd = msg.fields.end() ; it != itEnd ; ++it) {
						ss << it->first << "=" << it->second << ";";
	}
  return ss.str();
}

tima::Message
tima::AbstractTimaNature::deserialize(int msg_id, const std::string& payload)
{
  tima::Message msg(msg_id, 0);
  std::string s(payload);
  auto pos = s.find(';');
  while (pos > 0 && pos != std::string::npos) {
    std::string ss = s.substr(0, pos);
    auto p2 = ss.find('=');
    msg.fields[ss.substr(0, p2)] = ss.substr(p2 + 1);
    s=s.substr(pos+1);
    pos = s.find(';');
  }
  return msg;
}

void
tima::AbstractTimaNature::print_automata(std::vector<tima::Automata*>& automata)
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
tima::AbstractTimaNature::build_stl_version()
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
