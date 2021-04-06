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

#include "inet/common/INETDefs.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/IRadio.h"
#include "inet/linklayer/contract/IMacProtocol.h"
#include "inet/linklayer/base/MacProtocolBase.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/common/ModuleAccess.h"

#include "LoRaMacControlInfo_m.h"
#include "LoRaMacFrame_m.h"

#if INET_VERSION < 0x0403 || ( INET_VERSION == 0x0403 && INET_PATCH_LEVEL == 0x00 )
#  error At least INET 4.3.1 is required. Please update your INET dependency and fully rebuild the project.
#endif
namespace flora {

using namespace inet;
using namespace inet::physicallayer;

class LoRaGWMac: public MacProtocolBase {
public:
    bool waitingForDC;
    cMessage *dutyCycleTimer;
    virtual void initialize(int stage) override;
    virtual void finish() override;
    //virtual InterfaceEntry *createInterfaceEntry();
    virtual void configureNetworkInterface() override;
    long GW_forwardedDown;
    long GW_droppedDC;

    virtual void handleUpperMessage(cMessage *msg) override;
    virtual void handleLowerMessage(cMessage *msg) override;
    virtual void handleSelfMessage(cMessage *message) override;

    void sendPacketBack(Packet *receivedFrame);
    void createFakeLoRaMacFrame();
    virtual MacAddress getAddress();

protected:
    MacAddress address;

    IRadio *radio = nullptr;
    IRadio::TransmissionState transmissionState = IRadio::TRANSMISSION_STATE_UNDEFINED;

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, intval_t value, cObject *details) override;
};

}

#endif /* LORA_LORAGWMAC_H_ */
