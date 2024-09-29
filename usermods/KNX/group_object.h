#pragma once
#include "group_address.h"

enum class ObjectFunction {
  SWITCH,
  ABSOLUTE_DIM,
  RELATIVE_DIM,
  PALETTE,
  PLAYLIST
};

enum class ObjectType {
  LISTEN,
  STATE
};

class GroupObject {
  private:
    ObjectFunction _function;
    ObjectType _type;
    GroupAddress _address;

    static const char _listen[], _state[], 
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
    GroupAddress setAddress(const char* address) { _address.fromString(address); }
    char* printInfo();

};

char* GroupObject::printInfo() {
    // "listen" + 1
    char* typeInfo = new char[7];
    // "absolute dim" + 1
    char* functionInfo = new char[13];
    switch (_type) {
      case ObjectType::LISTEN:
        strcpy_P(typeInfo, (PGM_P)FPSTR(_listen));
        break;
      case ObjectType::STATE:
        strcpy_P(typeInfo, (PGM_P)FPSTR(_state));
        break;
      default:
        strcpy_P(typeInfo, "unknown");

    }
      
}

// add more strings here to reduce flash memory usage
const char GroupObject::_listen[] PROGMEM        = "listen";
const char GroupObject::_state[] PROGMEM         = "state";
const char GroupObject::_switch[] PROGMEM        = "switch";
const char GroupObject::_absolute_dim[] PROGMEM  = "absolute dim";
const char GroupObject::_relative_dim[] PROGMEM  = "relative dim";
const char GroupObject::_palette[] PROGMEM       = "palette";
const char GroupObject::_playlist[] PROGMEM      = "playlist";

