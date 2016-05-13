#ifndef __TIMA_CORE__
#define __TIMA_CORE__

#include <string>
#include <vector>
#include <map>
#include <memory>


#define GET_FIELD(ctx, field) (((tima::ActionContext*)ctx)->msg.get(field))
#define GET_GLOBAL(ctx, id) (((tima::ActionContext*)ctx)->get_global(id))

namespace tima {

class Message {
public:
  int msg_id;
  std::map< std::string, std::string > fields;
  Message(int m_id): msg_id(m_id) {}
  Message(): msg_id(0) {}

  std::string& get(const std::string& field) {
    return fields[field];
  }
  void set(const std::string& field, const std::string& v) {
    fields[field] = v;
  }
};

class TimaNativeContext {
  public:
      TimaNativeContext(std::string device_name, void* user_data) : device_name(device_name), user_data(user_data) {}
      std::string get_device_name() { return device_name; }
      void* get_user_data() { return user_data; }
      std::string get_global(const std::string i) {
        return "1123";
      };
  private:
      std::string device_name;
      void* user_data;
 };

/**
 * context for actions ! msg : destination
 */
class SendTimaContext : public TimaNativeContext {
public:
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
class MailboxContext : public TimaNativeContext {
public:
  int msg_id;
  Message read_message;
  MailboxContext(int a, const std::string& device_name, void* user_data):
          TimaNativeContext(device_name, user_data),
          msg_id(a) {}
};

/**
 * context for generic actions
 */
class ActionContext : public TimaNativeContext {
public:
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
