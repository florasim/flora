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

import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;
import inet.common.Units;
import inet.linklayer.common.MacAddress;

cplusplus {{
using namespace inet;
}}

namespace flora;

class LoRaPhyPreamble extends inet::FieldsChunk {
    inet::Hz centerFrequency;
    inet::Hz bandwidth;
    int spreadFactor;
    inet::W power;
    bool UseHeader;
    int codeRendundance;
    inet::MacAddress receiverAddress;
}