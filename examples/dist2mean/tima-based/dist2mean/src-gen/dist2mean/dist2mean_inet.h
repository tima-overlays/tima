#ifndef __INET_DIST2MEAN_INET_H_
#define __INET_DIST2MEAN_INET_H_

#include <omnetpp.h>

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "inet/common/INETDefs.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/geometry/common/Coord.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"

#include "inet/applications/tima/Tima_m.h"

#include "inet/applications/tima/executor.h"
#include "inet/applications/tima/automata.h"
#include "inet/applications/tima/mailbox.h"
#include "inet/applications/tima/tima.h"

#include "inet/applications/tima/omnetpp_tima_nature.h"

namespace inet {


class INET_API dist2mean_inet : public ApplicationBase
{
  protected:

    int destinationPort = 10000;
    int localPort = 10000;

    // communication
    UDPSocket socket;

    // control messages
    cMessage* msg_tick = nullptr;

    // myself as a module
    std::string myself;
    L3Address myAddress;
    
   	// my position
   	Coord position;

    int msec;

    std::shared_ptr< tima::AbstractTimaNature > nature;
    std::unique_ptr<tima::Executor> executor;

    std::map<std::string, std::string> options;

  protected:

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;

    virtual bool handleNodeStart(IDoneCallback *doneCallback) override;
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback) override;
    virtual void handleNodeCrash() override;

    virtual void processStart();

private:
    void configure_next_timer();
	std::vector<tima::Automaton*> build_stl_version();
};

} //namespace

#endif
