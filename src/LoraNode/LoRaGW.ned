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

package flora.LoraNode;

import inet.node.contract.INetworkNode;
import inet.mobility.static.StationaryMobility;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.contract.INetworkLayer;
import inet.linklayer.loopback.LoopbackInterface;
import flora.LoRa.LoRaGWNic;
import flora.LoRaApp.SimpleLoRaApp;
import flora.LoRa.PacketForwarder;
import inet.linklayer.contract.IEthernetInterface;
import inet.applications.contract.IApp;
import inet.transportlayer.contract.IUdp;
import inet.networklayer.contract.IRoutingTable;
import inet.common.MessageDispatcher;
import inet.linklayer.ethernet.contract.IEthernetLayer;
import inet.protocolelement.contract.IProtocolLayer;

module LoRaGW like INetworkNode
{
    parameters:
        @networkNode();
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        *.routingTableModule = default(routingTableType != "" ? absPath(".routingTable") : "");
        *.energySourceModule = default(exists(energyStorage) ? absPath(".energyStorage") : "");

        int numEthInterfaces = default(0);  // minimum number of ethernet interfaces
        int numWlanInterfaces = 1;

        bool hasIpv4 = default(true);
        string networkLayerType = default("Ipv4NetworkLayer");
        string routingTableType = default("Ipv4RoutingTable");
        int numUdpApps = default(0);
        bool hasUdp = default(firstAvailableOrEmpty("Udp") != "");
        string udpType = default(firstAvailableOrEmpty("UDP"));

        LoRaGWNic.radio.antenna.mobilityModule = default("^.^.^.mobility");

        *.mobilityModule = default(exists(mobility) ? absPath(".mobility") : "");
        
        ethernet.registerProtocol = default(true);

        @display("bgb=783.96,587.688;i=device/antennatower");
    gates:
        inout ethg[numEthInterfaces] @labels(EtherFrame-conn) @allowUnconnected;
        input radioIn[numWlanInterfaces] @directIn;

    submodules:
        at: MessageDispatcher {
            parameters:
                @display("p=394.80002,142.128;b=100,5,,,,1");
        }
        tn: MessageDispatcher {
            parameters:
                @display("p=394.80002,276.36002;b=100,5,,,,1");
        }
        nl: MessageDispatcher {
            parameters:
                @display("p=394.80002,391.41602;b=100,5,,,,1");
        }
        cb: MessageDispatcher {
            @display("p=393.60602,394.788;b=500,5");
        }
        bl: MessageDispatcher {
            @display("p=399.516,464.526;b=500,5");
        }
        li: MessageDispatcher {
            parameters:
                @display("p=394.80002,442.17603;b=100,5,,,,1");
        }
        interfaceTable: InterfaceTable {
            @display("p=135.36,69.936005");
        }
        mobility: StationaryMobility {
            @display("p=135.36,199.656");
        }
        LoRaGWNic: LoRaGWNic {
            @display("p=626.04004,94.75201");
        }
        packetForwarder: PacketForwarder {
            @display("p=394.80002,69.936005;is=vl");
        }
        eth[sizeof(ethg)]: <default("EthernetInterface")> like IEthernetInterface {
            parameters:
                @display("p=394.80002,503.088,row,150;q=txQueue");
        }
        udp: <default(firstAvailableOrEmpty("Udp"))> like IUdp if hasUdp {
            parameters:
                @display("p=394.80002,206.42401");
        }
        ipv4: <default("Ipv4NetworkLayer")> like INetworkLayer if hasIpv4 {
            parameters:
                @display("p=394.80002,333.888;q=queue");
        }
        routingTable: <routingTableType> like IRoutingTable if routingTableType != "" {
            parameters:
                @display("p=135.36,313.584;is=s");
        }
        bridging: <default("")> like IProtocolLayer if typename != "" {
            @display("p=248.22,419.61002");
        }

        ethernet: <default(sizeof(ethg) > 0 ? "EthernetEncapsulation" : "")> like IEthernetLayer if typename != "" {
            @display("p=273.042,502.35");
        }
    connections allowunconnected:
        at.out++ --> udp.appIn;
        at.in++ <-- udp.appOut;

        packetForwarder.lowerLayerOut --> LoRaGWNic.upperLayerIn;
        LoRaGWNic.upperLayerOut --> packetForwarder.lowerLayerIn;
        packetForwarder.socketOut --> at.in++;
        packetForwarder.socketIn <-- at.out++;

        for i=0..sizeof(ethg)-1 {
            ethg[i] <--> { @display("m=s"); } <--> eth[i].phys;
            eth[i].upperLayerOut --> li.in++;
            eth[i].upperLayerIn <-- li.out++;
        }

        ipv4.ifIn <-- nl.out++;
        ipv4.ifOut --> nl.in++;

        tn.out++ --> ipv4.transportIn;
        tn.in++ <-- ipv4.transportOut;
        cb.out++ --> nl.in++;
        cb.in++ <-- nl.out++;
        udp.ipOut --> tn.in++;
        udp.ipIn <-- tn.out++;
        
        cb.out++ --> bridging.upperLayerIn if exists(bridging);
        bridging.upperLayerOut --> cb.in++ if exists(bridging);

        cb.out++ --> bl.in++ if !exists(bridging);
        bl.out++ --> cb.in++ if !exists(bridging);

        bridging.lowerLayerOut --> bl.in++ if exists(bridging);
        bl.out++ --> bridging.lowerLayerIn if exists(bridging);

        bl.out++ --> ethernet.upperLayerIn if exists(ethernet);
        ethernet.upperLayerOut --> bl.in++ if exists(ethernet);

        ethernet.lowerLayerOut --> li.in++ if exists(ethernet);
        li.out++ --> ethernet.lowerLayerIn if exists(ethernet);
}
