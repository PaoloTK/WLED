#pragma once

#include "GroupObject.h"
#include "wled.h"
#include <cstdio>
#include <KnxTpUart.h>

class KnxUsermod : public Usermod {

  private:

    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    static const char _name[];
    static const char _enabled[];
    static const char _address[];
    static const char _listen[];
    static const char _state[];
    static const char _time[];
    static const char _invalidaddress[];
    static const char _invalidgroup[];
    static const char _rxPin[];
    static const char _txPin[];

    int8_t txPin; // TX pin to connect to RX pin from KNX UART device
    int8_t rxPin; // RX pin to connect to TX pin from KNX UART device

    String individualAddress;
    int relativeDimTime;

    std::vector<GroupObject> mainObjects;
    std::vector<std::vector<GroupObject>> segmentsObjects;

    bool isDimming = false;
    bool dimUp = false;
    byte lastKnownBri = 0;
    float relativeDimIncrement = 51; // 5 seconds for 0 to 100%

    std::unique_ptr<KnxTpUart> knxPtr;

    // Allocate pins for the bus connection
    void allocatePins();
    // Initialize the bus connection
    void initBus();
    // Read telegram from bus and adjust light if necessary
    void updateFromBus();
    // Populate group object vectors
    void populateObjects();
    // Push the required objects for each function
    void pushObjects(std::vector<GroupObject>& objects, std::initializer_list<ObjectFunction> functions);
    // Dim light relatively based on bus telegrams
    void dimLight();
    // Count the delimiter in the address to validate and recognize address style
    int countDelimiter(const String& address, const char delimiter);
    // KNX invidual addresses should be in the format X.Y.Z, with X and Y 0-15 and Z 1-255
    bool validateAddress(const String& address);
    // Group addresses can be in 3-level X/Y/Z (0-31/0-7/0-255), 2-level X/Z (0-31/0-2047) or free style Z (0-65535), and the members can't add up to 0
    bool validateGroup(const String& address);
    // Does the telegram source group match the target group
    bool isGroupTarget(const KnxTelegram telegram, const String& target);
    // Returns object that matches function and type
    GroupObject getObject(std::vector<GroupObject> objects, const ObjectFunction& function, const ObjectType& type);

  public:

    inline void enable(bool enable) { enabled = enable; }

    inline bool isEnabled() { return enabled; }

    void setup() {
      if (isEnabled()) {
        initBus();

        lastKnownBri = bri;
        // @FIX: relativeDimIncrement must update after config save too
        relativeDimIncrement = float(255) / relativeDimTime * 100;

      }

      initDone = true;
    }

    void loop() {
      if (!enabled || strip.isUpdating()) return;

      // do your magic here
      if (millis() - lastTime > 100) {
        if (knxPtr) {
          //updateFromBus();
          dimLight();
        }

        lastTime = millis();
      }
    }

