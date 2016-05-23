
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <memory>
#include <algorithm>
#include <limits>

#include <omnetpp.h>
#include "inet/common/INETDefs.h"

#include "tima.h"

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
    map<string, int> neighbors;

    string myself;

    Info(const string& f, int nr_hellos): myself(f), remaining_hellos(int nr_hellos) {
    }

    bool isRequesting(const string& i) {
        return find(begin(requesting), end(requesting), i) != end(requesting);
    }

    void removeRequest(const string& i) {
        requesting.erase(i);
    }

	

    virtual std::string computeValue(const std::string& id) override {
        if (id == "myself") {
            return myself;
        }
        return nil;
    }
};


void
init_device_data(
	string& device_name,
	map<string, string>& options,
	shared_ptr<tima::GlobalStorage> st)
{
	st->setValue("posX", options["posX"]);
	st->setValue("posY", options["posY"]);
	auto ud = new Info(device_name, std::stoi(options["nr_hellos"]));
	ud->posX = std::stoi(options["posX"]);
	ud->posY = std::stoi(options["posY"]);
	st->setUserData(ud);
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

    auto d = (i_x - ud->posX)*(i_x - ud->posX) + (i_y - ud->posY)*(i_y - ud->posY);

    ud->neighbors.emplace(sender, d);
}

void
println(const string& name,
      TimaNativeContext* ctx,
      string sender,
      string msg)
{
    auto ud = (Info*)ctx->get_user_data();
    HEAD;
    cerr << sender << msg << ". In addition: you are connecting to " << ud->connecting_with << '\n';
}

void
decrease_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	ud->remaining_hellos--;
}


bool
zero_nr_hellos(const string& name,
      TimaNativeContext* ctx)
{
	auto ud = (Info*)ctx->get_user_data();
	return ud->remaining_hellos != 0;
}
