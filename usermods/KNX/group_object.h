#pragma once
#include "group_address.h"

enum class ObjectFunction {
  SWITCH,
  ABSOLUTE_DIM,
  RELATIVE_DIM,
  PALETTE,
  PLAYLIST
};

// https://support.knx.org/hc/en-us/articles/4401873529490-Devices-Icons
enum class ObjectType {
  LISTEN,
  STATE
};

class GroupObject {
  private:
    ObjectFunction _function;
    ObjectType _type;
    GroupAddress _address;

    static const char _unknown[], _listen[], _state[], 
                      _switch[], _absolute_dim[], _relative_dim[],
                      _palette[], _playlist[];
  public:
    GroupObject(ObjectFunction function, ObjectType type) :
    _function(function),
    _type(type) {}
    GroupObject(ObjectFunction function, ObjectType type, GroupAddress address) :
    _function(function),
    _type(type),
    _address(address) {}
    ObjectFunction getFunction() { return _function; }
    ObjectType getType() { return _type; }
    GroupAddress getAddress() { return _address; }
    bool setAddress(const char* address) { return _address.fromString(address); }
    char* printInfo();

};

char* GroupObject::printInfo() {
    const char* typeStr = nullptr;
    const char* functionStr = nullptr;

    switch (_function) {
        case ObjectFunction::SWITCH:
            functionStr = _switch;
            break;
        case ObjectFunction::ABSOLUTE_DIM:
            functionStr = _absolute_dim;
            break;
        case ObjectFunction::RELATIVE_DIM:
            functionStr = _relative_dim;
            break;
        case ObjectFunction::PALETTE:
            functionStr = _palette;
            break;
        case ObjectFunction::PLAYLIST:
            functionStr = _playlist;
            break;
        default:
            functionStr = _unknown;
            break;
    }

    switch (_type) {
        case ObjectType::LISTEN:
            typeStr = _listen;
            break;
        case ObjectType::STATE:
            typeStr = _state;
            break;
        default:
            typeStr = _unknown;
            break;
    }

    // "absolute dim" + "listen" + space + terminator
    static char info[20];
    snprintf(info, sizeof(info), "%s %s", functionStr, typeStr);
    
    return info;
}

// add more strings here to reduce flash memory usage
const char GroupObject::_unknown[] PROGMEM       = "unknown";
const char GroupObject::_listen[] PROGMEM        = "listen";
const char GroupObject::_state[] PROGMEM         = "state";
const char GroupObject::_switch[] PROGMEM        = "switch";
const char GroupObject::_absolute_dim[] PROGMEM  = "absolute dim";
const char GroupObject::_relative_dim[] PROGMEM  = "relative dim";
const char GroupObject::_palette[] PROGMEM       = "palette";
const char GroupObject::_playlist[] PROGMEM      = "playlist";

