

#include "omnetpp_tima_nature.h"

#include "mailbox.h"

#include "Tima_m.h"

#include <iostream>
#include <cstring>
#include <stdexcept>

#include <omnetpp.h>

#include <map>
#include <vector>
#include <string>

#include "inet/common/INETDefs.h"

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/power/contract/IEnergyStorage.h"
#include "inet/physicallayer/idealradio/IdealTransmitter.h"
#include "inet/common/ModuleAccess.h"

#include "Tima_m.h"

using namespace inet;

namespace tima {

namespace omnet {


enum ControlMessageTypes {
    IDLE,
    START,
    TICK_MESSAGE,
    FUTURE_MESSAGE
};

class OMNetTimaNature : public tima::AbstractTimaNature  {
public:
  OMNetTimaNature(std::string device_name, inet::UDPSocket& socket,std::map<std::string, inet::L3Address> possibleNeighbors, inet::ApplicationBase* app_base):
      AbstractTimaNature(device_name), socket(socket), app_base(app_base), possibleNeighbors(possibleNeighbors) {}

  virtual void initialize() {
      // so far, empty
  }

  virtual void configure_communication(int port);
  virtual void configure_timer(uint64_t nanoseconds);
  virtual EventType identify_event(int* msg_id, char** payload);
  virtual void schedule_events();
  virtual void handle_error(const char * msg);
  virtual void send_network_message(const std::string& dst, int port, const std::string& msg);
  virtual void broadcast(int port, const std::string& msg);
  virtual void print_trace(const std::string& msg);

  virtual void report_received_message() override {
      ((TimaAppBase*)app_base)->emitReceived();
  }

  virtual void report_sent_message() override {
      ((TimaAppBase*)app_base)->emitSent();
  }

  void process_message(cMessage* m);

private:


  struct ScheduledMsg {
      std::string msg;
      std::string dst;
      int port;

      ScheduledMsg(const std::string& m, const std::string& d, int p) : msg(m), dst(d), port(p) {}
  };

  inet::UDPSocket& socket;
  inet::ApplicationBase* app_base;
  std::map<std::string, inet::L3Address> possibleNeighbors;
};


void OMNetTimaNature::print_trace(const std::string& msg)
{
   EV_TRACE << msg << '\n';
}


void
OMNetTimaNature::process_message(cMessage* m)
{
    auto all = (ScheduledMsg*)m->getContextPointer();
    auto msg = all->msg;
    auto dst = all->dst;
    auto port = all->port;
    inet::L3Address addr;
    if (dst != "255.255.255.255") {
        addr = possibleNeighbors[dst];
    }
    else {
        inet::L3AddressResolver resolver;
        addr = resolver.resolve(dst.c_str(), inet::L3AddressResolver::ADDR_IPv4);
    }
    if (addr.isUnspecified())
        EV_ERROR << "cannot resolve destination address: " << dst << endl;
    else  {
        auto pos = msg.find(';');
        inet::Tima* pkt = new inet::Tima(("rumor " + msg.substr(0, pos)).c_str());
        pkt->setId(msg.substr(0, pos).c_str());
        pkt->setPayload(msg.substr(pos+1).c_str());
        socket.sendTo(pkt, addr, port);
        EV_TRACE << "Sending " << msg.substr(0, pos) << " to " << dst << "(" << addr<< ")" << "\n";
    }
    delete all;
}

void
OMNetTimaNature::send_network_message(const std::string& dst, int port, const std::string& msg)
{
    cMessage* m = new cMessage("future message", FUTURE_MESSAGE);
    m->setContextPointer(new ScheduledMsg(msg, dst, port));
    app_base->scheduleAt(simTime() + (rand() % 100) / 10000.0, m);
}

void
OMNetTimaNature::broadcast(int port, const std::string& msg)
{

    cMessage* m = new cMessage("future message(Hello)", FUTURE_MESSAGE);
    m->setContextPointer(new ScheduledMsg(msg, "255.255.255.255", port));
    app_base->scheduleAt(simTime() + (rand() % 100) / 10000.0, m);

}

void
OMNetTimaNature::configure_communication(int port)
{
    socket.setOutputGate(app_base->gate("udpOut"));
    socket.bind(port);
    socket.setBroadcast(true);
}

void
OMNetTimaNature::configure_timer(uint64_t nanoseconds)
{
  cMessage* m = new cMessage("a tick");
  m->setKind(ControlMessageTypes::TICK_MESSAGE);
  app_base->scheduleAt(simTime() + ((double)nanoseconds / 1000000000.0 ), m);
}

tima::AbstractTimaNature::EventType
OMNetTimaNature::identify_event(int* msg_id, char** payload)
{
	*msg_id = 0;
    *payload = nullptr;
	return AbstractTimaNature::EventType::NONE;
}

void
OMNetTimaNature::schedule_events()
{
    // Wait FOR EVENT
}


void
OMNetTimaNature::handle_error(const char *msg)
{
  do {
      std::cerr << msg << std::endl;
      throw std::runtime_error(msg);
  } while (0);
}


void
TimaAppBase::initialize(int stage)
{
    inet::ApplicationBase::initialize(stage);

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

              signal_received_id = this->registerSignal("msg_received");
              signal_sent_id = this->registerSignal("msg_sent");
              signal_power_level = this->registerSignal("power_level");
              {
                cModule *hostModule = getParentModule();
                hostModule->subscribe(inet::power::IEnergyStorage::residualCapacityChangedSignal, this);
              }
            }
            break;
        case INITSTAGE_PHYSICAL_ENVIRONMENT_2:
            {
                cModule* host = inet::getContainingNode(this);
                IMobility* mobility = check_and_cast<IMobility*>(host->getSubmodule("mobility"));
                physicallayer::IdealTransmitter* transmitter = check_and_cast<physicallayer::IdealTransmitter*>(host->getModuleByPath(".wlan[0].radio.transmitter"));

                this->position = mobility->getCurrentPosition();
                this->radious = transmitter->getMaxCommunicationRange().get();

                EV_TRACE << "My position is " << this->position  << "\n";
            }
            break;
        case INITSTAGE_LAST:

            break;
        default:
            break;
    }
}

void
TimaAppBase::handleMessageWhenUp(cMessage *msg)
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

void
TimaAppBase::finish()
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;
}

bool
TimaAppBase::handleNodeStart(IDoneCallback *doneCallback)
{
    msg_tick->setKind(START);
    scheduleAt(simTime() + 0.01, msg_tick);
    return true;
}

bool
TimaAppBase::handleNodeShutdown(IDoneCallback *doneCallback)
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;

    return true;
}

void
TimaAppBase::handleNodeCrash()
{
    if (msg_tick)
        cancelAndDelete(msg_tick);
    msg_tick = nullptr;
}

void
TimaAppBase::configure_next_timer() {
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

void
TimaAppBase::processStart()
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
    options.emplace("radious", std::to_string(radious));


    EV_TRACE << "Creating protocol's executer\n";
    executor = std::unique_ptr<tima::Executor>(
            new tima::Executor(build_stl_version(), nature, options, get_device_initialization_routine())
    );
    configure_next_timer();
}

void
TimaAppBase::receiveSignal(cComponent *source, simsignal_t signalID, double value)
{
    if (signalID == inet::power::IEnergyStorage::residualCapacityChangedSignal) {
        emit(signal_power_level, value);
    }
}

void
TimaAppBase::emitSent()
{
    emit(signal_sent_id, 1);
}

void
TimaAppBase::emitReceived()
{
    emit(signal_received_id, 1);
}

} // namespace omnet

} // namespace tima
