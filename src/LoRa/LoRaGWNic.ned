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

package loranetwork.LoRa;

import inet.linklayer.contract.IWirelessNic;
import inet.physicallayer.contract.packetlevel.IRadio;
import loranetwork.LoRaPhy.LoRaMedium;

module LoRaGWNic like IWirelessNic 
{
    parameters:
        string interfaceTableModule;
        @display("i=block/ifcard");
        *.interfaceTableModule = default(absPath(interfaceTableModule));
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input upperMgmtIn;
        output upperMgmtOut;
        input radioIn @labels(IRadioFrame);
    submodules:
        mac: LoRaGWMac {
            parameters:
                @display("p=100,150");
            gates:
                upperMgmtIn;
                upperMgmtOut;    
        }
        radio: LoRaGWRadio {
            parameters:
                @display("p=100,250");
        }
    connections:
        upperLayerIn --> mac.upperLayerIn;
        mac.lowerLayerOut --> radio.upperLayerIn;
        mac.upperLayerOut --> { @display("m=n"); } --> upperLayerOut;
        radioIn --> { @display("m=s"); } --> radio.radioIn;
        radio.upperLayerOut --> mac.lowerLayerIn;
        
        upperMgmtIn --> mac.upperMgmtIn;
        mac.upperMgmtOut --> upperMgmtOut;
}
