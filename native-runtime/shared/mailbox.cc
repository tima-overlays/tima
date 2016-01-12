#include "mailbox.hpp"

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cstring>

tima::Mailbox* tima::Mailbox::_singleton = nullptr;

bool
tima::Mailbox::exists(std::string& name, TimaNativeContext* context)
{
  // std::cerr << "Executing this " << std::endl;
  auto ctx = (MailboxContext*) context;
  auto it = Mailbox::get_instance()->messages.find(name);
  if (it != Mailbox::get_instance()->messages.end()) {
    // found
    std::vector<Message>::iterator it0 = it->second.begin();
    std::vector<Message>::iterator it1 = it->second.end();

    bool b = std::any_of(it0, it1,
                       [&] (Message e)
                       {
                        //  std::cerr << "\t\t" << ctx->msg_id << " " << ctx->src_id << std::endl;
                        //  std::cerr << "\t\t" << e.msg_id << " " << e.automaton_id << std::endl;
                         return e.msg_id == ctx->msg_id && e.automaton_id == ctx->src_id;
                       }
    );

    // remove found element

    it->second.erase(std::remove_if(it0, it1, [&] (Message e)
                              {
                                std::cerr << "\t\t" << ctx->msg_id << " " << ctx->src_id << std::endl;
                                // std::cerr << "\t\t" << e.msg_id << " " << e.automaton_id << std::endl;
                                return e.msg_id == ctx->msg_id && e.automaton_id == ctx->src_id;
                              }
    ), it->second.end());

    if (it->second.size() > 0)
      std::cerr << "\tFound ? " << b << " size " << it->second.size() << std::endl;
    return b;
  }
  throw std::runtime_error(std::string("No automaton with name ") + name);
}

bool
tima::Mailbox::exists_network_message(std::string& name, TimaNativeContext* context)
{
  auto ctx = (MailboxContext*) context;
  auto inst = get_instance();
  auto it = std::find_if(inst->network_messages.begin(),
                  inst->network_messages.end(), [&] (Message e) {
                    return ctx->msg_id == e.msg_id;
                  });

  if (it != inst->network_messages.end()) {
    ctx->read_message = *it;
    get_instance()->network_messages.erase(it);
    return true;
  }
  return false;
}

void
tima::Mailbox::add_received_network_message(int msg_id, char* payload)
{
  Message msg(msg_id, 0);
  while (*payload) {
    msg.payload.push_back(*payload);
    payload++;
  }
  Mailbox::get_instance()->network_messages.push_back(msg);
}

void
tima::Mailbox::send(std::string& name, TimaNativeContext* context)
{
  auto ctx = (SendTimaContext*)context;
  auto dst = ctx->dst_name;
  auto it = Mailbox::get_instance()->messages.find(dst);
  if (it != Mailbox::get_instance()->messages.end()) {
    // found
    // std::cout << "Adding to the queue" << std::endl;
    it->second.push_back(Message(ctx->msg_id, ctx->dst_id));
  }
  else {
    throw std::runtime_error(std::string("No automaton with name ") + name);
  }
}


tima::Mailbox*
tima::Mailbox::get_instance()
{
  if (_singleton == nullptr) {
    _singleton = new tima::Mailbox();
  }
  return _singleton;
}

void
tima::Mailbox::add_automaton(std::string& name)
{
  std::vector<Message> v;
  messages.emplace(name, v);
}
