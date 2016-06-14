//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.applications.ewma2;

import inet.applications.contract.IUDPApp;

import inet.applications.broadcasting.BroadcastingBase;

//
// This module is an application that implement the Gossip protocol
// to disemminate messages.
//
// Some nodes distribute messages in the network using unicast UDP.
//
simple EWMA2 extends BroadcastingBase like IUDPApp
{
    parameters:
       
        // parameters describing a round 
        int T_max @unit(s) = default(0.6s);
        int T_prob @unit(s) = default(T_max/3);
        int T_corr @unit(s) = default(T_max/3);
        int T_act @unit(s) = default(T_max/3);
        
        // horrible hack to specify (by hand) the neighbors in the MST
        string mst = default("");
        
        @class(EWMA2);
}
