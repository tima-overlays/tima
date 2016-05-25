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
#include "dist2mean_m.h"


#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"

#include <algorithm>

#include <climits>

using namespace std;
using inet::dist2mean::Broadcast;
using inet::dist2mean::Hello;

namespace inet {

Define_Module(Dist2Mean);

enum ControlMessageTypes {
    IDLE,
    START,
    SAY_HELLO,
    WAKEUP,
    DISPLAY_TIME,
    BROADCAST_DELAY
};

#define oo INT_MAX
#define nil nil_edge


void
Dist2Mean::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    switch (stage) {
        case INITSTAGE_LOCAL:

            nr_hello_msg = par("nr_hello_messages").longValue();
            is_source = par("is_source").boolValue();

            remote_port = par("remotePort").longValue();
            local_port = par("localPort").longValue();

            ctrlMsg0 = new cMessage("controlMSG", IDLE);

            break;
        case INITSTAGE_PHYSICAL_ENVIRONMENT_2:
            {
                cModule* host = getContainingNode(this);
                IMobility* mobility = check_and_cast<IMobility*>(host->getSubmodule("mobility"));

                this->position = mobility->getCurrentPosition();

                EV_TRACE << "My position is " << this->position  << "\n";
            }
            break;
        case INITSTAGE_LAST:

            if (is_source) {
                cMessage* ctrlWakeup = new cMessage("controlMSG", WAKEUP);
                ctrlWakeup->setKind( WAKEUP);
                scheduleAt(par("wakeUpTime").doubleValue() + simTime(), ctrlWakeup);
            }

            break;
        default:
            break;
    }
}

void
Dist2Mean::handleMessageWhenUp(cMessage *msg)
{

    if (msg->isSelfMessage()) {

        switch (msg->getKind()) {
            case START:
                processStart();
//                sm_proptocol->reportMessage(MSG_INITIALIZE);
                break;
            case SAY_HELLO:
                for ( auto addr : possible_neighbors ) {
                    Hello* pkt = new Hello("Hello");
                    pkt->setX(position.x);
                    pkt->setY(position.y);
                    pkt->setSender(myself.c_str());
                    socket.sendTo(pkt, addr, remote_port);

                }
                this->nr_hello_msg--;
                if (this->nr_hello_msg) {
                    ctrlMsg0->setKind(SAY_HELLO);
                    scheduleAt(simTime() + par("helloTime").doubleValue(),  ctrlMsg0);
                }
                break;
            case WAKEUP:
                configure_neighbors();
                payload = "this is the payload";
                send_message();
                cancelAndDelete(msg);
                break;
            case BROADCAST_DELAY:
                send_message();
                cancelAndDelete(msg);
                break;
            case DISPLAY_TIME:
                {
                    cancelAndDelete(msg);
                }
                break;
//            case TEST_DELAY:
//                {
//                    cancelAndDelete(msg);
//                }
                break;
            default:
                break;
        }
    }
    else if (msg->getKind() == UDP_I_DATA) {

        cPacket* pkt = PK(msg);

        bool done = processMessage<Hello>(pkt, &Dist2Mean::on_hello_received);
        if (!done) {
            configure_neighbors();
            done = processMessage<Broadcast>(pkt, &Dist2Mean::on_payload_received);
        }
        delete pkt;

    }

}



void
Dist2Mean::finish()
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;
}

bool
Dist2Mean::handleNodeStart(IDoneCallback *doneCallback)
{
    ctrlMsg0->setKind(START);
    scheduleAt(simTime() + 0.01, ctrlMsg0);
    return true;
}

bool
Dist2Mean::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;

    return true;
}

void
Dist2Mean::handleNodeCrash()
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;
}

void
Dist2Mean::processStart()
{
    myself = this->getParentModule()->getFullName();
    L3AddressResolver().tryResolve(myself.c_str(), myAddress);
    EV_TRACE << "Starting the process in module " << myself << " (" << myAddress.str() << ")" << "\n";

    /* process the possible neighbors */
    const char *destAddrs = par("addresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << ((token)?token:"NULL") << endl;
        else if (myself != token)
            possible_neighbors.push_back(result);
    }

    socket.setOutputGate(gate("udpOut"));
    socket.bind(local_port);
    socket.setBroadcast(true);


    ctrlMsg0->setKind(SAY_HELLO);
    scheduleAt(simTime() + par("helloTime").doubleValue(),  ctrlMsg0);
}


