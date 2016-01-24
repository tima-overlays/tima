#ifndef __MAILBOX__
#define __MAILBOX__

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "automata.h"
#include "tima_nature.h"

namespace tima {


class Message {
public:
  int msg_id;
  int automaton_id; // the source
  std::vector<char> payload;
  Message(int m_id, int a_id): msg_id(m_id), automaton_id(a_id) {}
  Message(): msg_id(0), automaton_id(0) {}
};


/**
 * context for actions ! msg : destination
 */
struct SendTimaContext : public TimaNativeContext {
  SendTimaContext(int msg_id, int dst_id, const std::string& dst_name, const std::string& device_name, void* user_data) : TimaNativeContext(device_name,user_data), msg_id(msg_id), dst_id(dst_id), dst_name(dst_name) {}

  int msg_id;
  int dst_id;
  std::string dst_name;
};


/**
 * context for guards (? msg : source) and (# msg)
 */
struct MailboxContext : public TimaNativeContext {
  int msg_id;
  int src_id;
  Message read_message;
  MailboxContext(int a, int b, const std::string& device_name, void* user_data): TimaNativeContext(device_name, user_data), msg_id(a), src_id(b) {}
};


/**
 * context for generic actions
 */
struct GenericActionContext : public TimaNativeContext {
  bool msg_received = false;
  Message msg;
  GenericActionContext(std::string device_name,void* user_data, Message msg, bool msg_received, std::shared_ptr<tima::AbstractTimaNature> nature) : TimaNativeContext(device_name, user_data),  msg_received(msg_received),msg(msg),nature(nature) {}
  void send_to(const std::string& dst, int port, const std::string& rumor);
  void broadcast(int port, std::string& msg);
private:
  std::shared_ptr<tima::AbstractTimaNature> nature;
};

class Mailbox {
private:
  static std::map<std::string, Mailbox*> _singleton;

  std::map< std::string, std::vector<Message> > messages;
  std::vector< Message > network_messages;

public:
  static Mailbox*  get_instance(std::string device_name);
  static bool exists(std::string& name, TimaNativeContext* context);
  static bool exists_network_message(std::string& name, TimaNativeContext* context);
  static void send(std::string& name, TimaNativeContext* context);
  static void add_received_network_message(int msg_id, const char* payload, TimaNativeContext* context);

  void add_automaton(std::string& name);
};

}


#endif
