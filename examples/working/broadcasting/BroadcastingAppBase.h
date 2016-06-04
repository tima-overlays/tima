#ifndef __INET_BROADCASTING_BASE_H_
#define __INET_BROADCASTING_BASE_H_

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

#include "BroadcastingAppBase_m.h"


namespace inet {


class INET_API BroadcastingAppBase : public ApplicationBase , public cListener
{
  protected:
    class Neighbor {
    public:
        std::string name;
        L3Address addr;
        Coord pos;
        int w;
    };
    enum ControlMessageTypes {
        IDLE,
        START,
        SAY_HELLO,
        WAKEUP,
        DISPLAY_TIME,
        BROADCAST_DELAY
    };

    // is the source of a broadcast
    bool is_source;

    // number of broadcast message to send
    int nr_broadcast_msg;

    // my direct edges (neighbors)
    std::map<std::string, Neighbor> neighbors;

    /* a hack to avoid the problem of two messages being lost when they are send at the same time to the same device */
    std::queue<cMessage*> old_msgs;

    // communication
    int remote_port = 10000;
    int local_port = 10000;
    UDPSocket socket;

    // myself as a module
    std::string myself;
    L3Address myAddress;
    // my position
    Coord position;
    int radious;

    // counter to assign ids to broadcast messages
    int last_id = 0;

  private:

    // control messages
    cMessage* ctrlMsg0 = nullptr;
    cMessage* ctrlDisplayTime = nullptr;

    // how many hello messages I must send
    int nr_hello_msg;

    /* signals used to record statistics */
    simsignal_t signal_received_id;
    simsignal_t signal_sent_id;
    simsignal_t signal_power_level;

    bool already_configured = false;

    void configure_neighbors();

    void on_hello_received(const broadcasting::Hello* msg);

  protected:

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;

    virtual bool handleNodeStart(IDoneCallback *doneCallback) override;
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback) override;
    virtual void handleNodeCrash() override;



    virtual void processStart();
    void delayed_broadcast(const std::string& key, double delay);

    template <typename K>
    class Action {
      void task(BroadcastingAppBase*, K*);
    };
    template <typename T> bool processMessage(cPacket* pkt, void (BroadcastingAppBase::*action)(const T* msg));

    virtual void on_payload_received(const broadcasting::Broadcast* m);
    virtual bool on_network_message_received(cPacket* pkt);

    virtual void time_to_broadcast_payload(void* user_data);

    void emitSent();
    void emitReceived();
    void emitPowerLevel(double value);

    L3Address getAddr(std::string id);

    void delay_broadcast(void* user_data);

    int get_next_id_for_msg();
    int get_last_id_for_msg();

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double value) override;

  public:
    BroadcastingAppBase();

};

} //namespace

#endif
