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


class UserData {
public:
    virtual std::string computeValue(const std::string& id) = 0;
};

class GlobalStorage {

    std::map<std::string, std::string> data;

    UserData* user_data;

public:

    GlobalStorage(UserData* ud) : user_data(ud) {}

    void setValue(const std::string& key, const std::string v) {
        data[key] = v;
    }

    std::string getValue(const std::string& key) {
        if (data.find(key) == data.end()) {
            return user_data->computeValue(key);
        }
        return data[key];
    }

    UserData* getUserData() {
        return user_data;
    }

    void setUserData(UserData* v) {
        user_data = v;
    }

};


class TimaNativeContext {
  public:

      TimaNativeContext(const std::string& device_name, std::shared_ptr<GlobalStorage> st) :
              device_name(device_name), storage(st) {}

      std::string get_device_name() { return device_name; }

      void* get_user_data() {
          return storage->getUserData();
      }

      std::shared_ptr<GlobalStorage> getStorage() {
           return storage;
       }

      std::string get_global(const std::string i) {
        return storage->getValue(i);
      };

      virtual void report_received_message() { }

      virtual void report_sent_message() { }

      virtual ~TimaNativeContext() {}

  private:
      std::string device_name;
      std::shared_ptr<GlobalStorage> storage;
 };


/**
 * context for guards (? msg : source) and (# msg)
 */
class MailboxContext : public TimaNativeContext {
public:
  Message read_message;
  MailboxContext(const std::string& device_name, std::shared_ptr<GlobalStorage> st):
          TimaNativeContext(device_name, st) {}
};

/**
 * context for generic actions
 */
class ActionContext : public TimaNativeContext {
public:
  bool msg_received = false;
  Message msg;

  ActionContext(std::string device_name,std::shared_ptr<GlobalStorage> st, Message msg, bool msg_received);

  virtual void send_to(const std::string& dst, int port, const Message& msg) = 0;

  virtual void broadcast(int port, const Message& msg) = 0;

  virtual void print_trace(const std::string& msg) = 0;

  virtual ~ActionContext() = 0;
};

}

#endif
