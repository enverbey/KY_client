#pragma once

#include <WiFi.h>

enum WiFiStatus {
    NO_LOG,
    ONLY_ERRORS,
    ALL
};

class BestWiFi {
public:
    BestWiFi(const String& ssid, const String& password);
    ~BestWiFi();

    bool connect();
    void displayNetworkInfo();
    void setupWiFiEventHandlers();
    void setWaitToConnect(uint8_t flag);
private:
    String _ssid;
    String _password;
    String _mdnsName;
    uint8_t __WAITTOCONNECT__ = false;
    uint8_t _maxAttempts = 20;
    uint16_t _connectDelay = 500; // ms
    WiFiStatus _displayLog = ALL;
};