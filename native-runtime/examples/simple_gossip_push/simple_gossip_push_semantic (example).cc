#include "simple_gossip_push.h"
#include "automata.h"
#include "mailbox.h"

#include "inet/applications/base/ApplicationBase.h"

#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

class Content {
public:
    std::set<std::string> known_devices; // those who sent me a hello
    std::map<std::string, int> known_rumors; // someone was gossiping with me
    int destinationPort = 10000;
    bool isSource = false;
    std::string rumor = "Nothing to chat about";
};

void
init_device_data(std::string& device_name,std::map<std::string, std::string>& options, void** ptr)
{
    Content* c = new Content();
    *ptr = c;
    std::for_each(options.begin(), options.end(), [&] (std::pair<std::string, std::string> p) {
       if (p.first == "isSource") {
           c->isSource = p.second=="true";
       }
       else if (p.first == "rumor") {
           c->rumor = p.second;
       }
    });
}

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
  auto content = (Content*)ctx->get_user_data();
  if (ctx->msg_received) {
    std::string device (ctx->msg.payload.begin(), ctx->msg.payload.end());
    content->known_devices.emplace(device);
  }
}

void
C::store_rumor(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  auto content = (Content*)ctx->get_user_data();
  if (ctx->msg_received) {
    std::string rumor(ctx->msg.payload.begin(), ctx->msg.payload.end());
    // adding rumor if it is unknown
    if (content->known_rumors.find(rumor) == content->known_rumors.end()) {
      EV_TRACE << "New Rumor received in "  << context->get_device_name() << " is: " << rumor  << '\n';
      content->known_rumors.emplace(rumor, 25);
    }
  }
}

void
C::greet(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  auto content = (Content*)ctx->get_user_data();
  std::string s = "hello;" + context->get_device_name();
  ctx->broadcast(content->destinationPort, s);
}

void
C::spread_new_rumor(std::string& name, tima::TimaNativeContext* context)
{
  // only node0 is able to generate rumors
  auto content = (Content*)context->get_user_data();
  if (content->isSource) {
    EV_TRACE << context->get_device_name() <<" is trying to spread a rumor"  << '\n';
    content->known_rumors.emplace(content->rumor, 25);
  }
}

void
C::gossip(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  auto content = (Content*)ctx->get_user_data();
  for (auto it = content->known_rumors.begin(), it_end = content->known_rumors.end(); it != it_end ; ++it) {
    auto rumor = (*it).first;
    auto value = (*it).second;
    content->known_rumors[rumor] = value - 1;
    std::for_each(content->known_devices.begin(), content->known_devices.end(), [&] (std::string s) {
        ctx->send_to(s, content->destinationPort, std::string("rumor;")+rumor);
    });
  }
  // remove old rumors
  for(auto it = content->known_rumors.begin(), it_end = content->known_rumors.end(); it != it_end ; ++it) {
      if (it->second == 0) {
         content->known_rumors.erase(it);
      }
  }
}
