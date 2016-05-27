#ifndef __INET_EWMA_H_
#define __INET_EWMA_H_

#include <omnetpp.h>

#include <map>
#include <vector>
#include <string>
#include <queue>
#include <set>

#include "inet/common/INETDefs.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/geometry/common/Coord.h"


#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"



#include "inet/applications/broadcasting/BroadcastingAppBase_m.h"
#include "inet/applications/broadcasting/BroadcastingAppBase.h"

#include "ewmaMsgs_m.h"

namespace inet {


class INET_API EWMA2 : public BroadcastingAppBase
{
  private:

    const std::string nil_edge = "";

    // mst of this node for this broadcast FIXME: use the implementation of MWST
    std::vector<std::string> local_mst;

    std::string payload;

    std::set<std::string> covered;


    bool isForwardingNode();

    virtual void on_payload_received(const broadcasting::Broadcast* m) override;
    virtual void time_to_broadcast_payload() override;

    void send_message(const std::vector<std::string>& dst);
    void send_to_uncovred();

  protected:


    virtual void processStart() override;
};

} //namespace

#endif
