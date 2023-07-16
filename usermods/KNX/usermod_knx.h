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

    String individualAddress;

    static const char _name[];
    static const char _enabled[];

    // int splitAddress(const char *const stringAddress, int * const arrayAddress)
    // {
    //   char *color_ = NULL;
    //   const char delim[] = ".";
    //   char *cxt = NULL;
    //   char *token = NULL;
    //   int position = 0;

    //   // We need to copy the string in order to keep it read only as strtok_r function requires mutable string
    //   color_ = (char *)malloc(strlen(color));
    //   if (NULL == color_) {
    //     return -1;
    //   }

    //   strcpy(color_, color);
    //   token = strtok_r(color_, delim, &cxt);

    //   while (token != NULL)
    //   {
    //     rgb[position++] = (int)strtoul(token, NULL, 10);
    //     token = strtok_r(NULL, delim, &cxt);
    //   }
    //   free(color_);

    //   return position;
    // }

  public:

    inline void enable(bool enable) { enabled = enable; }

    inline bool isEnabled() { return enabled; }

    void setup() {
      Serial.println("Hello from KNX!");
      initDone = true;
    }

    void connected() {
    }

    void loop() {
    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR(_enabled)] = enabled;
      //save these vars persistently whenever settings are saved
      top["individualAddress"] = individualAddress;
    }

    bool readFromConfig(JsonObject& root)
    {
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();

      configComplete &= getJsonValue(top["individualAddress"], individualAddress, "0.0.0");  

      return configComplete; 
    }

    void addToJsonInfo(JsonObject& root)
    {
      // if "u" object does not exist yet we need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      JsonArray knxArr = user.createNestedArray(FPSTR(_name));
      knxArr.add(F("Individual address: "));
      knxArr.add(individualAddress);
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