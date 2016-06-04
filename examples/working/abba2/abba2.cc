//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "abba2.h"
#include "abbaMsgs_m.h"


#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <chrono>
#include <random>

using namespace std;
using inet::broadcasting::Broadcast;

namespace inet {

Define_Module(Abba2);

#define  PIPI  3.141592653589793238463

void
Abba2::on_payload_received(const Broadcast* m) {

    string key = string(m->getId());

    bool firstTime = !is_source && payloads[key].empty();

    if (firstTime) {
        double angle = 0;
        double delta = 2*PIPI / 36;
        while (angle < 2*PIPI) {
            auto y = std::sin(angle)*radious;
            auto x = std::cos(angle)*radious;
            received_from[key].insert(make_pair(x+position.x, y + position.y));
            angle += delta;
        }
    }

    auto mm = (abba::ABBABroadcast*)m;
    auto r = radious;

    auto a = mm->getX();
    auto b = mm->getY();

    for (auto i = received_from[key].begin(), f = received_from[key].end() ; i != f ; ++i) {
        auto x = i->first;
        auto y = i->second;

        if ((x - a)*(x - a) + (y - b)*(y - b) < r*r) {
            received_from[key].erase(i);
        }
    }

    if (firstTime) {
        emitReceived();
        payloads[key] = m->getPayload();
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::minstd_rand0 generator (seed); // minstd_rand0 is a standard linear_congruential_engine
        std::uniform_real_distribution<double> distribution(0.1,0.5);
        delayed_broadcast(key, distribution(generator));
    }
}


void
Abba2::send_message(string& key)
{
    if (is_source || received_from[key].size() > 0) {
        cerr << myself << " has still " << received_from[key].size() << " points " << endl;

        L3AddressResolver resolver;
        L3Address addr = resolver.resolve("255.255.255.255", L3AddressResolver::ADDR_IPv4);
        abba::ABBABroadcast* m = new abba::ABBABroadcast("payload");
        m->setPayload(payloads[key].c_str());
        m->setId(key.c_str());
        m->setSender(myself.c_str());
        m->setX(position.x);
        m->setY(position.y);
        socket.sendTo(m, addr, remote_port);
        emitSent();
    }
}


void
Abba2::time_to_broadcast_payload(void* user_data)
{
//    BroadcastingAppBase::time_to_broadcast_payload(user_data);
    string key;
    if (is_source) {
        key = myself + "-" + to_string(get_next_id_for_msg());
        payloads[key] = " this is the payload, initially sent from " + myself;
    }
    else {
        char* s = (char*)user_data;
        key = string(s);
        delete s;
    }
    cout << "Broadcasting in " << myself << " at " << simTime() << endl;
    send_message(key);
}



} //namespace
