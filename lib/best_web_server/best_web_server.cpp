#include "../best_data/best_data.hpp"
#include "best_web_server.hpp"
#include <WebServer.h>
#include <atomic>
#include <map>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>
extern uint8_t startStream;
AsyncWebServer server(80);
extern Preferences preferences;
extern String token;
extern best_data dataManager;
std::atomic<unsigned long> last_heart_beat{0};
extern std::atomic<unsigned long> last_data_push;
void push_data_to_server();
void best_web_server()
{

	server.on("/data", HTTP_POST,
        [](AsyncWebServerRequest *request){
            last_heart_beat.store(millis());
        },
        nullptr, // onUpload
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
        {
            last_heart_beat.store(millis());
            static std::vector<char> bodyBuffer;

            // İlk parça ise hafızayı temizle ve veri için ayır
            if (index == 0) {
                bodyBuffer.clear();
                bodyBuffer.reserve(total);
            }

            // Gelen veriyi vektöre ekle
            bodyBuffer.insert(bodyBuffer.end(), data, data + len);

            // Tüm veri alındıysa işle
            if (index + len == total) {
                JsonDocument doc;
                DeserializationError err = deserializeJson(doc, bodyBuffer);

                if (err) {
                    Serial.printf("❌ POST JSON ayrıştırılamadı. Hata: %s\n", err.c_str());
                    request->send(400, "application/json", "{\"error\":\"invalid_json\"}");
                    return;
                }
                
                JsonObject obj = doc.as<JsonObject>();
                if (obj.isNull()) {
                    Serial.println("⚠️ POST isteği geldi ancak geçerli JSON objesi bulunamadı.");
                    request->send(400, "application/json", "{\"error\":\"no_json_object\"}");
                    return;
                }

                for (JsonPair kv : obj) {
                    Serial.printf("🔄 Veri alındı: %s = %s\n", kv.key().c_str(), kv.value().as<String>().c_str());
                    dataManager.push_data(kv.key().c_str(), kv.value().as<String>());
                }

                request->send(200, "application/json", "{\"status\":\"success\"}");

                // Belleği serbest bırakmak için vektörü temizle
                bodyBuffer.clear();

                // Data push
                push_data_to_server();
			    last_data_push.store(millis());
            }
        }
    );

	server.on("/setStream", HTTP_POST,
        [](AsyncWebServerRequest *request){
        },
        nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, data, len);

			if (err) {
                Serial.printf("❌ /setStream JSON ayrıştırılamadı. Hata: %s\n", err.c_str());
                request->send(400, "application/json", "{\"error\":\"invalid_json\"}");
                return;
            }

            if (!doc["stream"].isNull()) {
                uint8_t streamValue = doc["stream"].as<uint8_t>();
				startStream = streamValue; 
                Serial.printf("✅ Stream durumu ayarlandı: %u\n", startStream);

				JsonDocument responseDoc;
				responseDoc["status"] = "success";
				responseDoc["stream_active"] = startStream;
				
				String responseJson;
				serializeJson(responseDoc, responseJson);
				request->send(200, "application/json", responseJson);
            } else {
                request->send(400, "application/json", "{\"error\":\"missing_stream_key\"}");
            }
        }
    );

    // Token kaydetme endpoint'i
  server.on("/token", HTTP_POST,
    [](AsyncWebServerRequest *request) {
      // boş bırakıyoruz çünkü body'yi aşağıdaki callback yakalıyor
    },
    nullptr,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // JSON ayrıştır
      DynamicJsonDocument doc(512);
      DeserializationError err = deserializeJson(doc, (const char*)data, len);
      
      if (err) {
        Serial.printf("❌ /token JSON ayrıştırılamadı. Hata: %s\n", err.c_str());
        request->send(400, "application/json", "{\"error\":\"invalid_json\"}");
        return;
      }

      if (!doc["token"].isNull()) {
        token = doc["token"].as<String>();

        preferences.begin("my-app", false);
        preferences.putString("token", token);
        preferences.end();

        Serial.printf("✅ Token NVM'ye kaydedildi: %s\n", token.c_str());
        request->send(200, "application/json", "{\"status\":\"success\"}");
      } else {
        request->send(400, "application/json", "{\"error\":\"missing_token_key\"}");
      }
    }
  );

	server.begin();
	Serial.println("HTTP server started");
}