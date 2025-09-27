#include "system.hpp"
#include "../best_wifi/best_wifi.hpp"
#include <WiFi.h>
#include <atomic>
#include <Preferences.h>
/*############################################################################*\
|*#                             GLOBAL VARIABLES                             #*|
\*############################################################################*/

const char* WIFI_SSID = "42 Student"; //"Zyxel_CCA1";
const char* WIFI_PASS = ""; //"4KKHMNF3E4";

Preferences preferences;
String token;
uint8_t  startStream = 1; // Stream açık
uint32_t streamOnTimeout  =  1000; // 30 saniye
uint32_t streamOffTimeout = 300000; // 5 dakika
std::atomic<unsigned long> last_data_push{0};

BestWiFi wifi(WIFI_SSID, WIFI_PASS);

/*############################################################################*\
|*#                          SYSTEM SETUP FUNCTIONS                          #*|
\*############################################################################*/

void initTokenFromNVS()
{
    preferences.begin("my-app", true); // true: read-only
    token = preferences.getString("token", token);
    preferences.end();
}

void system_setup() {
    Serial.begin(115200);
    initTokenFromNVS();
    Serial.println(">> Sistem başlatılıyor...");
    delay(5000);
    Serial.printf("token : %s", token.c_str());
    delay(5000);
    Serial.println();
    wifi.setWaitToConnect(true); //Wifi ye bağlanana kadar bekle.
    wifi.connect();
    best_web_server();
    register_gateway(); //İlk başta register oluyoruz.
    last_data_push.store(millis());
}