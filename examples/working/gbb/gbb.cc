#include "inet/applications/tima/automata.h"
#include "inet/applications/tima/tima.h"
#include "inet/applications/tima/mailbox.h"
#include "gbb.h"
#include <cstring>

namespace gbb {

//int
//get_msg_id_from_name(const char* name)
//{
	//	if (!strcmp(name, "broadcast"))
	//		return MESSAGES_ID::broadcast_MSG_ID;
	//	if (!strcmp(name, "msg_activate_timer"))
	//		return MESSAGES_ID::msg_activate_timer_MSG_ID;
	//	if (!strcmp(name, "msg_wakeup"))
	//		return MESSAGES_ID::msg_wakeup_MSG_ID;
	//	if (!strcmp(name, "msg_timer"))
	//		return MESSAGES_ID::msg_timer_MSG_ID;
	//	if (!strcmp(name, "msg_timer_on"))
	//		return MESSAGES_ID::msg_timer_on_MSG_ID;
	//	if (!strcmp(name, "hello"))
	//		return MESSAGES_ID::hello_MSG_ID;
//	return -1;
//}

/** Automaton MainPhase */



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

bool gbb_zero_remaining_broadcasts(
		   const std::string& name, tima::TimaNativeContext* ctx 
		  );
	
static bool
MainPhase_still_sending_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = gbb_zero_remaining_broadcasts(
		name, ctx
	);
	return tmp1;
}


void gbb_initial_dissemination(
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
	gbb_initial_dissemination(
		name, ctx,
		tmp1
	);
}

static const struct tima::Transition MainPhase_transitions_for_still_sending[] = {
	{
		dst : 1,
		is_msg_transition : false,
		guard : MainPhase_still_sending_transition_guard_0,
		action : MainPhase_still_sending_transition_0_do
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




static void
MainPhase_Sleeping_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

void gbb_initial_dissemination(
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
	std::string tmp1 = "This is a message";
	gbb_initial_dissemination(
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

void gbb_disseminate(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string,
			std::string,
			std::string
			);

static void
MainPhase_Sleeping_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from Sleeping to Sleeping */
	std::string tmp1 = GET_FIELD(ctx, "sender");
	std::string tmp2 = GET_FIELD(ctx, "key");
	std::string tmp3 = GET_FIELD(ctx, "payload");
	gbb_disseminate(
		name, ctx,
		tmp1,
		tmp2,
		tmp3
	);
}

	
static bool
MainPhase_Sleeping_transition_guard_1(
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
		dst : 0,
		is_msg_transition : true,
		guard : MainPhase_Sleeping_transition_guard_0,
		action : MainPhase_Sleeping_transition_0_do
	},
	{
		dst : 2,
		is_msg_transition : true,
		guard : MainPhase_Sleeping_transition_guard_1,
		action : MainPhase_Sleeping_transition_1_do
	}
};



static struct tima::State MainPhase_states[] = {
	{
		name : "still_sending",
		urgent : false,
		timeout : 2000, // milliseconds
		timeout_destination : 0,
		timeout_action : MainPhase_still_sending_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&MainPhase_transitions_for_still_sending,
		action : MainPhase_still_sending_do,
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
	},
	{
		name : "Sleeping",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 2, // without taking into account the default transition
		transitions : (struct tima::Transition*)&MainPhase_transitions_for_Sleeping,
		action : MainPhase_Sleeping_do,
	}
};

static struct tima::Automaton MainPhase = {
	name : "MainPhase",
	initial : 2,
	nr_states : 3,
	states : (struct tima::State*)&MainPhase_states
};


/** Automaton phase0 */



static void
phase0_s0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

void gbb_store(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string,
			std::string,
			std::string
			);

static void
phase0_s0_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s0 to s0 */
	std::string tmp1 = GET_FIELD(ctx, "sender");
	std::string tmp2 = GET_FIELD(ctx, "x");
	std::string tmp3 = GET_FIELD(ctx, "y");
	gbb_store(
		name, ctx,
		tmp1,
		tmp2,
		tmp3
	);
}

	
static bool
phase0_s0_transition_guard_0(
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
phase0_s0_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s0 to s0 */
}

	
static bool
phase0_s0_transition_guard_1(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = false;
	return tmp1;
}



static void
phase0_s0_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

static const struct tima::Transition phase0_transitions_for_s0[] = {
	{
		dst : 0,
		is_msg_transition : true,
		guard : phase0_s0_transition_guard_0,
		action : phase0_s0_transition_0_do
	},
	{
		dst : 0,
		is_msg_transition : false,
		guard : phase0_s0_transition_guard_1,
		action : phase0_s0_transition_1_do
	}
};




static void
phase0_s0_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
phase0_s0_0_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s0_0 to s0 */
}

	
static bool
phase0_s0_0_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = false;
	return tmp1;
}



