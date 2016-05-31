#ifndef __INET_DIST2MEAN_H_
#define __INET_DIST2MEAN_H_

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

#include "inet/applications/broadcasting/BroadcastingAppBase.h"
#include "inet/applications/broadcasting/BroadcastingAppBase_m.h"


namespace inet {

class INET_API Abba2 : public inet::BroadcastingAppBase
{
  private:

    class Range {
    public:
        double min;
        double max;
        Range (double b, double top) : min(b), max(top) {}

        bool intersect(double from, double to) {
            if (from >= min && from <= max && to >= max) {
                //
                max = to;
                return true;
            }
            else if (from >= min && from <= max && to >= min && to <= max) {
                return true;
            }
            else if (from <= min && to >= max) {
                max = to;
                min = from;
                return true;
            }
            else if (from <= min && to >= min && to <= max) {
                min = from;
                return true;
            }
            else {
                return false;
            }
        }
    };

    /* payload of the message to broadcast */
    std::map< std::string, std::string >  payloads;
    /* indicates the set of nodes from whom I received this message */
    std::map< std::string, std::vector<Range>> received_from;

    virtual void on_payload_received(const broadcasting::Broadcast* m) override;
    virtual void time_to_broadcast_payload(void* user_data) override;

    void send_message(std::string& key);

    int add(std::vector<Range>& v, double a1, double a2 );
};

} //namespace

#endif
