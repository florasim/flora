/*
 * Copyright (C) 2003 Andras Varga; CTIE, Monash University, Australia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <ctype.h>
#include "DevAddr.h"

namespace inet {

unsigned int DevAddr::autoAddressCtr;
bool DevAddr::simulationLifecycleListenerAdded;

const DevAddr DevAddr::UNSPECIFIED_ADDRESS;
const DevAddr DevAddr::BROADCAST_ADDRESS("ff:ff:ff:ff");

unsigned char DevAddr::getAddressByte(unsigned int k) const
{
    if (k >= DEVADDR_ADDRESS_SIZE)
        throw cRuntimeError("Array of size 4 indexed with %d", k);
    int offset = (DEVADDR_ADDRESS_SIZE - k - 1) * 8;
    return 0xff & (address >> offset);
}

void DevAddr::setAddressByte(unsigned int k, unsigned char addrbyte)
{
    if (k >= DEVADDR_ADDRESS_SIZE)
        throw cRuntimeError("Array of size 4 indexed with %d", k);
    int offset = (DEVADDR_ADDRESS_SIZE - k - 1) * 8;
    address = (address & (~(((uint32)0xff) << offset))) | (((uint32)addrbyte) << offset);
}

bool DevAddr::tryParse(const char *hexstr)
{
    if (!hexstr)
        return false;

    // check syntax
    int numHexDigits = 0;
    for (const char *s = hexstr; *s; s++) {
        if (isxdigit(*s))
            numHexDigits++;
        else if (*s != ' ' && *s != ':' && *s != '-')
            return false; // wrong syntax
    }
    if (numHexDigits != 2 * DEVADDR_ADDRESS_SIZE)
        return false;

    // Converts hex string into the address
    // if hext string is shorter, address is filled with zeros;
    // Non-hex characters are discarded before conversion.
    address = 0;    // clear top 16 bits too that setAddressByte() calls skip
    int k = 0;
    const char *s = hexstr;
    for (int pos = 0; pos < DEVADDR_ADDRESS_SIZE; pos++) {
        if (!s || !*s) {
            setAddressByte(pos, 0);
        }
        else {
            while (*s && !isxdigit(*s))
                s++;
            if (!*s) {
                setAddressByte(pos, 0);
                continue;
            }
            unsigned char d = isdigit(*s) ? (*s - '0') : islower(*s) ? (*s - 'a' + 10) : (*s - 'A' + 10);
            d = d << 4;
            s++;

            while (*s && !isxdigit(*s))
                s++;
            if (!*s) {
                setAddressByte(pos, 0);
                continue;
            }
            d += isdigit(*s) ? (*s - '0') : islower(*s) ? (*s - 'a' + 10) : (*s - 'A' + 10);
            s++;

            setAddressByte(pos, d);
            k++;
        }
    }
    return true;
}

void DevAddr::setAddress(const char *hexstr)
{
    if (!tryParse(hexstr))
        throw cRuntimeError("DevAddr: wrong address syntax '%s': 8 hex digits expected, with optional embedded spaces, hyphens or colons", hexstr);
}

void DevAddr::getAddressBytes(unsigned char *addrbytes) const
{
    for (int i = 0; i < DEVADDR_ADDRESS_SIZE; i++)
        addrbytes[i] = getAddressByte(i);
}

void DevAddr::setAddressBytes(unsigned char *addrbytes)
{
    address = 0;    // clear top 16 bits too that setAddressByte() calls skip
    for (int i = 0; i < DEVADDR_ADDRESS_SIZE; i++)
        setAddressByte(i, addrbytes[i]);
}

std::string DevAddr::str() const
{
    char buf[20];
    char *s = buf;
    for (int i = 0; i < DEVADDR_ADDRESS_SIZE; i++, s += 3)
        sprintf(s, "%2.2X-", getAddressByte(i));
    *(s - 1) = '\0';
    return std::string(buf);
}

int DevAddr::compareTo(const DevAddr& other) const
{
    return (address < other.address) ? -1 : (address == other.address) ? 0 : 1;    // note: "return address-other.address" is not OK because 64-bit result does not fit into the return type
}

DevAddr DevAddr::generateAutoAddress()
{
#if OMNETPP_VERSION >= 0x500
    if (!simulationLifecycleListenerAdded) {
        // NOTE: EXECUTE_ON_STARTUP is too early and would add the listener to StaticEnv
        DevAddr::SimulationLifecycleListener();
        getEnvir()->addLifecycleListener(new DevAddr::SimulationLifecycleListener());
        simulationLifecycleListenerAdded = true;
    }
#endif // if OMNETPP_VERSION >= 0x500
    ++autoAddressCtr;

    uint32 intAddr = 0x00000000ULL + (autoAddressCtr & 0xffffffffUL);
    DevAddr addr(intAddr);
    return addr;
}

} // namespace inet

