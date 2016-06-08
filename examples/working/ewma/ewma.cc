#include "inet/applications/tima/automata.h"
#include "inet/applications/tima/tima.h"
#include "inet/applications/tima/mailbox.h"
#include "ewma.h"
#include <cstring>

namespace ewma {

//int
//get_msg_id_from_name(const char* name)
//{
	//	if (!strcmp(name, "broadcast"))
	//		return MESSAGES_ID::broadcast_MSG_ID;
	//	if (!strcmp(name, "msg_wakeup"))
	//		return MESSAGES_ID::msg_wakeup_MSG_ID;
	//	if (!strcmp(name, "hello"))
	//		return MESSAGES_ID::hello_MSG_ID;
//	return -1;
//}

/** Automaton MainPhase */



static void
MainPhase_Sleeping_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

void initial_dissemination(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string
			);

static void
MainPhase_Sleeping_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from Sleeping to still_sending */
	std::string tmp1 = "This is the message";
	initial_dissemination(
		name, ctx,
		tmp1
	);
}

	
static bool
MainPhase_Sleeping_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == msg_wakeup_MSG_ID;
		if (b) {
			if (b) {
				std::string tmp1 = GET_GLOBAL(ctx, "is_source");
				std::string tmp2 = "true";
				bool tmp3 = tmp1 == tmp2;
				b = tmp3;
			}
		}
		return b;
	});
	return tmp1;
}


static void
MainPhase_Sleeping_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from Sleeping to Sleeping */
}

	
static bool
MainPhase_Sleeping_transition_guard_1(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == msg_wakeup_MSG_ID;
		if (b) {
			if (b) {
				std::string tmp1 = GET_GLOBAL(ctx, "is_source");
				std::string tmp2 = "false";
				bool tmp3 = tmp1 == tmp2;
				b = tmp3;
			}
		}
		return b;
	});
	return tmp1;
}

void schedule_dissemination(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string,
			std::string,
			std::string
			);

static void
MainPhase_Sleeping_transition_2_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from Sleeping to received */
	std::string tmp1 = GET_FIELD(ctx, "key");
	std::string tmp2 = GET_FIELD(ctx, "payload");
	std::string tmp3 = GET_FIELD(ctx, "covered");
	schedule_dissemination(
		name, ctx,
		tmp1,
		tmp2,
		tmp3
	);
}

	
static bool
MainPhase_Sleeping_transition_guard_2(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == broadcast_MSG_ID;
		if (b) {
		}
		return b;
	});
	return tmp1;
}


static const struct tima::Transition MainPhase_transitions_for_Sleeping[] = {
	{
		dst : 1,
		is_msg_transition : true,
		guard : MainPhase_Sleeping_transition_guard_0,
		action : MainPhase_Sleeping_transition_0_do
	},
	{
		dst : 0,
		is_msg_transition : true,
		guard : MainPhase_Sleeping_transition_guard_1,
		action : MainPhase_Sleeping_transition_1_do
	},
	{
		dst : 2,
		is_msg_transition : true,
		guard : MainPhase_Sleeping_transition_guard_2,
		action : MainPhase_Sleeping_transition_2_do
	}
};




static void
MainPhase_still_sending_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
MainPhase_still_sending_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from still_sending to stop */
}

bool zero_remaining_broadcasts(
		   const std::string& name, tima::TimaNativeContext* ctx 
		  );
	
static bool
MainPhase_still_sending_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = zero_remaining_broadcasts(
		name, ctx
	);
	return tmp1;
}


void initial_dissemination(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string
			);

static void
MainPhase_still_sending_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	std::string tmp1 = "This is a message";
	initial_dissemination(
		name, ctx,
		tmp1
	);
}

static const struct tima::Transition MainPhase_transitions_for_still_sending[] = {
	{
		dst : 3,
		is_msg_transition : false,
		guard : MainPhase_still_sending_transition_guard_0,
		action : MainPhase_still_sending_transition_0_do
	}
};




static void
MainPhase_received_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

void schedule_dissemination(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string,
			std::string,
			std::string
			);

