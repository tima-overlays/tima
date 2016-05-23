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

#include "ewmaMsgs_m.h"


namespace inet {


class INET_API EWMA2 : public ApplicationBase
{
  private:

    const std::string nil_edge = "";

    enum States {
        Sleeping,
        Find,
        Found,
        Connecting
    };

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


    // mst of this node for this broadcast FIXME: use the implementation of MWST
    std::vector<std::string> local_mst;

    /* a hack to avoid the problem of two messages being lost when they are send at the same time to the same device */
    std::queue<cMessage*> old_msgs;


    std::string payload;

    std::set<std::string> covered;

    States SN;



    void configure_neighbors();
    void on_hello_received(const ewma_Hello* msg);
    bool isForwardingNode();

    void on_payload_received(const ewma_Broadcast* m);

    void send_message(const std::vector<std::string>& dst);
    void send_to_uncovred();

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
      void task(EWMA2* mwst, K*);
    };

    template <typename T> bool processMessage(cPacket* pkt, void (EWMA2::*action)(const T* msg));


    virtual void processStart();



  public: // and by making this public, I am just signing my death sentence

    bool processReceivedGossip(cPacket* pkt);
    bool processReceivedHello(cPacket* pkt);
    void addNewAddress(std::string id);
};

} //namespace

#endif
