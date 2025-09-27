#include "best_http_request.hpp"
#include <WiFiClientSecure.h>
#include <esp32-hal-log.h>

HttpResponse BestHttpRequest::request(
    HttpMethod method,
    const String& url,
    const String& body,
    const std::map<String, String>& headers,
    int timeout
) {
    printf("<===================REQUEST-START===================>\n");
    HttpResponse response;
    response.success = false;
    response.statusCode = 0;
    response.responseTime = 0;
    
    unsigned long startTime = millis();
    
    Serial.println("=== RAW SOCKET HTTP REQUEST ===");
    Serial.print("🌐 URL: ");
    Serial.println(url);
    Serial.print("🔧 Method: ");
    Serial.println(methodToString(method));
    
    // URL'yi parse et
    String host, path;
    int port;
    String protocol = parseUrl(url, host, port, path);
    bool isHttps = (protocol == "https");
    
    if (host.isEmpty()) {
        response.error = "Invalid URL format";
        Serial.println("❌ Geçersiz URL formatı");
        return response;
    }
    
    Serial.print("🏠 Host: ");
    Serial.println(host);
    Serial.print("🚪 Port: ");
    Serial.println(port);
    Serial.print("📁 Path: ");
    Serial.println(path);
    Serial.print("🔒 HTTPS: ");
    Serial.println(isHttps ? "Yes" : "No");
    
    // WiFi bağlantısını kontrol et
    if (WiFi.status() != WL_CONNECTED) {
        response.error = "WiFi not connected";
        Serial.println("❌ WiFi bağlantısı yok");
        return response;
    }
    
    // Raw socket oluştur
    WiFiClient client;
    WiFiClientSecure secureClient;
    
    WiFiClient* activeClient;
    
    if (isHttps) {
        secureClient.setInsecure(); // SSL sertifika doğrulamasını atla (hızlı bağlantı için)
        activeClient = &secureClient;
        Serial.println("🔒 HTTPS bağlantısı kuruluyor...");
    } else {
        activeClient = &client;
        Serial.println("🌐 HTTP bağlantısı kuruluyor...");
    }
    
    // Timeout ayarla
    activeClient->setTimeout(timeout);
    
    // Bağlantı kur
    Serial.print("⚡ Socket bağlantısı kuruluyor: ");
    if (!activeClient->connect(host.c_str(), port, 200)) {
        response.error = "Connection failed";
        Serial.println("❌ BAŞARISIZ");
        return response;
    }
    
    Serial.println("✅ BAŞARILI");
    
    // HTTP request oluştur
    String httpRequest = "";
    
    // Request line
    httpRequest += methodToString(method) + " " + path + " HTTP/1.1\r\n";
    
    // Zorunlu header'lar
    httpRequest += "Host: " + host + "\r\n";
    httpRequest += "Connection: close\r\n";
    httpRequest += "User-Agent: ESP32-RawSocket/1.0\r\n";
    httpRequest += "Accept: */*\r\n";
    
    // Content-Length (POST/PUT için)
    if (method == POST || method == PUT || method == PATCH) {
        if (!body.isEmpty()) {
            httpRequest += "Content-Length: " + String(body.length()) + "\r\n";
        } else {
            httpRequest += "Content-Length: 0\r\n";
        }
    }
    
    // Özel header'lar ekle
    for (const auto& header : headers) {
        httpRequest += header.first + ": " + header.second + "\r\n";
    }
    
    // Header'ları bitir
    httpRequest += "\r\n";
    
    // Body ekle (varsa)
    if (!body.isEmpty()) {
        httpRequest += body;
    }
    
    Serial.println("📤 HTTP Request gönderiliyor:");
    Serial.println("--- REQUEST START ---");
    Serial.print(httpRequest);
    Serial.println("--- REQUEST END ---");
    
    // Request'i gönder
    size_t bytesSent = activeClient->print(httpRequest);
    Serial.print("📊 Gönderilen bytes: ");
    Serial.println(bytesSent);
    
    if (bytesSent != httpRequest.length()) {
        response.error = "Failed to send complete request";
        Serial.println("❌ Request tamamen gönderilemedi");
        activeClient->stop();
        return response;
    }
    
    // Response bekle
    Serial.println("📥 Response bekleniyor...");
    
    unsigned long responseStartTime = millis();
    while (activeClient->connected() && !activeClient->available()) {
        if (millis() - responseStartTime > timeout) {
            response.error = "Response timeout";
            Serial.println("⏰ Response timeout");
            activeClient->stop();
            return response;
        }
        delay(10);
    }
    
    String rawResponse = "";
    char buffer[BUFFER_SIZE];

    while (activeClient->connected() || activeClient->available())
    {
        if (millis() - responseStartTime > timeout) {
            Serial.println("⏰ Toplam response okuma süresi aşıldı!");
            response.error = "Response read timeout";
            break;
        }
        if (activeClient->available())
        {
            int bytesRead = activeClient->readBytes(buffer, min((size_t)activeClient->available(), (size_t)BUFFER_SIZE - 1));
            if (bytesRead > 0)
            {
                rawResponse.concat(buffer, bytesRead);
            }
        } else {
            delay(20);
        }
    }

    // Bağlantıyı kapat
    activeClient->stop();
    response.responseTime = millis() - startTime;

    // ======================= DEBUG İÇİN EKLENECEK KOD =======================
    Serial.println("\n\n**************************************************");
    Serial.println(">>> PARSE EDİLECEK HAM VERİ BAŞLANGICI <<<");
    Serial.println("**************************************************");
    Serial.print(rawResponse); // Verinin sonundaki görünmez karakterleri de görebilmek için println yerine print kullanıyoruz.
    Serial.println("\n**************************************************");
    Serial.println(">>> PARSE EDİLECEK HAM VERİ SONU <<<");
    Serial.print(">>> Alınan Toplam Karakter Sayısı: ");
    Serial.println(rawResponse.length());
    Serial.println("**************************************************\n\n");
    // ========================================================================

    Serial.println("📥 Raw Response alındı:");
    Serial.println("--- RESPONSE START ---");
    Serial.println(rawResponse);
    Serial.println("--- RESPONSE END ---");
    
    // Response'u parse et
    if (parseResponse(rawResponse, response)) {
        response.success = true;
        Serial.print("✅ HTTP Request başarılı! Status: ");
        Serial.print(response.statusCode);
        Serial.print(" | Response time: ");
        Serial.print(response.responseTime);
        Serial.println("ms");
    } else {
        response.error = "Failed to parse response";
        Serial.println("❌ Response parse edilemedi");
    }
    
    return response;
    printf("<===================REQUEST-END===================>\n");
}

