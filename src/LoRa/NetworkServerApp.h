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

#ifndef __LORANETWORK_NETWORKSERVERAPP_H_
#define __LORANETWORK_NETWORKSERVERAPP_H_

#include <omnetpp.h>
#include "inet/physicallayer/contract/packetlevel/RadioControlInfo_m.h"
#include <vector>
#include <tuple>
#include <algorithm>
#include "inet/common/INETDefs.h"

#include "LoRaMacControlInfo_m.h"
#include "LoRaMacFrame_m.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UDPSocket.h"
#include "LoRaApp/LoRaAppPacket_m.h"
#include <list>

namespace inet {

class knownNode
{
public:
    DevAddr srcAddr;
    int framesFromLastADRCommand;
    int lastSeqNoProcessed;
    int numberOfSentADRPackets;
    std::list<double> adrListSNIR;
    cOutVector *historyAllSNIR;
    cOutVector *historyAllRSSI;
    std::list<double> receivedSeqNumber;//cOutVector *receivedSeqNumber;
    cOutVector *calculatedSNRmargin;
};

class knownGW
{
public:
    L3Address ipAddr;
};

class receivedPacket
{
public:
    LoRaMacFrame* rcvdPacket;
    cMessage* endOfWaiting;
    std::vector<std::tuple<L3Address, double, double>> possibleGateways; // <address, sinr, rssi>
};

class INET_API NetworkServerApp : public cSimpleModule, cListener
{
  protected:
    std::vector<knownNode> knownNodes;
    std::vector<knownGW> knownGateways;
    std::vector<receivedPacket> receivedPackets;
    int localPort = -1, destPort = -1;
    std::vector<std::tuple<DevAddr, int>> recvdPackets;
    // state
    UDPSocket socket;
    cMessage *selfMsg = nullptr;
    int totalReceivedPackets;
    std::string adrMethod;
    double adrDeviceMargin;
    std::map<int, int> numReceivedPerNode;

  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    void processLoraMACPacket(cPacket *pk);
    void startUDP();
    void setSocketOptions();
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    bool isPacketProcessed(LoRaMacFrame* pkt);
    void updateKnownNodes(LoRaMacFrame* pkt);
    void addPktToProcessingTable(LoRaMacFrame* pkt);
    void processScheduledPacket(cMessage* selfMsg);
    void evaluateADR(LoRaMacFrame* pkt, L3Address pickedGateway, double SNIRinGW, double RSSIinGW);
    void receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject *details) override;
    bool evaluateADRinServer;

    cHistogram receivedRSSI;
  public:
    simsignal_t LoRa_ServerPacketReceived;
    int counterOfSentPacketsFromNodes = 0;
    int counterOfSentPacketsFromNodesPerSF[6];
    int counterUniqueReceivedPackets = 0;
    int counterUniqueReceivedPacketsPerSF[6];
};
} //namespace inet
#endif