    void onStateChange(uint8_t mode) {
      if (!initDone) return;

      if (knxPtr)
      {
        // Light Switch State
        if (bri != lastKnownBri) {
          lastKnownBri = bri;

          // if (bri) {
          //   // @FIX: only write if group is valid, use ternary to remove else statement for switch state
          //   knxPtr->groupWriteBool(switchState.group, true);
          //   knxPtr->groupWrite1ByteInt(absoluteDimState.group, bri);
          // }
          // else {
          //   knxPtr->groupWriteBool(switchState.group, false);
          // }
        }
      }
    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;

      JsonObject pins = top.createNestedObject(F("Serial Pins"));
      pins[FPSTR(_txPin)] = txPin;
      pins[FPSTR(_rxPin)] = rxPin;

      JsonObject indivAddr = top.createNestedObject(F("Individual Address"));
      indivAddr[FPSTR(_address)] = validateAddress(individualAddress) ? individualAddress : FPSTR(_invalidaddress);

      for(const auto& object : mainObjects) {
        String function = object.getFunctionName();
        String type = object.getTypeName();
        String address = object.address;

        JsonObject functionObj = (top.getMember(function)) ? JsonObject(top.getMember(function)) : JsonObject(top.createNestedObject(function));
        functionObj[type] = validateAddress(address) ? address : FPSTR(_invalidgroup);
      }

      // for (size_t i = 0; i < knxFunctions.size(); ++i) {
      //   String& group = knxFunctions[i].group;
      //   String& name = knxFunctions[i].name;
      //   // @FIX: Verify invalid function doesn't evaluates as listen
      //   String function = (knxFunctions[i].function == Listen) ? FPSTR(_listen) : FPSTR(_state);

      //   JsonObject functionObj = (top.getMember(name)) ? JsonObject(top.getMember(name)) : JsonObject(top.createNestedObject(name));

      //   functionObj[FPSTR(_enabled)] = knxFunctions[i].enabled;
      //   functionObj[function] = validateGroup(group) ? group : FPSTR(_invalidgroup);
      //   if (name == relativeDimListen.name) {
      //     functionObj[FPSTR(_time)] = relativeDimTime;
      //   }
      //   String test;
      //   getJsonValue(functionObj[function], test, "AGG");
      //   Serial.println("TEST: " + test);
      // }
      

      // for (size_t i = 0; i < knxFunctions.size(); ++i) {
      //   String& listen = knxFunctions[i].listenGroup;
      //   String& state = knxFunctions[i].stateGroup;
      //   String& name = knxFunctions[i].name;

      //   JsonObject functionObj = top.createNestedObject(name);
      //   functionObj[FPSTR(_enabled)] = knxFunctions[i].enabled;
      //   functionObj[FPSTR(_listen)] = validateGroup(listen) ? listen : FPSTR(_invalidgroup);
      //   functionObj[FPSTR(_state)] = validateGroup(state) ? state : FPSTR(_invalidgroup);
      //   if (name == relativeDimFunction.name) {
      //     functionObj[FPSTR(_time)] = relativeDimTime;
      //   }
      // }

      // JsonObject swGroups = top.createNestedObject(F("Switch Groups"));
      // swGroups[FPSTR(_enabled)] = switchFunction.enabled;
      // swGroups[FPSTR(_listen)] = validateGroup(switchFunction.listenGroup) ? switchFunction.listenGroup : FPSTR(_invalidgroup);
      // swGroups[FPSTR(_state)] = validateGroup(switchFunction.stateGroup) ? switchFunction.stateGroup : FPSTR(_invalidgroup);

      // JsonObject absDimGroups = top.createNestedObject(F("Absolute Dim Groups"));
      // absDimGroups[FPSTR(_enabled)] = absoluteDimFunction.enabled;
      // absDimGroups[FPSTR(_listen)] = validateGroup(absoluteDimFunction.listenGroup) ? absoluteDimFunction.listenGroup : FPSTR(_invalidgroup);
      // absDimGroups[FPSTR(_state)] = validateGroup(absoluteDimFunction.stateGroup) ? absoluteDimFunction.stateGroup : FPSTR(_invalidgroup);

      // JsonObject relDimGroups = top.createNestedObject(F("Relative Dim Groups"));
      // relDimGroups[FPSTR(_enabled)] = relativeDimFunction.enabled;
      // relDimGroups[FPSTR(_listen)] = validateGroup(relativeDimFunction.listenGroup) ? relativeDimFunction.listenGroup : FPSTR(_invalidgroup);
      // relDimGroups[FPSTR(_time)] = relativeDimTime;
      
    }

    bool readFromConfig(JsonObject& root)
    {
      // Build vectors here before assigning values
      populateObjects();

      JsonObject top = root[FPSTR(_name)];
      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);

      JsonObject pins = top[F("Serial Pins")];
      configComplete &= !pins.isNull();
      configComplete &= getJsonValue(pins[FPSTR(_rxPin)], rxPin, -1);
      configComplete &= getJsonValue(pins[FPSTR(_txPin)], txPin, -1);

      JsonObject indivAddr = top[F("Individual Address")];
      configComplete &= !indivAddr.isNull();
      configComplete &= getJsonValue(indivAddr[FPSTR(_address)], individualAddress, FPSTR(_invalidaddress));

      for(const auto& object : mainObjects) {
        String function = object.getFunctionName();
        String type = object.getTypeName();
        String address = object.address;

        JsonObject functionObj = (top.getMember(function)) ? JsonObject(top.getMember(function)) : JsonObject(top.createNestedObject(function));
        configComplete &= !functionObj.isNull();

        configComplete &= getJsonValue(functionObj[type], address, FPSTR(_invalidgroup));

      }

