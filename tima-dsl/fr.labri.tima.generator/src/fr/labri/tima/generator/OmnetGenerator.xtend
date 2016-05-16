package fr.labri.tima.generator

import fr.labri.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

class OmnetGenerator extends NativeGenerator {
	
	new(IRAutomata a) {
		super(a)
	}
	
	override def void generateFiles(String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		super.generateFiles(name, fsa, context)
		
		fsa.generateFile('''«name»/«name»_inet.ned''', omnet_inet_app_descriptor(name, name + "_inet"))
		fsa.generateFile('''«name»/«name»_inet.h''', omnet_inet_app_header(name + "_inet"))
		fsa.generateFile('''«name»/«name»_inet.cc''', 
			omnet_inet_app_code(name + "_inet")
		)
	}
	
	def omnet_inet_app_descriptor(String package_name, String project_name){
	'''
	package inet.applications.«package_name»;
	
	import inet.applications.contract.IUDPApp;
	
	//
	// This module is an application that implement the Gossip protocol
	// to disemminate messages.
	//
	// Some nodes distribute messages in the network using unicast UDP.
	//
	simple «project_name» like IUDPApp
	{
	    parameters:
	        
	        int destinationPort = default(10000);
	        int localPort = default(10000);
	        
	        string extra_options = default("");
	
	        string addresses = default(""); // network members
	          
		gates:
	        input udpIn @labels(UDPControlInfo/up);
	        output udpOut @labels(UDPControlInfo/down);
	}
	'''
	}
	
	def String omnet_inet_app_code(String project_name) {
	'''
	#include "«project_name».h"
	#include "Tima_m.h"
	
	#include "inet/networklayer/common/L3AddressResolver.h"
	#include "inet/transportlayer/contract/udp/UDPControlInfo.h"
	
	#include <algorithm>
	#include <sstream>
	
	namespace inet {
	
	Define_Module(«project_name»);
	
	
	void «project_name»::initialize(int stage)
	{
	    ApplicationBase::initialize(stage);
	
	    if (stage == INITSTAGE_LOCAL) {
	
	        localPort = par("localPort");
	        destinationPort = par("destinationPort");
	
	        const char *params = par("extra_options");
	        cStringTokenizer tokenizer(params,":");
	        const char *token;
	
	        while ((token = tokenizer.nextToken()) != nullptr) {
	            cStringTokenizer tokenizer2(token, "=");
	            const char* name = tokenizer2.nextToken();
	            const char* value= tokenizer2.nextToken();
	            options.emplace(name, value);
	        }
	
	
	        msg_tick = new cMessage("msg_ctrl", ControlMessageTypes::IDLE);
	    }
	}
	
	void «project_name»::handleMessageWhenUp(cMessage *msg)
	{
	    cPacket* pkt = nullptr;
	    tima::AbstractTimaNature::EventType eventType = tima::AbstractTimaNature::EventType::NONE;
	    if (msg->isSelfMessage()) {
	
	        switch (msg->getKind()) {
	            case ControlMessageTypes::START:
	                processStart();
	                break;
	            case ControlMessageTypes::TICK_MESSAGE:
	                eventType = tima::AbstractTimaNature::EventType::TICK;
	                break;
	        }
	    }
	    else if (msg->getKind() == UDP_I_DATA) {
	        eventType = tima::AbstractTimaNature::EventType::NETWORK_MESSAGE;
	    }
	
	    switch (eventType) {
	        case tima::AbstractTimaNature::EventType::TICK:
	            cancelAndDelete(msg);
	            executor->tick(msec);
	            configure_next_timer();
	            break;
	        case tima::AbstractTimaNature::EventType::NETWORK_MESSAGE:
	        {
	            pkt = PK(msg);
	
	            Tima* tima_msg = check_and_cast_nullable<Tima*>(dynamic_cast<Tima*>(pkt));
	            if (tima_msg != nullptr) {
	                std::istringstream convert(tima_msg->getId());
                    int id;
                    convert >> id;
                    executor->add_received_network_message(id, (char*)tima_msg->getPayload());
	            }
	
	//            EV_DEBUG << "A network message: " << tima_msg->getId() << " " << tima_msg->getPayload() << '\n';
	
	            delete pkt;
	
	            break;
	        }
	    }
	}
	
	void «project_name»::finish()
	{
	    if (msg_tick)
	        cancelAndDelete(msg_tick);
	    msg_tick = nullptr;
	}
	
	bool «project_name»::handleNodeStart(IDoneCallback *doneCallback)
	{
	    msg_tick->setKind(START);
	    scheduleAt(simTime() + 0.01, msg_tick);
	    return true;
	}
	
	bool «project_name»::handleNodeShutdown(IDoneCallback *doneCallback)
	{
	    if (msg_tick)
	        cancelAndDelete(msg_tick);
	    msg_tick = nullptr;
	
	    return true;
	}
	
	void «project_name»::handleNodeCrash()
	{
	    if (msg_tick)
	        cancelAndDelete(msg_tick);
	    msg_tick = nullptr;
	}
	
	void «project_name»::configure_next_timer() {
	    msec = executor->global_deadline();
	//    EV_DEBUG << "next deadline is in " << msec << " milliseconds\n";
	    if (msec == tima::never_timeout) {
	        msec = 100;
	    }
	    if (msec > 30) {
	        msec = 30;
	    }
	    nature->configure_timer((uint64_t) ((msec)) * 1000000);
	    nature->schedule_events();
	}
	
	void «project_name»::processStart()
	{
	    myself = this->getParentModule()->getFullName();
	    L3AddressResolver().tryResolve(myself.c_str(), myAddress);
	    EV_TRACE << "Starting the process in module " << myself << " (" << myAddress.str() << ")" << "\n";
	
	
	
	    const char *destAddrs = par("addresses");
	    cStringTokenizer tokenizer(destAddrs);
	    const char *token;
	    std::vector<L3Address> possibleNeighbors;
	
	    while ((token = tokenizer.nextToken()) != nullptr) {
	        L3Address result;
	        L3AddressResolver().tryResolve(token, result);
	        if (result.isUnspecified())
	            EV_ERROR << "cannot resolve destination address: " << ((token)?token:"NULL") << endl;
	        else if (myself != token)
	            possibleNeighbors.push_back(result);
	    }
	
	    nature = std::make_shared<OMNetTimaNature>(myself, socket, possibleNeighbors, this);
	    nature->initialize();
	    nature->configure_communication(localPort);
	
	
	    EV_TRACE << "Creating protocol's executer\n";
	    executor = std::unique_ptr<tima::Executor>(new tima::Executor(nature, options));
	    configure_next_timer();
	}
	
	} //namespace
	'''
	}
	
	def omnet_inet_app_header(String project_name) {
	'''
	#ifndef __INET_«project_name.toUpperCase»_H_
	#define __INET_«project_name.toUpperCase»_H_
	
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
	#include "tima.h"
	
	#include "omnetpp_tima_nature.h"
	
	namespace inet {
	
	
	class INET_API «project_name» : public ApplicationBase
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
	'''
	}
}
