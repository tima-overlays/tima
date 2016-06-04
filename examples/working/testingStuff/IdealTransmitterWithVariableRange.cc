//
// Copyright (C) 2013 OpenSim Ltd.
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

#include "IdealTransmitterWithVariableRange.h"
#include "inet/physicallayer/idealradio/IdealTransmission.h"
#include "inet/mobility/contract/IMobility.h"

namespace inet {

Define_Module(IdealTransmitterWithVariableRange);


IdealTransmitterWithVariableRange::IdealTransmitterWithVariableRange() :
    physicallayer::IdealTransmitter(),
    minCommunicationRange(NaN),
    currentCommunicationRange(NaN)
{
}


void
IdealTransmitterWithVariableRange::initialize(int stage)
{
    physicallayer::IdealTransmitter::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        minCommunicationRange = m(par("minCommunicationRange"));
        currentCommunicationRange = maxCommunicationRange;
    }
}


std::ostream&
IdealTransmitterWithVariableRange::printToStream(std::ostream& stream, int level) const
{
    stream << "IdealTransmitterWithVariableRange";
    if (level >= PRINT_LEVEL_TRACE)
        stream << ", headerBitLength = " << headerBitLength
               << ", bitrate = " << bitrate
               << ", maxCommunicationRange = " << maxCommunicationRange
               << ", minCommunicationRange = " << minCommunicationRange
               << ", maxInterferenceRange = " << maxInterferenceRange
               << ", maxDetectionRange = " << maxDetectionRange;
    return stream;
}


const physicallayer::ITransmission *
IdealTransmitterWithVariableRange::createTransmission(const physicallayer::IRadio *transmitter, const cPacket *macFrame, const simtime_t startTime) const
{
    physicallayer::TransmissionRequest *controlInfo = dynamic_cast<physicallayer::TransmissionRequest *>(macFrame->getControlInfo());
    bps transmissionBitrate = controlInfo && !isNaN(controlInfo->getBitrate().get()) ? controlInfo->getBitrate() : bitrate;
    const simtime_t duration = (macFrame->getBitLength() + headerBitLength) / transmissionBitrate.get();
    const simtime_t endTime = startTime + duration;
    IMobility *mobility = transmitter->getAntenna()->getMobility();
    const Coord startPosition = mobility->getCurrentPosition();
    const Coord endPosition = mobility->getCurrentPosition();
    const EulerAngles startOrientation = mobility->getCurrentAngularPosition();
    const EulerAngles endOrientation = mobility->getCurrentAngularPosition();
    return new physicallayer::IdealTransmission(transmitter, macFrame, startTime, endTime, startPosition, endPosition, startOrientation, endOrientation, currentCommunicationRange, maxInterferenceRange, maxDetectionRange);
}


} // namespace inet

