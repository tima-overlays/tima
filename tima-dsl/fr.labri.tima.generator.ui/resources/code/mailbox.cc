#include "mailbox.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cstring>

#include "tima.h"


using std::find_if;
using std::begin;
using std::end;
using std::string;


std::map<string, tima::Mailbox*> tima::Mailbox::_singleton;


bool
tima::Mailbox::exists2(const string& name,
        tima::TimaNativeContext* context,
        std::function<bool(tima::Message&)> pred)
{

    auto ctx = static_cast <MailboxContext*>(context);
//    if (ctx == nullptr) {
//        throw std::runtime_error("Wrong context used in this call");
//    }
    auto inst = get_instance(context->get_device_name());

    auto it = find_if(
                begin(inst->network_messages),
                end(inst->network_messages),
                pred
              );

    bool b = (it != end(inst->network_messages));

    if (b) {
        /* it is a network message */
        ctx->read_message = *it;
        inst->network_messages.erase(it);
    }
    else {
        /* per automaton queue */
        auto queue_automaton = inst->messages.find(name);
        if (queue_automaton != end(inst->messages)) {

            auto it = find_if(
                            begin(queue_automaton->second),
                            end(queue_automaton->second),
                            pred
                          );

            b = (it != end(queue_automaton->second));
            if (b) {
                ctx->read_message = *it;
                queue_automaton->second.erase(it);
//                queue_automaton->second.erase(
//                        std::remove_if(begin(queue_automaton->second), end(queue_automaton->second), pred, end(queue_automaton->second))
//                );
            }
        }
    }

    return b;

}


void
tima::Mailbox::add_received_network_message(int msg_id, const char* payload, TimaNativeContext* context)
{
//  std::cout << msg_id << " Payload " << payload << std::endl;
  TemporaryActionContext* ctx = (TemporaryActionContext*) context;
  Mailbox::get_instance(context->get_device_name())->network_messages.push_back(ctx->nature->deserialize(msg_id, payload));
}


void
tima::Mailbox::send(tima::Message& msg, string target, TimaNativeContext* context)
{
  auto inst = get_instance(context->get_device_name());
  auto it = inst->messages.find(target);
  if (it != inst->messages.end()) {
    // found
//    std::cout << "Adding to the queue " << ctx->msg_id << ctx->dst_id << std::endl;
    it->second.push_back(msg);
  }
  else {
    throw std::runtime_error(string("No automaton with name ") + target);
  }
}

void
tima::Mailbox::add_automaton(string& name)
{
  std::vector<Message> v;
  messages.emplace(name, v);
}

tima::Mailbox*
tima::Mailbox::get_instance(string device_name)
{
  auto it = _singleton.find(device_name);
  if (it == _singleton.end()) {
      _singleton.emplace(device_name, new tima::Mailbox());
  }
  return _singleton[device_name];
}
