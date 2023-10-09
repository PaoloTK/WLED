#include "wled.h"

enum class ObjectFunction {
  Switch,
  Absolute_Dim,
  Relative_Dim,
  Preset,
  Playlist,
  Color_Temperature,
  Color_1,
  Color_2,
  Color_3,
  Effect,
  Speed,
  Intensity,
  Palette
};

enum class ObjectType {
  Input, // Listen object (write flag)
  Output // State object (read and transmit flags)
};

class GroupObject {
  private:

  public:
    ObjectFunction function;
    ObjectType type;
    std::array<uint8_t, 3> address;
    bool enabled;

    GroupObject(ObjectFunction function,
                ObjectType type,
                std::array<uint8_t, 3> address = {0, 0, 0},
                bool enabled = false)
    : function(function)
    , type(type)
    , address{address[0], address[1], address[2]}
    , enabled(enabled)
    { }

    uint8_t getAddress() const;
    String getFunctionName() const;
    String getTypeName() const;    
    String getObjectName() const;
};

uint8_t GroupObject::getAddress() const {
  return address;
}

String GroupObject::getFunctionName() const {
  String name;

  switch (function) {
    case ObjectFunction::Switch: name = "Switch"; break;
    case ObjectFunction::Absolute_Dim: name = "Absolute dim"; break;
    case ObjectFunction::Relative_Dim: name = "Relative dim"; break;
    case ObjectFunction::Preset: name = "Preset"; break;
    case ObjectFunction::Playlist: name = "Playlist"; break;
    case ObjectFunction::Color_Temperature: name = "Color temperature"; break;
    case ObjectFunction::Color_1: name = "Color 1"; break;
    case ObjectFunction::Color_2: name = "Color 2"; break;
    case ObjectFunction::Color_3: name = "Color 3"; break;
    case ObjectFunction::Effect: name = "Effect"; break;
    case ObjectFunction::Speed: name = "Speed"; break;
    case ObjectFunction::Intensity: name = "Intensity"; break;
    case ObjectFunction::Palette: name = "Palette"; break;
    default: name = "Function";
  }

  return name;
}

String GroupObject::getTypeName() const {
  String name;

  switch (type) {
    case ObjectType::Input: name = "Input"; break;
    case ObjectType::Output: name = "Output"; break;
    default: name = "Type";
  }
  
  return name;
}

String GroupObject::getObjectName() const {
  String name = getFunctionName() + " " + getTypeName();

  return name;
}