#ifndef __INET_SIMPLE_GOSSIP_PUSH_INET_H_
#define __INET_SIMPLE_GOSSIP_PUSH_INET_H_

#include <omnetpp.h>

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"

#include "Tima_m.h"

#include "executor.h"
#include "automata.h"
#include "mailbox.h"

#include "omnetpp_tima_nature.h"

namespace inet {


class INET_API simple_gossip_push_inet : public ApplicationBase
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
};

} //namespace

#endif
