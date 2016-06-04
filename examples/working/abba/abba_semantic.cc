
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

#include "abba.h"

using namespace std;
using namespace tima;


#define HEAD {cerr << ctx->get_device_name() << ": ";}

namespace abba {

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
    std::map< std::string, std::set< std::pair<double, double> > > received_from;

	int lastId;    

	int newCounter;

	int remaining_broadcasts;

	int timeToWait; // in milliseconds

	int radious;

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
		throw runtime_error("unimplemented value : " + id);
    }
};


void
init_device_data_abba(
	string& device_name,
	map<string, string>& options,
	shared_ptr<tima::GlobalStorage> st)
{
	st->setValue("posX", options["posX"]);
	st->setValue("posY", options["posY"]);
	auto ud = new Info(device_name, std::stoi(options["nr_hellos"]));
	ud->posX = std::stoi(options["posX"]);
	ud->posY = std::stoi(options["posY"]);
	ud->radious = stoi(options["radious"]);

	ud->is_source = options["is_source"] == "true";

	if (ud->is_source) {
	    ud->remaining_broadcasts = stoi(options["remaining_broadcasts"]);

	}

	st->setUserData(ud);
}



void
abba_store(const string& name,
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
}


void
abba_print(const string& name,
      TimaNativeContext* ctx,
      string msg)
{
    auto ud = (Info*)ctx->get_user_data();
    HEAD;
    cerr << msg << " at " << simTime() << '\n';
}


void
abba_decrease_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	ud->remaining_hellos--;
}




void
abba_dec_counter(const string& name,
      TimaNativeContext* ctx, string c)
{
	auto ud = (Info*)ctx->get_user_data();
	ud->newCounter = stoi(c) - 1;
}


bool
abba_zero_nr_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_hellos == 0;
}


bool
abba_no_zero_nr_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_hellos != 0;
}


static void
send_message(TimaNativeContext* ctx, string& key)
{
	auto ud = (Info*)ctx->get_user_data();

    bool must_send = !ud->received_from[key].empty() || (ud->is_source && !ud->payloads[key].empty());

    if (must_send) {

       cout << "====================== Sending in " << ud->myself << " because there are some uncovered points " << ud->received_from[key].size() << "\n";
//        emitSent();
       MessageBroadcast m;
       m.payload(ud->payloads[key]);
       m.key(key);
       m.sender(ud->myself);
       m.x(to_string(ud->posX));
       m.y(to_string(ud->posY));
       ((ActionContext*)ctx)->broadcast(10000, m);
	   ctx->report_sent_message();	

//        for (auto& d : ud->neighbors) {
//
//            cerr << "\t to " << d.first << endl;
//        }

    }
}

void
abba_initial_dissemination(const string& name,
      TimaNativeContext* ctx, string payload)
{
	auto ud = (Info*)ctx->get_user_data();
	string key = ud->myself + "-" + to_string(ud->lastId++);
	ud->payloads[key] = payload;
	ud->remaining_broadcasts--;
	ctx->report_received_message();
	cerr << "\t >>>>>>>>>>> " << ud->remaining_broadcasts << " at " << simTime() << endl;
	send_message(ctx, key);
}


void
abba_disseminate(const string& name,
      TimaNativeContext* ctx, string key)
{
	auto ud = (Info*)ctx->get_user_data();
    send_message(ctx, key);
}


void
abba_schedule_dissemination(const string& name,
      TimaNativeContext* ctx, string src, string key, string payload, string rx, string ry)
{

#define  PIPI  3.141592653589793238463

	auto ud = (Info*)ctx->get_user_data();

	bool firstTime = !ud->is_source && ud->payloads[key].empty();

	auto r = ud->radious;

    if (firstTime) {
        double angle = 0;
        double delta = 2*PIPI / 36;
        while (angle < 2*PIPI) {
            auto y = std::sin(angle)*r;
            auto x = std::cos(angle)*r;
            ud->received_from[key].insert(make_pair(x+ud->posX, y + ud->posY));
            angle += delta;
        }
    }



    auto a = stod(rx);
    auto b = stod(ry);

    for (auto i = ud->received_from[key].begin(), f = ud->received_from[key].end() ; i != f ; ++i) {
        auto x = i->first;
        auto y = i->second;

        if ((x - a)*(x - a) + (y - b)*(y - b) < r*r) {
            ud->received_from[key].erase(i);
        }
    }


	if (!ud->payloads[key].empty()) {
        return;
    }

	ctx->report_received_message();

	ud->payloads[key] = payload;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::minstd_rand0 generator (seed); // minstd_rand0 is a standard linear_congruential_engine
    std::uniform_real_distribution<double> distribution(0.1,0.5);
	ud->timeToWait = distribution(generator); // this is in milliseconds/(m^2)

	MessageMsg_activate_timer timer;
	timer.src(key);
	timer.count(to_string(ud->timeToWait));

	tima::Mailbox::send(timer, "Timer", ctx);

	
    cerr << "Message received at " << ud->myself << " (base time " << simTime() << ") and waiting " << ud->timeToWait << " with key " << key << endl;
}




bool
abba_zero_remaining_broadcasts(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_broadcasts == 0;
}

}
