#pragma once
#include "wled.h"

class IndividualAddress {
    private:
        uint16_t _address = 0;
    public:
        IndividualAddress() {}
        IndividualAddress(uint16_t address) { _address = address; }
        bool fromString(const char* address);
        char* toString();

        operator bool () const { return _address != 0; }
};

bool IndividualAddress::fromString(const char *address)
{
    uint16_t addr = 0, delim = 0, acc = 0;
    bool valid = true;

    while (*address && valid)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');
            // Highest address 15.15.255, last part can't be 0
            valid = (delim < 2) ? acc <= 15 : 0 < acc && acc <= 255;
        }
        else if (c == '.')
        {
            switch (delim)
            {
            case 0:
                addr |= (acc << 12);
                break;
            case 1:
                addr |= (acc << 8);
                break;
            default:
                // Too many dots
                valid = false;
                break;
            }
            acc = 0;
            delim++;
        } else {
          // Invalid character
          valid = false;
          break;
      }
  }

  if (delim != 2) {
      // Not enough dots
      valid = false;
  }

  addr |= acc;

  if (valid) {
      _address = addr;
      return true;
  }
  else {
      return false;
      // @FIX Add user facing error
  }
}

char *IndividualAddress::toString() {
  // XX.XX.XXX + 1
  char* addr = new char[10];

  sprintf(addr, "%d.%d.%d", (_address >> 12) & 0x0F, (_address >> 8) & 0x0F, _address & 0xFF);

  return addr;
}