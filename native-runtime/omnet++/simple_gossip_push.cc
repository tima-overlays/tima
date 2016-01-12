#include "automata.h"
#include "mailbox.hpp"
#include "simple_gossip_push.h"
#include <cstring>

/** ID for each automaton */
enum AUTOMATA_ID {
	ANY_AUTOMATON_ID,
	Main_AUTOMATON_ID,
	ticker_gossip_AUTOMATON_ID,
	ticker_newgossip_AUTOMATON_ID,
	ticker_hello_AUTOMATON_ID
};

enum MESSAGES_ID {
	ANY_MSG_ID,
	gossip_time_MSG_ID,
	hello_time_MSG_ID,
	new_rumor_time_MSG_ID,
	hello_MSG_ID,
	rumor_MSG_ID
};

int
get_msg_id_from_name(const char* name)
{
	if (!strcmp(name, "gossip_time"))
		return MESSAGES_ID::gossip_time_MSG_ID;
	if (!strcmp(name, "hello_time"))
		return MESSAGES_ID::hello_time_MSG_ID;
	if (!strcmp(name, "new_rumor_time"))
		return MESSAGES_ID::new_rumor_time_MSG_ID;
	if (!strcmp(name, "hello"))
		return MESSAGES_ID::hello_MSG_ID;
	if (!strcmp(name, "rumor"))
		return MESSAGES_ID::rumor_MSG_ID;
}

/** Automaton Main */

static void
Main_s_remembering_hello_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_remembering_hello_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_remembering_hello_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	C::store_sender(name, ctx);
}

static void
Main_w_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_w_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_w_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s_greeting_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_greeting_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_greeting_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	C::greet(name, ctx);
}

static void
Main_s_gossiping_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_gossiping_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_gossiping_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s_new_gossiping_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_new_gossiping_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_new_gossiping_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	C::spread_new_rumor(name, ctx);
}

static void
Main_s_taking_note_of_rumor_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_taking_note_of_rumor_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_taking_note_of_rumor_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	C::store_rumor(name, ctx);
}

static void
Main_s_loop_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_loop_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_loop_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
Main_s_do_gossip_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_do_gossip_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
Main_s_do_gossip_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	C::gossip(name, ctx);
}