      // for (size_t i = 0; i < knxFunctions.size(); ++i) {
      //   String& group = knxFunctions[i].group;
      //   String& name = knxFunctions[i].name;
      //   // @FIX: Verify invalid function doesn't evaluates as listen
      //   String function = (knxFunctions[i].function == Listen) ? FPSTR(_listen) : FPSTR(_state);

      //   JsonObject functionObj = (top.getMember(name)) ? JsonObject(top.getMember(name)) : JsonObject(top.createNestedObject(name));

      //   configComplete &= !functionObj.isNull();

      //   configComplete &= getJsonValue(functionObj[FPSTR(_enabled)], knxFunctions[i].enabled, false);
      //   configComplete &= getJsonValue(functionObj[function], group, FPSTR(_invalidgroup));
      //   if (name == relativeDimListen.name) {
      //     configComplete &= getJsonValue(functionObj[FPSTR(_time)], relativeDimTime, 5000);
      //   }
      // }

      // for (size_t i = 0; i < knxFunctions.size(); ++i) {
      //   String& listen = knxFunctions[i].listenGroup;
      //   String& state = knxFunctions[i].stateGroup;
      //   String& name = knxFunctions[i].name;

      //   JsonObject functionObj = top.createNestedObject(name);
      //   configComplete &= !functionObj.isNull();
      //   configComplete &= getJsonValue(functionObj[FPSTR(_enabled)], knxFunctions[i].enabled, false);
      //   configComplete &= getJsonValue(functionObj[FPSTR(_listen)], listen, FPSTR(_invalidgroup));
      //   configComplete &= getJsonValue(functionObj[FPSTR(_state)], state, FPSTR(_invalidgroup));
      //   if (name == relativeDimFunction.name) {
      //     configComplete &= getJsonValue(functionObj[FPSTR(_time)], relativeDimTime, 5000);
      //   }
      // }

      // JsonObject swGroups = top[F("Switch Groups")];
      // configComplete &= !swGroups.isNull();      
      // configComplete &= getJsonValue(swGroups[FPSTR(_enabled)], switchFunction.enabled, false);
      // configComplete &= getJsonValue(swGroups[FPSTR(_listen)], switchFunction.listenGroup, FPSTR(_invalidgroup));
      // configComplete &= getJsonValue(swGroups[FPSTR(_state)], switchFunction.stateGroup, FPSTR(_invalidgroup));

      // JsonObject absDimGroups = top[F("Absolute Dim Groups")];
      // configComplete &= !absDimGroups.isNull();
      // configComplete &= getJsonValue(absDimGroups[FPSTR(_enabled)], absoluteDimFunction.enabled, false);
      // configComplete &= getJsonValue(absDimGroups[FPSTR(_listen)], absoluteDimFunction.listenGroup, FPSTR(_invalidgroup));
      // configComplete &= getJsonValue(absDimGroups[FPSTR(_state)], absoluteDimFunction.stateGroup, FPSTR(_invalidgroup));

      // JsonObject relDimGroups = top[F("Relative Dim Groups")];
      // configComplete &= !relDimGroups.isNull();
      // configComplete &= getJsonValue(relDimGroups[FPSTR(_enabled)], relativeDimFunction.enabled, false);
      // configComplete &= getJsonValue(relDimGroups[FPSTR(_listen)], relativeDimFunction.listenGroup, FPSTR(_invalidgroup));
      // configComplete &= getJsonValue(relDimGroups[FPSTR(_time)], relativeDimTime, 5000);