static void
MainPhase_received_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from received to received */
	std::string tmp1 = GET_GLOBAL(ctx, "currentKey");
	std::string tmp2 = GET_FIELD(ctx, "payload");
	std::string tmp3 = GET_FIELD(ctx, "covered");
	schedule_dissemination(
		name, ctx,
		tmp1,
		tmp2,
		tmp3
	);
}

	
static bool
MainPhase_received_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == broadcast_MSG_ID;
		if (b) {
			if (b) {
				std::string tmp1 = m.get("key");
				std::string tmp2 = GET_GLOBAL(ctx, "currentKey");
				bool tmp3 = tmp1 == tmp2;
				b = tmp3;
			}
		}
		return b;
	});
	return tmp1;
}


void disseminate(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string
			);

static void
MainPhase_received_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	std::string tmp1 = GET_GLOBAL(ctx, "currentKey");
	disseminate(
		name, ctx,
		tmp1
	);
}

static const struct tima::Transition MainPhase_transitions_for_received[] = {
	{
		dst : 2,
		is_msg_transition : true,
		guard : MainPhase_received_transition_guard_0,
		action : MainPhase_received_transition_0_do
	}
};




static void
MainPhase_stop_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}



static const struct tima::Transition MainPhase_transitions_for_stop[] = {
};



static struct tima::State MainPhase_states[] = {
	{
		name : "Sleeping",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 3, // without taking into account the default transition
		transitions : (struct tima::Transition*)&MainPhase_transitions_for_Sleeping,
		action : MainPhase_Sleeping_do,
	},
	{
		name : "still_sending",
		urgent : false,
		timeout : 1800, // milliseconds
		timeout_destination : 1,
		timeout_action : MainPhase_still_sending_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&MainPhase_transitions_for_still_sending,
		action : MainPhase_still_sending_do,
	},
	{
		name : "received",
		urgent : false,
		timeout : 200, // milliseconds
		timeout_destination : 0,
		timeout_action : MainPhase_received_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&MainPhase_transitions_for_received,
		action : MainPhase_received_do,
	},
	{
		name : "stop",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 0, // without taking into account the default transition
		transitions : (struct tima::Transition*)&MainPhase_transitions_for_stop,
		action : MainPhase_stop_do,
	}
};

static struct tima::Automaton MainPhase = {
	name : "MainPhase",
	initial : 0,
	nr_states : 4,
	states : (struct tima::State*)&MainPhase_states
};


/** Automaton Phase0 */



static void
Phase0_done_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}



static const struct tima::Transition Phase0_transitions_for_done[] = {
};




static void
Phase0_s0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

void store(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string,
			std::string,
			std::string
			);

static void
Phase0_s0_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s0 to s0 */
	std::string tmp1 = GET_FIELD(ctx, "sender");
	std::string tmp2 = GET_FIELD(ctx, "x");
	std::string tmp3 = GET_FIELD(ctx, "y");
	store(
		name, ctx,
		tmp1,
		tmp2,
		tmp3
	);
}

	
static bool
Phase0_s0_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == hello_MSG_ID;
		if (b) {
		}
		return b;
	});
	return tmp1;
}


static void
Phase0_s0_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s0 to s0 */
}

	
static bool
Phase0_s0_transition_guard_1(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = false;
	return tmp1;
}



static void
Phase0_s0_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

static const struct tima::Transition Phase0_transitions_for_s0[] = {
	{
		dst : 1,
		is_msg_transition : true,
		guard : Phase0_s0_transition_guard_0,
		action : Phase0_s0_transition_0_do
	},
	{
		dst : 1,
		is_msg_transition : false,
		guard : Phase0_s0_transition_guard_1,
		action : Phase0_s0_transition_1_do
	}
};




static void
Phase0_s0_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
Phase0_s0_0_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s0_0 to s0 */
}

	
static bool
Phase0_s0_0_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = false;
	return tmp1;
}



static void
Phase0_s0_0_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

static const struct tima::Transition Phase0_transitions_for_s0_0[] = {
	{
		dst : 1,
		is_msg_transition : false,
		guard : Phase0_s0_0_transition_guard_0,
		action : Phase0_s0_0_transition_0_do
	}
};




static void
Phase0_initialized_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	MessageMsg_wakeup tmp1;
	tima::Mailbox::send(tmp1, "MainPhase", ctx);
}


static void
Phase0_initialized_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from initialized to done */
}

	
static bool
Phase0_initialized_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = true;
	return tmp1;
}


