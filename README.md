# esp-led
Smart RGB/LED Control for ESP32/ESP8266

![image](https://github.com/user-attachments/assets/fa4d4f01-f9f8-494a-b6f1-6a08406da38b)

## Features
- Support for both White and RGB Lamp (or LED-Strip)
- Web/Mobile Application (PWA)
- Integration with any Smart Home Assistant (such as Alise) via MQTT broker
- Web Hooks
- MQTT Protocol

## Connection

![image](https://github.com/user-attachments/assets/793dc957-1312-4eb5-bbee-432d5c9db3fc)

This is a basic connection scheme. You can use any pin for the connection (just change it in `constants.h`).

I recommend connecting the MOSFET to the controller using a 200-ohm resistor to protect the controller pin. Additionally, connect the MOSFET's gate to GND with a 10k-ohm resistor to prevent unwanted activation from induced current.

## Installation

- Install [Platform.IO](https://platformio.org/install):
- Modify `credentials.h` and optionally `constants.h`
- Upload filesystem and firmware

```bash
git clone https://github.com/DrA1ex/esp_led.git
cd esp_led

chmod +x ./upload_fs.sh

# For ESP32-C3
pio run -t upload -e esp32-c3-release
PLATFORM=esp32-c3 ./upload_fs.sh

# For ESP8266
pio run -t upload -e esp8266-release
PLATFORM=esp8266 ./upload_fs.sh
```
