#include "dist2mean_inet.h"
#include "inet/applications/tima/Tima_m.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/mobility/contract/IMobility.h"

#include <algorithm>
#include <sstream>

uint32_t get_nr_automaton_in_dist2mean();	
struct tima::Automaton& get_automaton_dist2mean(uint32_t idx);

void
init_device_data_dist2mean(
	std::string& device_name,
	std::map<std::string, std::string>& options,
	std::shared_ptr<tima::GlobalStorage> st);

namespace inet {

Define_Module(dist2mean_inet);


void dist2mean_inet::initialize(int stage)
{
	ApplicationBase::initialize(stage);
	
	switch (stage) {
        case INITSTAGE_LOCAL: {

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

            break;
        default:
            break;
    }
}

void dist2mean_inet::handleMessageWhenUp(cMessage *msg)
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
            case ControlMessageTypes::FUTURE_MESSAGE:
                std::dynamic_pointer_cast<OMNetTimaNature>(nature)->process_message(msg);
                cancelAndDelete(msg);
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
                EV_TRACE << " a message " << tima_msg->getName() << "\n";
                int id = std::stoi(tima_msg->getId());
                executor->add_received_network_message(id, (char*)tima_msg->getPayload());
            }

//            EV_DEBUG << "A network message: " << tima_msg->getId() << " " << tima_msg->getPayload() << '\n';

            delete pkt;

            break;
        }
    }
}

void dist2mean_inet::finish()
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;
}

bool dist2mean_inet::handleNodeStart(IDoneCallback *doneCallback)
{
    msg_tick->setKind(START);
    scheduleAt(simTime() + 0.01, msg_tick);
    return true;
}

bool dist2mean_inet::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;

    return true;
}

void dist2mean_inet::handleNodeCrash()
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;
}

void dist2mean_inet::configure_next_timer() {
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

void dist2mean_inet::processStart()
{
	myself = this->getParentModule()->getFullName();
	L3AddressResolver().tryResolve(myself.c_str(), myAddress);
	EV_TRACE << "Starting the process in module " << myself << " (" << myAddress.str() << ")" << "\n";
	
	
	
	const char *destAddrs = par("addresses");
	cStringTokenizer tokenizer(destAddrs);
	const char *token;
	std::map<std::string, inet::L3Address> possibleNeighbors;
	
	while ((token = tokenizer.nextToken()) != nullptr) {
	    L3Address result;
	    L3AddressResolver().tryResolve(token, result);
	    if (result.isUnspecified())
	        EV_ERROR << "cannot resolve destination address: " << ((token)?token:"NULL") << endl;
	    else if (myself != token)
	        possibleNeighbors.emplace(token, result);
	}
	
	nature = std::make_shared<OMNetTimaNature>(myself, socket, possibleNeighbors, this);
	nature->initialize();
	nature->configure_communication(localPort);
	
	options.emplace("posX", std::to_string(position.x));
	options.emplace("posY", std::to_string(position.y));
	
	
	EV_TRACE << "Creating protocol's executer\n";
	executor = std::unique_ptr<tima::Executor>(new tima::Executor(build_stl_version(), nature, options, &init_device_data_dist2mean));
	configure_next_timer();
}



std::vector<tima::Automaton*>
dist2mean_inet::build_stl_version()
{
  std::vector<tima::Automaton*> automatas;
  uint32_t n = get_nr_automaton_in_dist2mean();
  for (size_t i = 0; i < n; i++) {
    /* code */
    struct tima::Automaton* x = &get_automaton_dist2mean(i);
    automatas.push_back(x);
  }
  return automatas;
}

} //namespace
