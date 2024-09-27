#pragma once
#include "wled.h"

enum GroupStyle {
  FREE = 0,
  TWO_LEVEL = 1,
  THREE_LEVEL = 2
};

class GroupAddress {
    private:
        GroupStyle _style = GroupStyle::THREE_LEVEL;
        uint16_t _address = 0;
    public:
        GroupAddress() {}
        GroupAddress(uint16_t address) { _address = address; }
        GroupAddress(uint16_t address, GroupStyle style) { _address = address; _style = style;}
        void setStyle(GroupStyle style) { _style = style; }
        bool fromString(const char* address);
        char* toString();

        operator bool () const { return _address != 0; }
};

bool GroupAddress::fromString(const char* address) {
  uint16_t addr = 0, delim = 0;
  uint32_t acc = 0;
  bool valid = true;

  while (*address && valid) {
    char c = *address++;
    if (c >= '0' && c <= '9') {
        acc = acc * 10 + (c - '0');
        switch(_style) {
          case GroupStyle::FREE:
            valid = delim == 0 && acc <= 0xFFFF;
            break;
          case GroupStyle::TWO_LEVEL:
            switch(delim) {
              case 0:
                valid = acc <= 0xF;
                break;
              case 1:
                valid = acc <= 0xFFF;
                break;
              default:
                valid = false;
            }
            break;
          case GroupStyle::THREE_LEVEL:
            switch(delim) {
              case 0:
                valid = acc <= 0xF;
              case 1:
                valid = acc <= 0xF;
              case 2:
                valid = acc <= 0xFFF;
              default:
                valid = false;
            }
            break;
          default:
            valid = false;
        }
    }
    else if (c == '/') {
        switch (_style) {
          case GroupStyle::FREE:
            valid = false;
            break;
          case GroupStyle::TWO_LEVEL:
            switch(delim) {
              case 0:
                addr |= (acc << 12);
                break;
              default:
                valid = false;
            }
            break;
          case GroupStyle::THREE_LEVEL:
            switch(delim) {
              case 0:
                addr |= (acc << 12);
              case 1:
                addr |= (acc << 8);
              default:
                valid = false;
            }
            break;
          default:
            valid = false;
        }
        acc = 0;
        delim++;
    } else {
        // Invalid character
        valid = false;
        break;
    }
  }

  switch (_style) {
    case GroupStyle::FREE:
      break;
    case GroupStyle::TWO_LEVEL:
      valid = delim == 1;
      break;
    case GroupStyle::THREE_LEVEL:
      valid = delim == 2;
      break;
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

char *GroupAddress::toString() {
  // XX/XX/XXX + 1
  char* addr = new char[10];

  switch(_style) {
    case GroupStyle::FREE:
      sprintf(addr, "%d", _address);
      break;
    case GroupStyle::TWO_LEVEL:
      sprintf(addr, "%d.%d", (_address >> 12) & 0x0F, _address & 0xFFF);  
      break;
    case GroupStyle::THREE_LEVEL:
      sprintf(addr, "%d.%d.%d", (_address >> 12) & 0x0F, (_address >> 8) & 0x0F, _address & 0xFF);
      break;
    default:
      sprintf(addr, "%d", 0);
  }

  return addr;
}