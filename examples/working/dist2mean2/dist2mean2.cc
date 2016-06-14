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

#include "dist2mean2.h"


#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"

#include <algorithm>

using namespace std;
using inet::broadcasting::Broadcast;

namespace inet {

Define_Module(Dist2Mean2);

Dist2Mean2::Dist2Mean2()
{
}


void
Dist2Mean2::on_payload_received(const Broadcast* m) {

    string key = string(m->getId());
    BroadcastingAppBase::on_payload_received(m);

    bool first_time = !is_source && received_from[key].empty(); // is the first time I received this message ?

    received_from[key].insert(m->getSender());
    if (first_time) {
        emitReceived();
        payloads[key] = m->getPayload();
        cMessage* mm = new cMessage("broadcast delay");
        mm->setContextPointer(strdup(key.c_str()));
        mm->setKind(BROADCAST_DELAY);
        double delay = (1.0 / neighbors[m->getSender()].w)*1000.0*2; // this is in s/(m^2)
        cout << "\t\t\tThe waiting time in " << myself << " is " << delay << endl;
        scheduleAt(simTime() + delay, mm);
    }
}


void
Dist2Mean2::send_message(string& key)
{

    bool must_send = received_from[key].empty();
    double mx = 0;
    double my = 0;
    if (!must_send) {

        for (auto& s : received_from[key]) {
            auto p = neighbors[s].pos;
            mx += p.x;
            my += p.y;
        }
        mx /= received_from[key].size();
        my /= received_from[key].size();
    }

    double dist = (mx - position.x)*(mx - position.x) + (my - position.y)*(my - position.y);

    must_send = must_send || dist > par("threshold").doubleValue();

    if (must_send) {

        EV_DEBUG << "====================== Sending in " << myself << " because the distance to mean  is " << dist << " > " << par("threshold").doubleValue() << "\n";
        cout << "====================== Sending in " << myself << " because the distance to mean  is " << dist << " > " << par("threshold").doubleValue() << "\n";
        emitSent();
        L3AddressResolver resolver;
        L3Address addr = resolver.resolve("255.255.255.255", L3AddressResolver::ADDR_IPv4);
        Broadcast* m = new Broadcast("payload");
        m->setPayload(payloads[key].c_str());
        m->setId(key.c_str());
        m->setSender(myself.c_str());
        socket.sendTo(m, addr, remote_port);
    }
}


void
Dist2Mean2::time_to_broadcast_payload(void* user_data)
{
    BroadcastingAppBase::time_to_broadcast_payload(user_data);
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
    cout << "Broadcasting in " << myself << endl;
    send_message(key);
}



} //namespace
