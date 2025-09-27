#include <HTTPClient.h>
#include "../best_data/best_data.hpp"
#include "../best_wifi/best_wifi.hpp"
#include <WiFi.h>

extern best_data dataManager;
extern String token;

//X-Client-Token ve X-Client-MAC

//Sunucuya info verisini gönderir. MAC + DATA
void push_info_to_server() {
    Serial.println(">> INFO verisi sunucuya gönderiliyor...");
    int maxAttempts = 100;
    int attempts = 0;

    while (attempts < maxAttempts) {
        while (WiFi.status() != WL_CONNECTED) {
            Serial.println("⚠️ WiFi bağlantısı yok, info gönderilemiyor.");
            delay(2000);
        }

        HTTPClient http;
        http.begin("https://api.xmangax.com/client/info/");
        http.addHeader("Content-Type", "application/json");
        http.addHeader("X-Client-Token", token);
        http.addHeader("X-Client-MAC", WiFi.macAddress());
        http.setTimeout(20000);

        String finalPayload = dataManager.get_json_info();

        Serial.print("Gönderilen JSON: ");
        Serial.println(finalPayload);

        int httpResponseCode = http.POST(finalPayload);

        if (httpResponseCode > 0) {
            Serial.printf("✅ Info verisi başarıyla gönderildi. HTTP Kodu: %d\n", httpResponseCode);
            http.end();
            return ;
        }
        else {
            Serial.printf("❌ Info verisi gönderilemedi. Hata: %d - %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
            attempts++;
            if (attempts < maxAttempts) {
                Serial.printf("🔄 Yeniden deneniyor... (%d/%d)\n", attempts, maxAttempts);
                delay(2000);
            }
        }
        http.end();
    }
}

// Sunucuya data bilgisi atar.
void push_data_to_server() {
    Serial.println(">> DATA verisi sunucuya gönderiliyor...");
    int maxAttempts = 3;
    int attempts = 0;

    while (attempts < maxAttempts) {
        while (WiFi.status() != WL_CONNECTED) {
            Serial.println("⚠️ WiFi bağlantısı yok, info gönderilemiyor.");
            delay(2000);
        }

        Serial.printf("HEADER_TOKEN: %s\n", token.c_str());

        HTTPClient http;
        http.begin("https://api.xmangax.com/client/data/");
        http.addHeader("X-Client-Token", token.c_str());
        http.addHeader("X-Client-MAC", WiFi.macAddress().c_str());
        http.addHeader("Content-Type", "application/json");
        http.setTimeout(20000);

        String finalPayload = dataManager.get_json();

        Serial.print("Gönderilen JSON: ");
        Serial.println(finalPayload);

        int httpResponseCode = http.POST(finalPayload);

        if (httpResponseCode > 0) {
            Serial.printf("✅ DATA verisi başarıyla gönderildi. HTTP Kodu: %d\n", httpResponseCode);
            http.end();
            return;
        } else {
            Serial.printf("❌ DATA verisi gönderilemedi. Hata: %d - %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
            attempts++;
            if (attempts < maxAttempts) {
                Serial.printf("🔄 Yeniden deneniyor... (%d/%d)\n", attempts, maxAttempts);
                delay(2000);
            }
        }
        http.end();
    }
    Serial.println("⛔ DATA verisi gönderilemedi. Maksimum deneme sayısına ulaşıldı.");
}