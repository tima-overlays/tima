#ifndef __dist2mean__
#define __dist2mean__

#include "inet/applications/tima/automata.h"
#include "inet/applications/tima/tima.h"
#include <string>

namespace dist2mean {

//int	get_msg_id_from_name(const char* name);

/** ID for each automaton */
enum AUTOMATA_ID {
	ANY_AUTOMATON_ID,
	MainPhase_AUTOMATON_ID,
	Timer_AUTOMATON_ID,
	Phase0_AUTOMATON_ID
};

enum MESSAGES_ID {
	ANY_MSG_ID,
	broadcast_MSG_ID,
	msg_activate_timer_MSG_ID,
	msg_wakeup_MSG_ID,
	msg_timer_MSG_ID,
	msg_timer_on_MSG_ID,
	hello_MSG_ID
};

class MessageBroadcast : public tima::Message {
public:
	MessageBroadcast(): Message(broadcast_MSG_ID) {}
	
	std::string sender() {
		return fields["sender"];
	}
	
	void sender(const std::string& v) {
		fields["sender"] = v;
	}
	std::string key() {
		return fields["key"];
	}
	
	void key(const std::string& v) {
		fields["key"] = v;
	}
	std::string payload() {
		return fields["payload"];
	}
	
	void payload(const std::string& v) {
		fields["payload"] = v;
	}
};		

class MessageMsg_activate_timer : public tima::Message {
public:
	MessageMsg_activate_timer(): Message(msg_activate_timer_MSG_ID) {}
	
	std::string count() {
		return fields["count"];
	}
	
	void count(const std::string& v) {
		fields["count"] = v;
	}
	std::string src() {
		return fields["src"];
	}
	
	void src(const std::string& v) {
		fields["src"] = v;
	}
};		

class MessageMsg_wakeup : public tima::Message {
public:
	MessageMsg_wakeup(): Message(msg_wakeup_MSG_ID) {}
	
	std::string sender() {
		return fields["sender"];
	}
	
	void sender(const std::string& v) {
		fields["sender"] = v;
	}
};		

class MessageMsg_timer : public tima::Message {
public:
	MessageMsg_timer(): Message(msg_timer_MSG_ID) {}
	
	std::string n() {
		return fields["n"];
	}
	
	void n(const std::string& v) {
		fields["n"] = v;
	}
	std::string key() {
		return fields["key"];
	}
	
	void key(const std::string& v) {
		fields["key"] = v;
	}
};		

class MessageMsg_timer_on : public tima::Message {
public:
	MessageMsg_timer_on(): Message(msg_timer_on_MSG_ID) {}
	
	std::string key() {
		return fields["key"];
	}
	
	void key(const std::string& v) {
		fields["key"] = v;
	}
};		

class MessageHello : public tima::Message {
public:
	MessageHello(): Message(hello_MSG_ID) {}
	
	std::string sender() {
		return fields["sender"];
	}
	
	void sender(const std::string& v) {
		fields["sender"] = v;
	}
	std::string x() {
		return fields["x"];
	}
	
	void x(const std::string& v) {
		fields["x"] = v;
	}
	std::string y() {
		return fields["y"];
	}
	
	void y(const std::string& v) {
		fields["y"] = v;
	}
};		

}

#endif
