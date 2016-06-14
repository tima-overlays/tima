//
// Copyright (C) 2013 OpenSim Ltd
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package inet.applications.testingStuff;

import inet.physicallayer.contract.packetlevel.ITransmitter;
import inet.physicallayer.idealradio.IdealTransmitter;

//
// This transmitter model produces transmissions that are parameterized with
// communication range, interference range, and detection range. It also
// supports an ideal communication channel with configuring the range parameters
// to infinity.
//
// See also ~IdealReceiver, ~IdealRadio, ~IdealRadioMedium, and
// ~IdealAnalogModel.
//
module IdealTransmitterWithVariableRange extends IdealTransmitter like ITransmitter
{
    parameters:
        double minCommunicationRange @unit(m);	// the minimum range where the reception of transmissions produced by this transmitter is possible
        @class(IdealTransmitterWithVariableRange);
        @display("i=block/wtx");
}
