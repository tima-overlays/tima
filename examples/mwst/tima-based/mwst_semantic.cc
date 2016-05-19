
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

using namespace tima;

static const string nil = "";

class Info: public UserData {
public:
    enum EdgeStatus {
        Basic, Branch, Rejected
    };
    int posX;
    int posY;
    string fragment;
    map<string, int> neighbors;
    map<string, EdgeStatus> status;
    set<string> requesting;
    set<string> finding;
    set<string> testing;
    string best_neighbor;
    int bw;
    string connecting_with;
    string parent;

    string myself;

    Info(const string& f) {
        bw = 0;
        best_neighbor = "";
        fragment = f;
        myself = f;
    }

    bool isRequesting(const string& i) {
        return find(begin(requesting), end(requesting), i) != end(requesting);
    }

    void removeRequest(const string& i) {
        requesting.erase(i);
    }

    virtual std::string computeValue(const std::string& id) override {
        if (id == "closest") {
            auto m = min_element(begin(neighbors), end(neighbors), [&](pair<const string, int>& e1, pair<const string, int>& e2) {
                return e1.second <= e2.second;
            });

            return (m != end(neighbors)) ? m->first: nil;
        }
        else if (id == "connecting_to") {
            return connecting_with;
        }
        else if (id == "myself") {
            return myself;
        }
        else if (id == "fragmentName") {
            return fragment;
        }
        else if (id == "wait_for_finding") {
            return to_string(finding.size());
        }
        else if (id == "wait_for_test"){
            return to_string(testing.size());
        }
        else if (id == "parent") {
            return parent;
        }
        else if (id == "best_weight") {
            return to_string(bw);
        }
        else if (id == "best_edge") {
            return best_neighbor;
        }
        else if (id == "test_edge") {
            int min_w = numeric_limits<int>::max();
            string test_edge = nil;
            for (auto i : neighbors) {
                auto name = i.first;
                auto w = i.second;
                if (status[name] == Basic
                        && w < min_w) {
                    min_w = w;
                    test_edge = name;
                }
            }
            return test_edge;
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
	auto ud = new Info(device_name);
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
store_requester(
            const std::string& name,
            tima::TimaNativeContext* ctx ,
            std::string j
            )
{
    auto ud = (Info*)ctx->get_user_data();
    ud->requesting.insert(j);
}


void
make_root(
            const std::string& name,
            tima::TimaNativeContext* ctx ,
            std::string a1,
            std::string a2
            )
{
    auto ud = (Info*)ctx->get_user_data();
    ud->parent = nil;
    if (a1 <= a2) {
        ud->fragment = a1 + " <-> " + a2;
    }
    else {
        ud->fragment = a2 + " <-> " + a1;
    }
}

void
make_parent(
            const std::string& name,
            tima::TimaNativeContext* ctx ,
            std::string p
            )
{
    auto ud = (Info*)ctx->get_user_data();
    ud->parent = p;
    ud->status[p] = Info::Branch;
}


void
check_edge(
            const std::string& name,
            tima::TimaNativeContext* ctx ,
            std::string j,
            std::string result
            )
{
    auto ud = (Info*)ctx->get_user_data();
    ud->testing.erase(j);
    if (result == "reject") {
        ud->status[j] = Info::Rejected;
    }
    else if (ud->bw > ud->neighbors[j]) {
        ud->bw = ud->neighbors[j];
        ud->best_neighbor = j;
    }
}

void
check_weight(const string& name,
             TimaNativeContext* ctx ,
             string j, string weight
            )
{
    auto ud = (Info*)ctx->get_user_data();
    ud->finding.erase(j);
    int w = std::stoi(weight);
    if (w < ud->bw) {
        ud->bw = w;
        ud->best_neighbor = j;
    }
}


void
println(const string& name,
	  TimaNativeContext* ctx,
	  string sender,
	  string x, string y)
{
	cout << ctx->get_device_name() << " knows that " << sender << " is located at (" << x << ", " << y << ")" << endl;
}

void
println(const string& name,
      TimaNativeContext* ctx,
      string sender,
      string msg)
{
    auto ud = (Info*)ctx->get_user_data();
    cout << ctx->get_device_name() << ": " << sender << msg << ". In addition: you are connecting to " << ud->connecting_with << endl;
}


void
initiate(const string& name,
         TimaNativeContext* ctx,
         string fragment_name
        )
{
    auto ud = (Info*)ctx->get_user_data();

    // initialization
    ud->connecting_with = nil;
    ud->fragment = fragment_name;
    ud->best_neighbor = "";
    ud->testing.clear();
    ud->finding.clear();
    ud->bw = numeric_limits<int>::max();

    /* send 'initiate' to neighbors in the MST */
    for (auto i : ud->neighbors) {
        auto name = i.first;
        if (ud->parent != name &&
                ud->status[name] == Info::Branch) {

            MessageInitiate m;
            m.sender(ctx->get_device_name());
            m.fragment(fragment_name);

            ((tima::ActionContext*)ctx)->send_to(name, 10000, m);

            ud->finding.emplace(name);
        }
    }

    for (auto i : ud->neighbors) {
        auto name = i.first;
        if (ud->status[name] == Info::Basic) {
            ud->testing.emplace(name);
        }
    }

}


void
send_connect(TimaNativeContext* ctx, const std::string& j)
{

    auto ud = (Info*)ctx->get_user_data();

    if (ud->isRequesting(j)) {
        /*connecting with someone who already sent me connect request */
        ud->status[j] = Info::Branch; // no sure
        /* I'm the root */
        ud->parent = nil;
//      TODO:  initiate(create_unique_name(j, ctx->get_device_name()));
        ud->removeRequest(j);
    }
    else {
//       TODO: SN = States::Connecting;
        ud->connecting_with = j;
        MessageConnect m;
        m.sender(ctx->get_device_name());
        ((ActionContext*)ctx)->send_to(j, 10000, m);
    }

}


void
mark_as_branch(
            const std::string& name,
            tima::TimaNativeContext* ctx ,
            std::string j
            )
{
    auto ud = (Info*)ctx->get_user_data();
    ud->connecting_with = j;
    ud->status[j] = Info::Branch;
}


bool
is_not_root(const std::string& name, tima::TimaNativeContext* ctx)
{
    auto ud = (Info*)ctx->get_user_data();
    return ud->parent != nil;
}

bool
is_root(const std::string& name, tima::TimaNativeContext* ctx)
{
    auto ud = (Info*)ctx->get_user_data();
    return ud->parent == nil;
}


bool
is_basic_and_requesting(const std::string& name, tima::TimaNativeContext* ctx, string j)
{
    auto ud = (Info*)ctx->get_user_data();
    return ud->status[j] == Info::Basic && ud->requesting.find(j) != ud->requesting.end();
}


bool
is_basic_but_not_requesting(const std::string& name, tima::TimaNativeContext* ctx, string j)
{
    auto ud = (Info*)ctx->get_user_data();
    return ud->status[j] == Info::Basic && ud->requesting.find(j) == ud->requesting.end();
}


bool
is_branch(const std::string& name, tima::TimaNativeContext* ctx, string j)
{
    auto ud = (Info*)ctx->get_user_data();
    return ud->status[j] == Info::Branch;
}



