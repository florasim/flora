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

#ifndef LORA_LORAGWMAC_H_
#define LORA_LORAGWMAC_H_

#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/linklayer/contract/IMACProtocol.h"
#include "inet/linklayer/base/MACProtocolBase.h"
#include "inet/common/ModuleAccess.h"

#include "LoRaMacControlInfo_m.h"
#include "LoRaMacFrame_m.h"

namespace inet {

using namespace physicallayer;

class LoRaGWMac: public MACProtocolBase {
public:
    bool waitingForDC;
    cMessage *dutyCycleTimer;
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual InterfaceEntry *createInterfaceEntry() override;
    long GW_forwardedDown;
    long GW_droppedDC;

    virtual void handleUpperPacket(cPacket *msg) override;
    virtual void handleLowerPacket(cPacket *msg) override;
    virtual void handleSelfMessage(cMessage *message) override;

    void sendPacketBack(LoRaMacFrame *receivedFrame);
    void createFakeLoRaMacFrame();
    virtual DevAddr getAddress();

protected:
    DevAddr address;

    IRadio *radio = nullptr;
    IRadio::TransmissionState transmissionState = IRadio::TRANSMISSION_STATE_UNDEFINED;

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject *details) override;
};

}

#endif /* LORA_LORAGWMAC_H_ */
