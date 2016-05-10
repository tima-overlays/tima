

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

#include "Tima_m.h"

void
OMNetTimaNature::initialize()
{
    // so far, this is empty
}

void OMNetTimaNature::print_trace(const std::string& msg)
{
   EV_TRACE << msg << '\n';
}

void
OMNetTimaNature::send_network_message(const std::string& dst, int port, const std::string& msg)
{
  inet::L3Address result;
  inet::L3AddressResolver().tryResolve(dst.c_str(), result);
  if (result.isUnspecified())
     EV_ERROR << "cannot resolve destination address: " << dst << endl;
  else  {
    inet::Tima* pkt = new inet::Tima("rumor");
    auto pos = msg.find(';');
    pkt->setId(msg.substr(0, pos).c_str());
    pkt->setPayload(msg.substr(pos+1).c_str());
    socket.sendTo(pkt, result, port);
  }
}

void
OMNetTimaNature::broadcast(int port, const std::string& msg)
{
    for ( auto addr : possibleNeighbors ) {
        inet::Tima* pkt = new inet::Tima("Hello");
        auto pos = msg.find(';');
        pkt->setId(msg.substr(0, pos).c_str());
        pkt->setPayload(msg.substr(pos+1).c_str());
        socket.sendTo(pkt, addr, port);
    }
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