static const struct tima::Transition Phase0_transitions_for_initialized[] = {
	{
		dst : 0,
		is_msg_transition : false,
		guard : Phase0_initialized_transition_guard_0,
		action : Phase0_initialized_transition_0_do
	}
};




static void
Phase0_greeting_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

void store(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string,
			std::string,
			std::string
			);

static void
Phase0_greeting_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from greeting to greeting */
	std::string tmp1 = GET_FIELD(ctx, "sender");
	std::string tmp2 = GET_FIELD(ctx, "x");
	std::string tmp3 = GET_FIELD(ctx, "y");
	store(
		name, ctx,
		tmp1,
		tmp2,
		tmp3
	);
}

	
static bool
Phase0_greeting_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == hello_MSG_ID;
		if (b) {
		}
		return b;
	});
	return tmp1;
}


void decrease_hellos(
			const std::string& name,
			tima::TimaNativeContext* ctx 
			);

static void
Phase0_greeting_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	decrease_hellos(
		name, ctx
	);
	std::string tmp1 = GET_GLOBAL(ctx, "posX");
	std::string tmp2 = GET_GLOBAL(ctx, "posY");
	MessageHello tmp3;
	tmp3.x(tmp1);
	tmp3.y(tmp2);
	tmp3.set("sender",ctx->get_device_name());
	((tima::ActionContext*)ctx)->broadcast(10000, tmp3);
}

static const struct tima::Transition Phase0_transitions_for_greeting[] = {
	{
		dst : 4,
		is_msg_transition : true,
		guard : Phase0_greeting_transition_guard_0,
		action : Phase0_greeting_transition_0_do
	}
};




static void
Phase0_s_check_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
Phase0_s_check_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s_check to s0 */
}

bool zero_nr_hellos(
		   const std::string& name, tima::TimaNativeContext* ctx 
		  );
	
static bool
Phase0_s_check_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = zero_nr_hellos(
		name, ctx
	);
	return tmp1;
}


static void
Phase0_s_check_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s_check to greeting */
}

bool no_zero_nr_hellos(
		   const std::string& name, tima::TimaNativeContext* ctx 
		  );
	
static bool
Phase0_s_check_transition_guard_1(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = no_zero_nr_hellos(
		name, ctx
	);
	return tmp1;
}


static const struct tima::Transition Phase0_transitions_for_s_check[] = {
	{
		dst : 1,
		is_msg_transition : false,
		guard : Phase0_s_check_transition_guard_0,
		action : Phase0_s_check_transition_0_do
	},
	{
		dst : 4,
		is_msg_transition : false,
		guard : Phase0_s_check_transition_guard_1,
		action : Phase0_s_check_transition_1_do
	}
};



static struct tima::State Phase0_states[] = {
	{
		name : "done",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 0, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_done,
		action : Phase0_done_do,
	},
	{
		name : "s0",
		urgent : false,
		timeout : 100, // milliseconds
		timeout_destination : 2,
		timeout_action : Phase0_s0_timeout_do,
		nr_transitions : 2, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_s0,
		action : Phase0_s0_do,
	},
	{
		name : "s0_0",
		urgent : true,
		timeout : 6900, // milliseconds
		timeout_destination : 3,
		timeout_action : Phase0_s0_0_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_s0_0,
		action : Phase0_s0_0_do,
	},
	{
		name : "initialized",
		urgent : true,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_initialized,
		action : Phase0_initialized_do,
	},
	{
		name : "greeting",
		urgent : false,
		timeout : 100, // milliseconds
		timeout_destination : 5,
		timeout_action : Phase0_greeting_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_greeting,
		action : Phase0_greeting_do,
	},
	{
		name : "s_check",
		urgent : true,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 2, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_s_check,
		action : Phase0_s_check_do,
	}
};

static struct tima::Automaton Phase0 = {
	name : "Phase0",
	initial : 4,
	nr_states : 6,
	states : (struct tima::State*)&Phase0_states
};


static const uint32_t nr_automaton = 2;

static struct tima::Automaton* automatons [] = {
	&MainPhase,
	&Phase0
};

uint32_t
get_nr_automaton_in_ewma()
{
	return nr_automaton;
}

struct tima::Automaton&
get_automaton_ewma(uint32_t idx)
{
	return *automatons[idx];
}

}

