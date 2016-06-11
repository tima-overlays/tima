
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <memory>
#include <algorithm>
#include <limits>
#include <chrono>
#include <random>

#include <omnetpp.h>
#include "inet/common/INETDefs.h"

#include "inet/applications/tima/tima.h"
#include "inet/applications/tima/mailbox.h"

#include "gbb.h"

using namespace std;
using namespace tima;


#define HEAD {cerr << ctx->get_device_name() << ": ";}

namespace gbb {

class Info: public UserData {
public:
	int remaining_hellos;
	map<string, string> payloads;
	map<string, bool> sent;
	map<string, pair<int, int> > coordinates;
    map<string, int> neighbors;

	int lastId = 0;    

	int k;

    Info(int nr_hellos):remaining_hellos(nr_hellos) { }

    virtual std::string computeValue(const std::string& id) override {
		throw runtime_error("unimplemented value : " + id);
    }
};


void
init_device_data_gbb(
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

	ud->k = stoi(options["k"]);

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

    bool must_send = (ctx->get_global("is_source") == "true" && !ud->payloads[key].empty()) || (!ud->sent[key]);

    if (must_send) {

		ud->sent[key] = true;

		vector<string> v;
		for (auto& s: ud->neighbors) {
			v.push_back(s.first);		
		}

		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		minstd_rand0 generator (seed); // minstd_rand0 is a standard linear_congruential_engine
		uniform_int_distribution<int> dist(0, v.size() - 1);

		auto n = min((int)ud->k, (int)v.size());

        cerr << "====================== Sending in " << ctx->get_device_name() << "\n";
		for (auto i = 0 ; i < n ; i++) {
			auto s = v[dist(generator)];
            MessageBroadcast m;
            m.payload(ud->payloads[key]);
            m.key(key);
            m.sender(ctx->get_device_name());
            ((ActionContext*)ctx)->send_to(s, 10000, m);
            cerr << "\t to " << s << endl;
        }
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
      TimaNativeContext* ctx, string sender, string key, string payload)
{
	auto ud = (Info*)ctx->get_user_data();

	if (!ud->payloads[key].empty()) {
        return;
    }

	ud->sent[key] = false;
	ud->payloads[key] = payload;
	ctx->report_received_message();

    send_message(ctx, key);
	cerr << "Message received at " << ctx->get_device_name() << " (base time " << simTime() << ") " << " with key " << key << endl;
}



bool
zero_remaining_broadcasts(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ctx->get_global("remaining_broadcasts") == "0";
}

}
