#pragma once

#include "wled.h"
// #include <KnxTpUart.h>

class KnxUsermod : public Usermod {

  private:
    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    int8_t txPin, rxPin;
    int16_t individualAddress;
    // KnxTpUart* knxPtr;

    // string that are used multiple times (this will save some flash memory)
    static const char _name[], _enabled[], _pin[], _txPin[], _rxPin[],
                      _individualAddress[], _connectTo[], _onBusCoupler[];

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
      top[FPSTR(_txPin)] = txPin;
      top[FPSTR(_rxPin)] = rxPin;
    }

    bool readFromConfig(JsonObject& root) override
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
      configComplete &= getJsonValue(top[FPSTR(_txPin)], txPin);
      configComplete &= getJsonValue(top[FPSTR(_rxPin)], rxPin);


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
      // addInfo('KNX:TX pin',1,'Connect to RX Pin on bus coupler')"
      oappend(SET_F("addInfo('"));
      oappend(String(FPSTR(_name)).c_str());
      oappend(SET_F(":"));
      oappend(String(FPSTR(_txPin)).c_str());
      oappend(SET_F("',1,'"));
      oappend(String(FPSTR(_connectTo)).c_str());
      oappend(String(FPSTR(_rxPin)).c_str());
      oappend(String(FPSTR(_onBusCoupler)).c_str());
      oappend(SET_F("');"));
      // addInfo('KNX:TX pin',1,'Connect to TX Pin on bus coupler')"
      oappend(SET_F("addInfo('"));
      oappend(String(FPSTR(_name)).c_str());
      oappend(SET_F(":"));
      oappend(String(FPSTR(_rxPin)).c_str());
      oappend(SET_F("',1,'"));
      oappend(String(FPSTR(_connectTo)).c_str());
      oappend(String(FPSTR(_txPin)).c_str());
      oappend(String(FPSTR(_onBusCoupler)).c_str());
      oappend(SET_F("');"));
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
  PinManagerPinType pins[2] = { { txPin, true }, { rxPin, false } };
  if (!pinManager.allocateMultiplePins(pins, 2, PinOwner::UM_KNX)) {
    txPin = -1;
    rxPin = -1;
    // @FIX Add user facing error
    return;
  }
}


// add more strings here to reduce flash memory usage
const char KnxUsermod::_name[] PROGMEM              = "KNX";
const char KnxUsermod::_enabled[] PROGMEM           = "enabled";
const char KnxUsermod::_txPin[] PROGMEM             = "TX pin";
const char KnxUsermod::_rxPin[] PROGMEM             = "RX pin";
const char KnxUsermod::_connectTo[] PROGMEM         = "Connect to ";
const char KnxUsermod::_onBusCoupler[] PROGMEM      = " on bus coupler";
const char KnxUsermod::_individualAddress[] PROGMEM = "individual address";