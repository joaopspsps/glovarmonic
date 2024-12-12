#pragma once

// The builtin LED on some boards have the HIGH/LOW states swapped.
#if defined(ARDUINO_ESP32_C3_DEVKITM_1) or defined(ARDUINO_LOLIN32_LITE)
#    define LED_BUILTIN_ON LOW
#    define LED_BUILTIN_OFF HIGH
#else
#    define LED_BUILTIN_ON HIGH
#    define LED_BUILTIN_OFF LOW
#endif

namespace peripherals {

void setupPins();

class RGB {
  public:
    uint8_t r, g, b;

    RGB(uint8_t r, uint8_t g, uint8_t b);

    void setColor() const;
};

// Default colors for the buttons
RGB rgbButtonIndex(0xff, 0x00, 0x00), rgbButtonMiddle(0x00, 0xff, 0x00),
    rgbButtonRing(0x00, 0x00, 0xff);

} // namespace peripherals
