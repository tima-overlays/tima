#include "inet/applications/tima/automata.h"
#include "inet/applications/tima/tima.h"
#include "inet/applications/tima/mailbox.h"
#include "dist2mean.h"
#include <cstring>

namespace dist2mean {

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
	std::string tmp1 = GET_FIELD(ctx, "sender");
	std::string tmp2 = GET_FIELD(ctx, "key");
	std::string tmp3 = GET_FIELD(ctx, "payload");
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
MainPhase_received_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from received to Sleeping */
	std::string tmp1 = GET_FIELD(ctx, "key");
	disseminate(
		name, ctx,
		tmp1
	);
}

	
static bool
MainPhase_received_transition_guard_1(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == msg_timer_on_MSG_ID;
		if (b) {
		}
		return b;
	});
	return tmp1;
}


static const struct tima::Transition MainPhase_transitions_for_received[] = {
	{
		dst : 0,
		is_msg_transition : true,
		guard : MainPhase_received_transition_guard_0,
		action : MainPhase_received_transition_0_do
	},
	{
		dst : 2,
		is_msg_transition : true,
		guard : MainPhase_received_transition_guard_1,
		action : MainPhase_received_transition_1_do
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
	std::string tmp1 = "This is a message";
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
				auto tmp1 = ctx->getStorage()->getValue("is_source");
				auto tmp2 = tmp1->to_string();
				std::string tmp3 = "true";
				bool tmp4 = tmp2 == tmp3;
				b = tmp4;
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
MainPhase_Sleeping_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from Sleeping to received */
	std::string tmp1 = GET_FIELD(ctx, "sender");
	std::string tmp2 = GET_FIELD(ctx, "key");
	std::string tmp3 = GET_FIELD(ctx, "payload");
	schedule_dissemination(
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
		dst : 3,
		is_msg_transition : true,
		guard : MainPhase_Sleeping_transition_guard_0,
		action : MainPhase_Sleeping_transition_0_do
	},
	{
		dst : 0,
		is_msg_transition : true,
		guard : MainPhase_Sleeping_transition_guard_1,
		action : MainPhase_Sleeping_transition_1_do
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
		dst : 1,
		is_msg_transition : false,
		guard : MainPhase_still_sending_transition_guard_0,
		action : MainPhase_still_sending_transition_0_do
	}
};



static struct tima::State MainPhase_states[] = {
	{
		name : "received",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 2, // without taking into account the default transition
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
	},
	{
		name : "still_sending",
		urgent : false,
		timeout : 2000, // milliseconds
		timeout_destination : 3,
		timeout_action : MainPhase_still_sending_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&MainPhase_transitions_for_still_sending,
		action : MainPhase_still_sending_do,
	}
};

static struct tima::Automaton MainPhase = {
	name : "MainPhase",
	initial : 2,
	nr_states : 4,
	states : (struct tima::State*)&MainPhase_states
};


/** Automaton Timer */



static void
Timer_checking_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
Timer_checking_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from checking to idle */
	std::string tmp1 = GET_FIELD(ctx, "key");
	MessageMsg_timer_on tmp2;
	tmp2.key(tmp1);
	tima::Mailbox::send(tmp2, "MainPhase", ctx);
}

	
static bool
Timer_checking_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == msg_timer_MSG_ID;
		if (b) {
			if (b) {
				std::string tmp1 = m.get("n");
				std::string tmp2 = "0";
				bool tmp3 = tmp1 == tmp2;
				b = tmp3;
			}
		}
		return b;
	});
	return tmp1;
}

void dec_counter(
			const std::string& name,
			tima::TimaNativeContext* ctx ,
			std::string
			);

static void
Timer_checking_transition_1_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from checking to active */
	std::string tmp1 = GET_FIELD(ctx, "n");
	dec_counter(
		name, ctx,
		tmp1
	);
	auto tmp2 = ctx->getStorage()->getValue("newCounter");
	auto tmp3 = tmp2->to_string();
	std::string tmp4 = GET_FIELD(ctx, "key");
	MessageMsg_timer tmp5;
	tmp5.n(tmp3);
	tmp5.key(tmp4);
	tima::Mailbox::send(tmp5, "Timer", ctx);
}

	
static bool
Timer_checking_transition_guard_1(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == msg_timer_MSG_ID;
		if (b) {
			if (b) {
				std::string tmp1 = m.get("n");
				std::string tmp2 = "0";
				bool tmp3 = tmp1 != tmp2;
				b = tmp3;
			}
		}
		return b;
	});
	return tmp1;
}


static const struct tima::Transition Timer_transitions_for_checking[] = {
	{
		dst : 2,
		is_msg_transition : true,
		guard : Timer_checking_transition_guard_0,
		action : Timer_checking_transition_0_do
	},
	{
		dst : 1,
		is_msg_transition : true,
		guard : Timer_checking_transition_guard_1,
		action : Timer_checking_transition_1_do
	}
};




