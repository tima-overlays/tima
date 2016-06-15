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
 	virtual std::string computeValue(const std::string& id) {
        throw runtime_error("unimplemented value : " + id);
    }
};

class GlobalStorage {

public:
    class Value {
    private:
        std::map< std::string, std::shared_ptr<Value> > values;
        std::string value;
    public:
        
        void setValue(std::string key, std::shared_ptr<Value> v) {
            values[key] = v;
        } 
        void setSimpleValue(std::string v) {
            values.clear();
            value = v;
        } 
        std::shared_ptr<Value> getValue(std::string key) {
            if (values.find(key) != values.end()) {
                return values[key];
            }
            throw std::runtime_error("Unknown key: " + key);
        } 
        
        std::string to_string() {
            if (values.size() > 0)
                throw std::runtime_error("This is a composed value");
            return value;
        }

		std::map< std::string, std::shared_ptr<Value> > getValues() {
			return values;
		}
        
        
    };
    
private:
    
    Value root;

    UserData* user_data;

public:
    GlobalStorage(UserData* ud) : user_data(ud) {}

    std::shared_ptr<Value> getValue(const std::string& key) {
        return root.getValue(key);
    }
    
    void setValue(const std::string key, std::string v) {
        auto value = std::make_shared<Value>();
        value->setSimpleValue(v);
        root.setValue(key, value);
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
        return storage->getValue(i)->to_string();
      };
      
      
      void set_global(const std::string i, std::string v) {
        storage->setValue(i, v);
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
