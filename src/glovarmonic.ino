#include <ReactESP.h>

#include "peripherals.hpp"
#include "serial.hpp"
#include "fs.hpp"
#include "wifi.hpp"
#include "http.hpp"
#include "ws.hpp"

// Remember the last button state to detect HIGH->LOW and LOW->HIGH transitions
uint8_t stateButtonIndexLast = HIGH, stateButtonMiddleLast = HIGH,
        stateButtonRingLast = HIGH;

reactesp::EventLoop eventLoop;

uint32_t buttonGestureStartTime;

// Helper functions
// ================

void hang_forever() {
    for (;;)
        delay(1000);
}

String getGlovarmonicId() {
    const uint64_t mac = ESP.getEfuseMac();
    const auto macLower3Bytes = static_cast<uint32_t>(mac & 0x0000000000ffffff);
    auto glovarmonicId = String(macLower3Bytes, HEX);
    glovarmonicId.toUpperCase();
    return glovarmonicId;
}

void blinkLed(uint32_t ms) {
    const auto stateLedBuiltin = digitalRead(PIN_LED_BUILTIN);

    digitalWrite(PIN_LED_BUILTIN, not stateLedBuiltin);
    delay(ms);
    digitalWrite(PIN_LED_BUILTIN, stateLedBuiltin);
}

void buttonActivated(const peripherals::RGB &rgb, const char *code) {
    rgb.setColor();

    SERIAL_PRINTF("[I] Button %s activated\n", code);
    ws::server.textAll(code);

    blinkLed(75); // Also works as button debounce
}

// Setup / Loop
// ============

void setup() {
    SERIAL_INIT();

    peripherals::setupPins();

    // Setup filesystem
    // ================

    if (not fs::setupFs()) {
        SERIAL_PRINTLN("[E] Could not initialize filesystem");
        hang_forever();
    }

    // Setup wifi connection
    // =====================

    const auto glovarmonicId = getGlovarmonicId();

    SERIAL_PRINT("[I] glovarmonicId: ");
    SERIAL_PRINTLN(glovarmonicId);

    wifi::setupWifi(glovarmonicId);

    // Setup servers
    // =============

    // Endpoints
    http::server.onNotFound(http::endpoints::get404);
    http::server.on("/heap", HTTP_GET, http::endpoints::getHeap);
    http::server.on("/connect_wifi", HTTP_GET, http::endpoints::getConnectWifi);
    http::server.on("/connect_wifi", HTTP_POST,
                    http::endpoints::postConnectWifi);
    http::server.on("/glovarmonic", HTTP_GET, http::endpoints::getGlovarmonic);

    // Redirect root URL based on wifi mode
    if (WiFi.getMode() == WIFI_AP)
        http::server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
            request->redirect("/connect_wifi");
        });
    else
        http::server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
            request->redirect("/glovarmonic");
        });

    // Static files
    http::serveStatic(&http::server);

    // WebSockets server
    ws::server.onEvent(ws::handleEvent);
    http::server.addHandler(&ws::server);

    // http::server.beginSecure(http::tlsCert, http::tlsKey, nullptr);
    http::server.begin();

    SERIAL_PRINTF("[I] HTTP server running at port %d\n", http::port);
    SERIAL_PRINTF("[I] WebSockets server running at %s\n", ws::server.url());

    // Setup reactesp event loop
    // =========================

    eventLoop.onRepeat(5000, []() { ws::server.cleanupClients(); });
}

void loop() {
    eventLoop.tick();

    const auto stateButtonIndex = digitalRead(PIN_BUTTON_INDEX),
               stateButtonMiddle = digitalRead(PIN_BUTTON_MIDDLE),
               stateButtonRing = digitalRead(PIN_BUTTON_RING);

    if (stateButtonIndex != stateButtonIndexLast or
        stateButtonMiddle != stateButtonMiddleLast or
        stateButtonRing != stateButtonRingLast) {
        // Some button was pressed or released
        if (stateButtonIndex != stateButtonIndexLast and
            stateButtonIndex == HIGH) {
            buttonActivated(peripherals::rgbButtonIndex, "1");
        } else if (stateButtonMiddle != stateButtonMiddleLast and
                   stateButtonMiddle == HIGH) {
            buttonActivated(peripherals::rgbButtonMiddle, "2");
        } else if (stateButtonRing != stateButtonRingLast and
                   stateButtonRing == HIGH) {
            buttonActivated(peripherals::rgbButtonRing, "3");
        } else if ((stateButtonIndex == LOW and stateButtonMiddle == LOW and
                    stateButtonRing == LOW) or
                   (stateButtonIndex == LOW and stateButtonRing == LOW) or
                   (stateButtonMiddle == LOW and stateButtonRing == LOW)) {
            // Button gesture started
            buttonGestureStartTime = millis();
        }

        stateButtonIndexLast = stateButtonIndex;
        stateButtonMiddleLast = stateButtonMiddle;
        stateButtonRingLast = stateButtonRing;
    } else {
        if (stateButtonIndex == LOW and stateButtonMiddle == LOW and
            stateButtonRing == LOW) {
            blinkLed(50);
            if (millis() - buttonGestureStartTime > 3000) {
                // Remove wifi credentials and restart
                fs::remove_("/wifi_ssid");
                fs::remove_("/wifi_psk");

                // Turn off LEDs
                digitalWrite(PIN_LED_BUILTIN, LED_BUILTIN_OFF);
                peripherals::RGB(0, 0, 0).setColor();
                delay(2000);

                ESP.restart();
            }
        } else if (stateButtonIndex == LOW and stateButtonRing == LOW) {
            blinkLed(50);
            if (millis() - buttonGestureStartTime > 3000) {
                // Turn off RGB LED
                peripherals::RGB(0, 0, 0).setColor();
                delay(2000);

                stateButtonIndexLast = HIGH;
                stateButtonRingLast = HIGH;
            }
        } else if (stateButtonMiddle == LOW and stateButtonRing == LOW) {
            blinkLed(50);
            if (millis() - buttonGestureStartTime > 3000) {
                // Enter deep sleep
                esp_deep_sleep_start();
            }
        }
    }

    delay(50);
}
