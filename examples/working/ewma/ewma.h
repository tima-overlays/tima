#ifndef __ewma__
#define __ewma__

#include "inet/applications/tima/automata.h"
#include "inet/applications/tima/tima.h"
#include <string>

namespace ewma {

//int	get_msg_id_from_name(const char* name);

/** ID for each automaton */
enum AUTOMATA_ID {
	ANY_AUTOMATON_ID,
	MainPhase_AUTOMATON_ID,
	Phase0_AUTOMATON_ID
};

enum MESSAGES_ID {
	ANY_MSG_ID,
	broadcast_MSG_ID,
	msg_wakeup_MSG_ID,
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
	std::string covered() {
		return fields["covered"];
	}
	
	void covered(const std::string& v) {
		fields["covered"] = v;
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
