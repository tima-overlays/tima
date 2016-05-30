package inet.applications.dist2mean;

import inet.applications.contract.IUDPApp;

//
// This module is an application that implement the Gossip protocol
// to disemminate messages.
//
// Some nodes distribute messages in the network using unicast UDP.
//
simple dist2mean_inet like IUDPApp
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
