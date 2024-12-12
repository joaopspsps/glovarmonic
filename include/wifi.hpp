#pragma once

namespace wifi {

const char apSsidPrefix[] = "GlovarmonicAP-";
const char hostnamePrefix[] = "glovarmonic-";

const char apPsk[] = "12345678";

const char filenameSsid[] = "/wifi_ssid";
const char filenamePsk[] = "/wifi_psk";

bool saveCredentials(const char *ssid, const char *psk);
bool retrieveCredentials(String &ssid, String &psk);

void setupWifi(const String &glovarmonicId);
bool connectSTA(const String &glovarmonicId, const char *ssid, const char *psk);
void startAP(const String &glovarmonicId);

} // namespace wifi
