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

#include "dist2mean.h"


#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"

#include <algorithm>

using namespace std;
using inet::broadcasting::Broadcast;

namespace inet {

Define_Module(Dist2Mean);

Dist2Mean::Dist2Mean()
{
}


void
Dist2Mean::on_payload_received(const Broadcast* m) {

    BroadcastingAppBase::on_payload_received(m);

    bool first_time = !is_source && received_from.empty(); // is the first time I received this message ?

    received_from.insert(m->getSender());
    if (first_time) {
        payload = m->getPayload();
        cMessage* mm = new cMessage("broadcast delay");
        mm->setKind(BROADCAST_DELAY);
        double delay = (1.0 / neighbors[m->getSender()].w)*1000.0*2; // this is in s/(m^2)
        cout << "\t\t\tThe waiting time in " << myself << " is " << delay << endl;
        scheduleAt(simTime() + delay, mm);
    }
}


void
Dist2Mean::send_message()
{

    bool must_send = received_from.empty();
    double mx = 0;
    double my = 0;
    if (!must_send) {

        for (auto& s : received_from) {
            auto p = neighbors[s].pos;
            mx += p.x;
            my += p.y;
        }
        mx /= received_from.size();
        my /= received_from.size();
    }

    double dist = (mx - position.x)*(mx - position.x) + (my - position.y)*(my - position.y);

    must_send = must_send || dist > par("threshold").doubleValue();

    if (must_send) {

        EV_DEBUG << "====================== Sending in " << myself << " because the distance to mean  is " << dist << " > " << par("threshold").doubleValue() << "\n";
        cout << "====================== Sending in " << myself << " because the distance to mean  is " << dist << " > " << par("threshold").doubleValue() << "\n";

        for (auto& d : neighbors) {
            Broadcast* m = new Broadcast("payload");
            m->setPayload(payload.c_str());
            m->setSender(myself.c_str());
            socket.sendTo(m, d.second.addr, remote_port);
        }

    }
}


void
Dist2Mean::time_to_broadcast_payload()
{
    BroadcastingAppBase::time_to_broadcast_payload();
    if (is_source) {
        payload = " this is the payload, initially sent from " + myself;
    }
    cout << "Broadcasting in " << myself << endl;
    send_message();
}



} //namespace
