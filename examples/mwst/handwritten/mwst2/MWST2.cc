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

#include "MWST2.h"
#include "MWSTMsgs_m.h"


#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"


#include <algorithm>

#include <climits>

namespace inet {

Define_Module(MWST2);

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
MWST2::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    switch (stage) {
        case INITSTAGE_LOCAL:

            nr_hello_msg = par("nr_hello_messages").longValue();
            spontaneously_awaken = par("spontaneously_awaken").boolValue();

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

            if (spontaneously_awaken) {
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
MWST2::handleMessageWhenUp(cMessage *msg)
{

    if (msg->isSelfMessage()) {

        switch (msg->getKind()) {
            case START:
                processStart();
//                sm_proptocol->reportMessage(MSG_INITIALIZE);
                break;
            case SAY_HELLO:
                for ( auto addr : possibleNeighbors ) {
                    Hello* pkt = new Hello("Hello");
                    pkt->setX(position.x);
                    pkt->setY(position.y);
                    pkt->setSender(myself.c_str());
                    socket.sendTo(pkt, addr, destinationPort);

                }
                this->nr_hello_msg--;
                if (this->nr_hello_msg) {
                    ctrlMsg0->setKind(SAY_HELLO);
                    scheduleAt(simTime() + par("helloTime").doubleValue(),  ctrlMsg0);
                }
                break;
            case WAKEUP:
                if (SN == States::Sleeping ) {
                    wakeup();
                }
                cancelAndDelete(msg);
                break;
            case DISPLAY_TIME:
                {
                    EV_TRACE << "Final MWST from " << myself << " point of view\n";

                    EV_TRACE << "Fragment: " << FN << "\n";
                    for (auto e : edges) {
                        if (SE[e] == EdgeStates::Branch) {
                            EV_TRACE << e << " with weight " << w[e] << "\n";
                        }
                    }
                    cancelAndDelete(msg);
                }
                break;
            case TEST_DELAY:
                {
                    Test* t = check_and_cast_nullable<Test*>(msg);
                    send_test(t->getFragmentId(), t->getSender(), true);
                    cancelAndDelete(msg);
                }
                break;
            case CONNECT_DELAY:
                {
                    Connect* t = check_and_cast_nullable<Connect*>(msg);
                    send_connect(t->getSender(), true);
                    cancelAndDelete(msg);
                }
                break;
            case INITIATE_DEALY:
                {
                    Initiate* t = check_and_cast_nullable<Initiate*>(msg);
                    send_initiate(t->getFragmentId(), t->getSender(), true);
                    cancelAndDelete(msg);
                }
                break;
            default:
                break;
        }
    }
    else if (msg->getKind() == UDP_I_DATA) {

        cPacket* pkt = PK(msg);

        bool done = processMessage<Hello>(pkt, &inet::MWST2::on_hello_received);
        if (!done) {
            done = done || processMessage<Connect>(pkt, &inet::MWST2::on_connect_received);
            done = done || processMessage<Initiate>(pkt, &inet::MWST2::on_initiate_received);
            done = done || processMessage<Test>(pkt, &inet::MWST2::on_test_received);
            done = done || processMessage<Accept>(pkt, &inet::MWST2::on_accept_received);
            done = done || processMessage<Reject>(pkt, &inet::MWST2::on_reject_received);
            done = done || processMessage<Report>(pkt, &inet::MWST2::on_report_received);
            done = done || processMessage<ChangeRoot>(pkt, &inet::MWST2::on_change_root_received);

            print_state();
        }
        delete pkt;

    }

}



void
MWST2::finish()
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;
}

bool
MWST2::handleNodeStart(IDoneCallback *doneCallback)
{
    ctrlMsg0->setKind(START);
    scheduleAt(simTime() + 0.01, ctrlMsg0);
    return true;
}

bool
MWST2::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;

    return true;
}

void
MWST2::handleNodeCrash()
{
    if (ctrlMsg0)
        cancelAndDelete(ctrlMsg0);
    ctrlMsg0 = nullptr;
}

void
MWST2::processStart()
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
MWST2::addNewAddress(std::string id)
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
MWST2::on_hello_received(const Hello* msg)
{

    // add coordinates
    auto it = coordinates.find(msg->getSender());
    if (it == coordinates.end()) {
//        EV_TRACE << " A hello from " << msg->getSender() <<  " at (" << msg->getX() << ", " << msg->getY() << ")\n";
        addNewAddress(msg->getSender());
        coordinates.insert(std::pair<std::string,  std::pair<int, int> >(msg->getSender(), std::pair<int, int>(msg->getX(), msg->getY())));
    }

}


std::string
MWST2::create_unique_name(const std::string& a1, const std::string& a2)
{
    if (a1 <= a2) {
        return a1 + " <-> " + a2;
    }
    else {
        return a2 + " <-> " + a1;
    }
}


void
MWST2::print_state()
{
    EV_TRACE << "State: " << SN  << " Fragment Id: " << FN  << "\n";
}


template <typename T> bool
MWST2::processMessage(cPacket* pkt, void (MWST2::*action)(const T* msg))
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
MWST2::configure_neighbors()
{
    // traverse set of received addresses
    for (auto it : addresses) {
        std::string name = it.first;
        int x_t = coordinates[name].first;
        int y_t = coordinates[name].second;
        int d = (x_t - position.x)*(x_t - position.x) + (y_t - position.y)*(y_t - position.y);
        edges.push_back(name);
        SE[name] = EdgeStates::Basic;
        w[name] = d;
    }

    // print (debug)
    EV_DEBUG << myself << " =>  \n";
    for (auto i : edges) {
        EV_DEBUG << "\t" << i  << " with cost " << w[i] << "\n";
    }
}


void
MWST2::send_connect(const std::string& j, bool now)
{

    auto it = requesting.find(j);
    if (it != requesting.end()) {
        /*connecting with someone who already sent me connect request */
        SE[j] = EdgeStates::Branch; // no sure
        /* I'm the root */
        parent = nil;
        std::string new_fragment = create_unique_name(j, myself);
        initiate(new_fragment);

        requesting.erase(it);
    }
    else {
        if (now) {
            SN = States::Connecting;
            connecting_with = j;
            Connect* pkt = new Connect("connect");
            pkt->setSender(myself.c_str());
            socket.sendTo(pkt, addresses[j], destinationPort);
        }
        else {
            Connect* pkt = new Connect("connect");
            pkt->setSender(j.c_str());
            pkt->setKind(CONNECT_DELAY);
            scheduleAt(simTime() + par("delay_test").doubleValue(), pkt);
        }
    }

}


void
MWST2::send_initiate(const std::string& fragmentId, const std::string& j, bool now)
{
    Initiate* pkt = new Initiate("initiate");
    pkt->setSender(myself.c_str());
    pkt->setFragmentId(fragmentId.c_str());
    if (now) {
        EV_TRACE << "Sending initiate to " << j << "(" << addresses[j] <<  ") \n";
        socket.sendTo(pkt, addresses[j], destinationPort);
    }
    else {
        pkt->setKind(INITIATE_DEALY);
        pkt->setSender(j.c_str());
        scheduleAt(simTime() + par("delay_test").doubleValue(), pkt);
    }
}

void
MWST2::send_test(const std::string& framentId, const std::string& j, bool now)
{
    Test* m = new Test((myself + "test").c_str());
    m->setSender(myself.c_str());
    m->setFragmentId(framentId.c_str());
    if (now) {
        socket.sendTo(m, addresses[j], destinationPort);
    }
    else {
        m->setSender(j.c_str());
        m->setKind(TEST_DELAY);
        scheduleAt(simTime() + par("delay_test").doubleValue(), m);
    }
    EV_TRACE << "Sending test message from " << myself << " to " << j << "(" << addresses[j] << ")" << "\n";

}


void
MWST2::wakeup()
{

    EV_TRACE << "This is wakeup \n";

    if (SN == States::Sleeping) {
        configure_neighbors();
    }

    auto m = std::min_element(edges.begin(), edges.end(), [&](std::string& e1, std::string& e2) {
        return w[e1] <= w[e2];
    });


    SE[*m] = EdgeStates::Branch;
    SN = States::Found;
    find_count = 0;
    FN = myself;

    send_connect(*m, false);

}


void
MWST2::on_connect_received(const Connect* msg)
{
    std::string j = msg->getSender();

    EV_TRACE << "Connect Received from " << msg->getSender() << "\n";
    print_state();
    if (SN == States::Sleeping) {
        wakeup();
    }

    if (SN == States::Connecting) {
        if (connecting_with == j) {
            /* perfect, I am connecting with someone who also want to connect with me */
            SE[j] = EdgeStates::Branch; // no sure
            if (myself < j) {
                parent = nil;
                /* I'm the root if my name is smaller */
                std::string new_fragment = create_unique_name(j, myself);
                initiate(new_fragment);
            }

        }
        else {
            /* I am connecting, but someone unexpected want to connect with me */
            /* put it in queue until I am available */
            requesting.insert(j);
        }
    }
    else {
        /* someone want to connect, but I am not available, put it in queue */
        requesting.insert(j);
    }
}

void
MWST2::initiate(const std::string& new_fragment_name)
{
    FN = new_fragment_name;

    SN = States::Find;
    bw = oo;
    best_edge = nil;


    for (auto i : edges) {
        if (parent != i &&
                SE[i] == EdgeStates::Branch) {

            send_initiate( FN , i, false);
            find_count++; // count of children being tested
            finding.insert(i);
        }
    }

    // TODO: we should only send test messages once we have received the response to all initiate message
    // For instance, in the on_report handler
    test();
}


void
MWST2::on_initiate_received(const Initiate* msg)
{
    EV_TRACE << "Initiate Received from " << msg->getSender() << "\n";
    print_state();
    std::string j = msg->getSender();


    /*
     * When you receive the initiate message you must:
     * 1 - change you fragment name if needed
     * 3 - record my parent in in_branch
     * 4 - go to find state
     * 5 - best-weight = oo
     * 6 - best-edge = nil_edge
     * 7 - be sure that the connection to my parent is a branch
     * 2 - forward the initiate message to all the neighbors in the tree
     * 8 - start testing neighbors that don't belong to the tree to get the shortest edge
     * */

    parent = j;
    SE[j] = EdgeStates::Branch;


    initiate(msg->getFragmentId());

}


void
MWST2::test()
{
    int min_w = oo;
    test_edge = nil;
    for (auto i : edges) {
        if (SE[i] == EdgeStates::Basic
                && w[i] < min_w && tested.find(i) == tested.end()) {
            min_w = w[i];
            test_edge = i;
        }
    }
    if (test_edge != nil) {
//        if (myself != "hostR0") {
            tested.insert(test_edge);
            send_test(FN, test_edge, false);
//        }
//        else {
//            test_edge = nil;
//        }
    }
    else {
        // there is no more neighbors to try. Report to parent
        report();
    }
}


void
MWST2::on_test_received(const Test* m)
{
    std::string j = m->getSender();
    EV_TRACE << "Test Received from " << j  << " and id " << m->getFragmentId() << "\n";
    print_state();
    if (SN == States::Sleeping) {
        wakeup();
    }

    if (FN != m->getFragmentId()) {
        // it is a different fragment
        Accept* a = new Accept("accept");
        a->setSender(myself.c_str());
        socket.sendTo(a, addresses[j], destinationPort);
    }
    else {
        if (SE[j] == EdgeStates::Basic) {
            SE[j] = EdgeStates::Rejected;
        }

        Reject* a = new Reject("reject");
        a->setSender(myself.c_str());
        socket.sendTo(a, addresses[j], destinationPort);
    }
}


void
MWST2::on_accept_received(const Accept* m)
{
    std::string j = m->getSender();
        print_state();

    if (w[j] < bw) {
        best_edge = j;
        bw = w[j];
    }
    EV_TRACE << "Accept Received from " << j << " best-w: " << bw << "(" << best_edge << ")" << " w[j]: " << w[j] << " find_count: " << find_count<< " in_branch: " << parent <<  "\n";
    test(); // TODO: try this change in the previous version
}


void
MWST2::on_reject_received(const Reject* m)
{
    std::string j = m->getSender();
    EV_TRACE << "Reject Received from " << j << "\n";
    print_state();
    if (SE[j] == EdgeStates::Basic) {

        SE[j] = EdgeStates::Rejected;
    }
    test();
}


void
MWST2::report()
{
    EV_INFO << "REPORT " << parent << " " << find_count << " " << test_edge <<  "\n";
    if (find_count) {
        EV_TRACE << "\t\tSome nodes still missing\n";
        for (auto n: finding) {
            EV_TRACE << "\t\t\t" << n << "\n";
        }
    }
    if (parent != nil) {
        if (find_count == 0 && test_edge == nil) {
            tested.clear();
            SN = States::Found;
            Report* a = new Report("report");
            a->setSender(myself.c_str());
            a->setWeight(bw);
            socket.sendTo(a, addresses[parent], destinationPort);
        }
    }
    else if (find_count == 0 && test_edge == nil) {
        /* it is the root */
        // TODO: change root
        tested.clear();
        if (bw < oo) {
            change_root();
        }
        else {
            // TODO: HALT
            EV_TRACE << "HALTTTTTT\n";
        }
    }
}


void
MWST2::on_report_received(const Report* m)
{
    int ww = m->getWeight();
    std::string j = m->getSender();

    print_state();

    find_count --;
    finding.erase(j);
    if (ww < bw) {
        bw = ww;
        best_edge = j;
    }

    EV_TRACE << "report Received from " << j << " with w:" << ww << " and bw: " << "(" << best_edge << ")" << bw << " parent: " << parent <<  "\n";
    report();
}


void
MWST2::change_root()
{
    if (SE[best_edge] == EdgeStates::Branch) {
        ChangeRoot* m = new ChangeRoot("change root");
        m->setSender(myself.c_str());
        socket.sendTo(m, addresses[best_edge], destinationPort);
    }
    else {
        send_connect(best_edge, false);
    }
}


void
MWST2::on_change_root_received(const ChangeRoot* msg)
{
    EV_TRACE << "change root Received from " << msg->getSender() << "\n";
    print_state();
    change_root();
}



} //namespace
