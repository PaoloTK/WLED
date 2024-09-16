#pragma once

#include "wled.h"
// #include <KnxTpUart.h>

class KnxUsermod : public Usermod {

  private:
    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    int8_t knxPins[2];
    int16_t individualAddress;
    // KnxTpUart* knxPtr;

    // string that are used multiple times (this will save some flash memory)
    static const char _name[];
    static const char _enabled[];
    static const char _individualAddress[];
    static const char _tx[];
    static const char _rx[];

    // Allocate pins for the bus connection
    void allocatePins();

    // void publishMqtt(const char* state, bool retain = false);

  public:
    inline void enable(bool enable) { enabled = enable; }
    inline bool isEnabled() { return enabled; }

    void setup() override {
      allocatePins();

      initDone = true;
    }

    void loop() override {
      if (!enabled || strip.isUpdating()) return;

      // do your magic here
      if (millis() - lastTime > 1000) {

        lastTime = millis();
      }
    }

    void addToJsonInfo(JsonObject& root) override
    {
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");
    }

    void addToJsonState(JsonObject& root) override
    {
      if (!initDone || !enabled) return;  // prevent crash on boot applyPreset()

      JsonObject usermod = root[FPSTR(_name)];
      if (usermod.isNull()) usermod = root.createNestedObject(FPSTR(_name));
    }

    void readFromJsonState(JsonObject& root) override
    {
      if (!initDone) return;  // prevent crash on boot applyPreset()

      JsonObject usermod = root[FPSTR(_name)];
      if (!usermod.isNull()) {
        // userVar0 = usermod["user0"] | userVar0;
      }
    }

    void addToConfig(JsonObject& root) override
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;
      JsonObject pins = top.createNestedObject(F("Serial Pins"));
      JsonArray pinArray = pins.createNestedArray("pin");
      pinArray.add(knxPins[0]);
      pinArray.add(knxPins[1]);
    }

    bool readFromConfig(JsonObject& root) override
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);

      JsonObject pins = top[F("Serial Pins")];
      configComplete = !pins.isNull();
      configComplete &= getJsonValue(pins["pin"][0], knxPins[0], -1);
      configComplete &= getJsonValue(pins["pin"][1], knxPins[1], -1);


    //   // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
    //   configComplete &= getJsonValue(top["testInt"], testInt, 42);  
    //   configComplete &= getJsonValue(top["testLong"], testLong, -42424242);

    //   // "pin" fields have special handling in settings page (or some_pin as well)
    //   configComplete &= getJsonValue(top["pin"][0], testPins[0], -1);
    //   configComplete &= getJsonValue(top["pin"][1], testPins[1], -1);

      return configComplete;
    }


    /*
     * appendConfigData() is called when user enters usermod settings page
     * it may add additional metadata for certain entry fields (adding drop down is possible)
     * be careful not to add too much as oappend() buffer is limited to 3k
     */
    void appendConfigData() override
    {
    }
  
#ifndef WLED_DISABLE_MQTT
    bool onMqttMessage(char* topic, char* payload) override {
      return false;
    }

    void onMqttConnect(bool sessionPresent) override {
    }
#endif

    void onStateChange(uint8_t mode) override {
      // do something if WLED state changed (color, brightness, effect, preset, etc)
    }

    uint16_t getId() override
    {
      return USERMOD_ID_KNX;
    }

   //More methods can be added in the future, this example will then be extended.
   //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};

void KnxUsermod::allocatePins() {
  PinManagerPinType pins[2] = { { knxPins[0], true }, { knxPins[1], false } };
  if (!pinManager.allocateMultiplePins(pins, 2, PinOwner::UM_KNX)) {
    knxPins[0] = -1;
    knxPins[1] = -1;
    // @FIX Add user facing error
    return;
  }
}


// add more strings here to reduce flash memory usage
const char KnxUsermod::_name[]    PROGMEM = "KnxUsermod";
const char KnxUsermod::_enabled[] PROGMEM = "enabled";
const char KnxUsermod::_individualAddress[] PROGMEM = "IndividualAddress:";
const char KnxUsermod::_tx[] PROGMEM = "TX:";
const char KnxUsermod::_rx[] PROGMEM = "RX:";