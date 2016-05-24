
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

static const string nil = "";

class Info: public UserData {
public:
    int posX;
    int posY;
	int remaining_hellos;
	bool is_source;
	string payload;
    map<string, int> neighbors;
    set<string> covered;
    set<string> children;

    string myself;

    Info(const string& f, int nr_hellos): myself(f), remaining_hellos(nr_hellos) {
    }

    virtual std::string computeValue(const std::string& id) override {
        if (id == "myself") {
            return myself;
        }
		else if (id == "is_source") {
			return is_source? "true" : "false";
		}
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
	auto ud = new Info(device_name, std::stoi(options["nr_hellos"]));
	ud->posX = std::stoi(options["posX"]);
	ud->posY = std::stoi(options["posY"]);
	ud->is_source = options["is_source"] == "true";

    process_list_of_nodes(options["children"], ud->children);

	st->setUserData(ud);
}


static void
process_list_of_nodes(string s, set<string>& l)
{
    size_t p = s.find_first_of(",");
    while (p != string::npos) {
        string child = s.substr(0, p);
        l.insert(child);
        s = s.substr(p+1, string::npos);
        p = s.find_first_of(",");
    }
    if (!s.empty()) {
        l.insert(s);
    }
}


void
store_p(const string& name,
	  TimaNativeContext* ctx,
	  string sender,
	  string x, string y)
{

    auto ud = (Info*)ctx->get_user_data();

    auto i_x = std::stoi(x);
    auto i_y = std::stoi(y);

    auto d = (i_x - ud->posX)*(i_x - ud->posX) + (i_y - ud->posY)*(i_y - ud->posY);

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
	//HEAD;
	// cout << ud->remaining_hellos << " =========  hellos left" << endl;
}


static void
send_message(TimaNativeContext* ctx, const set<string>& dst, string payload)
{
    auto ud = (Info*)ctx->get_user_data();
    set<string> previous;
    for (auto& d: ud->covered) previous.insert(d);
    for (auto& d : ud->neighbors) {
        ud->covered.insert(d.first);
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
            cerr << "====================== Sending in " << ud->myself << "\n";
            for (auto& c : ud->covered) {
                cerr << "\t The following is covered: " << c << "\n";
            }
            MessageBroadcast msg;
            msg.sender(ud->myself);
            msg.payload(payload);
            string cc;
            for (auto& c : ud->covered) cc += c + ",";

            msg.covered(cc.substr(0, cc.size() - 1));

            ((ActionContext*)ctx)->broadcast(10000, msg);
        }
    }
}


void
initial_dissemination(const string& name,
      TimaNativeContext* ctx, string payload)
{
	auto ud = (Info*)ctx->get_user_data();
	//HEAD;
	// cout << ud->remaining_hellos << " =========  hellos left" << endl;
	ud->covered.insert(ud->myself);
	send_message(ctx, ud->children, payload);
}


void
disseminate(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	//HEAD;
	// cout << ud->remaining_hellos << " =========  hellos left" << endl;

	/* send to members of (local_mst - covered) */
    set<string> v;
    for (auto& d: ud->children) {
        if (ud->covered.find(d) == ud->covered.end()) {
            v.insert(d);
        }
    }
    send_message(ctx, v, ud->payload);
}


void
schedule_dissemination(const string& name,
      TimaNativeContext* ctx, string src, string payload, string covered)
{
	auto ud = (Info*)ctx->get_user_data();
	//HEAD;
	// cout << ud->remaining_hellos << " =========  hellos left" << endl;
	if (!ud->payload.empty()) {
        return;
    }
    ud->payload = payload;
    if (ud->is_source || ud->children.size() > 0) {
        process_list_of_nodes(covered, ud->covered);
    }
    cerr << "Message received at " << ud->myself << " => " << covered << endl;
}


void
reschedule_dissemination(const string& name,
      TimaNativeContext* ctx, string src, string payload, string covered)
{
	auto ud = (Info*)ctx->get_user_data();
    if (!ud->payload.empty()) {
        return;
    }
    ud->payload = payload;
    if (ud->is_source || ud->children.size() > 0) {
        process_list_of_nodes(covered, ud->covered);
    }
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
