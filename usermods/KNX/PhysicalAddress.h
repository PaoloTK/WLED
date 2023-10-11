/*
 KNXAddress.h - Class for KNX physical address 
 */

#pragma once

#include "KNXAddress.h"

class PhysicalAddress: public KNXAddress
{
    using KNXAddress::KNXAddress;

    public:
        bool fromString(const char *address);
        bool fromString(const String &address) { return fromString(address.c_str()); }

        size_t printTo(Print& p) const;
        String toString() const;
};

size_t PhysicalAddress::printTo(Print& p) const
{
    size_t n = 0;
    for(int i = 0; i < 2; i++) {
        n += p.print(_address.bytes[i], DEC);
        n += p.print('.');
    }
    n += p.print(_address.bytes[2], DEC);
    return n;
}

String PhysicalAddress::toString() const
{
    char szRet[9];
    sprintf(szRet,"%u.%u.%u", _address.bytes[0], _address.bytes[1], _address.bytes[2]);
    return String(szRet);
}

bool PhysicalAddress::fromString(const char *address)
{
    uint16_t acc = 0;
    uint8_t dots = 0;

    while (*address)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');

            // First two members max value is 15, third is 255
            if (acc > 255 || (dots < 2 && acc > 15)) {
                return false;
            }
        }
        else if (c == '.')
        {
            if (dots == 2) {
                // Too many dots (there must be 2 dots)
                return false;
            }
            _address.bytes[dots++] = acc;
            acc = 0;
        }
        else
        {
            // Invalid char
            return false;
        }
    }

    if (dots != 2) {
        // Too few dots (there must be 2 dots)
        return false;
    }

    // Third member should not be 0
    if (acc == 0) {
        return false;
    }

    _address.bytes[2] = acc;
    return true;
}

const PhysicalAddress PHYSADDR_NONE(0, 0, 0);