
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

#include "ewma.h"

using namespace std;
using namespace tima;


#define HEAD {cerr << ctx->get_device_name() << ": ";}

namespace ewma {

static const string nil = "";

class Info: public UserData {
public:
	int remaining_hellos;
	int remaining_broadcasts;
	map<string, string> payloads;
    map<string, int> neighbors;
    map<string, set<string> > covered;
    set<string> children;

    int lastId = 0;

    Info(int nr_hellos): remaining_hellos(nr_hellos) {
    }

    virtual std::string computeValue(const std::string& id) override {
        return nil;
    }
};

static void
process_list_of_nodes(string s, set<string>& l);


void
init_device_data_ewma(
	string& device_name,
	map<string, string>& options,
	shared_ptr<tima::GlobalStorage> st)
{
	st->setValue("posX", options["posX"]);
	st->setValue("posY", options["posY"]);
	st->setValue("is_source", options["is_source"]);
	st->setValue("myself", device_name);

	auto h = stoi(options["nr_hellos"]);
	Info* ud = new Info(h);
    process_list_of_nodes(options["children"], ud->children);

	if (st->getValue("is_source")->to_string() == "true") { 
	    ud->remaining_broadcasts = stoi(options["remaining_broadcasts"]);
	}

	st->setUserData(ud);
}


static void
process_list_of_nodes(string s, set<string>& l)
{
    size_t p = s.find_first_of(",");
    while (p != string::npos) {
        string child = s.substr(0, p);
        cerr << "\t a child " << child << endl;
        l.insert(child);
        s = s.substr(p+1, string::npos);
        p = s.find_first_of(",");
    }
    if (!s.empty()) {
        cerr << "\t a child " << s << endl;
        l.insert(s);
    }
}


void
store(const string& name,
	  TimaNativeContext* ctx,
	  string sender,
	  string x, string y)
{

    auto ud = (Info*)ctx->get_user_data();

    auto i_x = std::stoi(x);
    auto i_y = std::stoi(y);

    auto px = stoi(ctx->get_global("posX"));
    auto py = stoi(ctx->get_global("posY"));

    auto d = (i_x - px)*(i_x - px) + (i_y - py)*(i_y - py);

    ud->neighbors.emplace(sender, d);
}


void
print(const string& name,
      TimaNativeContext* ctx,
      string sender,
      string msg)
{
    auto ud = (Info*)ctx->get_user_data();
    HEAD;
    cerr << sender << msg << '\n';
    for (auto& p: ud->neighbors) {
        cerr << "\t" << p.first << " is at " << p.second << " units." << endl;
    }
    cerr << "List of children: " << endl;
    for (auto& x: ud->children) {
        cerr << " a children is " << x << endl;
    }
}


void
decrease_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	ud->remaining_hellos--;
	HEAD;
	cout << ud->remaining_hellos << " =========  hellos left" << endl;
}


static void
send_message(TimaNativeContext* ctx, const set<string>& dst, string key)
{
    auto ud = (Info*)ctx->get_user_data();
    set<string> previous;
    for (auto& d: ud->covered[key]) previous.insert(d);
    for (auto& d : ud->neighbors) {
        ud->covered[key].insert(d.first);
    }

    if (dst.size() > 0) {

        /*only send if some children in the mst are not covered, but send to all*/
        bool must_send = false;
        for (auto& d : ud->children) {
            if (previous.find(d) == previous.end()) {
                must_send = true;
                break;
            }
        }
        if (must_send) {
            cerr << "====================== Sending in " << ctx->get_global("myself") << "\n";

            MessageBroadcast msg;
            msg.sender(ctx->get_global("myself"));
            msg.key(key);
            msg.payload(ud->payloads[key]);
            string cc;
            for (auto& c : ud->covered[key]) cc += c + ",";

            msg.covered(cc.substr(0, cc.size() - 1));

            ((ActionContext*)ctx)->broadcast(10000, msg);
            ctx->report_sent_message();

        }
    }
}


void
initial_dissemination(const string& name,
      TimaNativeContext* ctx, string payload)
{
	auto ud = (Info*)ctx->get_user_data();
	string key = ctx->get_global("myself") + "-" + to_string(ud->lastId++);
	ud->covered[key].insert(ctx->get_global("myself"));
	ud->payloads[key] = payload;
	ud->remaining_broadcasts--;
	ctx->report_received_message();
	send_message(ctx, ud->children, key);
}


void
disseminate(const string& name,
      TimaNativeContext* ctx, string key)
{
	auto ud = (Info*)ctx->get_user_data();

	/* send to members of (local_mst - covered) */
    set<string> v;
    for (auto& d: ud->children) {
        if (ud->covered[key].find(d) == ud->covered[key].end()) {
            v.insert(d);
        }
    }
    send_message(ctx, v, key);
}


void
schedule_dissemination(const string& name,
      TimaNativeContext* ctx, string key, string payload, string covered)
{
	auto ud = (Info*)ctx->get_user_data();

	if (!ud->payloads[key].empty()) return;

	ctx->report_received_message();

    ud->payloads[key] = payload;

    if (ctx->get_global("is_source") == "true" || ud->children.size() > 0) {
        process_list_of_nodes(covered, ud->covered[key]);
    }

    ctx->set_global("currentKey", key);
    cerr << "Message received at " << ctx->get_global("myself") << " with key " << key  << " => " << covered << endl;
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


bool
zero_remaining_broadcasts(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_broadcasts == 0;
}

}







