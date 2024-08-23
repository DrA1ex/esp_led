# esp-led
Smart LED Control for ESP32/ESP8266

![esp_led local_(iPhone XR)](https://github.com/user-attachments/assets/ed7e8e77-52eb-44dc-898a-b081195702f2)

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
