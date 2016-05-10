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

package inet.tutorials.wireless;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;


// - create a network and specify the size to 500x500
// - drop an IPv4NetworkConfigurator and rename it to "configurator"
// - drop an IdealRadioMedium module and rename to "radioMedium"
// - drop two standardhosts at the 100,100 and 400,400 position and
//   rename them to hostA and hostB
network complex0_mwst
{
    @display("bgb=500,500");
    string hostType = default("WirelessHost");
    string mediumType = default("IdealRadioMedium");
    submodules:
        configurator: IPv4NetworkConfigurator {
            @display("p=149,29");
        }
        radioMedium: <mediumType> like IRadioMedium {
            @display("p=309,24");
        }
        hostR0: <hostType> like INetworkNode {
            @display("p=157,155");
        }
        hostR1: <hostType> like INetworkNode {
            @display("p=220,116");
        }
        hostR2: <hostType> like INetworkNode {
            @display("p=180,271");
        }
        hostR3: <hostType> like INetworkNode {
            @display("p=234,322");
        }
        hostR4: <hostType> like INetworkNode {
            @display("p=300,165");
        }
        hostR5: <hostType> like INetworkNode {
            @display("p=346,194");
        }
        hostR6: <hostType> like INetworkNode {
            @display("p=346,339");
        }
        hostR7: <hostType> like INetworkNode {
            @display("p=379,283");
        }
}
