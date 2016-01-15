#include "simple_gossip_push_inet.h"
#include "Tima_m.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"

#include <algorithm>

namespace inet {

Define_Module(simple_gossip_push_inet);


void simple_gossip_push_inet::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        localPort = par("localPort");
        destinationPort = par("destinationPort");

        const char *params = par("extra_options");
        cStringTokenizer tokenizer(params,":");
        const char *token;

        while ((token = tokenizer.nextToken()) != nullptr) {
            cStringTokenizer tokenizer2(token, "=");
            const char* name = tokenizer2.nextToken();
            const char* value= tokenizer2.nextToken();
            options.emplace(name, value);
        }


        msg_tick = new cMessage("msg_ctrl", ControlMessageTypes::IDLE);
    }
}

void simple_gossip_push_inet::handleMessageWhenUp(cMessage *msg)
{
    cPacket* pkt = nullptr;
    tima::AbstractTimaNature::EventType eventType = tima::AbstractTimaNature::EventType::NONE;
    if (msg->isSelfMessage()) {

        switch (msg->getKind()) {
            case ControlMessageTypes::START:
                processStart();
                break;
            case ControlMessageTypes::TICK_MESSAGE:
                eventType = tima::AbstractTimaNature::EventType::TICK;
                break;
        }
    }
    else if (msg->getKind() == UDP_I_DATA) {
        eventType = tima::AbstractTimaNature::EventType::NETWORK_MESSAGE;
    }

    switch (eventType) {
        case tima::AbstractTimaNature::EventType::TICK:
            cancelAndDelete(msg);
            executor->tick(msec);
            configure_next_timer();
            break;
        case tima::AbstractTimaNature::EventType::NETWORK_MESSAGE:
        {
            pkt = PK(msg);

            Tima* tima_msg = check_and_cast_nullable<Tima*>(dynamic_cast<Tima*>(pkt));
            if (tima_msg != nullptr) {
                executor->add_received_network_message(get_msg_id_from_name(tima_msg->getId()), (char*)tima_msg->getPayload());
            }

//            EV_DEBUG << "A network message: " << tima_msg->getId() << " " << tima_msg->getPayload() << '\n';

            delete pkt;

            break;
        }
    }
}

void simple_gossip_push_inet::finish()
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;
}

bool simple_gossip_push_inet::handleNodeStart(IDoneCallback *doneCallback)
{
    msg_tick->setKind(START);
    scheduleAt(simTime() + 0.01, msg_tick);
    return true;
}

bool simple_gossip_push_inet::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;

    return true;
}

void simple_gossip_push_inet::handleNodeCrash()
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;
}

void simple_gossip_push_inet::configure_next_timer() {
    msec = executor->global_deadline();
//    EV_DEBUG << "next deadline is in " << msec << " milliseconds\n";
    if (msec == tima::never_timeout) {
        msec = 100;
    }
    if (msec > 30) {
        msec = 30;
    }
    nature->configure_timer((uint64_t) ((msec)) * 1000000);
    nature->schedule_events();
}

void simple_gossip_push_inet::processStart()
{
    myself = this->getParentModule()->getFullName();
    L3AddressResolver().tryResolve(myself.c_str(), myAddress);
    EV_TRACE << "Starting the process in module " << myself << " (" << myAddress.str() << ")" << "\n";



    const char *destAddrs = par("addresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;
    std::vector<L3Address> possibleNeighbors;

    while ((token = tokenizer.nextToken()) != nullptr) {
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << ((token)?token:"NULL") << endl;
        else if (myself != token)
            possibleNeighbors.push_back(result);
    }

    nature = std::make_shared<OMNetTimaNature>(myself, socket, possibleNeighbors, this);
    nature->initialize();
    nature->configure_communication(localPort);


    EV_TRACE << "Creating protocol's executer\n";
    executor = std::unique_ptr<tima::Executor>(new tima::Executor(nature, options));
    configure_next_timer();
}

//void simple_gossip_push_inet::addNewAddress(std::string id)
//{
//    if (myself != id) {
//        L3Address result;
//        L3AddressResolver().tryResolve(id.c_str(), result);
//        auto it = addresses.find(id);
//        if (it == addresses.end()) {
//            EV_TRACE << "Hello from " << id << "\n";
//            addresses.insert(std::pair<string, L3Address>(id, result));
//        }
//    }
//}
//
//void GossipPush::addNewInfection(Gossip* g)
//{
//    bool exists = std::any_of(infections.begin(), infections.end(), [&](GossipInfection t) {
//        return (g->getId() == t.idMsg) && (g->getSource() == t.source);
//    });
//
//    if (!exists) {
//        GossipInfection t;
//        t.idMsg = g->getId();
//        t.roundsLeft = roundRatio;
//        t.source = g->getSource();
//        t.text = g->getMsg();
//        infections.push_back(t);
//
//        UDPDataIndication *ctrl = check_and_cast<UDPDataIndication *>(g->getControlInfo());
//
//        EV_TRACE << "A new foreign message : '"  <<  t.text << "' from " << t.source << " through "<< ctrl->getSrcAddr() << "\n";
//    }
//}

} //namespace
