# esp-led
Умный свет RGB/LED для ESP32/ESP8266

<a href="https://espressif.github.io/esp-launchpad/?flashConfigURL=https://dra1ex.github.io/esp-launchpad/config.toml">
    <img alt="Попробуйте с ESP Launchpad" src="https://espressif.github.io/esp-launchpad/assets/try_with_launchpad.png" width="180">
</a>

## WebUI

![image](https://github.com/user-attachments/assets/fa4d4f01-f9f8-494a-b6f1-6a08406da38b)

## Возможности
- Поддержка одноцветных и RGB ламп (или светодиодных лент).
- Веб/мобильное приложение (PWA).
- Интеграция с домашними ассистентами (например, Алиса) через MQTT брокеры.
- Веб-API.
- Протокол MQTT.

## Подключение

![image](https://github.com/user-attachments/assets/793dc957-1312-4eb5-bbee-432d5c9db3fc)

Это базовая схема подключения. Вы можете использовать любой пин для подключения (предварительно можно изменить его в `constants.h`, либо позже через WebUI).

Для защиты контроллера рекомендуется подключить резистор на 200 Ом между MOSFET и пином контроллера. Также подключите затвор MOSFET к GND с помощью резистора на 10 кОм, чтобы избежать случайного включения из-за наведенных токов.

Для RGB-подключений настройка аналогична, но понадобятся три отдельных MOSFET для каждого цветового канала.

## Установка

### Веб-установщик

1. Откройте [ESP Launchpad](https://espressif.github.io/esp-launchpad/?flashConfigURL=https://dra1ex.github.io/esp-launchpad/config.toml).
2. Выберите **esp-led** и платформу.

### Ручная сборка / OTA

1. Установите [Platform.IO](https://platformio.org/install).
2. (По желанию) Измените файл `credentials.h` и настройте `constants.h`. Вы можете обновить эти параметры позже через веб-интерфейс.
3. Загрузите файловую систему и прошивку.

**Примечание:** Этот репозиторий содержит git-submodule. Используйте опцию `--recursive` при клонировании.

```bash
git clone --recursive https://github.com/DrA1ex/esp_led.git
cd esp_led

# Сделайте скрипт исполняемым
chmod +x ./upload_fs.sh

# Укажите платформу: esp32-c3 или esp8266
PLATFORM=esp32-c3

# Установите окружение: debug, release или ota
ENV=release

# Для OTA: укажите адрес вашего ESP
ADDRESS=esp_led.local

# Дополнительные переменные окружения, если включен OTA
if [ "$ENV" = "ota" ]; then OTA=1 else OTA=0 ADDRESS= fi

pio run -t upload -e $PLATFORM-$ENV --upload-port "$ADDRESS"
./upload_fs.sh --upload-port "$ADDRESS"
```

## Веб-API

| Эндпоинт             | Метод    | Параметры               | Ответ                                                  | Описание                                             |
|----------------------|-----------|--------------------------|-------------------------------------------------------|-----------------------------------------------------|
| `/api/status`        | `GET`     | Нет                      | `{"status": "ok", "value": number, "brightness": number}` | Получает текущие значения питания и яркости.      |
| `/api/power`         | `GET`     | `value` (1 или 0)       | `{"status": "ok"}`                                    | Устанавливает состояние питания (включено/выключено).|
| `/api/brightness`    | `GET`     | `value` (0-100)         | `{"status": "ok"}`                                    | Обновляет уровень яркости.                           |
| `/api/debug`         | `GET`     | Нет                      | Простой текст                                          | Предоставляет отладочную информацию.                 |
| `/api/restart`       | `GET`     | Нет                      | Простой текст: "OK"                                   | Перезапускает сервер и сохраняет конфигурацию.      |

## Протокол MQTT

| Топик Команд *              | Топик Уведомлений *            | Тип         | Значения              | Комментарии                          |
|------------------------|---------------------------|-------------|-----------------------|--------------------------------------|
| `MQTT_TOPIC_POWER`     | `MQTT_OUT_TOPIC_POWER`    | `uint8_t`   | 0..1                  | Состояние питания: ВКЛ (1) / ВЫКЛ (0) |
| `MQTT_TOPIC_BRIGHTNESS`| `MQTT_OUT_TOPIC_BRIGHTNESS`| `uint16_t`  | 0..`PWM_MAX_VALUE`    | Уровень яркости, можно переключать на диапазон 0..100 (`MQTT_CONVERT_BRIGHTNESS`) |
| `MQTT_TOPIC_COLOR`     | `MQTT_OUT_TOPIC_COLOR`    | `uint32_t`  | 0..0xFFFFFF           | Значение цвета (формат RGB) |
| `MQTT_TOPIC_NIGHT_MODE`| `MQTT_OUT_TOPIC_NIGHT_MODE`| `uint8_t`   | 0..1                  | Состояние ночного режима: ВКЛ (1) / ВЫКЛ (0) |

\* Актуальные значения топиков определены в `constants.h`.

## Разное

### Настройка Secure WebSocket-прокси с Nginx

Если вы хостите веб-интерфейс где-то еще (например используете [GitHub](https://dra1ex.github.io/esp_led/), используя SSL, вам нужно настроить Secure WebSocket (`wss://...`) сервер вместо обычного `ws://` от ESP. 
Браузеры потребуют безопасное соединение для работы WebSocket, если сама страница загружается по https, поэтому эта настройка обязательна.

Для этого вы можете использовать Nginx в качестве прокси для создания соединения WebSocket с поддержкой SSL.

#### Шаг 0: Установите Nginx

```sh
apt install nginx
```

#### Шаг 1: Создайте конфигурацию Nginx

Создайте файл `/etc/nginx/conf.d/ws.locations` и добавьте следующий контент:

```nginx
location /w_esp_led/ws {
    proxy_pass http://<YOUR-ESP-IP-HERE_1>/ws; # Замените на фактический адрес вашего сервиса
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection keep-alive;
    proxy_set_header Host $host;
}

# Вы можете создать прокси для нескольких хостов
location /w_esp_led_2/ws {
    proxy_pass http://<YOUR-ESP-IP-HERE_2>/ws; # Замените на фактический адрес вашего сервиса
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection keep-alive;
    proxy_set_header Host $host;
}
```

#### Шаг 2: Перезагрузите конфигурацию Nginx

После сохранения конфигурационного файла перезагрузите Nginx, чтобы применить изменения:

```sh
nginx -s reload
```

**Примечание**: На забудьте заменить `<YOUR-ESP-IP-HERE_1>` и `<YOUR-ESP-IP-HERE_2>` на фактические IP-адреса ваших ESP-устройств.
                                          
#### Шаг 3: Проверьте результат

Откройте WebUI в браузере: https://dra1ex.github.io/esp_led/?host=ADDRESS/w_esp_led
