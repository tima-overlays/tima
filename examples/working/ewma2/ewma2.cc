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
using inet::broadcasting::Broadcast;

namespace inet {

Define_Module(EWMA2);


void
EWMA2::processStart()
{
    BroadcastingAppBase::processStart();
    /* process the local mst */
    cStringTokenizer tokenizer2(par("mst"));
    const char* token;
    while ((token = tokenizer2.nextToken()) != nullptr) {
        local_mst.push_back(token);
        EV_TRACE << "\t in mst " << token <<"\n";
    }
}


void
EWMA2::on_payload_received(const Broadcast* m) {
    if (!payload.empty()) {
        return;
    }

    emitReceived();

    payload = m->getPayload();
    if (isForwardingNode()) {
        ewma::EWMABroadcast* mmm = (ewma::EWMABroadcast*)m;
        for (uint32_t k = 0 ; k < mmm->getCoveredArraySize() ; ++k) {
            covered.insert(string(mmm->getCovered(k)));
        }
        cMessage* mm = new cMessage("broadcast delay");
        mm->setKind(BROADCAST_DELAY);
        scheduleAt(simTime() + par("delay_test").doubleValue(), mm);
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
    for (auto& d : neighbors) {
        covered.insert(d.first);
    }

    if (dst.size() > 0) {
        EV_DEBUG << "====================== Sending in " << myself << "\n";
        cerr << "====================== Sending in " << myself << "\n";
        for (auto& c : covered) {
            EV_DEBUG << "\t The following is covered: " << c << "\n";
            cerr << "\t The following is covered: " << c << "\n";
        }
        emitSent();
        /*only send if some children in the mst are not covered, but send to all uncovered*/
        for (auto& d : neighbors) {
            if (previous.find(d.first) != previous.end()) {
                continue;
            }
            ewma::EWMABroadcast* m = new ewma::EWMABroadcast("payload");
            m->setSender(myself.c_str());
            m->setPayload(payload.c_str());
            m->setCoveredArraySize(covered.size());
            vector<string> v;
            for (auto& c : covered) v.push_back(c);
            for (uint32_t i = 0 ; i < v.size() ; i++) {
                m->setCovered(i, v[i].c_str());
            }

            socket.sendTo(m, d.second.addr, remote_port);
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

void
EWMA2::time_to_broadcast_payload()
{
    BroadcastingAppBase::time_to_broadcast_payload();
    cout << "Broadcasting in " << myself << endl;
    if (is_source) {
        payload = " this is the payload, initially sent from " + myself;
        covered.insert(myself);
        send_message(local_mst);
    }
    else {
        send_to_uncovred();
    }
}


} //namespace
