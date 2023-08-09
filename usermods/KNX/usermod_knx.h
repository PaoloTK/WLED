#pragma once

#include <memory>
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

    bool validateAddress(String address);
    bool validateGroup(String address);

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
      indivAddr["Address"] = individualAddress;
      JsonObject switchAddr = top.createNestedObject(F("Switch Group"));
      switchAddr[FPSTR(_enabled)] = enableSwitch;
      switchAddr["Address"] = switchGroup;
      
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

bool KnxUsermod::validateAddress(String address) {
  return true;
}

bool KnxUsermod::validateGroup(String address) {
  return true;
}

// add more strings here to reduce flash memory usage
const char KnxUsermod::_name[]    PROGMEM = "KNX";
const char KnxUsermod::_enabled[] PROGMEM = "enabled";