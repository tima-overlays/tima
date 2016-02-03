#ifndef __TIMA_CORE__
#define __TIMA_CORE__

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace tima {

struct Message {
public:
  int msg_id;
  int automaton_id; // the source
  std::map< std::string, std::string > fields;
  Message(int m_id, int a_id): msg_id(m_id), automaton_id(a_id) {}
  Message(): msg_id(0), automaton_id(0) {}
};

class TimaNativeContext {
  public:
      TimaNativeContext(std::string device_name, void* user_data) : device_name(device_name), user_data(user_data) {}
      std::string get_device_name() { return device_name; }
      void* get_user_data() { return user_data; }
  private:
      std::string device_name;
      void* user_data;
 };

/**
 * context for actions ! msg : destination
 */
struct SendTimaContext : public TimaNativeContext {
  SendTimaContext(int msg_id, int dst_id, const std::string& dst_name, const std::string& device_name, void* user_data):
            TimaNativeContext(device_name,user_data),
            msg_id(msg_id), dst_id(dst_id),
            dst_name(dst_name) {}

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
  MailboxContext(int a, int b, const std::string& device_name, void* user_data):
          TimaNativeContext(device_name, user_data),
          msg_id(a),
          src_id(b) {}
};

/**
 * context for generic actions
 */
struct ActionContext : public TimaNativeContext {
  bool msg_received = false;
  Message msg;

  ActionContext(std::string device_name,void* user_data, Message msg, bool msg_received);
  virtual void send_to(const std::string& dst, int port, const Message& msg) = 0;
  virtual void broadcast(int port, const Message& msg) = 0;
  virtual void print_trace(const std::string& msg) = 0;

  virtual ~ActionContext() = 0;
};

}

#endif
