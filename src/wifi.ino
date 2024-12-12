#include <ESPmDNS.h>
#include <WiFi.h>

#include "fs.hpp"
#include "peripherals.hpp"
#include "serial.hpp"

namespace wifi {

bool saveCredentials(const char *ssid, const char *psk) {
    return fs::writeQuick(filenameSsid, ssid) and
           fs::writeQuick(filenamePsk, psk);
}

bool retrieveCredentials(String *ssid, String *psk) {
    return fs::readQuick(filenameSsid, ssid) and
           fs::readQuick(filenamePsk, psk);
}

void setupWifi(const String &glovarmonicId) {
    String ssid, psk;

    if (::wifi::retrieveCredentials(&ssid, &psk) and
        ::wifi::connectSTA(glovarmonicId, ssid.c_str(), psk.c_str()))
        return;

    ::wifi::startAP(glovarmonicId);
}

bool connectSTA(const String &glovarmonicId, const char *ssid,
                const char *psk) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, psk);

    // Blink builtin LED while waiting for connection
    uint8_t stateLedBuiltin = LED_BUILTIN_OFF;
    auto startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startAttemptTime < 10000) {
        digitalWrite(PIN_LED_BUILTIN, not stateLedBuiltin);
        stateLedBuiltin = not stateLedBuiltin;
        delay(200);
    }
    digitalWrite(PIN_LED_BUILTIN, LED_BUILTIN_OFF);

    if (WiFi.status() != WL_CONNECTED) {
        SERIAL_PRINTLN(
            "[E] Could not connect to WiFi station, falling back to AP mode");
        return false;
    }

    SERIAL_PRINTLN("[I] Connected to wifi:");
    SERIAL_PRINTF("[I]     ssid: %s\n", ssid);
    SERIAL_PRINTF("[I]     psk: %s\n", psk);

    // Start mDNS
    // ==========

    const auto hostname = String(hostnamePrefix) + glovarmonicId;
    if (not MDNS.begin(hostname)) {
        SERIAL_PRINTLN("[E] Could not begin mDNS");
        return false;
    }

    SERIAL_PRINTLN("[I] mDNS configured");

    return true;
}

void startAP(const String &glovarmonicId) {
    // Turn on builtin LED when in AP mode
    digitalWrite(PIN_LED_BUILTIN, LED_BUILTIN_ON);

    String apSsid = String(apSsidPrefix) + glovarmonicId;

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid.c_str(), apPsk,
                1, // Channel
                0, // Hidden access point?
                4  // Maximum simultaneous connections
    );

    SERIAL_PRINTLN("[I] WiFi access point started:");
    SERIAL_PRINT("[I]     SSID: ");
    SERIAL_PRINTLN(apSsid);
    SERIAL_PRINT("[I]     Address: ");
    SERIAL_PRINTLN(WiFi.softAPIP());
}

} // namespace wifi
