
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

static const string nil = "";

class Info: public UserData {
public:
	string myself;
    int posX;
    int posY;
	int remaining_hellos;
	bool is_source;
	map<string, string> payloads;
	map<string, pair<int, int> > coordinates;
    map<string, int> neighbors;
	map<string, set<string>> received_from;

	int threshold = 4000;

	int lastId;    

	int newCounter;

	int remaining_broadcasts;

	int timeToWait; // in milliseconds

    Info(const string& f, int nr_hellos): myself(f), remaining_hellos(nr_hellos) { }

    virtual std::string computeValue(const std::string& id) override {
        if (id == "myself") {
            return myself;
        }
		else if (id == "is_source") {
			return is_source? "true" : "false";
		}
		else if (id == "newCounter") {
			return to_string(newCounter);
		}
		else if (id == "timeToWait") {
			return to_string(timeToWait);
		}
		else if (id == "remaining_broadcasts") {
			return to_string(remaining_broadcasts);
		}
        return nil;
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
	auto ud = new Info(device_name, std::stoi(options["nr_hellos"]));
	ud->posX = std::stoi(options["posX"]);
	ud->posY = std::stoi(options["posY"]);
	ud->is_source = options["is_source"] == "true";

	if (ud->is_source) {
	    ud->remaining_broadcasts = stoi(options["remaining_broadcasts"]);
	}

	st->setUserData(ud);
}



void
store_pp(const string& name,
	  TimaNativeContext* ctx,
	  string sender,
	  string x, string y)
{

    auto ud = (Info*)ctx->get_user_data();

    auto i_x = std::stoi(x);
    auto i_y = std::stoi(y);

    auto d = (i_x - ud->posX)*(i_x - ud->posX) + (i_y - ud->posY)*(i_y - ud->posY);

    ud->neighbors.emplace(sender, d);
	ud->coordinates.emplace(sender, make_pair(i_x, i_y));


	if (ud->myself == "hostR13") {
	    HEAD;
	    cerr << " ++++ neighbor to " << sender << endl;
	}
}


void
print_p(const string& name,
      TimaNativeContext* ctx,
      string msg)
{
    auto ud = (Info*)ctx->get_user_data();
    HEAD;
    cerr << msg << " at " << simTime() << '\n';
}


void
decrease_hellos_p(const string& name,
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
	ud->newCounter = stoi(c) - 1;
}


bool
zero_nr_hellos_p(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_hellos == 0;
}


bool
no_zero_nr_hellos_p(const string& name,
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

    int dist = (mx - ud->posX)*(mx - ud->posX) + (my - ud->posY)*(my - ud->posY);

    must_send = must_send || dist > ud->threshold;

    if (must_send) {

       cout << "====================== Sending in " << ud->myself << " because the distance to mean  is " << dist << " > " << ud->threshold << "\n";
//        emitSent();
        for (auto& d : ud->neighbors) {
            MessageBroadcast m;
            m.payload(ud->payloads[key]);
            m.key(key);
            m.sender(ud->myself);
            ((ActionContext*)ctx)->send_to(d.first, 10000, m);
            cerr << "\t to " << d.first << endl;
        }

    }
}

void
initial_dissemination_p(const string& name,
      TimaNativeContext* ctx, string payload)
{
	auto ud = (Info*)ctx->get_user_data();
	string key = ud->myself + "-" + to_string(ud->lastId++);
	ud->payloads[key] = payload;
	ud->remaining_broadcasts--;
	cerr << "\t >>>>>>>>>>> " << ud->remaining_broadcasts << endl;
	send_message(ctx, key);
}


void
disseminate_p(const string& name,
      TimaNativeContext* ctx, string key)
{
	auto ud = (Info*)ctx->get_user_data();
    send_message(ctx, key);
}


void
schedule_dissemination_p(const string& name,
      TimaNativeContext* ctx, string src, string key, string payload)
{
	auto ud = (Info*)ctx->get_user_data();
	ud->received_from[key].insert(src);

	if (!ud->payloads[key].empty()) {
        return;
    }

	ud->payloads[key] = payload;
	ud->timeToWait = (2000000 / ud->neighbors[src]); // this is in milliseconds/(m^2)

	MessageMsg_activate_timer timer;
	timer.src(key);
	timer.count(to_string(ud->timeToWait));

	tima::Mailbox::send(timer, "Timer", ctx);

	
    cerr << "Message received at " << ud->myself << " and waiting " << ud->timeToWait << " with key " << key << endl;
}




bool
zero_remaining_broadcasts(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_broadcasts == 0;
}





