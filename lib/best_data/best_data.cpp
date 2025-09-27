#include "best_data.hpp"
#include <Arduino.h>
#include <cstring>
#include <ArduinoJson.h>
#include <ctime>
#include <WiFi.h>
#include "../best_wifi/best_wifi.hpp"

best_data dataManager;
extern BestWiFi wifi;
// Güvenli kopyalama için yardımcı fonksiyon
static void copy_to_buffer(char* dest, size_t destSize, const String& src) {
	if (destSize == 0) return;
	size_t srcLen = (size_t)src.length();
	size_t copyLen = srcLen < (destSize - 1) ? srcLen : (destSize - 1);
	if (copyLen > 0) {
		memcpy(dest, src.c_str(), copyLen);
	}
	dest[copyLen] = '\0';
}

// === $$$$$ >> Veri Yönetimi << $$$$$ ===
// Yeni bir data ekle (mevcutsa bir şey yapmaz)
void best_data::add_data
	(
		const String& name,
		const String& type,
		const String& max_val,
		const String& min_val,
		const String& default_val,
		const AccessLevel& access,
		char valNames[6][20]
	)
{
	// Data mevcutsa ekleme
	if (data.find(name) != data.end()) return;

	s_data_info entry;
	// Bufferları boş olarak başlat
	entry.name[0] = '\0';
	entry.type[0] = '\0';
	entry.max_val[0] = '\0';
	entry.min_val[0] = '\0';
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 20; j++) {
			entry.valNames[i][j] = '\0';
		}
	}
	entry.val[0] = '\0';
	entry.updated = 0;

	// Verileri güvenli bir şekilde kopyala
	copy_to_buffer(entry.name, sizeof(entry.name), name);
	copy_to_buffer(entry.type, sizeof(entry.type), type);
	copy_to_buffer(entry.max_val, sizeof(entry.max_val), max_val);
	copy_to_buffer(entry.min_val, sizeof(entry.min_val), min_val);
	if (valNames != nullptr) {
		for (int i = 0; i < 6; i++) {
			copy_to_buffer(entry.valNames[i], sizeof(entry.valNames[i]), String(valNames[i]));
		}
	}
	copy_to_buffer(entry.val, sizeof(entry.val), default_val);

	// Güncellenme zamanını şimdi olarak ayarla
	entry.updated = time(NULL);
	entry.access = access;

	// Yeni veriyi ekle
	data[name] = entry;
}

// Var olan bir datanın değerini güncelle (veya eksikse oluştur)
void best_data::push_data
	(
		const String& name,
		const String& val
	)
{
	auto it = data.find(name);
	if (it != data.end()) {
		copy_to_buffer(it->second.val, sizeof(it->second.val), val);
		it->second.updated = time(NULL);
	} else {
		// TR: isim ve değer ile minimal data(giriş) oluştur
		s_data_info entry;
		entry.name[0] = '\0';
		entry.type[0] = '\0';
		entry.max_val[0] = '\0';
		entry.min_val[0] = '\0';
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 20; j++) {
				entry.valNames[i][j] = '\0';
			}
		}
		entry.val[0] = '\0';
		entry.updated = time(NULL);
		entry.access = AccessLevel::ReadOnly;
		copy_to_buffer(entry.name, sizeof(entry.name), name);
		copy_to_buffer(entry.val, sizeof(entry.val), val);
		data[name] = entry;
	}
}

// === $$$$$ >> Tekil Veri Erişimleri << $$$$$ ===
String best_data::get_type(const String& name) {
	auto it = data.find(name);
	if (it != data.end()) {
		const s_data_info& e = it->second;
		return (e.type);
	}
	return String("");
}
String best_data::get_max(const String& name) {
	auto it = data.find(name);
	if (it != data.end()) {
		const s_data_info& e = it->second;
		return (e.max_val);
	}
	return String("");
}
String best_data::get_min(const String& name) {
	auto it = data.find(name);
	if (it != data.end()) {
		const s_data_info& e = it->second;
		return (e.min_val);
	}
	return String("");
}
String best_data::get_value(const String& name) {
	auto it = data.find(name);
	if (it != data.end()) {
		const s_data_info& e = it->second;
		return (e.val);
	}
	return String("");
}
String best_data::get_updated(const String& name) {
	auto it = data.find(name);
	if (it != data.end()) {
		const s_data_info& e = it->second;
		return String(e.updated);
	}
	return String("");
}
AccessLevel best_data::get_access(const String& name) {
	auto it = data.find(name);
	if (it != data.end()) {
		const s_data_info& e = it->second;
		return e.access;
	}
	return AccessLevel::NoAccess;
}

// === $$$$$ >> JSON Dönüşümleri << $$$$$ ===
String best_data::get_json() {
    JsonDocument doc;

	JsonObject root = doc.to<JsonObject>();
    root["mac_address"] = WiFi.macAddress();
    JsonArray data_info_array = root["data"].to<JsonArray>();

    for (auto const& [key, val] : data) {
        JsonObject obj = data_info_array.add<JsonObject>();
        obj["name"] = val.name;
		obj["value"] = val.val;
        obj["updated"] = val.updated;
    }

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    return jsonOutput;
}

String best_data::get_json_info() {
    JsonDocument doc;

	JsonObject root = doc.to<JsonObject>();
    root["mac_address"] = WiFi.macAddress();
    JsonArray data_info_array = root["data_info"].to<JsonArray>();

	for (auto const& [key, val] : data) {
        JsonObject obj = data_info_array.add<JsonObject>();
        obj["name"] = val.name;
        obj["type"] = val.type;
        obj["max_val"] = val.max_val;
        obj["min_val"] = val.min_val;
		JsonArray val_names_array = obj["val_names"].to<JsonArray>();
		for (int i = 0; i < 6; i++) {
			val_names_array.add(val.valNames[i]);
		}
        obj["updated"] = val.updated;
        obj["access"] = (int)val.access;
    }

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    return jsonOutput;
}