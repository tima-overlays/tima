#include "simple_gossip_push.h"
#include "automata.h"
#include "mailbox.hpp"


#include <sstream>
#include <map>
#include <set>
#include <vector>

static std::set<std::string> known_devices; // those who sent me a hello
static std::set<std::string> known_rumors; // someone was gosiiping with me

static int generated_rumors = 1;

bool
C::True(std::string& name, tima::TimaNativeContext* context)
{
  return true;
}

bool
C::False(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

void
C::store_sender(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  if (ctx->msg_received) {
    std::cout << "Sender is : ";
    std::string device;
    for (auto it = ctx->msg.payload.begin(), itEnd = ctx->msg.payload.end(); it != itEnd ; ++it) {
      std::cout << *it;
      device += *it;
    }
    std::cout << std::endl;
    known_devices.emplace(device);
  }
}

void
C::store_rumor(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  if (ctx->msg_received) {
    std::cout << "Rumor is : ";
    std::string rumor;
    for (auto it = ctx->msg.payload.begin(), itEnd = ctx->msg.payload.end(); it != itEnd ; ++it) {
      std::cout << *it;
      rumor += *it;
    }
    std::cout << std::endl;
    if (known_rumors.find(rumor) == known_rumors.end()) {
      known_rumors.emplace(rumor, 5);
    }
  }
}

void
C::greet(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  std::sringstream str;
  str << "hello" << ";" << ctx->get_device_name();
  std::string s;
  str >> s;
  ctx->broadcast(s.c_str();
}

void
C::spread_new_rumor(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  // only node0 is able to generate rumors
  if (ctx->get_device_name() == "node0" && generated_rumors > 0 ) {
    std::s = "rumor";
    known_rumors.emplace(s, 5);
  }
}

void
C::gossip(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  for (auto it = known_rumors.begin(), it_end = known_rumors.end(); it != it_end ; ++it) {
    auto rumor = (*it).first;
    auto value = (*it).second;
    known_rumors[rumor] = value - 1;
    // choose a random device to gossip
    int idx = (int)(known_devices.size()*(std::rand()/((double)(RAND_MAX))));
    for (auto it = known_devices.begin(), it_end = known_devices.end(); it != it_end ; ++it) {
      if (idx == 0) {
        ctx->send_to(*it, rumor);
        break;
      }
      idx--;
    }
  }
  // remove old rumors
  known_rumors.erase(std::remove_if(known_rumors.begin(), known_rumors.end(), [&] (std::pair<std::string, int> p)
                        {
                          return p.second == 0;
                        }
  ), known_rumors.end());
}
