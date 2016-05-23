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

#include "ewma2.h"
#include "ewmaMsgs_m.h"


#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"

#include <algorithm>

#include <climits>

using namespace std;

namespace inet {

Define_Module(EWMA2);

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
EWMA2::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    switch (stage) {
        case INITSTAGE_LOCAL:

            nr_hello_msg = par("nr_hello_messages").longValue();
            is_source = par("is_source").boolValue();

            remote_port = par("remotePort").longValue();
            local_port = par("localPort").longValue();

            ctrlMsg0 = new cMessage("controlMSG", IDLE);

            ctrlDisplayTime = new cMessage("DISPLAY TIME", DISPLAY_TIME);

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

            SN = States::Sleeping;

            if (is_source) {
                cMessage* ctrlWakeup = new cMessage("controlMSG", WAKEUP);
                ctrlWakeup->setKind( WAKEUP);
                scheduleAt(par("wakeUpTime").doubleValue() + simTime(), ctrlWakeup);
            }

            scheduleAt(simTime() + par("display_time").doubleValue(), ctrlDisplayTime);
            break;
        default:
            break;
    }
}

void
EWMA2::handleMessageWhenUp(cMessage *msg)
{

    if (msg->isSelfMessage()) {

        switch (msg->getKind()) {
            case START:
                processStart();
//                sm_proptocol->reportMessage(MSG_INITIALIZE);
                break;
            case SAY_HELLO:
                for ( auto addr : possible_neighbors ) {
                    ewma_Hello* pkt = new ewma_Hello("Hello");
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
                covered.insert(myself);
                payload = "this is the payload";
                send_message(local_mst);
                cancelAndDelete(msg);
                break;
            case BROADCAST_DELAY:
                send_to_uncovred();
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

        bool done = processMessage<ewma_Hello>(pkt, &EWMA2::on_hello_received);
        if (!done) {
            configure_neighbors();
            done = processMessage<ewma_Broadcast>(pkt, &EWMA2::on_payload_received);
        }
        delete pkt;

    }

}



void
EWMA2::finish()
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;
}

bool
EWMA2::handleNodeStart(IDoneCallback *doneCallback)
{
    ctrlMsg0->setKind(START);
    scheduleAt(simTime() + 0.01, ctrlMsg0);
    return true;
}

bool
EWMA2::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;

    return true;
}

void
EWMA2::handleNodeCrash()
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;
}

void
EWMA2::processStart()
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

    /* process the local mst */
    cStringTokenizer tokenizer2(par("mst"));
    while ((token = tokenizer2.nextToken()) != nullptr) {
        local_mst.push_back(token);
        EV_TRACE << "\t in mst " << token <<"\n";
    }

    socket.setOutputGate(gate("udpOut"));
    socket.bind(local_port);
    socket.setBroadcast(true);


    ctrlMsg0->setKind(SAY_HELLO);
    scheduleAt(simTime() + par("helloTime").doubleValue(),  ctrlMsg0);
}


void
EWMA2::addNewAddress(string id)
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
EWMA2::on_hello_received(const ewma_Hello* msg)
{

    // add coordinates
    auto it = coordinates.find(msg->getSender());
    if (it == coordinates.end()) {
        EV_TRACE << " A hello from " << msg->getSender() <<  " at (" << msg->getX() << ", " << msg->getY() << ")\n";
        addNewAddress(msg->getSender());
        coordinates.insert(pair<string,  pair<int, int> >(msg->getSender(), pair<int, int>(msg->getX(), msg->getY())));
    }

}

void
EWMA2::on_payload_received(const ewma_Broadcast* m) {
    if (!payload.empty()) {
        return;
    }
    payload = m->getPayload();
    if (isForwardingNode()) {
        for (uint32_t k = 0 ; k < m->getCoveredArraySize() ; ++k) {
            covered.insert(string(m->getCovered(k)));
        }
        cMessage* mm = new cMessage("broadcast delay");
        mm->setKind(BROADCAST_DELAY);
        scheduleAt(simTime() + par("delay_test").doubleValue(), mm);
    }
    EV_DEBUG << "Message received at " << simTime() << "\n";
}


template <typename T> bool
EWMA2::processMessage(cPacket* pkt, void (EWMA2::*action)(const T* msg))
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
EWMA2::configure_neighbors()
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

    EV_DEBUG << "Printing EDGES - LOCAL_MST =>\n";
    for (auto& i : edges) {
        if (find(begin(local_mst), end(local_mst), i) == local_mst.end()) {
            EV_DEBUG << "\t" << i << " is not a  forwarding child " << " because it has a cost of " << w[i] << "\n";
        }
    }
}


bool
EWMA2::isForwardingNode()
{
    return is_source || (local_mst.size() > 0);
}


void
EWMA2::send_message(const vector<string>& dst)
{
    set<string> previous;
    for (auto& d: covered) previous.insert(d);
    for (auto& d : edges) {
        covered.insert(d);
    }

    if (dst.size() > 0) {
        EV_DEBUG << "====================== Sending in " << myself << "\n";
        for (auto& c : covered) {
            EV_DEBUG << "\t The following is covered: " << c << "\n";
        }
        /*only send if some children in the mst are not covered, but send to all uncovered*/
        for (auto& d : edges) {
            if (previous.find(d) != previous.end()) {
                continue;
            }
            L3Address addr = addresses[d];
            ewma_Broadcast* m = new ewma_Broadcast("payload");
            m->setPayload(payload.c_str());
            m->setCoveredArraySize(covered.size());
            vector<string> v;
            for (auto& c : covered) v.push_back(c);
            for (uint32_t i = 0 ; i < v.size() ; i++) {
                m->setCovered(i, v[i].c_str());
            }

            socket.sendTo(m, addr, remote_port);
        }
    }
}


void
EWMA2::send_to_uncovred()
{
    /* send to members of (local_mst - covered) */
    vector<string> v;
    for (auto& d: local_mst) {
        if (covered.find(d) == covered.end()) {
            v.push_back(d);
        }
    }
    send_message(v);
}


} //namespace
