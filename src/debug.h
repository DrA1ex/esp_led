#pragma once

#define __DEBUG_LEVEL_VERBOSE 0

#ifdef DEBUG

#include <Arduino.h>

#define D_PRINT(x) Serial.println(x)
#define D_PRINTF(...) Serial.printf(__VA_ARGS__)
#define D_WRITE(x) Serial.print(x)

#define D_PRINT_HEX(ptr, length)                        \
        D_WRITE("HEX: ");                               \
        for (unsigned int i = 0; i < length; ++i) {     \
            D_PRINTF("%02X ", ptr[i]);                  \
        }                                               \
        D_PRINT()

#if DEBUG_LEVEL == __DEBUG_LEVEL_VERBOSE
#define VERBOSE(ARG) ARG
#else
#define VERBOSE(ARG)
#endif

#else
#define D_PRINT(x)
#define D_PRINTF(...)
#define D_WRITE(x)
#define D_PRINT_HEX(ptr, length)

#define VERBOSE(ARG)
#endif