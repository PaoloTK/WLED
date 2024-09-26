#pragma once

#include "wled.h"
#include "group_address.h"
#include "individual_address.h"

// #include <KnxTpUart.h>

class KnxUsermod : public Usermod {
  private:

    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    int8_t txPin, rxPin;
    IndividualAddress individualAddress;
    GroupStyle groupStyle;
    // KnxTpUart* knxPtr;

    static const char _name[], _enabled[], _pin[], _txPin[], _rxPin[],
                      _individualAddress[], _groupStyle[],
                      _connectTo[], _onBusCoupler[];

    // Allocate pins for the bus connection
    void allocatePins();

  public:
    inline void enable(bool enable) { enabled = enable; }
    inline bool isEnabled() { return enabled; }
    inline uint16_t getId() override { return USERMOD_ID_KNX; }

    void setup() override;
    void loop() override;
    void addToJsonInfo(JsonObject &root) override;
    void addToJsonState(JsonObject &root) override;
    void readFromJsonState(JsonObject &root) override;
    void addToConfig(JsonObject &root) override;
    bool readFromConfig(JsonObject &root) override;
    void appendConfigData() override;
    void onStateChange(uint8_t mode) override;
};

void KnxUsermod::setup() {
  allocatePins();

  initDone = true;
}

void KnxUsermod::loop() {
  if (!enabled || strip.isUpdating()) return;

  // do your magic here
  if (millis() - lastTime > 1000) {

    lastTime = millis();
  }
}

void KnxUsermod::addToJsonInfo(JsonObject& root)
{
  JsonObject user = root["u"];
  if (user.isNull()) user = root.createNestedObject("u");
}

void KnxUsermod::addToJsonState(JsonObject& root)
{
  if (!initDone || !enabled) return;  // prevent crash on boot applyPreset()

  JsonObject usermod = root[FPSTR(_name)];
  if (usermod.isNull()) usermod = root.createNestedObject(FPSTR(_name));
}

void KnxUsermod::readFromJsonState(JsonObject& root)
{
  if (!initDone) return;  // prevent crash on boot applyPreset()

  JsonObject usermod = root[FPSTR(_name)];
  if (!usermod.isNull()) {
    // userVar0 = usermod["user0"] | userVar0;
  }
}

void KnxUsermod::addToConfig(JsonObject& root)
{
  char* IA = individualAddress.toString();

  JsonObject top = root.createNestedObject(FPSTR(_name));
  top[FPSTR(_enabled)] = enabled;
  top[FPSTR(_txPin)] = txPin;
  top[FPSTR(_rxPin)] = rxPin;
  top[FPSTR(_individualAddress)] = IA;
  top[FPSTR(_groupStyle)] = groupStyle;

  delete[] IA;

}

bool KnxUsermod::readFromConfig(JsonObject& root)
{
  const char* IA;
  JsonObject top = root[FPSTR(_name)];

  bool configComplete = !top.isNull();
  configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled, false);
  configComplete &= getJsonValue(top[FPSTR(_txPin)], txPin, -1);
  configComplete &= getJsonValue(top[FPSTR(_rxPin)], rxPin, -1);
  configComplete &= getJsonValue(top[FPSTR(_individualAddress)], IA);
  configComplete &= getJsonValue(top[FPSTR(_groupStyle)], groupStyle, GroupStyle::THREE_LEVEL);

  individualAddress.fromString(IA);

  return configComplete;
}

void KnxUsermod::appendConfigData()
{
  oappend(SET_F("addInfo('KNX:TX pin',1,'Connect to RX Pin on bus coupler')"));
  oappend(SET_F("addInfo('KNX:RX pin',1,'Connect to TX Pin on bus coupler')"));

  oappend(SET_F("dd=addDropdown('KNX','Group style');"));
  oappend(SET_F("addOption(dd,'Free-Level',0);"));
  oappend(SET_F("addOption(dd,'2-Level',1);"));
  oappend(SET_F("addOption(dd,'3-Level',2);"));
}

void KnxUsermod::onStateChange(uint8_t mode) {
  // do something if WLED state changed (color, brightness, effect, preset, etc)
}

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
const char KnxUsermod::_individualAddress[] PROGMEM = "individual address";
const char KnxUsermod::_groupStyle[] PROGMEM        = "group style";