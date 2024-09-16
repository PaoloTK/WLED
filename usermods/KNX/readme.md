# KNX

This usermod allows you to control a WLED light from the KNX bus. For now, only KNX TP is supported, but I plan to add support for KNX IP in the future.

## Requirements
- WLED device (so far I have only tested with ESP32).
- KNX Twisted Pair install.
- KNX to TP-UART adapter, I use the Siemens 5WG1117-2AB12.
- If you use the Siemens device, you will also need a bidirectional level shifter since ESP logic works at 3.3v and Siemens at 5v.

## Wiring
This is the wiring I'm using with the hardware mentioned above.

![circuit](https://github.com/PaoloTK/WLED/assets/60204407/c07dd732-eac8-4b03-920f-80d96062edf0)


## Usermod installation
Download my fork and make sure basic ESP32 compiling works. Then use the following platform_override.ini:

```[platformio]
default_envs = custom_esp32dev_knx
[env:custom_esp32dev_knx]
extends = env:esp32dev
lib_compat_mode = soft
lib_deps = ${esp32.lib_deps}
   https://github.com/PaoloTK/arduino-tpuart-knx-user-forum
build_flags = ${common.build_flags_esp32} -D WLED_RELEASE_NAME=ESP32 -D USERMOD_KNX
```

# Configuration
Head over to the usermods settings page and you'll find a KNX section. Enable it with the checkbox below, then configure it based on your needs:
- the TX and RX pins should be set to what you have connected the IO wires coming from the level shifter to. Remember to invert direction (TX out of the ESP goes into RX of the Siemens and viceversa)
- Individual address will be used to send telegrams on the bus. Choose something that's unused on the same line as your other devices.
- If you want your WLED light on/off state to be controlled from the bus, enable the Switch Groups checkbox and fill the group and state fields. The group field is the one you add your light switches on/off functions to, the state field is used to let the bus know the state of the WLED light so that even if you change its state from WLED GUI the bus is still in sync.
- If you want your WLED light brightness to be controlled from the bus absolutely, enable the Absolute Dim Groups checkbox and fill the group and state fields.
- If you want your WLED light brightness to be controlled from the bus relatively, enable the Relative Dim Groups checkout and fill the group and time field. The time field takes a value in millisecond for how long it should take to change the light brightness from 1% to 100%.
- When you're done, be sure to check the "reboot after save?" checkbox and hit save. Your WLED device should start communicated over KNX.

## ToDo
- Implement all JSON parameters
- Add cleanup in case mod gets disabled
- Update configuration if parameters change without restart
- Add option to turn on/off light via dimming
- Replace 0/0/0 with _invalidgroup# KNX

This usermod allows you to control a WLED light from the KNX bus. For now, only KNX TP is supported, but I plan to add support for KNX IP in the future.

## Requirements
- WLED device (so far I have only tested with ESP32).
- KNX Twisted Pair install.
- KNX to TP-UART adapter, I use the Siemens 5WG1117-2AB12.
- If you use the Siemens device, you will also need a bidirectional level shifter since ESP logic works at 3.3v and Siemens at 5v.

## Wiring
This is the wiring I'm using with the hardware mentioned above.

![circuit](https://github.com/PaoloTK/WLED/assets/60204407/c07dd732-eac8-4b03-920f-80d96062edf0)


## Usermod installation
Download my fork and make sure basic ESP32 compiling works. Then use the following platform_override.ini:

```[platformio]
default_envs = custom_esp32dev_knx
[env:custom_esp32dev_knx]
extends = env:esp32dev
lib_compat_mode = soft
lib_deps = ${esp32.lib_deps}
   https://github.com/PaoloTK/arduino-tpuart-knx-user-forum
build_flags = ${common.build_flags_esp32} -D WLED_RELEASE_NAME=ESP32 -D USERMOD_KNX
```

# Configuration
Head over to the usermods settings page and you'll find a KNX section. Enable it with the checkbox below, then configure it based on your needs:
- the TX and RX pins should be set to what you have connected the IO wires coming from the level shifter to. Remember to invert direction (TX out of the ESP goes into RX of the Siemens and viceversa)
- Individual address will be used to send telegrams on the bus. Choose something that's unused on the same line as your other devices.
- If you want your WLED light on/off state to be controlled from the bus, enable the Switch Groups checkbox and fill the group and state fields. The group field is the one you add your light switches on/off functions to, the state field is used to let the bus know the state of the WLED light so that even if you change its state from WLED GUI the bus is still in sync.
- If you want your WLED light brightness to be controlled from the bus absolutely, enable the Absolute Dim Groups checkbox and fill the group and state fields.
- If you want your WLED light brightness to be controlled from the bus relatively, enable the Relative Dim Groups checkout and fill the group and time field. The time field takes a value in millisecond for how long it should take to change the light brightness from 1% to 100%.
- When you're done, be sure to check the "reboot after save?" checkbox and hit save. Your WLED device should start communicated over KNX.

## ToDo
- MQTT if (WLED_MQTT_CONNECTED)
- Temperature report over KNX