/*
 KNXAddress.h - Base class that provides KNXAddress
 Mostly based on IPAddress.h
 */

#pragma once

#include <Arduino.h>
#include <Printable.h>
#include <stdint.h>
#include <WString.h>

// A class to make it easier to handle and pass around KNX Addresses

class KNXAddress: public Printable
{
    protected:
        union {
            uint8_t bytes[3];
            uint16_t word;
        } _address;

        // Access the raw byte array containing the address.  Because this returns a pointer
        // to the internal structure rather than a copy of the address this function should only
        // be used when you know that the usage of the returned uint8_t* will be transient and not
        // stored.
        uint8_t* raw_address()
        {
            return _address.bytes;
        }

    public:
        // Constructors
        KNXAddress();
        KNXAddress(uint8_t first_member, uint8_t second_member, uint8_t third_member);
        KNXAddress(uint16_t address);
        KNXAddress(const uint8_t *address);
        virtual ~KNXAddress() {}

        // Overloaded cast operator to allow KNXAddress objects to be used where a pointer
        // to a two-byte uint8_t array is expected
        operator uint16_t() const
        {
            return _address.word;
        }
        bool operator==(const KNXAddress& addr) const
        {
            return _address.word == addr._address.word;
        }
        bool operator==(const uint8_t* addr) const;

        // Overloaded index operator to allow getting and setting individual members of the address
        uint8_t operator[](int index) const
        {
            return _address.bytes[index];
        }
        uint8_t& operator[](int index)
        {
            return _address.bytes[index];
        }

        // Overloaded copy operators to allow initialisation of KNXAddress objects from other types
        KNXAddress& operator=(const uint8_t *address);
        KNXAddress& operator=(uint16_t address);

        virtual size_t printTo(Print& p) const = 0;
        virtual String toString() const = 0;
};

KNXAddress::KNXAddress()
{
    _address.word = 0;
}

KNXAddress::KNXAddress(uint8_t first_member, uint8_t second_member, uint8_t third_member)
{
    _address.bytes[0] = first_member;
    _address.bytes[1] = second_member;
    _address.bytes[2] = third_member;
}

KNXAddress::KNXAddress(uint16_t address)
{
    _address.word = address;
}

KNXAddress::KNXAddress(const uint8_t *address)
{
    memcpy(_address.bytes, address, sizeof(_address.bytes));
}

KNXAddress& KNXAddress::operator=(const uint8_t *address)
{
    memcpy(_address.bytes, address, sizeof(_address.bytes));
    return *this;
}

KNXAddress& KNXAddress::operator=(uint16_t address)
{
    _address.word = address;
    return *this;
}

bool KNXAddress::operator==(const uint8_t* addr) const
{
    return memcmp(addr, _address.bytes, sizeof(_address.bytes)) == 0;
}

