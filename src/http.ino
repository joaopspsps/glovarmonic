#include <ESPAsyncWebServer.h>

#include "fs.hpp"
#include "http.hpp"
#include "wifi.hpp"

namespace http {

void serveStatic(AsyncWebServer *server) {
    // Font files
    server->serveStatic("/static/font", LittleFS, "/font");

    // Font files
    server->serveStatic("/static/images", LittleFS, "/images");

    // Javascript files
    server->on("/static/js/iro.min.js", HTTP_GET,
               [](AsyncWebServerRequest *request) {
                   request->redirect("/static/js/iro.min.js.gz");
               });
    server->on("/static/js/iro.min.js.gz", HTTP_GET,
               [](AsyncWebServerRequest *request) {
                   AsyncWebServerResponse *response =
                       request->beginResponse(LittleFS, "/js/iro.min.js.gz",
                                              F("application/javascript"));
                   response->addHeader(F("Content-Encoding"), F("gzip"));
                   request->send(response);
               });
}

namespace endpoints {
    void get404(AsyncWebServerRequest *request) { request->send(404); }

    void getHeap(AsyncWebServerRequest *request) {
        request->send(200, F("text/plain"), String(ESP.getFreeHeap()));
    }

    void getConnectWifi(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(
            LittleFS, "/html/connect_wifi.html.gz", F("text/html"));
        response->addHeader(F("Content-Encoding"), F("gzip"));
        request->send(response);
    }

    void postConnectWifi(AsyncWebServerRequest *request) {
        String ssid, psk;
        bool gotSsid = false, gotPsk = false;

        auto n_params = request->params();
        for (int i = 0; i < n_params; ++i) {
            const auto p = request->getParam(i);
            if (p->name() == "ssid") {
                ssid = p->value();
                gotSsid = true;
            } else if (p->name() == "psk") {
                psk = p->value();
                gotPsk = true;
            }
        }

        if (not gotSsid or not gotPsk) {
            request->send(400, F("text/plain"),
                          "Could not get ssid and/or psk values from the form");
            return;
        }

        if (not wifi::saveCredentials(ssid.c_str(), psk.c_str())) {
            request->send(500, F("text/plain"),
                          "Could not save WiFi credentials to filesystem");
            return;
        }

        request->send(200, F("text/plain"),
                      "WiFi credencials saved successfully");
        delay(2000);
        ESP.restart();
    }

    void getGlovarmonic(AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(
            LittleFS, "/html/glovarmonic.html.gz", F("text/html"));
        response->addHeader(F("Content-Encoding"), F("gzip"));
        request->send(response);
    }
} // namespace endpoints

} // namespace http
