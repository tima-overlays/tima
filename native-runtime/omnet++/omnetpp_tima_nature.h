#ifndef __OMNETPP_TIMA_NATURE__
#define __OMNETPP_TIMA_NATURE__

#include "tima_nature.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "inet/transportlayer/contract/udp/UDPControlInfo.h"

enum ControlMessageTypes {
    IDLE,
    START,
    TICK_MESSAGE
};

class OMNetTimaNature : public tima::AbstractTimaNature  {
public:
  OMNetTimaNature(std::string device_name, inet::UDPSocket& socket,std::vector<inet::L3Address> possibleNeighbors, inet::ApplicationBase* app_base):
      AbstractTimaNature(device_name), socket(socket), possibleNeighbors(possibleNeighbors), app_base(app_base) {}
  virtual void initialize();
  virtual void configure_communication(int port);
  virtual void configure_timer(uint64_t nanoseconds);
  virtual EventType identify_event();
  virtual void schedule_events();
  virtual void handle_error(const char * msg);
  virtual void send_network_message(const std::string& dst, int port, const std::string& msg);
  virtual void broadcast(int port, const std::string& msg);

private:
  inet::UDPSocket& socket;
  inet::ApplicationBase* app_base;
  std::vector<inet::L3Address> possibleNeighbors;
};

#endif
