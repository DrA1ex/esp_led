#include <LittleFS.h>

#include "app/application.h"

Application ApplicationInstance;

void setup() {
#ifdef DEBUG
#ifdef ARDUINO_ARCH_ESP32
    Serial.begin(115200);
#else
    Serial.begin(74880);
#endif

    {
        auto start_t = millis();
        while (!Serial && (millis() - start_t) < 15000ul) delay(1);
    }

    delay(2000);
#endif

    analogWriteResolution(PWM_RESOLUTION);

    ApplicationInstance.begin();
}

void loop() {
    ApplicationInstance.event_loop();
}
