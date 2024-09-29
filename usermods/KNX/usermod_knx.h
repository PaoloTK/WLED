#pragma once

#include "wled.h"
#include "individual_address.h"
#include "group_object.h"
#include <KnxTpUart.h>

class KnxUsermod : public Usermod {
  private:

    bool enabled = false;
    bool initDone = false;
    unsigned long lastTime = 0;

    int8_t txPin, rxPin;
    IndividualAddress individualAddress;
    GroupStyle groupStyle;
    KnxTpUart* knx;
    GroupObject mainListenObjects[5] = {
      GroupObject(ObjectFunction::SWITCH, ObjectType::LISTEN),
      GroupObject(ObjectFunction::ABSOLUTE_DIM, ObjectType::LISTEN),
      GroupObject(ObjectFunction::RELATIVE_DIM, ObjectType::LISTEN),
      GroupObject(ObjectFunction::PALETTE, ObjectType::LISTEN),
      GroupObject(ObjectFunction::PLAYLIST, ObjectType::LISTEN)
    };

    GroupObject mainStateObjects[4] = {
      GroupObject(ObjectFunction::SWITCH, ObjectType::STATE),
      GroupObject(ObjectFunction::ABSOLUTE_DIM, ObjectType::STATE),
      GroupObject(ObjectFunction::PALETTE, ObjectType::STATE),
      GroupObject(ObjectFunction::PLAYLIST, ObjectType::STATE)
    };

    static const char _name[], _enabled[], _disabled[],
                      _pin[], _txPin[], _rxPin[],
                      _individualAddress[], _groupStyle[];

    // Allocate pins for the bus connection
    bool allocatePins();
    void initBus();

  public:
    inline void enable(bool enable) { enabled = enable; }
    inline bool isEnabled() { return enabled; }
    inline uint16_t getId() override { return USERMOD_ID_KNX; }

    void setup() override;
    void loop() override;
    void addToJsonInfo(JsonObject &root) override;
    void addToConfig(JsonObject &root) override;
    bool readFromConfig(JsonObject &root) override;
    void appendConfigData() override;
    void onStateChange(uint8_t mode) override;
};

void KnxUsermod::setup() {
  initBus();

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

  JsonArray knxArr = user.createNestedArray(FPSTR(_name));
  knxArr.add(enabled ? FPSTR(_enabled) : FPSTR(_disabled));

  if (enabled) {
    JsonArray addressArr = user.createNestedArray(FPSTR(_individualAddress));
    addressArr.add(individualAddress.toString());
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
  oappend(SET_F("addInfo('KNX:TX pin',1,'Connect to RX Pin on bus coupler');"));
  oappend(SET_F("addInfo('KNX:RX pin',1,'Connect to TX Pin on bus coupler');"));

  oappend(SET_F("gs=addDropdown('KNX','group style');"));
  oappend(SET_F("addOption(gs,'Free-Level',0);"));
  oappend(SET_F("addOption(gs,'2-Level',1);"));
  oappend(SET_F("addOption(gs,'3-Level',2);"));
}

void KnxUsermod::onStateChange(uint8_t mode) {
  // do something if WLED state changed (color, brightness, effect, preset, etc)
}

bool KnxUsermod::allocatePins() {
  PinManagerPinType pins[2] = { { txPin, true }, { rxPin, false } };
  if (!pinManager.allocateMultiplePins(pins, 2, PinOwner::UM_KNX)) {
    txPin = -1;
    rxPin = -1;
    // @FIX Add user facing error
    return false;
  }
  return true;
}

void KnxUsermod::initBus() {
  if (individualAddress && allocatePins()) {
    knx = new KnxTpUart(&Serial1, individualAddress.toString());
    if (knx) {
      
    }
  }
}

// add more strings here to reduce flash memory usage
const char KnxUsermod::_name[] PROGMEM              = "KNX";
const char KnxUsermod::_enabled[] PROGMEM           = "enabled";
const char KnxUsermod::_disabled[] PROGMEM          = "disabled";
const char KnxUsermod::_txPin[] PROGMEM             = "TX pin";
const char KnxUsermod::_rxPin[] PROGMEM             = "RX pin";
const char KnxUsermod::_individualAddress[] PROGMEM = "individual address";
const char KnxUsermod::_groupStyle[] PROGMEM        = "group style";