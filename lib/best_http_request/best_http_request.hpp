#pragma once
#include <WiFi.h>
#include <map>
#include <string>

enum HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH
};

struct HttpResponse {
    int statusCode;
    String headers;
    String body;
    bool success;
    String error;
    long responseTime;
};

class BestHttpRequest {
private:
    static const int DEFAULT_TIMEOUT = 400; // 200ms timeout
    static const int BUFFER_SIZE = 256;
    
    static String methodToString(HttpMethod method);
    static bool parseResponse(const String& rawResponse, HttpResponse& response);
    static String parseUrl(const String& url, String& host, int& port, String& path);
    static WiFiClient createSecureConnection(const String& host, int port, bool isHttps);
    
public:
    // Ana HTTP request fonksiyonu
    static HttpResponse request(
        HttpMethod method,
        const String& url,
        const String& body = "",
        const std::map<String, String>& headers = {},
        int timeout = DEFAULT_TIMEOUT
    );
    
    // Kolay kullanım fonksiyonları
    static HttpResponse get(const String& url, const std::map<String, String>& headers = {});
    static HttpResponse post(const String& url, const String& body, const std::map<String, String>& headers = {});
    static HttpResponse put(const String& url, const String& body, const std::map<String, String>& headers = {});
    static HttpResponse del(const String& url, const std::map<String, String>& headers = {});
    
    // JSON helper fonksiyonları
    static HttpResponse postJson(const String& url, const String& jsonBody, const std::map<String, String>& extraHeaders = {});
    static HttpResponse putJson(const String& url, const String& jsonBody, const std::map<String, String>& extraHeaders = {});
    
    // Debug ve test fonksiyonları
    static void printResponse(const HttpResponse& response);
    static bool testConnection(const String& host, int port = 80);
};

// Backward compatibility için eski fonksiyon
void best_http_request();