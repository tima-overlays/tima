#ifndef __simple_gossip_push__
#define __simple_gossip_push__

#include "automata.h"
#include <string>

int	get_msg_id_from_name(const char* name);

class C {
public:
	static void store_sender(std::string& name, tima::TimaNativeContext* context);
	static bool True(std::string& name, tima::TimaNativeContext* context);
	static void greet(std::string& name, tima::TimaNativeContext* context);
	static void gossip(std::string& name, tima::TimaNativeContext* context);
	static void store_rumor(std::string& name, tima::TimaNativeContext* context);
	static bool False(std::string& name, tima::TimaNativeContext* context);
	static void spread_new_rumor(std::string& name, tima::TimaNativeContext* context);
};

#endif
