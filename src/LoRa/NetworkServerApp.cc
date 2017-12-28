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

#include "NetworkServerApp.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/applications/base/ApplicationPacket_m.h"

namespace inet {

Define_Module(NetworkServerApp);


void NetworkServerApp::initialize(int stage)
{
    if (stage == 0) {
        ASSERT(recvdPackets.size()==0);
        LoRa_ServerPacketReceived = registerSignal("LoRa_ServerPacketReceived");
        localPort = par("localPort");
        destPort = par("destPort");
        adrMethod = par("adrMethod").stdstringValue();
    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        startUDP();
        getSimulation()->getSystemModule()->subscribe("LoRa_AppPacketSent", this);
        evaluateADRinServer = par("evaluateADRinServer");
        receivedRSSI.setName("Received RSSI");
        numOfReceivedPackets = 0;
        for(int i=0;i<6;i++)
        {
            counterOfReceivedPacketsPerSF[i] = 0;
            counterOfSentPacketsFromNodesPerSF[i] = 0;
        }
    }
}


void NetworkServerApp::startUDP()
{
    socket.setOutputGate(gate("udpOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
}


void NetworkServerApp::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("udpIn")) {
        LoRaMacFrame *frame = check_and_cast<LoRaMacFrame *>(msg);
        if (simTime() >= getSimulation()->getWarmupPeriod())
        {
            numOfReceivedPackets++;
            counterOfReceivedPacketsPerSF[frame->getLoRaSF()-7]++;
        }
        updateKnownNodes(frame);
        processLoraMACPacket(PK(msg));
    }
    if(msg->isSelfMessage())
    {
        processScheduledPacket(msg);
    }
}

void NetworkServerApp::processLoraMACPacket(cPacket *pk)
{
    LoRaMacFrame *frame = check_and_cast<LoRaMacFrame *>(pk);
    if(isPacketProcessed(frame))
    {
        delete pk;
        return;
    }
    addPktToProcessingTable(frame);
}

void NetworkServerApp::finish()
{
    recordScalar("LoRa_NS_DER", double(counterOfReceivedPackets)/counterOfSentPacketsFromNodes);
    for(uint i=0;i<knownNodes.size();i++)
    {
        delete knownNodes[i].historyAllSNIR;
        delete knownNodes[i].historyAllRSSI;
        delete knownNodes[i].receivedSeqNumber;
        delete knownNodes[i].calculatedSNRmargin;
        recordScalar("Send ADR for node", knownNodes[i].numberOfSentADRPackets);
    }
    receivedRSSI.recordAs("receivedRSSI");
    recordScalar("numOfReceivedPackets", numOfReceivedPackets);
    for(uint i=0;i<receivedPackets.size();i++)
    {
        delete receivedPackets[i].rcvdPacket;
    }
    recordScalar("numOfReceivedPacketsPerSF SF7", counterOfReceivedPacketsPerSF[0]);
    recordScalar("numOfReceivedPacketsPerSF SF8", counterOfReceivedPacketsPerSF[1]);
    recordScalar("numOfReceivedPacketsPerSF SF9", counterOfReceivedPacketsPerSF[2]);
    recordScalar("numOfReceivedPacketsPerSF SF10", counterOfReceivedPacketsPerSF[3]);
    recordScalar("numOfReceivedPacketsPerSF SF11", counterOfReceivedPacketsPerSF[4]);
    recordScalar("numOfReceivedPacketsPerSF SF12", counterOfReceivedPacketsPerSF[5]);
    if (counterOfSentPacketsFromNodesPerSF[0] > 0)
        recordScalar("DER SF7", double(counterOfReceivedPacketsPerSF[0]) / counterOfSentPacketsFromNodesPerSF[0]);
    else
        recordScalar("DER SF7", 0);

    if (counterOfSentPacketsFromNodesPerSF[1] > 0)
        recordScalar("DER SF8", double(counterOfReceivedPacketsPerSF[1]) / counterOfSentPacketsFromNodesPerSF[1]);
    else
        recordScalar("DER SF8", 0);

    if (counterOfSentPacketsFromNodesPerSF[2] > 0)
        recordScalar("DER SF9", double(counterOfReceivedPacketsPerSF[2]) / counterOfSentPacketsFromNodesPerSF[2]);
    else
        recordScalar("DER SF9", 0);

    if (counterOfSentPacketsFromNodesPerSF[3] > 0)
        recordScalar("DER SF10", double(counterOfReceivedPacketsPerSF[3]) / counterOfSentPacketsFromNodesPerSF[3]);
    else
        recordScalar("DER SF10", 0);

    if (counterOfSentPacketsFromNodesPerSF[4] > 0)
        recordScalar("DER SF11", double(counterOfReceivedPacketsPerSF[4]) / counterOfSentPacketsFromNodesPerSF[4]);
    else
        recordScalar("DER SF11", 0);

    if (counterOfSentPacketsFromNodesPerSF[5] > 0)
        recordScalar("DER SF12", double(counterOfReceivedPacketsPerSF[5]) / counterOfSentPacketsFromNodesPerSF[5]);
    else
        recordScalar("DER SF12", 0);
}

bool NetworkServerApp::isPacketProcessed(LoRaMacFrame* pkt)
{
    for(uint i=0;i<knownNodes.size();i++)
    {
        if(knownNodes[i].srcAddr == pkt->getTransmitterAddress())
        {
            if(knownNodes[i].lastSeqNoProcessed > pkt->getSequenceNumber()) return true;
        }
    }
    return false;
}

void NetworkServerApp::updateKnownNodes(LoRaMacFrame* pkt)
{
    bool nodeExist = false;
    for(uint i=0;i<knownNodes.size();i++)
    {
        if(knownNodes[i].srcAddr == pkt->getTransmitterAddress())
        {
            nodeExist = true;
            if(knownNodes[i].lastSeqNoProcessed < pkt->getSequenceNumber())
            {
                knownNodes[i].lastSeqNoProcessed = pkt->getSequenceNumber();
            }
            break;
        }
    }
    if(nodeExist == false)
    {
        knownNode newNode;
        newNode.srcAddr= pkt->getTransmitterAddress();
        newNode.lastSeqNoProcessed = pkt->getSequenceNumber();
        newNode.framesFromLastADRCommand = 0;
        newNode.numberOfSentADRPackets = 0;
        newNode.historyAllSNIR = new cOutVector;
        newNode.historyAllSNIR->setName("Vector of SNIR per node");
        //newNode.historyAllSNIR->record(pkt->getSNIR());
        newNode.historyAllSNIR->record(math::fraction2dB(pkt->getSNIR()));
        newNode.historyAllRSSI = new cOutVector;
        newNode.historyAllRSSI->setName("Vector of RSSI per node");
        newNode.historyAllRSSI->record(pkt->getRSSI());
        newNode.receivedSeqNumber = new cOutVector;
        newNode.receivedSeqNumber->setName("Received Sequence number");
        newNode.calculatedSNRmargin = new cOutVector;
        newNode.calculatedSNRmargin->setName("Calculated SNRmargin in ADR");
        knownNodes.push_back(newNode);
    }
}

void NetworkServerApp::addPktToProcessingTable(LoRaMacFrame* pkt)
{
    bool packetExists = false;
    UDPDataIndication *cInfo = check_and_cast<UDPDataIndication*>(pkt->getControlInfo());
    for(uint i=0;i<receivedPackets.size();i++)
    {
        if(receivedPackets[i].rcvdPacket->getTransmitterAddress() == pkt->getTransmitterAddress() && receivedPackets[i].rcvdPacket->getSequenceNumber() == pkt->getSequenceNumber())
        {
            packetExists = true;
            receivedPackets[i].possibleGateways.emplace_back(cInfo->getSrcAddr(), math::fraction2dB(pkt->getSNIR()), pkt->getRSSI());
            delete pkt;
        }
    }
    if(packetExists == false)
    {
        receivedPacket rcvPkt;
        rcvPkt.rcvdPacket = pkt;
        rcvPkt.endOfWaiting = new cMessage("endOfWaitingWindow");
        rcvPkt.endOfWaiting->setContextPointer(pkt);
        rcvPkt.possibleGateways.emplace_back(cInfo->getSrcAddr(), math::fraction2dB(pkt->getSNIR()), pkt->getRSSI());
        scheduleAt(simTime() + 1.2, rcvPkt.endOfWaiting);
        receivedPackets.push_back(rcvPkt);
    }
}

void NetworkServerApp::processScheduledPacket(cMessage* selfMsg)
{
    LoRaMacFrame *frame = static_cast<LoRaMacFrame *>(selfMsg->getContextPointer());
    L3Address pickedGateway;
    double SNIRinGW = -99999999999;
    double RSSIinGW = -99999999999;
    int packetNumber;
    for(uint i=0;i<receivedPackets.size();i++)
    {
        if(receivedPackets[i].rcvdPacket->getTransmitterAddress() == frame->getTransmitterAddress() && receivedPackets[i].rcvdPacket->getSequenceNumber() == frame->getSequenceNumber())
        {
            packetNumber = i;
            for(uint j=0;j<receivedPackets[i].possibleGateways.size();j++)
            {
                if(SNIRinGW < std::get<1>(receivedPackets[i].possibleGateways[j]))
                {
                    RSSIinGW = std::get<2>(receivedPackets[i].possibleGateways[j]);
                    SNIRinGW = std::get<1>(receivedPackets[i].possibleGateways[j]);
                    pickedGateway = std::get<0>(receivedPackets[i].possibleGateways[j]);
                }
            }
        }
    }
    emit(LoRa_ServerPacketReceived, true);
    if (simTime() >= getSimulation()->getWarmupPeriod())
    {
        counterOfReceivedPackets++;
    }
    receivedRSSI.collect(frame->getRSSI());
    evaluateADR(frame, pickedGateway, SNIRinGW, RSSIinGW);
    delete receivedPackets[packetNumber].rcvdPacket;
    delete selfMsg;
    receivedPackets.erase(receivedPackets.begin()+packetNumber);
}

void NetworkServerApp::evaluateADR(LoRaMacFrame* pkt, L3Address pickedGateway, double SNIRinGW, double RSSIinGW)
{
    bool sendADR = false;
    bool sendADRAckRep = false;
    double SNRm; //needed for ADR
    int numberOfPickedNodes;

    LoRaAppPacket *rcvAppPacket = check_and_cast<LoRaAppPacket*>(pkt->decapsulate());
    if(rcvAppPacket->getOptions().getADRACKReq())
    {
        sendADRAckRep = true;
    }

    for(uint i=0;i<knownNodes.size();i++)
    {
        if(knownNodes[i].srcAddr == pkt->getTransmitterAddress())
        {
            knownNodes[i].receivedSNIR.push_back(SNIRinGW);
            //knownNodes[i].historyAllSNIR->record(SNIRinGW);
            knownNodes[i].historyAllSNIR->record(SNIRinGW);
            knownNodes[i].historyAllRSSI->record(RSSIinGW);
            knownNodes[i].receivedSeqNumber->record(pkt->getSequenceNumber());
            if(knownNodes[i].receivedSNIR.size() > 20) knownNodes[i].receivedSNIR.pop_front();
            if(evaluateADRinServer)
            {
                knownNodes[i].framesFromLastADRCommand++;
                if(knownNodes[i].framesFromLastADRCommand > 20)
                {
                    numberOfPickedNodes = i;
                    knownNodes[i].framesFromLastADRCommand = 0;
                    sendADR = true;
                    if(adrMethod == "max")
                    {
                        SNRm = *max_element(knownNodes[i].receivedSNIR.begin(), knownNodes[i].receivedSNIR.end());
                    }
                    if(adrMethod == "avg")
                    {
                        double totalSNR = *knownNodes[i].receivedSNIR.begin();
                        int numberOfFields = 1;
                        for (std::list<double>::iterator it=knownNodes[i].receivedSNIR.begin()++; it != knownNodes[i].receivedSNIR.end(); ++it)
                        {
                            totalSNR+=*it;
                            numberOfFields++;
                        }
                        SNRm = totalSNR/numberOfFields;
                    }
                }
            }
            if(sendADR || sendADRAckRep)
            {
                knownNodes[i].numberOfSentADRPackets++;
            }
        }
    }

    if(sendADR || sendADRAckRep)
    {
        LoRaAppPacket *mgmtPacket = new LoRaAppPacket("ADRcommand");
        mgmtPacket->setMsgType(TXCONFIG);

        if(evaluateADRinServer && sendADR)
        {
            double SNRmargin;
            double requiredSNR;
            double margin_db = 15;
            if(pkt->getLoRaSF() == 7) requiredSNR = -7.5;
            if(pkt->getLoRaSF() == 8) requiredSNR = -10;
            if(pkt->getLoRaSF() == 9) requiredSNR = -12.5;
            if(pkt->getLoRaSF() == 10) requiredSNR = -15;
            if(pkt->getLoRaSF() == 11) requiredSNR = -17.5;
            if(pkt->getLoRaSF() == 12) requiredSNR = -20;

            SNRmargin = SNRm - requiredSNR - margin_db;
            knownNodes[numberOfPickedNodes].calculatedSNRmargin->record(SNRmargin);
            int Nstep = round(SNRmargin/3);
            LoRaOptions newOptions;

            // Increase the data rate with each step
            int calculatedSF = pkt->getLoRaSF();
            while(Nstep > 0 && calculatedSF > 7)
            {
                calculatedSF--;
                Nstep--;
            }

            // Decrease the Tx power by 3 for each step, until min reached
            double calculatedPowerdBm = pkt->getLoRaTP();
            while(Nstep > 0 && calculatedPowerdBm > 2)
            {
                calculatedPowerdBm-=3;
                Nstep--;
            }
            if(calculatedPowerdBm < 2) calculatedPowerdBm = 2;

            // Increase the Tx power by 3 for each step, until max reached
            while(Nstep < 0 && calculatedPowerdBm < 14)
            {
                calculatedPowerdBm+=3;
                Nstep++;
            }
            if(calculatedPowerdBm > 14) calculatedPowerdBm = 14;

            newOptions.setLoRaSF(calculatedSF);
            newOptions.setLoRaTP(calculatedPowerdBm);
            mgmtPacket->setOptions(newOptions);
        }

        LoRaMacFrame *frameToSend = new LoRaMacFrame("ADRPacket");
        frameToSend->encapsulate(mgmtPacket);
        frameToSend->setReceiverAddress(pkt->getTransmitterAddress());
        //FIXME: What value to set for LoRa TP
        //frameToSend->setLoRaTP(pkt->getLoRaTP());
        frameToSend->setLoRaTP(14);
        frameToSend->setLoRaCF(pkt->getLoRaCF());
        frameToSend->setLoRaSF(pkt->getLoRaSF());
        frameToSend->setLoRaBW(pkt->getLoRaBW());
        socket.sendTo(frameToSend, pickedGateway, destPort);
    }
    delete rcvAppPacket;
}

void NetworkServerApp::receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject *details)
{
    if (simTime() >= getSimulation()->getWarmupPeriod())
    {
        counterOfSentPacketsFromNodes++;
        counterOfSentPacketsFromNodesPerSF[value-7]++;
    }
}

} //namespace inet
