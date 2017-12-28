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

package loranetwork.LoraNode;

import inet.mobility.static.StationaryMobility;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.contract.INetworkLayer;
import inet.linklayer.loopback.LoopbackInterface;
import loranetwork.LoRa.LoRaGWNic;
import loranetwork.LoRaApp.SimpleLoRaApp;
import loranetwork.LoRa.PacketForwarder;
import inet.linklayer.contract.IWiredNic;
import inet.applications.contract.IUDPApp;
import inet.transportlayer.contract.IUDP;
import inet.networklayer.contract.IRoutingTable;

module LoRaGW
{
    parameters:
        @networkNode();
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        *.routingTableModule = default(routingTableType != "" ? absPath(".routingTable") : "");
        string networkLayerType = default("IPv4NetworkLayer");
        string routingTableType = default("IPv4RoutingTable");
        int numUdpApps = default(0);
        bool hasUdp = true;
        string udpType = default(firstAvailableOrEmpty("UDP"));

        @display("bgb=611,560;i=device/antennatower");
    gates:
        inout ethg[] @labels(EtherFrame-conn);
    submodules:
        interfaceTable: InterfaceTable {
            @display("p=30,26");
        }
        mobility: StationaryMobility {
            @display("p=24,88");
        }
        LoRaGWNic: LoRaGWNic {
            @display("p=163.56,367.728");
        }
        packetForwarder: PacketForwarder {
            @display("p=256.056,162.432;is=vl");
        }
        eth[sizeof(ethg)]: <default("EthernetInterface")> like IWiredNic {
            parameters:
                @display("p=340.656,442.17603,row,60;q=txQueue");
        }
        udp: <udpType> like IUDP if hasUdp {
            parameters:
                @display("p=314.712,240.264");
        }
        networkLayer: <networkLayerType> like INetworkLayer {
            @display("p=340.656,337.272");
        }
        routingTable: <routingTableType> like IRoutingTable if routingTableType != "" {
            parameters:
                @display("p=53,250;is=s");
        }
    connections allowunconnected:
        packetForwarder.lowerLayerOut --> LoRaGWNic.upperLayerIn;
        LoRaGWNic.upperLayerOut --> packetForwarder.lowerLayerIn;
        packetForwarder.udpOut --> udp.appIn++;
        packetForwarder.udpIn <-- udp.appOut++;
        for i=0..sizeof(ethg)-1 {
            ethg[i] <--> { @display("m=s"); } <--> eth[i].phys;
            eth[i].upperLayerOut --> networkLayer.ifIn++;
            eth[i].upperLayerIn <-- networkLayer.ifOut++;
        }
        udp.ipOut --> networkLayer.transportIn++;
        udp.ipIn <-- networkLayer.transportOut++;
}
