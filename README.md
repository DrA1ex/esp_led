# esp-led
Smart RGB/LED Control for ESP32/ESP8266

![esp_led local_(iPhone XR)](https://github.com/user-attachments/assets/ed7e8e77-52eb-44dc-898a-b081195702f2)

## Connection

<img width="704" alt="image" src="https://github.com/user-attachments/assets/6d57e383-b652-46b2-81fa-6e31082bcb49">

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
