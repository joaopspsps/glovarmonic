#pragma once

#ifndef DISABLE_SERIAL

#define SERIAL_INIT()                                                          \
    do {                                                                       \
        Serial.begin(115200);                                                  \
        while (!Serial && Serial.available())                                  \
            delay(50);                                                         \
    } while (0)
#define SERIAL_PRINT(msg) Serial.print(msg)
#define SERIAL_PRINTLN(msg) Serial.println(msg)
#define SERIAL_PRINTF(...) Serial.printf(__VA_ARGS__)

#else

#define SERIAL_INIT()
#define SERIAL_PRINT(msg)
#define SERIAL_PRINTLN(msg)
#define SERIAL_PRINTF(...)

#endif
