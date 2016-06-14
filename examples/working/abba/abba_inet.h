#ifndef __INET_ABBA_INET_H_
#define __INET_ABBA_INET_H_

#include <omnetpp.h>

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "inet/applications/tima/executor.h"
#include "inet/applications/tima/automata.h"
#include "inet/applications/tima/mailbox.h"
#include "inet/applications/tima/tima.h"

#include "inet/applications/tima/omnetpp_tima_nature.h"

namespace inet {


class INET_API abba_inet : public tima::omnet::TimaAppBase
{
  private:
  
  	virtual std::vector<tima::Automaton*> build_stl_version() override;
  	virtual tima::device_initialization_t get_device_initialization_routine() override;
};

} //namespace

#endif
