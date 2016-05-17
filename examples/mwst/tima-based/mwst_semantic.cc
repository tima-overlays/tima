
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <memory>
#include <algorithm>
#include <limits>

#include "tima.h"

#include "mwst.h"

using namespace std;

class Info {
public:
    enum EdgeStatus {
        Basic, Branch, Rejected
    };
    int posX;
    int posY;
    string fragment;
    map<string, int> neighbors;
    map<string, EdgeStatus> status;
    set<string> finding;
    set<string> testing;
    string best_neighbor;
    int bw;

    Info(const string& f) {
        bw = 0;
        best_neighbor = "";
        fragment = f;
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
	auto ud = new Info(device_name);
	ud->posX = std::stoi(options["posX"]);
	ud->posY = std::stoi(options["posY"]);
	st->setUserData(ud);
}


void
store(const string& name,
	  tima::TimaNativeContext* ctx,
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
	  tima::TimaNativeContext* ctx,
	  string sender,
	  string x, string y)
{
	cout << ctx->get_device_name() << " knows that " << sender << " is located at (" << x << ", " << y << ")" << endl;
}


void
identify(const std::string& name,
         tima::TimaNativeContext* ctx
        )
{
    auto ud = (Info*)ctx->get_user_data();

    // initialization
    ud->best_neighbor = "";
    ud->bw = numeric_limits<int>::max();

    /* send 'initiate' to neighbors in the MST */
    MessageInitiate m;
    m.set("sender", ctx->get_device_name());
    m.fragment(ud->fragment);

    for_each(begin(ud->neighbors), end(ud->neighbors), [&](pair<const string, int>& p) {

        if (ud->status[p.first] == Info::Branch) {
            ((tima::ActionContext*)ctx)->send_to(p.first, 10000, m);
            ud->finding.emplace(p.first);
            cout << " sending initiate to " << p.first << endl;
        }

    });

    /* send 'test' to neighbors in the MST */
    MessageTest m2;
    m2.set("sender", ctx->get_device_name());
    m2.fragment(ud->fragment);

    for_each(begin(ud->neighbors), end(ud->neighbors), [&](pair<const string, int>& p) {

        if (ud->status[p.first] == Info::Basic) {
            ud->testing.emplace(p.first);
        }

    });

    if (ud->testing.size() > 0) {
        auto dst = *ud->testing.begin();
        cout << ctx->get_device_name() << " is sending 'test' to " << dst << endl;
        ((tima::ActionContext*)ctx)->send_to(dst, 10000, m2);
        ud->testing.erase(ud->testing.begin());
    }

}


void
accept(const std::string& name,
       tima::TimaNativeContext* ctx,
       std::string j
      )
{

}


void
reject(const std::string& name,
       tima::TimaNativeContext* ctx,
       std::string j
      )
{

}


void
report(const std::string& name,
       tima::TimaNativeContext* ctx,
       std::string,
       std::string
      )
{

}


bool
must_report(const std::string& name, tima::TimaNativeContext* ctx)
{
    return false;
}


