package inet.applications.tima;

import inet.applications.contract.IUDPApp;

//
// This module is an application that implement the Gossip protocol
// to disemminate messages.
//
// Some nodes distribute messages in the network using unicast UDP.
//
simple TimaAppBase
{
    parameters:
        
        int destinationPort = default(10000);
        int localPort = default(10000);
        
        string extra_options = default("");

        string addresses = default(""); // network members
        
        @statistic[msg_received](title="A node receive a message";record=sum; source=msg_received);
        @statistic[msg_sent](title="The node sends the message";record=sum;source=msg_sent);
        @statistic[power_level](title="The power level"; record=min; source=power_level);
          
	gates:
        input udpIn @labels(UDPControlInfo/up);
        output udpOut @labels(UDPControlInfo/down);
}
