#include "peripherals.hpp"

namespace peripherals {

void setupPins() {
    // Buttons
    // =======

    pinMode(PIN_BUTTON_INDEX, INPUT_PULLUP);
    pinMode(PIN_BUTTON_MIDDLE, INPUT_PULLUP);
    pinMode(PIN_BUTTON_RING, INPUT_PULLUP);

    digitalWrite(PIN_BUTTON_INDEX, HIGH);
    digitalWrite(PIN_BUTTON_MIDDLE, HIGH);
    digitalWrite(PIN_BUTTON_RING, HIGH);

    // RGB LED
    // =======

    pinMode(PIN_LED_RGB_R, OUTPUT);
    pinMode(PIN_LED_RGB_G, OUTPUT);
    pinMode(PIN_LED_RGB_B, OUTPUT);

    analogWrite(PIN_LED_RGB_R, 0);
    analogWrite(PIN_LED_RGB_G, 0);
    analogWrite(PIN_LED_RGB_B, 0);

    // Builtin LED
    // ===========

    pinMode(PIN_LED_BUILTIN, OUTPUT);

    digitalWrite(PIN_LED_BUILTIN, LED_BUILTIN_OFF);
}

// RGB
// ===

RGB::RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

void RGB::setColor() const {
    analogWrite(PIN_LED_RGB_R, r);
    analogWrite(PIN_LED_RGB_G, g);
    analogWrite(PIN_LED_RGB_B, b);
}

} // namespace peripherals