static void
Timer_active_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
Timer_active_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from active to active */
}

	
static bool
Timer_active_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = false;
	return tmp1;
}



static void
Timer_active_timeout_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}

static const struct tima::Transition Timer_transitions_for_active[] = {
	{
		dst : 1,
		is_msg_transition : false,
		guard : Timer_active_transition_guard_0,
		action : Timer_active_transition_0_do
	}
};




static void
Timer_idle_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}


static void
Timer_idle_transition_0_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	/*transition from idle to active */
	std::string tmp1 = GET_FIELD(ctx, "count");
	std::string tmp2 = GET_FIELD(ctx, "src");
	MessageMsg_timer tmp3;
	tmp3.n(tmp1);
	tmp3.key(tmp2);
	tima::Mailbox::send(tmp3, "Timer", ctx);
}

	
static bool
Timer_idle_transition_guard_0(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
	bool tmp1 = tima::Mailbox::exists2(name, ctx, [&](tima::Message& m){
		bool b = m.msg_id == msg_activate_timer_MSG_ID;
		if (b) {
		}
		return b;
	});
	return tmp1;
}


static const struct tima::Transition Timer_transitions_for_idle[] = {
	{
		dst : 1,
		is_msg_transition : true,
		guard : Timer_idle_transition_guard_0,
		action : Timer_idle_transition_0_do
	}
};



static struct tima::State Timer_states[] = {
	{
		name : "checking",
		urgent : true,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 2, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Timer_transitions_for_checking,
		action : Timer_checking_do,
	},
	{
		name : "active",
		urgent : false,
		timeout : 1, // milliseconds
		timeout_destination : 0,
		timeout_action : Timer_active_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Timer_transitions_for_active,
		action : Timer_active_do,
	},
	{
		name : "idle",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Timer_transitions_for_idle,
		action : Timer_idle_do,
	}
};

static struct tima::Automaton Timer = {
	name : "Timer",
	initial : 2,
	nr_states : 3,
	states : (struct tima::State*)&Timer_states
};


/** Automaton Phase0 */



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
		dst : 3,
		is_msg_transition : false,
		guard : Phase0_s_check_transition_guard_1,
		action : Phase0_s_check_transition_1_do
	}
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
	MessageMsg_wakeup tmp1;
	tima::Mailbox::send(tmp1, "MainPhase", ctx);
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
	auto tmp1 = ctx->getStorage()->getValue("posX");
	auto tmp2 = tmp1->to_string();
	auto tmp3 = ctx->getStorage()->getValue("posY");
	auto tmp4 = tmp3->to_string();
	MessageHello tmp5;
	tmp5.x(tmp2);
	tmp5.y(tmp4);
	tmp5.set("sender",ctx->get_device_name());
	((tima::ActionContext*)ctx)->broadcast(10000, tmp5);
}

static const struct tima::Transition Phase0_transitions_for_greeting[] = {
	{
		dst : 3,
		is_msg_transition : true,
		guard : Phase0_greeting_transition_guard_0,
		action : Phase0_greeting_transition_0_do
	}
};




static void
Phase0_done_do(
			const std::string& name,
			tima::TimaNativeContext* ctx)
{
}



static const struct tima::Transition Phase0_transitions_for_done[] = {
};



static struct tima::State Phase0_states[] = {
	{
		name : "s_check",
		urgent : true,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 2, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_s_check,
		action : Phase0_s_check_do,
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
		timeout : 4900, // milliseconds
		timeout_destination : 4,
		timeout_action : Phase0_s0_0_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_s0_0,
		action : Phase0_s0_0_do,
	},
	{
		name : "greeting",
		urgent : false,
		timeout : 100, // milliseconds
		timeout_destination : 0,
		timeout_action : Phase0_greeting_timeout_do,
		nr_transitions : 1, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_greeting,
		action : Phase0_greeting_do,
	},
	{
		name : "done",
		urgent : false,
		timeout : tima::never_timeout, // milliseconds
		timeout_destination : tima::null_destination,
		timeout_action : nullptr,
		nr_transitions : 0, // without taking into account the default transition
		transitions : (struct tima::Transition*)&Phase0_transitions_for_done,
		action : Phase0_done_do,
	}
};

static struct tima::Automaton Phase0 = {
	name : "Phase0",
	initial : 3,
	nr_states : 5,
	states : (struct tima::State*)&Phase0_states
};


static const uint32_t nr_automaton = 3;

static struct tima::Automaton* automatons [] = {
	&MainPhase,
	&Timer,
	&Phase0
};

uint32_t
get_nr_automaton_in_dist2mean()
{
	return nr_automaton;
}

struct tima::Automaton&
get_automaton_dist2mean(uint32_t idx)
{
	return *automatons[idx];
}

}