String BestHttpRequest::methodToString(HttpMethod method) {
    switch (method) {
        case GET: return "GET";
        case POST: return "POST";
        case PUT: return "PUT";
        case DELETE: return "DELETE";
        case PATCH: return "PATCH";
        default: return "GET";
    }
}

bool BestHttpRequest::parseResponse(const String& rawResponse, HttpResponse& response) {
    if (rawResponse.isEmpty()) {
        printf("❌ Boş response\n");
        return false;
    }
    
    int headerEndIndex = rawResponse.indexOf("\r\n\r\n");
    if (headerEndIndex == -1) {
        headerEndIndex = rawResponse.indexOf("\n\n");
        if (headerEndIndex == -1) {
            printf("❌ Header ve body ayrımı bulunamadı\n");
            return false;
        }
        headerEndIndex += 2;
    } else {
        headerEndIndex += 4;
    }
    
    // Status code'u parse et
    int statusStart = rawResponse.indexOf(" ") + 1;
    int statusEnd = rawResponse.indexOf(" ", statusStart);
    if (statusStart > 0 && statusEnd > statusStart) {
        response.statusCode = rawResponse.substring(statusStart, statusEnd).toInt();
    }
    
    // Header'ları ayır
    response.headers = rawResponse.substring(0, headerEndIndex);
    
    // Body'yi ayır
    if (headerEndIndex < rawResponse.length()) {
        response.body = rawResponse.substring(headerEndIndex);
    }
    
    return true;
}

