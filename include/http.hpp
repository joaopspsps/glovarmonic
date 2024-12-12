#pragma once

#include <ESPAsyncWebServer.h>

namespace http {

const int port = 80;

AsyncWebServer server(port);

// Unused
const char tlsCert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIE+zCCAuOgAwIBAgIUQgGcpk/h+fwNogq/vJQqhyIWPPowDQYJKoZIhvcNAQEL\n"
    "BQAwDTELMAkGA1UEBhMCQlIwHhcNMjQxMTIwMTIzMTM0WhcNMjUxMTIwMTIzMTM0\n"
    "WjANMQswCQYDVQQGEwJCUjCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIB\n"
    "AN2x1/OxC6C/wO6ZxKqJVB6zHObTXHHRojCxFkFjiwXdvF5Qao8i36zo1hYe7riJ\n"
    "wbqYmrWo2n+he31N0VprdF5XnBPa06TBybqvaxzbuyCvcOmSjG4d6Pi+7NjTIatR\n"
    "hdRnfrXhjxCWanfbdX6ACT6SVjf/9YHyQrXBA9i0Vyz3Xkfwycf3DFETv07LdNSy\n"
    "pByn6iZONoO/QSvwFL3RpMQaS35wllx0JwRSg6c0O87w5BuZ0XvnC+RBrzbfZOO1\n"
    "x5hkhND2sNuV9gBaqQVLzacnP40BOs6AJ+yWjJJr4w3syXKtaRQ6dFB6M1zlPCkz\n"
    "fGI0wl4/k01xz1V2X2RjUyZGi77mhcdvs7FRqGzFO71oyAjNhd6BHOnb6alro3id\n"
    "3MDNyOQAVvxz6Yb0bE26DKS+GcbuefKqxuKu5ovJV03Jr6fpUKj2RFgfUAW0s/l5\n"
    "4/AVKNKY7iaFMkqlrrJSQH6E1DTh6ZSENYn+w3m2gHvvnFCqmhVRsEaF45Duxm87\n"
    "Y4hFubnFGgg0jnZEUBjlS/rrPGocvVGZrcOhpi+Or+wrJkQUZCPVx1Wt2Qo1vdMk\n"
    "xYE43FXyI4kk1JQ4ZyhF9L+pmhKkMhVVdymEqryYMlX+i0qWQIAIMrpyc0OYtNXf\n"
    "mSnfknuAozypyB6+C/xCrzbLSfV8Z3OtDGICeLwLtLClAgMBAAGjUzBRMB0GA1Ud\n"
    "DgQWBBTJc77SSulWiPtzH89LiuvwfPEaYjAfBgNVHSMEGDAWgBTJc77SSulWiPtz\n"
    "H89LiuvwfPEaYjAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4ICAQCq\n"
    "qK88fI/sjWGp8nEz8RwPjjzd2dSUxaFZZ62pKEiQF0hlPUmgAaJgelGqlbbSLw4F\n"
    "MEiYftrRlc/4aaihzAmwVzJSbcXp6oV5rDpMnfugVgEqWndoAx4Pz0cMIqv61piC\n"
    "EH+Qwywml2FjsYXZwPZl9VuGGhmfq3FhZgiOiB/khbO3He4fkVYomNWNuNtmk/xP\n"
    "DdOH//JoxnJUp3XzkaMRtpltjGwV2b75EsP9gXL4aGTF9qQ23AfuQ6+LBPoZzFJe\n"
    "N09H9E06R4dlKpX/prkdK0M5Eki9+4Ge7KY5Tvke7Rkx82pW1/bKin8vCfeWJKKH\n"
    "YDW6Uiztb5glYP5Gy6d80jo8YG9AXXSQDjJsk5q3GRptES2qq8S7Cd3oQOx42NRG\n"
    "hOibaCBup7v0olM97V8s40Gpg1uyKXesoXgl8gWt9ALXt9FRS+inUzR6YFmAnzLa\n"
    "ML/sgY6GFOwW++5Fzr9ppBibvSNCHdxzSVjTTdCbKdiT015FwA2fJI9Ry8ildbxJ\n"
    "g/raWMCugkJbSxlHu93Te6CaDMbZORF9I+uCvg9Yjk0aecI2PTxsHnpghYS3VQTZ\n"
    "K5E9Dzp+N4kB0L0aOAM6vOR+/hyP5wZCwjsgouymCZlLqHfGw6jhsCiX/jX3DMir\n"
    "2bE+2APT98ApwMOXcTcKva/Nj/gQnPbB8TUuyHcdVQ==\n"
    "-----END CERTIFICATE-----\n";
const char tlsKey[] =
    "-----BEGIN PRIVATE KEY-----\n"
    "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\n"
    "eDiMQRld0pge0zjBip8dup4EL+ed\n"
    "-----END PRIVATE KEY-----\n";

void serveStatic(AsyncWebServer *server);

namespace endpoints {
    void get404(AsyncWebServerRequest *request);

    void getHeap(AsyncWebServerRequest *request);

    void getConnectWifi(AsyncWebServerRequest *request);
    void postConnectWifi(AsyncWebServerRequest *request);

    void getGlovarmonic(AsyncWebServerRequest *request);
} // namespace endpoints

} // namespace http
