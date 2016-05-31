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

using namespace std;
using inet::broadcasting::Broadcast;

namespace inet {

Define_Module(Abba2);

//double find_angle()

int
Abba2::add(vector<Range>& v, double a1, double a2 )
{
    for (int i = 0 ; i < v.size(); i++) {
        if (v[i].intersect(a1,2)) {
            return i;
        }
    }
    v.push_back(Range(a1, a2));
    return -1;
}

#define  PIPI  3.141592653589793238463

void
Abba2::on_payload_received(const Broadcast* m) {


    string key = string(m->getId());

    auto mm = (abba::ABBABroadcast*)m;
    auto r = radious;

    auto x = mm->getX() - position.x;
    auto y = -(mm->getY() - position.y);

    auto a = sqrt(x*x + y*y);

    auto alpha = atan2( y, x );
    auto beta = atan2( sqrt(4*r*r  - a*2), a/2 );

    if (alpha < 0) alpha += 2*PI;
    if (beta < 0) beta += 2*PI;

    auto a1 = alpha - beta;
    auto a2 = alpha + beta;


    cerr << myself << " angle from " << a1 << " to " << a2 << " received." << endl;
//
//    int i = -1;
//    while ( (i = add(received_from[key], a1, a2)) != -1) {
//        a1 = received_from[key][i].min;
//        a2 = received_from[key][i].max;
//    }




//
//
//    string key = string(m->getId());
//    BroadcastingAppBase::on_payload_received(m);
//
//    bool first_time = !is_source && received_from[key].empty(); // is the first time I received this message ?
//
//    received_from[key].insert(m->getSender());
//    if (first_time) {
//        emitReceived();
//        payloads[key] = m->getPayload();
//        cMessage* mm = new cMessage("broadcast delay");
//        mm->setContextPointer(strdup(key.c_str()));
//        mm->setKind(BROADCAST_DELAY);
//        double delay = (1.0 / neighbors[m->getSender()].w)*1000.0*2; // this is in s/(m^2)
//        cout << "\t\t\tThe waiting time in " << myself << " is " << delay << endl;
//        scheduleAt(simTime() + delay, mm);
//    }
}


void
Abba2::send_message(string& key)
{
    for (auto& addr: possible_neighbors) {
        abba::ABBABroadcast* m = new abba::ABBABroadcast("payload");
        m->setPayload(payloads[key].c_str());
        m->setId(key.c_str());
        m->setSender(myself.c_str());
        m->setX(position.x);
        m->setY(position.y);
        socket.sendTo(m, addr, remote_port);
    }
}


void
Abba2::time_to_broadcast_payload(void* user_data)
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
