
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

namespace dist2mean {


void
statistic_message_sent (TimaNativeContext* ctx)
{
	ctx->report_sent_message();
}


void
statistic_message_received (TimaNativeContext* ctx)
{
	ctx->report_received_message();
}


int
compute_distance(TimaNativeContext* ctx, int x1, int y1, int x2, int y2)
{
	return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
}


bool
compute_distance_to_mean(TimaNativeContext* ctx, string key)
{
    auto st = ctx->getStorage();

    auto received = st->getValue("received");
    auto coordinates = st->getValue("coordinates");

    auto broadcast_session = received->getValue(key)->getValue("from");


    double mx = 0;
    double my = 0;
    int count = 0;

    for (auto& s : broadcast_session->getValues()) {
        auto p = coordinates->getValue(s.first);
		auto x = p->getValue("x");
		auto y = p->getValue("y");
        mx += x.to_int();
        my += y.to_int();
		count ++;
    }

    mx /= count;
    my /= count;

    auto px = stoi(ctx->get_global("posX"));
    auto py = stoi(ctx->get_global("posY"));

    return (px - mx)*(px - mx) + (py - my)*(py - my); 
}

} /* namespace */
