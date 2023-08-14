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

    byte lastKnownBri = 0;

    std::unique_ptr<KnxTpUart> knxPtr;

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
        Serial1.begin(19200, SERIAL_8E1);
        lastKnownBri = bri;
      }

      initDone = true;
    }

    void loop() {
      if (!enabled || strip.isUpdating()) return;

      // do your magic here
      if (millis() - lastTime > 100) {
        if (knxPtr)
        {
          KnxTpUartSerialEventType eType = knxPtr->serialEvent();
          if (eType == KNX_TELEGRAM || eType == IRRELEVANT_KNX_TELEGRAM)
          {
              KnxTelegram* telegram = knxPtr->getReceivedTelegram();
          }
          lastTime = millis();
        }
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
            knxPtr->groupWriteBool(switchGroup, true);
          }
          else {
            knxPtr->groupWriteBool(switchGroup, false);
          }
        }
      }
    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;
      //save these vars persistently whenever settings are saved
      JsonObject indivAddr = top.createNestedObject(F("Individual Address"));
      indivAddr["Address"] = validateAddress(individualAddress) ? individualAddress : "INVALID ADDRESS";
      JsonObject switchAddr = top.createNestedObject(F("Switch Group"));
      switchAddr[FPSTR(_enabled)] = enableSwitch;
      switchAddr["Address"] = validateGroup(switchGroup) ? switchGroup : "INVALID GROUP";
      
    }

    bool readFromConfig(JsonObject& root)
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);

      JsonObject indivAddr = top[F("Individual Address")];
      configComplete = !indivAddr.isNull();
      configComplete &= getJsonValue(indivAddr["Address"], individualAddress, "0.0.0");

      JsonObject switchAddr = top[F("Switch Group")];
      configComplete = !switchAddr.isNull();      
      configComplete &= getJsonValue(switchAddr[FPSTR(_enabled)], enableSwitch, false);
      configComplete &= getJsonValue(switchAddr["Address"], switchGroup, "0/0/0");

      return configComplete; 
    }

    void addToJsonInfo(JsonObject& root)
    {
      // if "u" object does not exist yet we need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      if (validateAddress(individualAddress)) {
        JsonArray individualAddressArr = user.createNestedArray(F("Individual address: "));
        individualAddressArr.add(individualAddress);
      }
      
      if (enableSwitch && validateGroup(switchGroup)) {
        JsonArray groupAddressArr = user.createNestedArray(F("Switch Group: "));      
        groupAddressArr.add(switchGroup);
      }
    }  

    uint16_t getId()
    {
      return USERMOD_ID_KNX;
    }
};

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