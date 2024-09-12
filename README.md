# esp-led
Smart RGB/LED Control for ESP32/ESP8266

Other: [[RGB LED LAMP](https://github.com/DrA1ex/esp_led_lamp)] [[RELAY](https://github.com/DrA1ex/esp_relay)]

<a href="https://espressif.github.io/esp-launchpad/?flashConfigURL=https://dra1ex.github.io/esp-launchpad/config.toml">
    <img alt="Try it with ESP Launchpad" src="https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png" width="180">
</a>

Инструкция на [русском](/README_RU.md).

## UI

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

To safeguard your controller, I recommend connecting a 200-ohm resistor between the MOSFET and the controller pin. Additionally, connect the MOSFET's gate to GND using a 10k-ohm resistor to prevent unwanted activation due to induced currents.

For RGB connections, the setup is similar, but you will need three separate MOSFETs for each color channel.

## Installation

### Web Installer

1. Open the [ESP Launchpad](https://espressif.github.io/esp-launchpad/?flashConfigURL=https://dra1ex.github.io/esp-launchpad/config.toml).
2. Select **esp-led** and your desired platform.

### Manual Build / OTA


1. Install [Platform.IO](https://platformio.org/install):
2. (Optional) Modify the `credentials.h` file and, if necessary, customize `constants.h`. You can change these parameters later through the Web UI.
3. Upload filesystem and firmware

**Note:** This repository contains a submodule. Please use the `--recursive` option when cloning.

```bash
git clone --recursive https://github.com/DrA1ex/esp_led.git
cd esp_led

# Make script executable
chmod +x ./upload_fs.sh

# Specify the platform: esp32-c3 or esp8266
PLATFORM=esp32-c3

# Set the environment: debug, release, or ota
ENV=release

# For OTA: set your ESP's address
ADDRESS=esp_led.local

# Additional envs if OTA enabled
if [ "$ENV" = "ota" ]; then OTA=1 else OTA=0 ADDRESS= fi

pio run -t upload -e $PLATFORM-$ENV --upload-port "$ADDRESS"
./upload_fs.sh --upload-port "$ADDRESS"
```

## Web API

| Endpoint             | Method    | Parameters               | Response                                                  | Description                                             |
|----------------------|-----------|--------------------------|-----------------------------------------------------------|---------------------------------------------------------|
| `/api/status`        | `GET`     | None                     | `{"status": "ok", "value": number, "brightness": number}` | Retrieves the current power and brightness values.      |
| `/api/power`         | `GET`     | `value` (1 or 0)         | {"status": "ok"}                                          | Sets the power state (on/off).                          |
| `/api/brightness`    | `GET`     | `value` (0-100)          | {"status": "ok"}                                          | Updates the brightness level.                           |
| `/api/debug`         | `GET`     | None                     | Plain Text                                                | Provides debugging information.                         |
| `/api/restart`       | `GET`     | None                     | Plain Text: "OK"                                          | Restarts the server and saves configuration.            |


## MQTT Protocol

| Topic In *       			| Topic Out *          			| Type        | Values		         | Comments                              |
|---------------------------|-------------------------------|-------------|----------------------|---------------------------------------|
| `MQTT_TOPIC_POWER`		| `MQTT_OUT_TOPIC_POWER` 		| `uint8_t`   | 0..1      	     	 | Power state: ON (1) / OFF (0)         |
| `MQTT_TOPIC_BRIGHTNESS`	| `MQTT_OUT_TOPIC_BRIGHTNESS` 	| `uint16_t`  | 0..`DAC_MAX_VALUE`   | Brightness level, can switch to 0..100 (`MQTT_CONVERT_BRIGHTNESS`) 	|
| `MQTT_TOPIC_COLOR`		| `MQTT_OUT_TOPIC_COLOR` 		| `uint32_t`  | 0..0xFFFFFF  		 | Color value (ARGB or RGB format)      |
| `MQTT_TOPIC_NIGHT_MODE`	| `MQTT_OUT_TOPIC_NIGHT_MODE` 	| `uint8_t`   | 0..1          		 | Night mode state: ON (1) / OFF (0)    |

\* Actual topic values decalred in `constants.h`


## Misc

### Configuring a Secure WebSocket Proxy with Nginx

If you're hosting a Web UI that uses SSL, you'll need to set up a Secure WebSocket (`wss://...`) server instead of the non-secure `ws://` provided by your ESP. Browsers require secure socket connections for WebSocket functionality, so this configuration is essential.

To achieve this, you can use Nginx as a proxy to create an SSL-enabled WebSocket connection.

#### Step 0: Install Nginx

```sh
apt install nginx
```

#### Step 1: Create the Nginx Configuration

Create a file at `/etc/nginx/conf.d/ws.locations` and add the following content:

```nginx
location /w_esp_led/ws {
    proxy_pass http://<YOUR-ESP-IP-HERE_1>/ws; # Replace with your actual service address
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection keep-alive;
    proxy_set_header Host $host;
}

# You can create proxy for multiple hosts
location /w_esp_led_2/ws {
    proxy_pass http://<YOUR-ESP-IP-HERE_2>/ws; # Replace with your actual service address
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection keep-alive;
    proxy_set_header Host $host;
}
```

#### Step 2: Reload the Nginx Configuration

After saving the configuration file, reload Nginx to apply the changes:

```sh
nginx -s reload
```

**Note**
Make sure to replace `<YOUR-ESP-IP-HERE_1>` and `<YOUR-ESP-IP-HERE_2>` with the actual IP addresses of your ESP devices.

#### Step 3: Check result

Open WebUi in browser https://dra1ex.github.io/esp_led/?host=ADDRESS/w_esp_led
