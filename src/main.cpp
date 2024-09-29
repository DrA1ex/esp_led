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

#ifdef ARDUINO_ARCH_ESP8266
    analogWriteFreq(std::min<uint32_t>(PWM_FREQUENCY, PWM_MAX_FREQUENCY));
#else
    analogWriteFrequency(std::min<uint32_t>(PWM_FREQUENCY, PWM_MAX_FREQUENCY));
#endif

    ApplicationInstance.begin();
}

void loop() {
    ApplicationInstance.event_loop();
}
