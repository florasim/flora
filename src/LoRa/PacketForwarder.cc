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

#include "PacketForwarder.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/applications/base/ApplicationPacket_m.h"

namespace inet {

Define_Module(PacketForwarder);


void PacketForwarder::initialize(int stage)
{

    if (stage == 0) {
        LoRa_GWPacketReceived = registerSignal("LoRa_GWPacketReceived");
        localPort = par("localPort");
        destPort = par("destPort");
    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        startUDP();
        getSimulation()->getSystemModule()->subscribe("LoRa_AppPacketSent", this);
    }

}


void PacketForwarder::startUDP()
{
    socket.setOutputGate(gate("udpOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    // TODO: is this required?
    //setSocketOptions();

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    // Create UDP sockets to multiple destination addresses (network servers)
    while ((token = tokenizer.nextToken()) != nullptr) {
        L3Address result;
        L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << token << endl;
        else
            EV << "Got destination address: " << token << endl;
        destAddresses.push_back(result);
    }
}


void PacketForwarder::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("lowerLayerIn")) {
        EV << "Received LoRaMAC frame" << endl;
        LoRaMacFrame *frame = check_and_cast<LoRaMacFrame *>(PK(msg));
        if(frame->getReceiverAddress() == DevAddr::BROADCAST_ADDRESS)
            processLoraMACPacket(PK(msg));
        //send(msg, "upperLayerOut");
        //sendPacket();
    } else if (msg->arrivedOn("udpIn")) {
        // FIXME : debug for now to see if LoRaMAC frame received correctly from network server
        EV << "Received UDP packet" << endl;
        LoRaMacFrame *frame = check_and_cast<LoRaMacFrame *>(PK(msg));
        //EV << frame->getLoRaTP() << endl;
        //delete frame;
        send(frame, "lowerLayerOut");
        //
    }
}

void PacketForwarder::processLoraMACPacket(cPacket *pk)
{
    // FIXME: Change based on new implementation of MAC frame.
    emit(LoRa_GWPacketReceived, 42);
    if (simTime() >= getSimulation()->getWarmupPeriod())
        counterOfReceivedPackets++;
    LoRaMacFrame *frame = check_and_cast<LoRaMacFrame *>(pk);

    physicallayer::ReceptionIndication *cInfo = check_and_cast<physicallayer::ReceptionIndication *>(pk->getControlInfo());
    W w_rssi = cInfo->getMinRSSI();
    double rssi = w_rssi.get()*1000;
    frame->setRSSI(math::mW2dBm(rssi));
    frame->setSNIR(cInfo->getMinSNIR());
    bool exist = false;
    EV << frame->getTransmitterAddress() << endl;
    //for (std::vector<nodeEntry>::iterator it = knownNodes.begin() ; it != knownNodes.end(); ++it)

    // FIXME : Identify network server message is destined for.
    L3Address destAddr = destAddresses[0];
    if (frame->getControlInfo())
       delete frame->removeControlInfo();

    socket.sendTo(frame, destAddr, destPort);

}

void PacketForwarder::sendPacket()
{

    /*LoRaAppPacket *mgmtCommand = new LoRaAppPacket("mgmtCommand");
    mgmtCommand->setMsgType(TXCONFIG);
    LoRaOptions newOptions;
    newOptions.setLoRaTP(uniform(0.1, 1));
    mgmtCommand->setOptions(newOptions);

    LoRaMacFrame *response = new LoRaMacFrame("mgmtCommand");
    response->encapsulate(mgmtCommand);
    response->setLoRaTP(pk->getLoRaTP());
    response->setLoRaCF(pk->getLoRaCF());
    response->setLoRaSF(pk->getLoRaSF());
    response->setLoRaBW(pk->getLoRaBW());
    response->setReceiverAddress(pk->getTransmitterAddress());
    send(response, "lowerLayerOut");*/

}

void PacketForwarder::receiveSignal(cComponent *source, simsignal_t signalID, long value, cObject *details)
{
    if (simTime() >= getSimulation()->getWarmupPeriod())
        counterOfSentPacketsFromNodes++;
}

void PacketForwarder::finish()
{
    recordScalar("LoRa_GW_DER", double(counterOfReceivedPackets)/counterOfSentPacketsFromNodes);
}



} //namespace inet
