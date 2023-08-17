#pragma once

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
    static const char _address[];
    static const char _group[];
    static const char _state[];
    static const char _rxPin[];
    static const char _txPin[];

    int8_t txIo; // TX pin to connect to RX pin from KNX UART device
    int8_t rxIo; // RX pin to connect to TX pin from KNX UART device

    String individualAddress;
    bool enableSwitch;
    String switchGroup;
    String switchStateGroup;
    bool enableAbsoluteDim;
    String absoluteDimGroup;
    String absoluteDimStateGroup;

    byte lastKnownBri = 0;

    std::unique_ptr<KnxTpUart> knxPtr;

    // Allocate pins for the bus connection
    void allocatePins();
    // Initialize the bus connection
    void initBus();
    // Read telegram from bus and adjust light if necessary
    void updateFromBus();
    // KNX invidual addresses should be in the format X.Y.Z, with X and Y 0-15 and Z 0-255
    bool validateAddress(const String& address);
    // Group addresses can be in 3-level X/Y/Z (0-31/0-7/0-255), 2-level X/Z (0-31/0-2047) or free style Z (0-65535)
    bool validateGroup(const String& address);
    // Does the telegram source group match the target group
    bool isTargetGroup(const KnxTelegram telegram, const String& target);

  public:

    inline void enable(bool enable) { enabled = enable; }

    inline bool isEnabled() { return enabled; }

    void setup() {
      if (isEnabled()) {
        initBus();
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

      JsonObject pins = top.createNestedObject(F("Serial Pins"));
      pins[FPSTR(_txPin)] = txIo;
      pins[FPSTR(_rxPin)] = rxIo;

      JsonObject indivAddr = top.createNestedObject(F("Individual Address"));
      indivAddr[FPSTR(_address)] = validateAddress(individualAddress) ? individualAddress : "INVALID ADDRESS";

      JsonObject swGroups = top.createNestedObject(F("Switch Groups"));
      swGroups[FPSTR(_enabled)] = enableSwitch;
      swGroups[FPSTR(_group)] = validateGroup(switchGroup) ? switchGroup : "INVALID GROUP";
      swGroups[FPSTR(_state)] = validateGroup(switchStateGroup) ? switchStateGroup : "INVALID GROUP";

      JsonObject absDimGroups = top.createNestedObject(F("Absolute Dim Groups"));
      absDimGroups[FPSTR(_enabled)] = enableAbsoluteDim;
      absDimGroups[FPSTR(_group)] = validateGroup(absoluteDimGroup) ? absoluteDimGroup : "INVALID GROUP";
      absDimGroups[FPSTR(_state)] = validateGroup(absoluteDimStateGroup) ? absoluteDimStateGroup : "INVALID GROUP";
      
    }

    bool readFromConfig(JsonObject& root)
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();
      configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);

      JsonObject pins = top[F("Serial Pins")];
      configComplete = !pins.isNull();
      configComplete &= getJsonValue(pins[FPSTR(_rxPin)], rxIo, -1);
      configComplete &= getJsonValue(pins[FPSTR(_txPin)], txIo, -1);

      JsonObject indivAddr = top[F("Individual Address")];
      configComplete = !indivAddr.isNull();
      configComplete &= getJsonValue(indivAddr[FPSTR(_address)], individualAddress, "0.0.0");

      JsonObject swGroups = top[F("Switch Groups")];
      configComplete = !swGroups.isNull();      
      configComplete &= getJsonValue(swGroups[FPSTR(_enabled)], enableSwitch, false);
      configComplete &= getJsonValue(swGroups[FPSTR(_group)], switchGroup, "0/0/0");
      configComplete &= getJsonValue(swGroups[FPSTR(_state)], switchStateGroup, "0/0/0");

      JsonObject absDimGroups = top[F("Absolute Dim Groups")];
      configComplete = !absDimGroups.isNull();      
      configComplete &= getJsonValue(absDimGroups[FPSTR(_enabled)], enableAbsoluteDim, false);
      configComplete &= getJsonValue(absDimGroups[FPSTR(_group)], absoluteDimGroup, "0/0/0");
      configComplete &= getJsonValue(absDimGroups[FPSTR(_state)], absoluteDimStateGroup, "0/0/0");

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

void KnxUsermod::allocatePins() {
  PinManagerPinType pins[2] = { { txIo, true }, { rxIo, false } };
  if (!pinManager.allocateMultiplePins(pins, 2, PinOwner::UM_KNX)) {
    txIo = -1;
    rxIo = -1;
    return;
  }
}

void KnxUsermod::initBus() {
  if (individualAddress) {
    allocatePins();
    knxPtr = std::unique_ptr<KnxTpUart>(new KnxTpUart(&Serial1, individualAddress));
    if (knxPtr) {
      if (switchGroup != "0/0/0") {
        knxPtr->addListenGroupAddress(switchGroup);
      }
      if (absoluteDimGroup != "0/0/0") {
        knxPtr->addListenGroupAddress(absoluteDimGroup);            
      }
    }

    Serial1.begin(19200, SERIAL_8E1, rxIo, txIo);
  }
}

void KnxUsermod::updateFromBus() {
  KnxTpUartSerialEventType eType = knxPtr->serialEvent();

  if (eType == KNX_TELEGRAM)
  {
    KnxTelegram* telegram = knxPtr->getReceivedTelegram();

    // Switch group
    if (isTargetGroup(*telegram, switchGroup)) {
      if (telegram->getBool()) {
        if (bri == 0) {
          bri = briLast;
          updateInterfaces(CALL_MODE_BUTTON);
        }
      }
      else {
        if (bri != 0) {
          bri = 0;
          updateInterfaces(CALL_MODE_BUTTON);
        }
      }
    }
    // Absolute Dim Group
    if (isTargetGroup(*telegram, absoluteDimGroup)) {
      if (telegram->get1ByteIntValue()) {
        bri = telegram->get1ByteIntValue();
        stateUpdated(CALL_MODE_DIRECT_CHANGE);
      }
    }
  }
}

bool KnxUsermod::isTargetGroup(KnxTelegram telegram, const String& target) {
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
  int area, line, device;
  int members = std::sscanf(address.c_str(), "%i.%i.%i", &area, &line, &device);

  if (members == 3) {
    // Devices should never have 0 as their "device" member
    if ((0 <= area) && (area <= 15) && (0 <= line) && (line <= 15) && (1 <= device) && (device <= 255)) {
      validAddress = true;
    }
  }

  return validAddress;
}

bool KnxUsermod::validateGroup(const String& address) {
  bool validAddress = false;
  int first, second, third;

  int members = std::sscanf(address.c_str(), "%i/%i/%i", &first, &second, &third);


  if ((first + second + third) != 0) {
    // 3-level structure
    if (members == 3) {
      if ((0 <= first) && (first <= 31) && (0 <= second) && (second <= 7) && (0 <= third) && (third <= 255)) {
        validAddress = true;
      }
    }
    // 2-level structure
    else if (members == 2) {
      if ((0 <= first) && (first <= 31) && (0 <= second) && (second <= 2047)) {
        validAddress = true;
      }
    }
    // free structure
    else if (members == 1) {
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
const char KnxUsermod::_group[]    PROGMEM = "group:";
const char KnxUsermod::_state[] PROGMEM = "state:";
const char KnxUsermod::_txPin[] PROGMEM = "TX-pin:";
const char KnxUsermod::_rxPin[] PROGMEM = "RX-pin:";
