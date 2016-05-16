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
tima::Mailbox::exists(
        const string& name,
        tima::TimaNativeContext* context)
{
  // std::cerr << "Executing this " << std::endl;
  auto ctx = (MailboxContext*) context;
  auto inst = get_instance(context->get_device_name());
  auto it = inst->messages.find(name);
  if (it != inst->messages.end()) {
    // found
    auto it0 = it->second.begin();
    auto it1 = it->second.end();

    if (it->second.size() == 0) return exists_network_message(name, context);

    auto f = it->second.front();
    if (f.msg_id == ctx->msg_id) {
        ctx->read_message = f;
        it->second.erase(std::remove_if(it0, it1, [&] (Message e)
                                  {
                                    return e.msg_id == ctx->msg_id; //&& e.automaton_id == ctx->src_id;
                                  }
        ), it->second.end());
        return true;
    }
    return exists_network_message(name, context);
  }
  throw std::runtime_error(string("No automaton with name ") + name);
}


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
                inst->network_messages.begin(),
                inst->network_messages.end(),
                pred
              );

    bool b = (it != inst->network_messages.end());

    if (b) {
        ctx->read_message.msg_id = it->msg_id;
        ctx->read_message.fields = it->fields;
        inst->network_messages.erase(it);
    }

    return b;

}


bool
tima::Mailbox::exists_network_message(const string& name, TimaNativeContext* context)
{
  // check if the first message has the properties I want
  auto ctx = (MailboxContext*) context;
  auto inst = get_instance(context->get_device_name());
  if (inst->network_messages.size() == 0) {
      return false;
  }
  if (inst->network_messages.front().msg_id  == ctx->msg_id ) {
      ctx->read_message = inst->network_messages.front();
      inst->network_messages.erase(inst->network_messages.begin());
      return true;
  }
  return false;
}


void
tima::Mailbox::add_received_network_message(int msg_id, const char* payload, TimaNativeContext* context)
{
  std::cout << msg_id << " Payload " << payload << std::endl;
  TemporaryActionContext* ctx = (TemporaryActionContext*) context;
  Mailbox::get_instance(context->get_device_name())->network_messages.push_back(ctx->nature->deserialize(msg_id, payload));
}


void
tima::Mailbox::send(tima::Message& msg, string& target, TimaNativeContext* context)
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
