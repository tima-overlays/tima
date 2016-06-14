package inet.applications.ewma;

import inet.applications.contract.IUDPApp;

import inet.applications.tima.TimaAppBase;

//
// This module is an application that implement the Gossip protocol
// to disemminate messages.
//
// Some nodes distribute messages in the network using unicast UDP.
//
simple ewma_inet extends TimaAppBase like IUDPApp
{
    parameters:
        @class(ewma_inet);
}
