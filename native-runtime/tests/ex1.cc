#include "automata.h"
#include "mailbox.hpp"
#include "ex1.h"
#include <cstring>

/** ID for each automaton */
enum AUTOMATA_ID {
	ANY_AUTOMATON_ID,
	Main_AUTOMATON_ID,
	Secondary_AUTOMATON_ID
};

enum MESSAGES_ID {
	ANY_MSG_ID,
	a_MSG_ID,
	z_MSG_ID
};

int
get_msg_id_from_name(const char* name)
{
	if (!strcmp(name, "a"))
		return MESSAGES_ID::a_MSG_ID;
	if (!strcmp(name, "z"))
		return MESSAGES_ID::z_MSG_ID;
}

/** Automaton Main */

static void
Main_s3_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s3_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s3_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s4_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s4_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s4_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s0_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s0_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s0_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s5_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s5_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s5_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	D::y(name, ctx);
}

static void
Main_s1_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s1_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s1_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	D::x(name, ctx);
}

static void
Main_s2_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s2_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s2_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s0_500_0_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s0_500_0_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s0_500_0_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s0_200_1_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s0_200_1_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s0_200_1_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static struct tima::Transition Main_s3[] = {
{
	.dst = 1,
	.guard = C::d,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s4[] = {
{
	.dst = 2,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s0[] = {
{
	.dst = 4,
	.guard = tima::Mailbox::exists,
	.msg_id = MESSAGES_ID::a_MSG_ID,
	.src_id = AUTOMATA_ID::Secondary_AUTOMATON_ID
},
{
	.dst = 5,
	.guard = C::b,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
},
{
	.dst = 3,
	.guard = tima::Mailbox::exists_network_message,
	.msg_id = MESSAGES_ID::z_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s5[] = {
{
	.dst = 2,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s1[] = {
{
	.dst = 2,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s2[] = {
{
	.dst = 4,
	.guard = C::c,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s0_500_0[] = {
{
	.dst = 5,
	.guard = C::b,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
},
{
	.dst = 3,
	.guard = tima::Mailbox::exists_network_message,
	.msg_id = MESSAGES_ID::z_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s0_200_1[] = {
{
	.dst = 5,
	.guard = C::b,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::State Main_states[] = {
{
.name = "s3",
.urgent = false,
.timeout = 1500, // milliseconds
.timeout_destination = 2,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s3,
.pre_action = Main_s3_pre_action,
.post_action = Main_s3_post_action,
.each_action = Main_s3_each_action
},
{
.name = "s4",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s4,
.pre_action = Main_s4_pre_action,
.post_action = Main_s4_post_action,
.each_action = Main_s4_each_action
},
{
.name = "s0",
.urgent = false,
.timeout = 500, // milliseconds
.timeout_destination = 6,
.nr_transitions = 3, // without taking into account the default transition
.transitions = Main_s0,
.pre_action = Main_s0_pre_action,
.post_action = Main_s0_post_action,
.each_action = Main_s0_each_action
},
{
.name = "s5",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s5,
.pre_action = Main_s5_pre_action,
.post_action = Main_s5_post_action,
.each_action = Main_s5_each_action
},
{
.name = "s1",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s1,
.pre_action = Main_s1_pre_action,
.post_action = Main_s1_post_action,
.each_action = Main_s1_each_action
},
{
.name = "s2",
.urgent = false,
.timeout = 400, // milliseconds
.timeout_destination = 0,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s2,
.pre_action = Main_s2_pre_action,
.post_action = Main_s2_post_action,
.each_action = Main_s2_each_action
},
{
.name = "s0_500_0",
.urgent = false,
.timeout = 200, // milliseconds
.timeout_destination = 7,
.nr_transitions = 2, // without taking into account the default transition
.transitions = Main_s0_500_0,
.pre_action = Main_s0_500_0_pre_action,
.post_action = Main_s0_500_0_post_action,
.each_action = Main_s0_500_0_each_action
},
{
.name = "s0_200_1",
.urgent = false,
.timeout = 300, // milliseconds
.timeout_destination = 0,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s0_200_1,
.pre_action = Main_s0_200_1_pre_action,
.post_action = Main_s0_200_1_post_action,
.each_action = Main_s0_200_1_each_action
}
};

static struct tima::Automata Main = {
	.name = "Main",
	.initial = 2,
	.nr_states = 8,
	.states = Main_states
};


/** Automaton Secondary */

static void
Secondary_s0_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Secondary_s0_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Secondary_s0_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Secondary_s2_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Secondary_s2_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Secondary_s2_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Secondary_s1_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Secondary_s1_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Secondary_s1_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	tima::TimaNativeContext* ctx2 = new tima::SendTimaContext(a_MSG_ID, Secondary_AUTOMATON_ID, "Main");
	tima::Mailbox::send(name, ctx2);
	free(ctx2);
}

static struct tima::Transition Secondary_s0[] = {
{
	.dst = 1,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Secondary_s2[] = {
{
	.dst = 0,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Secondary_s1[] = {
{
	.dst = 0,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::State Secondary_states[] = {
{
.name = "s0",
.urgent = false,
.timeout = 700, // milliseconds
.timeout_destination = 2,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Secondary_s0,
.pre_action = Secondary_s0_pre_action,
.post_action = Secondary_s0_post_action,
.each_action = Secondary_s0_each_action
},
{
.name = "s2",
.urgent = false,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Secondary_s2,
.pre_action = Secondary_s2_pre_action,
.post_action = Secondary_s2_post_action,
.each_action = Secondary_s2_each_action
},
{
.name = "s1",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Secondary_s1,
.pre_action = Secondary_s1_pre_action,
.post_action = Secondary_s1_post_action,
.each_action = Secondary_s1_each_action
}
};

static struct tima::Automata Secondary = {
	.name = "Secondary",
	.initial = 0,
	.nr_states = 3,
	.states = Secondary_states
};


static const uint32_t nr_automatas = 2;

static struct tima::Automata* automatons [] = {
	&Main,
	&Secondary
};

uint32_t
get_nr_automatas()
{
	return nr_automatas;
}

struct tima::Automata&
get_automata(uint32_t idx)
{
	return *automatons[idx];
}

