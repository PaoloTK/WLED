#pragma once
#include "group_address.h"

enum class ObjectFunction {
  SWITCH,
  ABSOLUTE_DIM,
  RELATIVE_DIM
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
  public:
    GroupObject(ObjectFunction function, ObjectType type, GroupAddress address) :
    _function(function),
    _type(type),
    _address(address) {}
    ObjectFunction getFunction() { return _function; }
    ObjectType getType() { return _type; }
    GroupAddress getAddress() { return _address; }

};