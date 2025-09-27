#include "system.hpp"
#include "../best_http_request/best_http_request.hpp"
#include "../best_web_server/best_web_server.hpp"
#include "../best_data/best_data.hpp"
#include "../best_wifi/best_wifi.hpp"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <atomic>

extern	std::atomic<unsigned long> last_heart_beat;

// // Raw socket
// void register_gateway() {
// 	if (WiFi.status() == WL_CONNECTED) {
// 		Serial.println("💚 Heartbeat gönderiliyor...");
// 		String jsonBody = "{";
// 		jsonBody += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
// 		jsonBody += "\"mac\":\"" + WiFi.macAddress() + "\"";
// 		jsonBody += "}";
// 		HttpResponse response = BestHttpRequest::postJson("http://esp-gateway-1.local/register", jsonBody);
// 		if (response.success)
// 		{
// 			Serial.println(response.body);
// 		}
// 		last_heart_beat.store(millis());
// 	}
// }

//Gateway'a register isteği atar.
void register_gateway() {
	Serial.println(">> GATEWAY/REGISTAR başlatılıyor...");
	if (WiFi.status() == WL_CONNECTED) {
		Serial.println("💚 Heartbeat gönderiliyor...");

		// HTTPClient nesnesi oluşturuluyor
		HTTPClient http;

		// Hedef URL ve POST isteği başlatılıyor
		http.begin("http://esp-gateway-1.local/register");
		http.addHeader("Content-Type", "application/json");

		// Zaman aşımı sürelerini 20 saniye olarak ayarla
        http.setTimeout(20000);

		// JSON formatında veri oluşturuluyor
		String jsonPayload = "{\"ip\":\"" + WiFi.localIP().toString() + "\", \"mac\":\"" + WiFi.macAddress() + "\"}";

		// POST isteği gönderiliyor ve yanıt kodu alınıyor
		int httpResponseCode = http.POST(jsonPayload);

		// İstek başarılı oldu mu kontrol ediliyor
		if (httpResponseCode > 0) {
			Serial.printf("HTTP Yanıt Kodu: %d\n", httpResponseCode);
			// Başarılı yanıt durumunda gelen body yazdırılıyor
			String responseBody = http.getString();
			Serial.println("Yanıt İçeriği:");
			Serial.println(responseBody);
		} else {
			// Hata durumunda hata kodu ve mesajı yazdırılıyor
			Serial.printf("HTTP Hatası: %d - %s\n", httpResponseCode, http.errorToString(httpResponseCode).c_str());
		}

		// Bağlantı sonlandırılıyor
		http.end();
	} else {
		Serial.println("⚠️ WiFi bağlantısı yok, heartbeat gönderilemiyor.");
	}
	last_heart_beat.store(millis());
}

//Gateway'a register isteği atılacak mı kontrol eder.
void check_register_status() {
	if ((millis() - last_heart_beat.load()) >= 15000)
	{
		Serial.printf("Zaman farkı: %lu ms\n", (millis() - last_heart_beat.load()));
		register_gateway();
	}
}

void steam_controller() {
	switch (startStream)
	{
	case 0:
		if ((millis() - last_data_push.load()) >= streamOffTimeout)
		{
			push_data_to_server();
			last_data_push.store(millis());
		}
		break;
	case 1:
		if ((millis() - last_data_push.load()) >= streamOnTimeout)
		{
			push_data_to_server();
			last_data_push.store(millis());
		}
		break;
	default:
		break;
	}
}

void system_loop() {
	Serial.printf("MAC=>>>=>>=>==>=>=>> %s\n", WiFi.macAddress().c_str());
	check_register_status();
	steam_controller();
	//Serial.printf("Client Mac: %s\n", WiFi.macAddress().c_str());
	delay(500);
}

//68:25:DD:F0:6D:08