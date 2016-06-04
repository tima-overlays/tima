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

#include "testingStuff.h"


#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"

#include "IdealTransmitterWithVariableRange.h"

#include <algorithm>

using namespace std;
using inet::broadcasting::Broadcast;

namespace inet {

Define_Module(TestingStuff);


void
TestingStuff::on_payload_received(const Broadcast* m) {

    string key = string(m->getId());
    BroadcastingAppBase::on_payload_received(m);
    cerr << myself << ": payload received" << endl;
}


void
TestingStuff::send_message(string& key)
{

    EV_DEBUG << "====================== Sending in " << myself  << "\n";
    cerr << "====================== Sending in " << myself << " with key " << key << "\n";
    emitSent();

    L3AddressResolver resolver;
    L3Address addr = resolver.resolve("255.255.255.255", L3AddressResolver::ADDR_IPv4);
    {
        Broadcast* m = new Broadcast("payload");
        m->setPayload(payloads[key].c_str());
        m->setId(key.c_str());
        m->setSender(myself.c_str());
        socket.sendTo(m, addr, remote_port);
    }

    cerr << " JEJEJEJJEJEJEJE " << addr << endl;

//    for (auto& d : neighbors) {
//        if (d.first == "hostR1") {
//            Broadcast* m = new Broadcast("payload");
//            m->setPayload(payloads[key].c_str());
//            m->setId(key.c_str());
//            m->setSender(myself.c_str());
//            socket.sendTo(m, d.second.addr, remote_port);
//        }
//    }

    if (get_last_id_for_msg() > 1) {
        cModule* host = getContainingNode(this);
        IdealTransmitterWithVariableRange* transmitter = check_and_cast<IdealTransmitterWithVariableRange*>(host->getModuleByPath(".wlan[0].radio.transmitter"));
        auto n = (transmitter->getCurrentCommunicationRange().get() + transmitter->getMinCommunicationRange().get()) / 2;
        m newValue = m(n);
        cerr << "Sending with range: " << newValue << endl;
        transmitter->setCurrentCommunicationRange(newValue);
    }

}


void
TestingStuff::time_to_broadcast_payload(void* user_data)
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
//    cout << "Broadcasting in " << myself << endl;
    send_message(key);
}



} //namespace
