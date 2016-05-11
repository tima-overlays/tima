#ifndef __INET_MWST_H_
#define __INET_MWST_H_

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

#include "MWSTMsgs_m.h"


namespace inet {


class INET_API MWST2 : public ApplicationBase
{
  private:

    const std::string nil_edge = "";

    enum States {
        Sleeping,
        Find,
        Found,
        Connecting
    };

    enum EdgeStates {
        Basic, Branch, Rejected
    };

    int destinationPort = 10000;
    int localPort = 10000;

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

    // apply spontaneously wakeup
    bool spontaneously_awaken;

    // network members
    std::map<std::string, L3Address> addresses;
    // coordinates
    std::map<std::string, std::pair<int, int> > coordinates;
    // my direct edges
    std::vector<std::string> edges;
    std::map<std::string, EdgeStates> SE;
    std::map<std::string, int> w;

    std::queue<cMessage*> old_msgs;

    // ugly variable to simulate broadcasts
    std::vector<L3Address> possibleNeighbors;


    States SN;

    // fragment identity
    std::string FN;


    // best weight
    int bw;

    // best edge
    std::string best_edge;

    // test edge
    std::string test_edge;

    // in-branch
    std::string parent;

    // find-count
    int find_count;

    // set of nodes were I am finding
    std::set< std::string > finding;

    // list of nodes trying to connect with me
    std::set<std::string> requesting;

    // set of already tested neighbors
    std::set< std::string > tested;

    //
    std::string connecting_with;


    void configure_neighbors();

    void print_state();


    void send_connect(const std::string& j, bool now);
    void send_initiate(const std::string& fragmentId, const std::string& j, bool now);
    void send_test(const std::string& framentId, const std::string& j, bool now);

    std::string create_unique_name(const std::string& a1, const std::string& a2);

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
      void task(MWST2* mwst, K*);
    };

    template <typename T> bool processMessage(cPacket* pkt, void (MWST2::*action)(const T* msg));


    virtual void processStart();



  public: // and by making this public, I am just signing my death sentence

    bool processReceivedGossip(cPacket* pkt);
    bool processReceivedHello(cPacket* pkt);
    void addNewAddress(std::string id);
private:
    void on_hello_received(const Hello* msg);

    void wakeup();
    void on_connect_received(const Connect* msg);
    void initiate(const std::string& new_fragment_name);
    void on_initiate_received(const Initiate* msg);
    void test();
    void on_test_received(const Test* msg);
    void on_accept_received(const Accept* msg);
    void on_reject_received(const Reject* msg);
    void report();
    void on_report_received(const Report* msg);
    void on_change_root_received(const ChangeRoot* msg);
    void change_root();
};

} //namespace

#endif
