#pragma once

#include "wled.h"
namespace KNX 
{
     #include <KnxTpUart.h>
}

class KnxUsermod : public Usermod {

  private:

    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    static const char _name[];
    static const char _enabled[];
    static const char _main[];
    static const char _middle[];
    static const char _sub[];

    int individualAddress[3];
    bool enableSwitch;
    int switchGroup[3];

    String addressToString(int address[3], bool isGroupAddress) {
      char stringAddress[12];
      const char delimiter = isGroupAddress ? '/' : '.';

      sprintf(stringAddress,"%u%c%u%c%u", address[0], delimiter, address[1], delimiter, address[2]);
      return String(stringAddress);
    }

  public:

    inline void enable(bool enable) { enabled = enable; }

    inline bool isEnabled() { return enabled; }

    void setup() {
      Serial.println("Hello from KNX!");
      initDone = true;
    }

    void connected() {
      Serial.println("Connected to WiFi!");      
    }

    void loop() {
    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;
      //save these vars persistently whenever settings are saved
      JsonObject indivAddr = top.createNestedObject(F("Individual Address"));
      indivAddr["Area"] = individualAddress[0];
      indivAddr["Line"] = individualAddress[1];
      indivAddr["Member"] = individualAddress[2];
      JsonObject switchAddr = top.createNestedObject(F("Switch Group"));
      switchAddr[FPSTR(_enabled)] = enableSwitch;
      switchAddr[FPSTR(_main)] = switchGroup[0];
      switchAddr[FPSTR(_middle)] = switchGroup[1];
      switchAddr[FPSTR(_sub)] = switchGroup[2];
      
    }

    bool readFromConfig(JsonObject& root)
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);

      JsonObject indivAddr = top[F("Individual Address")];
      configComplete = !indivAddr.isNull();
      configComplete &= getJsonValue(indivAddr["Area"], individualAddress[0], 0);
      configComplete &= getJsonValue(indivAddr["Line"], individualAddress[1], 0);
      configComplete &= getJsonValue(indivAddr["Member"], individualAddress[2], 0);

      JsonObject switchAddr = top[F("Switch Group")];
      configComplete = !switchAddr.isNull();      
      configComplete &= getJsonValue(switchAddr[FPSTR(_enabled)], enableSwitch, false);
      configComplete &= getJsonValue(switchAddr[FPSTR(_main)], switchGroup[0], 0);
      configComplete &= getJsonValue(switchAddr[FPSTR(_middle)], switchGroup[1], 0);
      configComplete &= getJsonValue(switchAddr[FPSTR(_sub)], switchGroup[2], 0);

      return configComplete; 
    }

    void addToJsonInfo(JsonObject& root)
    {
      // if "u" object does not exist yet we need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      JsonArray individualAddressArr = user.createNestedArray(F("Individual address: "));
      individualAddressArr.add(addressToString(individualAddress, false));
      if (enableSwitch) {
        JsonArray groupAddressArr = user.createNestedArray(F("Switch Group: "));      
        groupAddressArr.add(addressToString(switchGroup, true));
      }
    }  

    void onStateChange(uint8_t mode) {
      // do something if WLED state changed (color, brightness, effect, preset, etc)
    }

    uint16_t getId()
    {
      return USERMOD_ID_KNX;
    }
};

// add more strings here to reduce flash memory usage
const char KnxUsermod::_name[]    PROGMEM = "KNX";
const char KnxUsermod::_enabled[] PROGMEM = "enabled";
const char KnxUsermod::_main[] PROGMEM = "main";
const char KnxUsermod::_middle[] PROGMEM = "middle";
const char KnxUsermod::_sub[] PROGMEM = "sub";