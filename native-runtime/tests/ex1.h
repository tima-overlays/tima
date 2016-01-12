#ifndef __ex1__
#define __ex1__

#include "automata.h"
#include <string>

int	get_msg_id_from_name(const char* name);

class C {
public:
	static bool b(std::string& name, tima::TimaNativeContext* context);
	static bool c(std::string& name, tima::TimaNativeContext* context);
	static bool d(std::string& name, tima::TimaNativeContext* context);
	static bool True(std::string& name, tima::TimaNativeContext* context);
	static bool False(std::string& name, tima::TimaNativeContext* context);
};

class D {
public:
	static void x(std::string& name, tima::TimaNativeContext* context);
	static void y(std::string& name, tima::TimaNativeContext* context);
};

#endif
