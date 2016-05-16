#ifndef __MAILBOX__
#define __MAILBOX__

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <functional>

#include "automata.h"
#include "tima_nature.h"

namespace tima {

class Mailbox {
private:
  static std::map<std::string, Mailbox*> _singleton;

  std::map< std::string, std::vector<tima::Message> > messages;
  std::vector< tima::Message > network_messages;

public:
  static Mailbox*  get_instance(std::string device_name);

  static bool exists(const std::string& name, TimaNativeContext* context);

  static bool exists2(const std::string& name, TimaNativeContext* context, std::function<bool(tima::Message&)> pred);

  static bool exists_network_message(const std::string& name, TimaNativeContext* context);

  static void send(tima::Message& msg, std::string& target, TimaNativeContext* context);

  static void add_received_network_message(int msg_id, const char* payload, TimaNativeContext* context);


  void add_automaton(std::string& name);

};

class TemporaryActionContext : public TimaNativeContext {
public:
  TemporaryActionContext(std::string device_name,std::shared_ptr<tima::GlobalStorage> st, std::shared_ptr<tima::AbstractTimaNature> nature)
            : TimaNativeContext(device_name, st), nature(nature) {}
  std::shared_ptr<tima::AbstractTimaNature> nature;
};

}


#endif
