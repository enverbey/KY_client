#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

void loginToCaptivePortal() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi bağlı değil.");
    return;
  }

  HTTPClient http;
  WiFiClient client;
  
  Serial.println("Captive portal'a giriş yapılıyor...");
  
  // Adım 1: Session cookie'sini almak için GET isteği
  Serial.println("1. Session cookie alınıyor...");
  http.begin(client, "http://10.19.254.254:8000/");
  http.addHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
  const char* headers[] = { "Set-Cookie", "Location" };
http.collectHeaders(headers, 2);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode != 200) {
    Serial.printf("GET isteği başarısız: %d\n", httpResponseCode);
    http.end();
    return;
  }
  
  // Cookie'yi headerlardan çıkar
String cookie = "";
for (int i = 0; i < http.headers(); i++) {
  Serial.println("Header: " + http.headerName(i) + " = " + http.header(i));
  if (http.headerName(i).equalsIgnoreCase("Set-Cookie")) {
    String cookieHeader = http.header(i);
    int semicolon = cookieHeader.indexOf(';');
    if (semicolon != -1) {
      cookie = cookieHeader.substring(0, semicolon);
    } else {
      cookie = cookieHeader;
    }
    Serial.println("Cookie alındı: " + cookie);
    break;
  }
}
  
  http.end();
  
  if (cookie.length() == 0) {
    Serial.println("Cookie alınamadı!");
    return;
  }
  
  // Adım 2: Login bilgilerini POST et
  Serial.println("2. Login bilgileri gönderiliyor...");
  
  http.begin(client, "http://10.19.254.254:8000/?zone=guest&lang=tr");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Referer", "http://10.19.254.254:8000/");
  http.addHeader("Cookie", cookie);
  http.addHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36");
  
  // POST verisini hazırla
  String postData = "auth_user=34534462416";
  postData += "&auth_pass=omeromerm1";
  postData += "&termofuse=1";
  postData += "&redirurl=https://www.google.com";
  postData += "&zone=guest";
  postData += "&accept=Giriş";

  Serial.println("POST verisi: " + postData);
  
  // HTTP redirect'i otomatik takip etme
  http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
  http.setTimeout(15000);
  
  httpResponseCode = http.POST(postData);
  
  Serial.printf("HTTP Response Code: %d", httpResponseCode);
  
  // Başarı kontrolü - 302 ve Location: https://www.google.com olmalı
  if (httpResponseCode == 302) {
    String location = "";
    
    // Location header'ını bul
    for (int i = 0; i < http.headers(); i++) {
      if (http.headerName(i).equalsIgnoreCase("Location")) {
        location = http.header(i);
        location.trim(); // Baş ve sondaki boşlukları temizle
        Serial.println("Location header: " + location);
        break;
      }
    }
    
    http.end();
    
    // Başarı kontrolü
    if (location == "https://www.google.com") {
      Serial.println("✓ Login başarılı! Google'a yönlendiriliyor.");
      
      
    } else {
      Serial.println("✗ HTTP 302 ancak beklenmeyen yönlendirme: " + location);
    }
    
  } else if (httpResponseCode == 200) {
    Serial.println("✗ HTTP 200 - Login sayfasında kaldı, muhtemelen başarısız");
    http.end();
  } else {
    Serial.printf("✗ Beklenmeyen HTTP kodu: %d", httpResponseCode);
    if (httpResponseCode > 0) {
      Serial.printf("Hata detayı: %s", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  }

}