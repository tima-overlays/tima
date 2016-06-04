#ifndef __OMNETPP_TIMA_NATURE__
#define __OMNETPP_TIMA_NATURE__

#include "tima_nature.h"
#include "executor.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "inet/common/INETDefs.h"
#include "inet/common/geometry/common/Coord.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "inet/transportlayer/contract/udp/UDPControlInfo.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace tima {

namespace omnet {


class INET_API TimaAppBase : public inet::ApplicationBase, public cListener {
protected:
    // communication
    int destinationPort = 10000;
    int localPort = 10000;

    inet::UDPSocket socket;

    // control messages
    cMessage* msg_tick = nullptr;

    // myself as a module
    std::string myself;
    inet::L3Address myAddress;

    // my position
    inet::Coord position;

    int radious;

    // used to schedule timers
    int msec;

    std::shared_ptr< tima::AbstractTimaNature > nature;
    std::unique_ptr<tima::Executor> executor;

    std::map<std::string, std::string> options;

    /* signals used to record statistics */
    simsignal_t signal_received_id;
    simsignal_t signal_sent_id;
    simsignal_t signal_power_level;

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;

    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
    virtual bool handleNodeShutdown(inet::IDoneCallback *doneCallback) override;
    virtual void handleNodeCrash() override;

    virtual void processStart();

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double value) override;

    void configure_next_timer();

    virtual std::vector<tima::Automaton*> build_stl_version() = 0;
    virtual device_initialization_t get_device_initialization_routine() = 0;

public:
    void emitSent();
    void emitReceived();
};

} // namespace omnet

} // namespace tima

#endif
