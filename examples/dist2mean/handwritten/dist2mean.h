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

#include "dist2mean_m.h"


namespace inet {


class INET_API Dist2Mean : public ApplicationBase
{
  private:

    const std::string nil_edge = "";

    int remote_port = 10000;
    int local_port = 10000;

    // communication
    UDPSocket socket;

    // control messages
    cMessage* ctrlMsg0 = nullptr;
    cMessage* ctrlDisplayTime = nullptr;

    // myself as a module
    std::string myself;
    L3Address myAddress;
    // my position
    Coord position;

    // how many hello messages I must send
    int nr_hello_msg;

    // is the source of a broadcast
    bool is_source;

    // my direct edges (neighbors)
    std::vector<std::string> edges;
    std::map<std::string, int> w;
    // network members (my neighbors)
    std::map<std::string, L3Address> addresses;
    // coordinates of my neighbors
    std::map<std::string, std::pair<int, int> > coordinates;

    // ugly variable to simulate broadcasts
    std::vector<L3Address> possible_neighbors;

    /* a hack to avoid the problem of two messages being lost when they are send at the same time to the same device */
    std::queue<cMessage*> old_msgs;

    /* payload of the message to boradcast */
    std::string payload;
    /* indicates the set of nodes from whom I received this message */
    std::set<std::string> received_from;

    void configure_neighbors();
    void on_hello_received(const dist2mean::Hello* msg);

    void on_payload_received(const dist2mean::Broadcast* m);

    void send_message();

  protected:

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;

    virtual bool handleNodeStart(IDoneCallback *doneCallback) override;
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback) override;
    virtual void handleNodeCrash() override;

    template <typename K>
    class Action {
      void task(Dist2Mean*, K*);
    };

    template <typename T> bool processMessage(cPacket* pkt, void (Dist2Mean::*action)(const T* msg));


    virtual void processStart();



  public: // and by making this public, I am just signing my death sentence

    bool processReceivedGossip(cPacket* pkt);
    bool processReceivedHello(cPacket* pkt);
    void addNewAddress(std::string id);
};

} //namespace

#endif