static void
phase0_s0_0_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	MessageMsg_wakeup tmp1;
	tima::Mailbox::send(tmp1, "MainPhase", ctx);
}

static const struct tima::Transition phase0_transitions_for_s0_0[] = {
	{
		dst : 0,
		is_msg_transition : false,
		guard : phase0_s0_0_transition_guard_0,
		action : phase0_s0_0_transition_0_do
	}
};




static void
phase0_s_check_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
phase0_s_check_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s_check to s0 */
}

bool gbb_zero_nr_hellos(
		   const std::string& name, tima::TimaNativeContext* ctx 
		  );
	
static bool
phase0_s_check_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = gbb_zero_nr_hellos(
		name, ctx
	);
	return tmp1;
}


static void
phase0_s_check_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from s_check to greeting */
}

bool gbb_no_zero_nr_hellos(
		   const std::string& name, tima::TimaNativeContext* ctx 
		  );
	
static bool
phase0_s_check_transition_guard_1(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = gbb_no_zero_nr_hellos(
		name, ctx
	);
	return tmp1;
}


static const struct tima::Transition phase0_transitions_for_s_check[] = {
	{
		dst : 0,
		is_msg_transition : false,
		guard : phase0_s_check_transition_guard_0,
		action : phase0_s_check_transition_0_do
	},
	{
		dst : 3,
		is_msg_transition : false,
		guard : phase0_s_check_transition_guard_1,
		action : phase0_s_check_transition_1_do
	}
};




static void
phase0_greeting_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

void gbb_store(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string,
			std::string,
			std::string
			);

static void
phase0_greeting_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from greeting to greeting */
	std::string tmp1 = GET_FIELD(ctx, "sender");
	std::string tmp2 = GET_FIELD(ctx, "x");
	std::string tmp3 = GET_FIELD(ctx, "y");
	gbb_store(
		name, ctx,
		tmp1,
		tmp2,
		tmp3
	);
}

	
static bool
phase0_greeting_transition_guard_0(
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


void gbb_decrease_hellos(
			const std::string& name,
			tima::TimaNativeContext* ctx 
			);

static void
phase0_greeting_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	gbb_decrease_hellos(
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

static const struct tima::Transition phase0_transitions_for_greeting[] = {
	{
		dst : 3,
		is_msg_transition : true,
		guard : phase0_greeting_transition_guard_0,
		action : phase0_greeting_transition_0_do
	}
};




static void
phase0_done_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}



static const struct tima::Transition phase0_transitions_for_done[] = {
};



static struct tima::State phase0_states[] = {
	{
		name : "s0",
		urgent : false,
		timeout : 100, // milliseconds
		timeout_destination : 1,
		timeout_action : phase0_s0_timeout_do,
		nr_transitions : 2, // without taking into account the default transition
		transitions : (struct tima::Transition*)&phase0_transitions_for_s0,
		action : phase0_s0_do,
	},
	{
		name : "s0_0",
		urgent : true,
		timeout : 4900, // milliseconds
		timeout_destination : 4,
		timeout_action : phase0_s0_0_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&phase0_transitions_for_s0_0,
		action : phase0_s0_0_do,
	},
	{
		name : "s_check",
		urgent : true,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 2, // without taking into account the default transition
		transitions : (struct tima::Transition*)&phase0_transitions_for_s_check,
		action : phase0_s_check_do,
	},
	{
		name : "greeting",
		urgent : false,
		timeout : 100, // milliseconds
		timeout_destination : 2,
		timeout_action : phase0_greeting_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&phase0_transitions_for_greeting,
		action : phase0_greeting_do,
	},
	{
		name : "done",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 0, // without taking into account the default transition
		transitions : (struct tima::Transition*)&phase0_transitions_for_done,
		action : phase0_done_do,
	}
};

static struct tima::Automaton phase0 = {
	name : "phase0",
	initial : 3,
	nr_states : 5,
	states : (struct tima::State*)&phase0_states
};


static const uint32_t nr_automaton = 2;

static struct tima::Automaton* automatons [] = {
	&MainPhase,
	&phase0
};

uint32_t
get_nr_automaton_in_gbb()
{
	return nr_automaton;
}

struct tima::Automaton&
get_automaton_gbb(uint32_t idx)
{
	return *automatons[idx];
}

}

