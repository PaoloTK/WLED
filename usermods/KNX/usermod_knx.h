#pragma once

#include <sstream>
#include <cstdio>
#include "wled.h"
#include <KnxTpUart.h>

class KnxUsermod : public Usermod {

  private:

    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    static const char _name[];
    static const char _enabled[];

    String individualAddress;
    bool enableSwitch;
    String switchGroup;
    String switchStateGroup;
    bool enableAbsoluteDim;
    String absoluteDimGroup;
    String absoluteDimStateGroup;

    byte lastKnownBri = 0;

    std::unique_ptr<KnxTpUart> knxPtr;

    void updateFromBus();

    // KNX invidual addresses should be in the format X.Y.Z, with X and Y 0-15 and Z 0-255
    bool validateAddress(const String& address);
    // Group addresses can be in 3-level X/Y/Z (0-31/0-7/0-255), 2-level X/Z (0-31/0-2047) or free style Z (0-65535)
    bool validateGroup(const String& address);

  public:

    inline void enable(bool enable) { enabled = enable; }

    inline bool isEnabled() { return enabled; }

    void setup() {
      if (isEnabled() && individualAddress) {
        knxPtr = std::unique_ptr<KnxTpUart>(new KnxTpUart(&Serial1, individualAddress));
        if (knxPtr) {
          if (switchGroup != "0/0/0") {
            knxPtr->addListenGroupAddress(switchGroup);
          }
          if (absoluteDimGroup != "0/0/0") {
            knxPtr->addListenGroupAddress(absoluteDimGroup);            
          }

        }
        Serial1.begin(19200, SERIAL_8E1, 18, 19);
        lastKnownBri = bri;
      }

      initDone = true;
    }

    void loop() {
      if (!enabled || strip.isUpdating()) return;

      // do your magic here
      if (millis() - lastTime > 100) {
        if (knxPtr) {
          updateFromBus();
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
          if (bri) {
            knxPtr->groupWriteBool(switchStateGroup, true);
          }
          else {
            knxPtr->groupWriteBool(switchStateGroup, false);
          }
        }
      }
    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;

      JsonObject indivAddr = top.createNestedObject(F("Individual Address"));
      indivAddr["Address"] = validateAddress(individualAddress) ? individualAddress : "INVALID ADDRESS";

      JsonObject swGroups = top.createNestedObject(F("Switch Groups"));
      swGroups[FPSTR(_enabled)] = enableSwitch;
      swGroups["Address"] = validateGroup(switchGroup) ? switchGroup : "INVALID GROUP";
      swGroups["State"] = validateGroup(switchStateGroup) ? switchStateGroup : "INVALID GROUP";

      JsonObject absDimGroups = top.createNestedObject(F("Absolute Dim Groups"));
      absDimGroups[FPSTR(_enabled)] = enableAbsoluteDim;
      absDimGroups["Address"] = validateGroup(absoluteDimGroup) ? absoluteDimGroup : "INVALID GROUP";
      absDimGroups["State"] = validateGroup(absoluteDimStateGroup) ? absoluteDimStateGroup : "INVALID GROUP";
      
    }

    bool readFromConfig(JsonObject& root)
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);

      JsonObject indivAddr = top[F("Individual Address")];
      configComplete = !indivAddr.isNull();
      configComplete &= getJsonValue(indivAddr["Address"], individualAddress, "0.0.0");

      JsonObject swGroups = top[F("Switch Groups")];
      configComplete = !swGroups.isNull();      
      configComplete &= getJsonValue(swGroups[FPSTR(_enabled)], enableSwitch, false);
      configComplete &= getJsonValue(swGroups["Address"], switchGroup, "0/0/0");
      configComplete &= getJsonValue(swGroups["State"], switchStateGroup, "0/0/0");

      JsonObject absDimGroups = top[F("Absolute Dim Groups")];
      configComplete = !absDimGroups.isNull();      
      configComplete &= getJsonValue(absDimGroups[FPSTR(_enabled)], enableAbsoluteDim, false);
      configComplete &= getJsonValue(absDimGroups["Address"], absoluteDimGroup, "0/0/0");
      configComplete &= getJsonValue(absDimGroups["State"], absoluteDimStateGroup, "0/0/0");

      return configComplete; 
    }

    void addToJsonInfo(JsonObject& root)
    {
      // if "u" object does not exist yet we need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      JsonArray addressArr = user.createNestedArray(F("Individual address:"));
      addressArr.add(individualAddress);
      
      if (enableSwitch) {
        JsonArray switchArr = user.createNestedArray(F("Switch group:"));
        switchArr.add(switchGroup);
        JsonArray switchStateArr = user.createNestedArray(F("Switch state group:"));
        switchStateArr.add(switchStateGroup);
      }
      if (enableAbsoluteDim) {
        JsonArray absDimArr = user.createNestedArray(F("Absolute dim group:"));
        absDimArr.add(absoluteDimGroup);
        JsonArray absDimStateArr = user.createNestedArray(F("Absolute dim state group:"));
        absDimStateArr.add(absoluteDimStateGroup);
      }
    }  

    uint16_t getId()
    {
      return USERMOD_ID_KNX;
    }
};

void KnxUsermod::updateFromBus() {
  KnxTpUartSerialEventType eType = knxPtr->serialEvent();

  if (eType == KNX_TELEGRAM)
  {
      KnxTelegram* telegram = knxPtr->getReceivedTelegram();
      if (telegram->getBool()) {
        if (bri == 0) {
          bri = briLast;
          updateInterfaces(CALL_MODE_BUTTON);
        }
      }
      // dunno if payload incorrect or off state
      else {
        Serial.println("NO");
        if (bri != 0) {
          bri = 0;
          updateInterfaces(CALL_MODE_BUTTON);
        }
      }
      if (telegram->get1ByteIntValue()) {
        Serial.println(telegram->get1ByteIntValue());
        bri = telegram->get1ByteIntValue();
        updateInterfaces(CALL_MODE_BUTTON);
      }
  }
}

bool KnxUsermod::validateAddress(const String& address) {
  bool validAddress = false;
  int area, line, device;
  int members = std::sscanf(address.c_str(), "%i.%i.%i", &area, &line, &device);

  if (members == 3) {
    if ((0 <= area) && (area <= 15) && (0 <= line) && (line <= 15) && (0 <= device) && (device <= 255)) {
      validAddress = true;
    }
  }

  return validAddress;
}

bool KnxUsermod::validateGroup(const String& address) {
  bool validAddress = false;
  int first, second, third;

  int members = std::sscanf(address.c_str(), "%i/%i/%i", &first, &second, &third);

  if (members == 3) {
    if ((0 <= first) && (first <= 31) && (0 <= second) && (second <= 7) && (0 <= third) && (third <= 255)) {
      validAddress = true;
    }
  }
  else if (members == 2) {
    if ((0 <= first) && (first <= 31) && (0 <= second) && (second <= 2047)) {
      validAddress = true;
    }
  }
  else if (members == 1) {
    if (first <= 65535) {
      validAddress = true;
    }
  }

  return validAddress; 
}

// add more strings here to reduce flash memory usage
const char KnxUsermod::_name[]    PROGMEM = "KNX";
const char KnxUsermod::_enabled[] PROGMEM = "enabled";