String BestHttpRequest::parseUrl(const String& url, String& host, int& port, String& path) {
    bool isHttps = false;
    String workingUrl = url;
    
    // Protocol kontrolü
    if (workingUrl.startsWith("https://")) {
        isHttps = true;
        port = 443;
        workingUrl = workingUrl.substring(8);
    } else if (workingUrl.startsWith("http://")) {
        isHttps = false;
        port = 80;
        workingUrl = workingUrl.substring(7);
    } else {
        // Protocol belirtilmemişse HTTP varsay
        isHttps = false;
        port = 80;
    }
    
    // Host ve path'i ayır
    int pathIndex = workingUrl.indexOf('/');
    if (pathIndex == -1) {
        host = workingUrl;
        path = "/";
    } else {
        host = workingUrl.substring(0, pathIndex);
        path = workingUrl.substring(pathIndex);
    }
    
    // Port kontrolü
    int portIndex = host.indexOf(':');
    if (portIndex != -1) {
        port = host.substring(portIndex + 1).toInt();
        host = host.substring(0, portIndex);
    }
    
    // Protocol türünü string olarak döndür
    return isHttps ? "https" : "http";
}

HttpResponse BestHttpRequest::get(const String& url, const std::map<String, String>& headers) {
    return request(GET, url, "", headers);
}

HttpResponse BestHttpRequest::post(const String& url, const String& body, const std::map<String, String>& headers) {
    return request(POST, url, body, headers);
}

HttpResponse BestHttpRequest::put(const String& url, const String& body, const std::map<String, String>& headers) {
    return request(PUT, url, body, headers);
}

HttpResponse BestHttpRequest::del(const String& url, const std::map<String, String>& headers) {
    return request(DELETE, url, "", headers);
}

HttpResponse BestHttpRequest::postJson(const String& url, const String& jsonBody, const std::map<String, String>& extraHeaders) {
    std::map<String, String> headers = extraHeaders;
    headers["Content-Type"] = "application/json";
    return request(POST, url, jsonBody, headers);
}

HttpResponse BestHttpRequest::putJson(const String& url, const String& jsonBody, const std::map<String, String>& extraHeaders) {
    std::map<String, String> headers = extraHeaders;
    headers["Content-Type"] = "application/json";
    return request(PUT, url, jsonBody, headers);
}

void BestHttpRequest::printResponse(const HttpResponse& response) {
    Serial.println("=== HTTP RESPONSE DETAILS ===");
    Serial.print("✅ Success: ");
    Serial.println(response.success ? "true" : "false");
    Serial.print("📊 Status Code: ");
    Serial.println(response.statusCode);
    Serial.print("⏱️  Response Time: ");
    Serial.print(response.responseTime);
    Serial.println("ms");
    
    if (!response.error.isEmpty()) {
        Serial.print("❌ Error: ");
        Serial.println(response.error);
    }
    
    Serial.println("📋 Headers:");
    Serial.println(response.headers);
    
    Serial.println("📄 Body:");
    Serial.println(response.body);
    Serial.println("========================");
}

bool BestHttpRequest::testConnection(const String& host, int port) {
    Serial.print("🧪 Testing connection to ");
    Serial.print(host);
    Serial.print(":");
    Serial.println(port);
    
    WiFiClient client;
    client.setTimeout(500); // 500ms timeout for test
    
    bool connected = client.connect(host.c_str(), port);
    if (connected) {
        client.stop();
        Serial.println("✅ Connection test successful");
    } else {
        Serial.println("❌ Connection test failed");
    }
    
    return connected;
}

// Backward compatibility için eski fonksiyon
void best_http_request() {
    Serial.println("🚀 Best HTTP Request library initialized");
    Serial.println("💡 Use BestHttpRequest::get(), post(), put(), del() methods");
    
    // Test örneği
    Serial.println("📝 Example usage:");
    Serial.println("  HttpResponse resp = BestHttpRequest::get(\"http://httpbin.org/get\");");
    Serial.println("  BestHttpRequest::printResponse(resp);");
}
