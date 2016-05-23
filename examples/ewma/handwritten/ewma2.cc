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

namespace inet {

Define_Module(EWMA2);

enum ControlMessageTypes {
    IDLE,
    START,
    SAY_HELLO,
    WAKEUP,
    DISPLAY_TIME,
    TEST_DELAY,
    CONNECT_DELAY,
    INITIATE_DEALY,
    I_REPORT,
    I_TEST,
    I_CONNECT
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

            remotePort = par("remotePort").longValue();
            localPort = par("localPort").longValue();

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
                for ( auto addr : possibleNeighbors ) {
                    inet::ewma::Hello* pkt = new inet::ewma::Hello("Hello");
                    pkt->setX(position.x);
                    pkt->setY(position.y);
                    pkt->setSender(myself.c_str());
                    socket.sendTo(pkt, addr, remotePort);

                }
                this->nr_hello_msg--;
                if (this->nr_hello_msg) {
                    ctrlMsg0->setKind(SAY_HELLO);
                    scheduleAt(simTime() + par("helloTime").doubleValue(),  ctrlMsg0);
                }
                break;
            case WAKEUP:
//                if (SN == States::Sleeping ) {
//                    wakeup();
//                }
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

        bool done = processMessage<inet::ewma::Hello>(pkt, &inet::EWMA2::on_hello_received);
        if (!done) {

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

    const char *destAddrs = par("addresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << ((token)?token:"NULL") << endl;
        else if (myself != token)
            possibleNeighbors.push_back(result);
    }

    socket.setOutputGate(gate("udpOut"));
    socket.bind(localPort);
    socket.setBroadcast(true);


    ctrlMsg0->setKind(SAY_HELLO);
    scheduleAt(simTime() + par("helloTime").doubleValue(),  ctrlMsg0);
}


void
EWMA2::addNewAddress(std::string id)
{
    if (myself != id) {
        L3Address result;
        L3AddressResolver().tryResolve(id.c_str(), result);
        auto it = addresses.find(id);
        if (it == addresses.end()) {
            addresses.insert(std::pair<std::string, L3Address>(id, result));
        }
    }
}


void
EWMA2::on_hello_received(const inet::ewma::Hello* msg)
{

    // add coordinates
    auto it = coordinates.find(msg->getSender());
    if (it == coordinates.end()) {
//        EV_TRACE << " A hello from " << msg->getSender() <<  " at (" << msg->getX() << ", " << msg->getY() << ")\n";
        addNewAddress(msg->getSender());
        coordinates.insert(std::pair<std::string,  std::pair<int, int> >(msg->getSender(), std::pair<int, int>(msg->getX(), msg->getY())));
    }

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
        std::string name = it.first;
        int x_t = coordinates[name].first;
        int y_t = coordinates[name].second;
        int d = (x_t - position.x)*(x_t - position.x) + (y_t - position.y)*(y_t - position.y);
        mst_edges.push_back(name);
        mst_w[name] = d;
    }

    // print (debug)
    EV_DEBUG << "EDGES " << myself << " =>  \n";
    for (auto& i : mst_edges) {
        EV_DEBUG << "\t" << i  << " with cost " << mst_w[i] << "\n";
    }
}


} //namespace
