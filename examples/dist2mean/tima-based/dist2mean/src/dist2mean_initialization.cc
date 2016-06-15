
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
init_device_data_dist2mean(
	string& device_name,
	map<string, string>& options,
	shared_ptr<tima::GlobalStorage> st)
{

	st->setValue("posX", options["posX"]);
	st->setValue("posY", options["posY"]);
	st->setValue("myself", device_name);
	st->setValue("is_source", options["is_source"]);
	st->setValue("radious", options["radious"]);


	if (st->getValue("is_source")->to_string() == "true") {
		st->setValue("remaining_broadcasts", options["remaining_broadcasts"]);
	}

	st->setValue("threshold", options["threshold"]);
	st->setValue("lastId", options["lastId"]);
	st->setValue("nr_hellos", options["nr_hellos"]);

}


}
