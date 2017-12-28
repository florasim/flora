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

#ifndef __INET_DEVADDR_H
#define __INET_DEVADDR_H

#include <string>

#include "inet/common/INETDefs.h"

namespace inet {

#define DEVADDR_ADDRESS_SIZE    4
#define DEVADDR_ADDRESS_MASK    0xffffffffULL

class InterfaceToken;

class INET_API DevAddr
{
  private:
    uint32 address;    // 4*8=48 bit address
    static unsigned int autoAddressCtr;    // global counter for generateAutoAddress()
    static bool simulationLifecycleListenerAdded;

  public:

#if OMNETPP_VERSION >= 0x500
    class SimulationLifecycleListener : public cISimulationLifecycleListener
    {
        virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) {
            if (eventType == LF_PRE_NETWORK_INITIALIZE)
                autoAddressCtr = 0;
        }

        virtual void listenerRemoved() {
            delete this;
        }
    };
#endif // if OMNETPP_VERSION >= 0x500

    static const DevAddr UNSPECIFIED_ADDRESS;
    static const DevAddr BROADCAST_ADDRESS;

    /**
     * Default constructor initializes address bytes to zero.
     */
    DevAddr() { address = 0; }

    /**
     * Initializes the address from the lower 48 bits of the 64-bit argument
     */
    explicit DevAddr(uint32 bits) { address = bits; }

    /**
     * Constructor which accepts a hex string (8 hex digits, may also
     * contain spaces, hyphens and colons)
     */
    explicit DevAddr(const char *hexstr) { setAddress(hexstr); }

    /**
     * Copy constructor.
     */
    DevAddr(const DevAddr& other) { address = other.address; }

    /**
     * Assignment.
     */
    DevAddr& operator=(const DevAddr& other) { address = other.address; return *this; }

    /**
     * Returns the address size in bytes, that is, 4.
     */
    unsigned int getAddressSize() const { return DEVADDR_ADDRESS_SIZE; }

    /**
     * Returns the kth byte of the address.
     */
    unsigned char getAddressByte(unsigned int k) const;

    /**
     * Sets the kth byte of the address.
     */
    void setAddressByte(unsigned int k, unsigned char addrbyte);

    /**
     * Sets the address and returns true if the syntax of the string
     * is correct. (See setAddress() for the syntax.)
     */
    bool tryParse(const char *hexstr);

    /**
     * Converts address value from hex string (12 hex digits, may also
     * contain spaces, hyphens and colons)
     */
    void setAddress(const char *hexstr);

    /**
     * Copies the address to the given pointer (array of 6 unsigned chars).
     */
    void getAddressBytes(unsigned char *addrbytes) const;
    void getAddressBytes(char *addrbytes) const { getAddressBytes((unsigned char *)addrbytes); }

    /**
     * Sets address bytes. The argument should point to an array of 6 unsigned chars.
     */
    void setAddressBytes(unsigned char *addrbytes);
    void setAddressBytes(char *addrbytes) { setAddressBytes((unsigned char *)addrbytes); }

    /**
     * Sets the address to the broadcast address (hex ff:ff:ff:ff).
     */
    void setBroadcast() { address = DEVADDR_ADDRESS_MASK; }

    /**
     * Returns true if this is the broadcast address (hex ff:ff:ff:ff).
     */
    bool isBroadcast() const { return address == DEVADDR_ADDRESS_MASK; }

    /**
     * Returns true if all address bytes are zero.
     */
    bool isUnspecified() const { return address == 0; }

    /**
     * Converts address to a hex string.
     */
    std::string str() const;

    /**
     * Converts address to 32 bits integer.
     */
    uint64 getInt() const { return address; }

    /**
     * Returns true if the two addresses are equal.
     */
    bool equals(const DevAddr& other) const { return address == other.address; }

    /**
     * Returns true if the two addresses are equal.
     */
    bool operator==(const DevAddr& other) const { return address == other.address; }

    /**
     * Returns true if the two addresses are not equal.
     */
    bool operator!=(const DevAddr& other) const { return address != other.address; }

    /**
     * Returns -1, 0 or 1 as result of comparison of 2 addresses.
     */
    int compareTo(const DevAddr& other) const;

    /**
     * Generates a unique address which begins with 0a:aa and ends in a unique
     * suffix.
     */
    static DevAddr generateAutoAddress();

    bool operator<(const DevAddr& other) const { return address < other.address; }

    bool operator>(const DevAddr& other) const { return address > other.address; }
};

inline std::ostream& operator<<(std::ostream& os, const DevAddr& mac)
{
    return os << mac.str();
}

} // namespace inet

#endif // ifndef __INET_DEVADDR_H

