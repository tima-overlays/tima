
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <memory>
#include <algorithm>
#include <limits>

#include <omnetpp.h>
#include "inet/common/INETDefs.h"

#include "inet/applications/tima/tima.h"
#include "inet/applications/tima/mailbox.h"

#include "dist2mean.h"

using namespace std;
using namespace tima;


#define HEAD {cerr << ctx->get_device_name() << ": ";}

namespace dist2mean {

static const string nil = "";

class Info: public UserData {
public:
	int remaining_hellos;
	map<string, string> payloads;
	map<string, pair<int, int> > coordinates;
    map<string, int> neighbors;
	map<string, set<string>> received_from;

	int threshold = 4000;

	int lastId;    


    Info(int nr_hellos): remaining_hellos(nr_hellos) { }

    virtual std::string computeValue(const std::string& id) override {
        throw runtime_error("unimplemented value : " + id);
    }
};


void
init_device_data_dist2mean(
	string& device_name,
	map<string, string>& options,
	shared_ptr<tima::GlobalStorage> st)
{

	st->setValue("posX", options["posX"]);
	st->setValue("posY", options["posY"]);
	st->setValue("is_source", options["is_source"]);
	st->setValue("radious", options["radious"]);

	auto ud = new Info(stoi(options["nr_hellos"]));

	if (st->getValue("is_source")->to_string() == "true") {
		st->setValue("remaining_broadcasts", options["remaining_broadcasts"]);
	}

	ud->threshold = stoi(options["threshold"]);

	st->setUserData(ud);
}



void
store(const string& name,
	  TimaNativeContext* ctx,
	  string sender,
	  string x, string y)
{

    auto ud = (Info*)ctx->get_user_data();

	auto px = stoi(ctx->get_global("posX"));
	auto py = stoi(ctx->get_global("posY"));

    auto i_x = std::stoi(x);
    auto i_y = std::stoi(y);

    auto d = (i_x - px)*(i_x - px) + (i_y - py)*(i_y - py);

    ud->neighbors.emplace(sender, d);
	ud->coordinates.emplace(sender, make_pair(i_x, i_y));


//	if (ctx->get_device_name() == "hostR13") {
//	    HEAD;
//	    cerr << " ++++ neighbor to " << sender << endl;
//	}
}


void
print(const string& name,
      TimaNativeContext* ctx,
      string msg)
{
    auto ud = (Info*)ctx->get_user_data();
    HEAD;
    cerr << msg << " at " << simTime() << '\n';
}


void
decrease_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	ud->remaining_hellos--;
}




void
dec_counter(const string& name,
      TimaNativeContext* ctx, string c)
{
	auto ud = (Info*)ctx->get_user_data();
	ctx->set_global("newCounter", to_string(stoi(c) - 1));
}


bool
zero_nr_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_hellos == 0;
}


bool
no_zero_nr_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_hellos != 0;
}


static void
send_message(TimaNativeContext* ctx, string& key)
{
	auto ud = (Info*)ctx->get_user_data();

    bool must_send = ud->received_from[key].empty();
    double mx = 0;
    double my = 0;
    if (!must_send) {

        for (auto& s : ud->received_from[key]) {
            auto p = ud->coordinates[s];
            mx += p.first;
            my += p.second;
        }
        mx /= ud->received_from[key].size();
        my /= ud->received_from[key].size();
    }

	auto px = stoi(ctx->get_global("posX"));
	auto py = stoi(ctx->get_global("posY"));

    int dist = (mx - px)*(mx - px) + (my - py)*(my - py);

    must_send = must_send || dist > ud->threshold;

    if (must_send) {

       cout << "====================== Sending in " << ctx->get_device_name() << " because the distance to mean  is " << dist << " > " << ud->threshold << "\n";

       MessageBroadcast m;
       m.payload(ud->payloads[key]);
       m.key(key);
       m.sender(ctx->get_device_name());
       ((ActionContext*)ctx)->broadcast(10000, m);

	   ctx->report_sent_message();

    }
}

void
initial_dissemination(const string& name,
      TimaNativeContext* ctx, string payload)
{
	auto ud = (Info*)ctx->get_user_data();
	string key = ctx->get_device_name() + "-" + to_string(ud->lastId++);
	ud->payloads[key] = payload;
	ctx->set_global("remaining_broadcasts", to_string(stoi(ctx->get_global("remaining_broadcasts")) - 1));
	ctx->report_received_message();
	cerr << "\t >>>>>>>>>>> " << ctx->get_global("remaining_broadcasts") << " at " << simTime() << endl;
	send_message(ctx, key);
}


void
disseminate(const string& name,
      TimaNativeContext* ctx, string key)
{
	auto ud = (Info*)ctx->get_user_data();
    send_message(ctx, key);
}


void
schedule_dissemination(const string& name,
      TimaNativeContext* ctx, string src, string key, string payload)
{
	auto ud = (Info*)ctx->get_user_data();
	ud->received_from[key].insert(src);

	if (!ud->payloads[key].empty()) {
        return;
    }

	ctx->report_received_message();
	ud->payloads[key] = payload;
	ctx->set_global("timeToWait", to_string(2000000 / ud->neighbors[src])); // this is in milliseconds/(m^2) 

	MessageMsg_activate_timer timer;
	timer.src(key);
	timer.count(ctx->get_global("timeToWait"));

	tima::Mailbox::send(timer, "Timer", ctx);

	
    cerr << "Message received at " << ctx->get_device_name() << " (base time " << simTime() << ") and waiting " << ctx->get_global("timeToWait") << " with key " << key << endl;
}




bool
zero_remaining_broadcasts(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ctx->get_global("remaining_broadcasts") == "0";
}

}