static struct tima::Transition Main_s_remembering_hello[] = {
{
	.dst = 1,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_w[] = {
{
	.dst = 3,
	.guard = tima::Mailbox::exists,
	.msg_id = MESSAGES_ID::gossip_time_MSG_ID,
	.src_id = AUTOMATA_ID::ticker_gossip_AUTOMATON_ID
},
{
	.dst = 2,
	.guard = tima::Mailbox::exists,
	.msg_id = MESSAGES_ID::gossip_time_MSG_ID,
	.src_id = AUTOMATA_ID::ticker_gossip_AUTOMATON_ID
},
{
	.dst = 4,
	.guard = tima::Mailbox::exists,
	.msg_id = MESSAGES_ID::gossip_time_MSG_ID,
	.src_id = AUTOMATA_ID::ticker_gossip_AUTOMATON_ID
},
{
	.dst = 0,
	.guard = tima::Mailbox::exists_network_message,
	.msg_id = MESSAGES_ID::hello_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
},
{
	.dst = 5,
	.guard = tima::Mailbox::exists_network_message,
	.msg_id = MESSAGES_ID::hello_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s_greeting[] = {
{
	.dst = 1,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s_gossiping[] = {
{
	.dst = 7,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s_new_gossiping[] = {
{
	.dst = 1,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s_taking_note_of_rumor[] = {
{
	.dst = 1,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s_loop[] = {
{
	.dst = 1,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition Main_s_do_gossip[] = {
{
	.dst = 1,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::State Main_states[] = {
{
.name = "s_remembering_hello",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s_remembering_hello,
.pre_action = Main_s_remembering_hello_pre_action,
.post_action = Main_s_remembering_hello_post_action,
.each_action = Main_s_remembering_hello_each_action
},
{
.name = "w",
.urgent = false,
.timeout = 1000, // milliseconds
.timeout_destination = 6,
.nr_transitions = 5, // without taking into account the default transition
.transitions = Main_w,
.pre_action = Main_w_pre_action,
.post_action = Main_w_post_action,
.each_action = Main_w_each_action
},
{
.name = "s_greeting",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s_greeting,
.pre_action = Main_s_greeting_pre_action,
.post_action = Main_s_greeting_post_action,
.each_action = Main_s_greeting_each_action
},
{
.name = "s_gossiping",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s_gossiping,
.pre_action = Main_s_gossiping_pre_action,
.post_action = Main_s_gossiping_post_action,
.each_action = Main_s_gossiping_each_action
},
{
.name = "s_new_gossiping",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s_new_gossiping,
.pre_action = Main_s_new_gossiping_pre_action,
.post_action = Main_s_new_gossiping_post_action,
.each_action = Main_s_new_gossiping_each_action
},
{
.name = "s_taking_note_of_rumor",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s_taking_note_of_rumor,
.pre_action = Main_s_taking_note_of_rumor_pre_action,
.post_action = Main_s_taking_note_of_rumor_post_action,
.each_action = Main_s_taking_note_of_rumor_each_action
},
{
.name = "s_loop",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s_loop,
.pre_action = Main_s_loop_pre_action,
.post_action = Main_s_loop_post_action,
.each_action = Main_s_loop_each_action
},
{
.name = "s_do_gossip",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = Main_s_do_gossip,
.pre_action = Main_s_do_gossip_pre_action,
.post_action = Main_s_do_gossip_post_action,
.each_action = Main_s_do_gossip_each_action
}
};

static struct tima::Automata Main = {
	.name = "Main",
	.initial = 1,
	.nr_states = 8,
	.states = Main_states
};


/** Automaton ticker_gossip */

static void
ticker_gossip_s0_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_gossip_s0_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_gossip_s0_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
ticker_gossip_s2_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_gossip_s2_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_gossip_s2_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
ticker_gossip_s1_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_gossip_s1_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_gossip_s1_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	tima::TimaNativeContext* ctx2 = new tima::SendTimaContext(gossip_time_MSG_ID, ticker_gossip_AUTOMATON_ID, "Main");
	tima::Mailbox::send(name, ctx2);
	free(ctx2);
}

static struct tima::Transition ticker_gossip_s0[] = {
{
	.dst = 1,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition ticker_gossip_s2[] = {
{
	.dst = 0,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition ticker_gossip_s1[] = {
{
	.dst = 0,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::State ticker_gossip_states[] = {
{
.name = "s0",
.urgent = false,
.timeout = 700, // milliseconds
.timeout_destination = 2,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_gossip_s0,
.pre_action = ticker_gossip_s0_pre_action,
.post_action = ticker_gossip_s0_post_action,
.each_action = ticker_gossip_s0_each_action
},
{
.name = "s2",
.urgent = false,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_gossip_s2,
.pre_action = ticker_gossip_s2_pre_action,
.post_action = ticker_gossip_s2_post_action,
.each_action = ticker_gossip_s2_each_action
},
{
.name = "s1",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_gossip_s1,
.pre_action = ticker_gossip_s1_pre_action,
.post_action = ticker_gossip_s1_post_action,
.each_action = ticker_gossip_s1_each_action
}
};

static struct tima::Automata ticker_gossip = {
	.name = "ticker_gossip",
	.initial = 0,
	.nr_states = 3,
	.states = ticker_gossip_states
};


/** Automaton ticker_newgossip */

static void
ticker_newgossip_s0_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_newgossip_s0_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_newgossip_s0_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
ticker_newgossip_s2_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_newgossip_s2_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_newgossip_s2_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
ticker_newgossip_s1_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_newgossip_s1_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_newgossip_s1_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	tima::TimaNativeContext* ctx2 = new tima::SendTimaContext(new_rumor_time_MSG_ID, ticker_newgossip_AUTOMATON_ID, "Main");
	tima::Mailbox::send(name, ctx2);
	free(ctx2);
}

static struct tima::Transition ticker_newgossip_s0[] = {
{
	.dst = 1,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition ticker_newgossip_s2[] = {
{
	.dst = 0,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition ticker_newgossip_s1[] = {
{
	.dst = 0,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::State ticker_newgossip_states[] = {
{
.name = "s0",
.urgent = false,
.timeout = 5000, // milliseconds
.timeout_destination = 2,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_newgossip_s0,
.pre_action = ticker_newgossip_s0_pre_action,
.post_action = ticker_newgossip_s0_post_action,
.each_action = ticker_newgossip_s0_each_action
},
{
.name = "s2",
.urgent = false,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_newgossip_s2,
.pre_action = ticker_newgossip_s2_pre_action,
.post_action = ticker_newgossip_s2_post_action,
.each_action = ticker_newgossip_s2_each_action
},
{
.name = "s1",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_newgossip_s1,
.pre_action = ticker_newgossip_s1_pre_action,
.post_action = ticker_newgossip_s1_post_action,
.each_action = ticker_newgossip_s1_each_action
}
};

static struct tima::Automata ticker_newgossip = {
	.name = "ticker_newgossip",
	.initial = 0,
	.nr_states = 3,
	.states = ticker_newgossip_states
};


/** Automaton ticker_hello */

static void
ticker_hello_s0_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_hello_s0_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_hello_s0_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
ticker_hello_s2_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_hello_s2_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_hello_s2_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
}

static void
ticker_hello_s1_pre_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_hello_s1_post_action(std::string& name, tima::TimaNativeContext* ctx)
{
}
static void
ticker_hello_s1_each_action(std::string& name, tima::TimaNativeContext* ctx)
{
	tima::TimaNativeContext* ctx2 = new tima::SendTimaContext(hello_time_MSG_ID, ticker_hello_AUTOMATON_ID, "Main");
	tima::Mailbox::send(name, ctx2);
	free(ctx2);
}

static struct tima::Transition ticker_hello_s0[] = {
{
	.dst = 1,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition ticker_hello_s2[] = {
{
	.dst = 0,
	.guard = C::False,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::Transition ticker_hello_s1[] = {
{
	.dst = 0,
	.guard = C::True,
	.msg_id = MESSAGES_ID::ANY_MSG_ID,
	.src_id = AUTOMATA_ID::ANY_AUTOMATON_ID
}
};

static struct tima::State ticker_hello_states[] = {
{
.name = "s0",
.urgent = false,
.timeout = 1500, // milliseconds
.timeout_destination = 2,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_hello_s0,
.pre_action = ticker_hello_s0_pre_action,
.post_action = ticker_hello_s0_post_action,
.each_action = ticker_hello_s0_each_action
},
{
.name = "s2",
.urgent = false,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_hello_s2,
.pre_action = ticker_hello_s2_pre_action,
.post_action = ticker_hello_s2_post_action,
.each_action = ticker_hello_s2_each_action
},
{
.name = "s1",
.urgent = true,
.timeout = tima::never_timeout, // milliseconds
.timeout_destination = tima::null_destination,
.nr_transitions = 1, // without taking into account the default transition
.transitions = ticker_hello_s1,
.pre_action = ticker_hello_s1_pre_action,
.post_action = ticker_hello_s1_post_action,
.each_action = ticker_hello_s1_each_action
}
};

static struct tima::Automata ticker_hello = {
	.name = "ticker_hello",
	.initial = 0,
	.nr_states = 3,
	.states = ticker_hello_states
};


static const uint32_t nr_automatas = 4;

static struct tima::Automata* automatons [] = {
	&Main,
	&ticker_gossip,
	&ticker_newgossip,
	&ticker_hello
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

