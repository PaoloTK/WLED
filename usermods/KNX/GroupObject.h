#include "wled.h"

enum class ObjectFunction {
  Switch,
  Absolute_Dim,
  Relative_Dim,
  Preset,
  Playlist,
  Color_Temperature,
  Color,
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
  public:
    ObjectFunction function;
    ObjectType type;
    String address;
    bool enabled;

    GroupObject(ObjectFunction function,
                ObjectType type,
                String address = "0/0/0", // @FIX: change to invalidgroup[]
                bool enabled = false)
    : function(function)
    , type(type)
    , address(address)
    , enabled(enabled)
    { }
};