      return configComplete; 
    }

    void addToJsonInfo(JsonObject& root)
    {
      // if "u" object does not exist yet we need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      user.createNestedArray(FPSTR(_name));

      JsonArray addressArr = user.createNestedArray(F("Individual address:"));
      addressArr.add(individualAddress);

      // for (size_t i = 0; i < knxFunctions.size(); ++i) {
      //   if (knxFunctions[i].enabled) {
      //     String& listen = knxFunctions[i].listenGroup;
      //     String& state = knxFunctions[i].stateGroup;
      //     String& name = knxFunctions[i].name;

      //     if (!listen.isEmpty() && listen != FPSTR(_invalidgroup)) {
      //       JsonArray listenArr = user.createNestedArray(name + " listen group:");
      //       listenArr.add(listen);
      //     }
      //     if (!state.isEmpty() && state != FPSTR(_invalidgroup)) {
      //       JsonArray stateArr = user.createNestedArray(name + " state group:");
      //       stateArr.add(state);
      //     }
      //     if (&knxFunctions[i] == &relativeDimFunction) {
      //       JsonArray relDimSpeedArr = user.createNestedArray(F("Relative dim speed:"));
      //       relDimSpeedArr.add(relativeDimTime);
      //       relDimSpeedArr.add(" ms");
      //     }
      //   }
      // }
    }  

    uint16_t getId()
    {
      return USERMOD_ID_KNX;
    }
};

void KnxUsermod::populateObjects() {
  mainObjects.clear();
  segmentsObjects.clear();

  pushObjects(mainObjects, {
    ObjectFunction::Switch,
    ObjectFunction::Absolute_Dim,
    ObjectFunction::Relative_Dim,
    ObjectFunction::Preset,
    ObjectFunction::Playlist
  });

  for (size_t i = 0; i < MAX_NUM_SEGMENTS; ++i) {
    segmentsObjects.push_back(std::vector<GroupObject>());
    
    pushObjects(segmentsObjects[i], {
      ObjectFunction::Switch,
      ObjectFunction::Absolute_Dim,
      ObjectFunction::Relative_Dim,
      ObjectFunction::Color_Temperature,
      ObjectFunction::Color_1,
      ObjectFunction::Color_2,
      ObjectFunction::Color_3,
      ObjectFunction::Effect,
      ObjectFunction::Speed,
      ObjectFunction::Intensity,
      ObjectFunction::Palette,      
    });
  }
}

void KnxUsermod::pushObjects(std::vector<GroupObject>& objects, std::initializer_list<ObjectFunction> functions) {
  for (auto &function : functions) {
    objects.push_back({function, ObjectType::Input});
    objects.push_back({function, ObjectType::Output});
  }
}

void KnxUsermod::allocatePins() {
  PinManagerPinType pins[2] = { { txPin, true }, { rxPin, false } };
  if (!pinManager.allocateMultiplePins(pins, 2, PinOwner::UM_KNX)) {
    txPin = -1;
    rxPin = -1;
    return;
  }
}

void KnxUsermod::initBus() {
  if (individualAddress) {
    allocatePins();
    knxPtr = std::unique_ptr<KnxTpUart>(new KnxTpUart(&Serial1, individualAddress));
    // if (knxPtr) {
      
    //   for (size_t i = 0; i < knxFunctions.size(); ++i) {
    //     GroupObject input = knxFunctions[i].input;
    //     String address = input.address;

    //     if (!address.isEmpty() && 
    //         validateGroup(address)) {
    //       knxPtr->addListenGroupAddress(address);
    //     }
    //   }
    // }

    Serial1.begin(19200, SERIAL_8E1, rxPin, txPin);
  }
}

void KnxUsermod::updateFromBus() {
  KnxTpUartSerialEventType eType = knxPtr->serialEvent();

  // if (eType == KNX_TELEGRAM)
  // {
  //   KnxTelegram* telegram = knxPtr->getReceivedTelegram();

  //   // Switch group
  //   if (isGroupTarget(*telegram, switchListen.group)) {
  //     if (telegram->getBool()) {
  //       if (bri == 0) {
  //         bri = briLast;
  //         updateInterfaces(CALL_MODE_BUTTON);
  //       }
  //     }
  //     else {
  //       if (bri != 0) {
  //         bri = 0;
  //         updateInterfaces(CALL_MODE_BUTTON);
  //       }
  //     }
  //   }
  //   // Absolute Dim Group
  //   if (isGroupTarget(*telegram, absoluteDimListen.group)) {
  //     if (telegram->get1ByteIntValue()) {
  //       bri = telegram->get1ByteIntValue();
  //       stateUpdated(CALL_MODE_DIRECT_CHANGE);
  //     }
  //   }
  //   // Relative Dim Group
  //   if (isGroupTarget(*telegram, relativeDimListen.group)) {
  //     // @FIX maybe switch to case switch to handle all cases including 0
  //     if (telegram->get4BitIntValue()) {
  //       isDimming = true;
  //       // @FIX only accounts for +100%/-100%
  //       // 9 = +100%
  //       if (telegram->get4BitIntValue() == 9) {
  //         dimUp = true;
  //       }
  //       // 1 = -100%
  //       else {
  //         dimUp = false;
  //       }
  //     }
  //     // 0 = dim button released
  //     else {
  //       isDimming = false;
  //     }
  //   }
    
  // }
}

