/*
 KNXAddress.h - Class for KNX group address 
 */

#pragma once

#include "KNXAddress.h"

class GroupAddress: public KNXAddress
{
    using KNXAddress::KNXAddress;

    public:
        bool fromString(const char *address);
        bool fromString(const String &address) { return fromString(address.c_str()); }

        size_t printTo(Print& p) const;
        String toString() const;
};

size_t GroupAddress::printTo(Print& p) const
{
    size_t n = 0;
    for(int i = 0; i < 2; i++) {
        n += p.print(_address.bytes[i], DEC);
        n += p.print('/');
    }
    n += p.print(_address.bytes[2], DEC);
    return n;
}

String GroupAddress::toString() const
{
    char szRet[9];
    sprintf(szRet,"%u/%u/%u", _address.bytes[0], _address.bytes[1], _address.bytes[2]);
    return String(szRet);
}

bool GroupAddress::fromString(const char *address)
{
    uint16_t acc = 0;
    uint16_t sum = 0;
    uint8_t slashes = 0;

    while (*address)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');

            // First member max value is 31, second is 15, third is 255
            if ((slashes == 0 && acc > 31) ||
                (slashes == 1 && acc > 15) ||
                acc > 255) {
                return false;
            }
        }
        else if (c == '/')
        {
            if (slashes == 2) {
                // Too many dots (there must be 2 dots)
                return false;
            }
            _address.bytes[slashes++] = acc;
            sum+=acc;
            acc = 0;
        }
        else
        {
            // Invalid char
            return false;
        }
    }

    if (slashes != 2) {
        // Too few slashes (there must be 2 slashes)
        return false;
    }

    // Sum of members should not be 0
    if (sum == 0) {
        return false;
    }

    _address.bytes[2] = acc;
    return true;
}

const GroupAddress GROUPADDR_NONE(0, 0, 0);