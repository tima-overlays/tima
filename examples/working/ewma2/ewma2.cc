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

    string key = string(m->getId());

    if (!payloads[key].empty()) {
        return;
    }

    payloads[key] = string(m->getPayload());

    emitReceived();

    if (isForwardingNode()) {
        ewma::EWMABroadcast* mmm = (ewma::EWMABroadcast*)m;
        for (uint32_t k = 0 ; k < mmm->getCoveredArraySize() ; ++k) {
            covered[key].insert(string(mmm->getCovered(k)));
        }
        delay_broadcast(strdup(key.c_str()));

    }
}


bool
EWMA2::isForwardingNode()
{
    return is_source || (local_mst.size() > 0);
}


void
EWMA2::send_message(const vector<string>& dst, string& key)
{
    set<string> previous;
    for (auto& d: covered[key]) previous.insert(d);
    for (auto& d : neighbors) {
        covered[key].insert(d.first);
    }

    if (dst.size() > 0) {
        EV_DEBUG << "====================== Sending in " << myself << "\n";
        cerr << "====================== Sending in " << myself << "\n";
        vector<string> v;
        for (auto& c : covered[key]) {
            EV_DEBUG << "\t The following is covered: " << c << "\n";
            cerr << "\t The following is covered: " << c << "\n";
            v.push_back(c);
        }
        emitSent();

        /*only send if some children in the mst are not covered, but send to all uncovered*/

        L3AddressResolver resolver;
        L3Address addr = resolver.resolve("255.255.255.255", L3AddressResolver::ADDR_IPv4);

        ewma::EWMABroadcast* m = new ewma::EWMABroadcast("payload");
        m->setSender(myself.c_str());
        m->setId(key.c_str());
        m->setPayload(payloads[key].c_str());
        m->setCoveredArraySize(covered[key].size());

        for (uint32_t i = 0 ; i < v.size() ; i++) {
            m->setCovered(i, v[i].c_str());
        }
        socket.sendTo(m, addr, remote_port);
    }
}


void
EWMA2::send_to_uncovered(string& key)
{
    /* send to members of (local_mst - covered) */
    vector<string> v;
    for (auto& d: local_mst) {
        if (covered[key].find(d) == covered[key].end()) {
            v.push_back(d);
        }
    }
    send_message(v, key);
}

void
EWMA2::time_to_broadcast_payload(void* user_data)
{
    BroadcastingAppBase::time_to_broadcast_payload(user_data);


    if (is_source) {
        string key = myself + "-" + to_string(get_next_id_for_msg());
        cerr << "Broadcasting " <<  key << " in " << myself << " at " << simTime() << endl;
        payloads[key] = " this is the payload, initially sent from " + myself;
        covered[key].insert(myself);
        send_message(local_mst, key);
    }
    else {
        char* s = (char*)user_data;
        string key = string(s);
        cerr << "Broadcasting key: " << s  << " in " << myself << " at " << simTime() << endl;
        delete s;

        send_to_uncovered(key);
    }
}


} //namespace