void KnxUsermod::dimLight() {
  if (isDimming) {
    if (dimUp) {
      if (int(bri) + relativeDimIncrement > 255) {
        bri = 255;
        isDimming = false;
      }
      else {
        // @fix seems to not work with very high dim time values
        bri += relativeDimIncrement;
      }
      stateUpdated(CALL_MODE_DIRECT_CHANGE);
    }
    else {
      if (int(bri) - relativeDimIncrement < 1) {
        bri = 1;
        isDimming = false;
      }
      else {
        bri -= relativeDimIncrement;
      }
      stateUpdated(CALL_MODE_DIRECT_CHANGE);
    }
  }
}

int KnxUsermod::countDelimiter(const String& address, const char delimiter) {
  int delimits = 0;

  for (int i=0; i < address.length(); i++) {
    if (address.c_str()[i] == delimiter) {
      delimits++;
    }
  }

  return delimits;
}

bool KnxUsermod::isGroupTarget(KnxTelegram telegram, const String& target) {
  String sourceGroup;
  int main, middle, sub;

  main = telegram.getTargetMainGroup();
  middle = telegram.getTargetMiddleGroup();
  sub = telegram.getTargetSubGroup();

  // @fix: only accounts for three level style groups
  sourceGroup = String(String(main) + '/' + String(middle) + '/' + String(sub));

  return (target == sourceGroup) ? true : false;
}

bool KnxUsermod::validateAddress(const String& address) {
  bool validAddress = false;
  int dots, members, area, line, device;
  
  dots = countDelimiter(address, '.');

  if (dots == 2) {
    members = std::sscanf(address.c_str(), "%i.%i.%i", &area, &line, &device);

    if (members == 3) {
      // Devices should never have 0 as their "device" member
      if ((0 <= area) && (area <= 15) &&
          (0 <= line) && (line <= 15) &&
          (1 <= device) && (device <= 255)) {
        validAddress = true;
      }
    }
  }

  return validAddress;
}

bool KnxUsermod::validateGroup(const String& address) {
  bool validAddress = false;
  int slashes, members, first, second, third;
  
  slashes = countDelimiter(address, '/');
  
  if (slashes < 3) {
    members = std::sscanf(address.c_str(), "%i/%i/%i", &first, &second, &third);

    // 3-level structure
    if (members == 3 && ((first + second + third) != 0)) {
      if ((0 <= first) && (first <= 31) && (0 <= second) && (second <= 7) && (0 <= third) && (third <= 255)) {
        validAddress = true;
      }
    }
    // 2-level structure
    else if (members == 2 && ((first + second) != 0)) {
      if ((0 <= first) && (first <= 31) && (0 <= second) && (second <= 2047)) {
        validAddress = true;
      }
    }
    // free structure
    else if (members == 1 && (first != 0)) {
      if (first <= 65535) {
        validAddress = true;
      }
    }  
  }

  return validAddress; 
}

// add more strings here to reduce flash memory usage
const char KnxUsermod::_name[]    PROGMEM = "KNX";
const char KnxUsermod::_enabled[] PROGMEM = "enabled";
const char KnxUsermod::_address[]    PROGMEM = "address:";
const char KnxUsermod::_listen[]    PROGMEM = "listen:";
const char KnxUsermod::_state[] PROGMEM = "state:";
const char KnxUsermod::_time[] PROGMEM = "time:";
const char KnxUsermod::_invalidaddress[] PROGMEM = "0.0.0";
const char KnxUsermod::_invalidgroup[] PROGMEM = "0/0/0";
const char KnxUsermod::_txPin[] PROGMEM = "TX-pin:";
const char KnxUsermod::_rxPin[] PROGMEM = "RX-pin:";