template <typename T> bool
Dist2Mean::processMessage(cPacket* pkt, void (Dist2Mean::*action)(const T* msg))
{
    T* t = check_and_cast_nullable<T*>(dynamic_cast<T*>(pkt));
    if (t != nullptr) {
        (this->*action)(t);
        return true;
    }
    else {
        return false;
    }
}


void
Dist2Mean::configure_neighbors()
{
    // traverse set of received addresses
    for (auto it : addresses) {
        string name = it.first;
        int x_t = coordinates[name].first;
        int y_t = coordinates[name].second;
        int d = (x_t - position.x)*(x_t - position.x) + (y_t - position.y)*(y_t - position.y);
        edges.push_back(name);
        w[name] = d;
    }

    // print (debug)
    EV_DEBUG << "EDGES " << myself << " =>  \n";
    for (auto& i : edges) {
        EV_DEBUG << "\t" << i  << " with cost " << w[i] << "\n";
    }
}


void
Dist2Mean::addNewAddress(string id)
{
    if (myself != id) {
        L3Address result;
        L3AddressResolver().tryResolve(id.c_str(), result);
        auto it = addresses.find(id);
        if (it == addresses.end()) {
            addresses.insert(pair<string, L3Address>(id, result));
        }
    }
}


void
Dist2Mean::on_hello_received(const Hello* msg)
{

    // add coordinates
    auto it = coordinates.find(msg->getSender());
    if (it == coordinates.end()) {
        EV_TRACE << " A hello from " << msg->getSender() <<  " at (" << msg->getX() << ", " << msg->getY() << ")\n";

        addNewAddress(msg->getSender());

        coordinates.emplace(msg->getSender(), make_pair(msg->getX(), msg->getY()));

        int d = (position.x - msg->getX())*(position.x - msg->getX())
                                + (position.y - msg->getY())*(position.y - msg->getY());

        w[msg->getSender()] = d;
//        coordinates.insert(pair<string,  make_pair(, make_pair) pair<int, int> >(, pair<int, int>()));
    }

}

void
Dist2Mean::on_payload_received(const Broadcast* m) {
    bool first_time = received_from.empty(); // is the first time I received this message ?



    received_from.insert(m->getSender());
    if (first_time) {
        payload = m->getPayload();
        cMessage* mm = new cMessage("broadcast delay");
        mm->setKind(BROADCAST_DELAY);
        double delay = (1.0 / w[m->getSender()])*1000.0*2; // this is in s/(m^2)
        cout << "\t\t\tThe waiting time in " << myself << " is " << delay << endl;
        scheduleAt(simTime() + delay, mm);
    }

    EV_DEBUG << "Message received at " << simTime() << " from " << m->getSender() << "\n";
    std::cout << myself << ": message received at " << simTime() << " from " << m->getSender() << "\n";
}


void
Dist2Mean::send_message()
{

    bool must_send = received_from.empty();
    double mx = 0;
    double my = 0;
    if (!must_send) {

        for (auto& s : received_from) {
            auto p = coordinates[s];
            mx += p.first;
            my += p.second;
        }
        mx /= received_from.size();
        my /= received_from.size();
    }

    double dist = (mx - position.x)*(mx - position.x) + (my - position.y)*(my - position.y);

    must_send = must_send || dist > par("threshold").doubleValue();

    if (must_send) {

        EV_DEBUG << "====================== Sending in " << myself << " because the distance to mean  is " << dist << " > " << par("threshold").doubleValue() << "\n";
        cout << "====================== Sending in " << myself << " because the distance to mean  is " << dist << " > " << par("threshold").doubleValue() << "\n";

        for (auto& d : edges) {
            L3Address addr = addresses[d];
            Broadcast* m = new Broadcast("payload");
            m->setPayload(payload.c_str());
            m->setSender(myself.c_str());
            socket.sendTo(m, addr, remote_port);
        }

    }
}



} //namespace
