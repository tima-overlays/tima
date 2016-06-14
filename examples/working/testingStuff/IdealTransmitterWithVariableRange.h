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

#ifndef __INET_IDEALTRANSMITTER_WITH_VARIABLE_RANGE_H
#define __INET_IDEALTRANSMITTER_WITH_VARIABLE_RANGE_H

#include "inet/physicallayer/idealradio/IdealTransmitter.h"

namespace inet {

//using physicallayer::IdealTransmitter;

/**
 * Implements the IdealTransmitter model, see the NED file for details.
 */
class INET_API IdealTransmitterWithVariableRange : public physicallayer::IdealTransmitter
{
  protected:

    m minCommunicationRange;

    m currentCommunicationRange;

  protected:
    virtual void initialize(int stage) override;

  public:
    IdealTransmitterWithVariableRange();

    virtual std::ostream& printToStream(std::ostream& stream, int level) const override;
    virtual const physicallayer::ITransmission *createTransmission(const physicallayer::IRadio *radio, const cPacket *packet, const simtime_t startTime) const override;
    virtual m getMinCommunicationRange() const { return minCommunicationRange; }
    virtual m getCurrentCommunicationRange() const { return currentCommunicationRange; }

    virtual void setCurrentCommunicationRange(m v) {
        currentCommunicationRange = v;
    }
};


} // namespace inet

#endif // ifndef __INET_IDEALTRANSMITTER_H

