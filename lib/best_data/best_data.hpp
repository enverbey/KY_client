#pragma once

#include <map>
#include <string>
#include <Arduino.h>
#include <ctime>

enum class AccessLevel {
	ReadOnly  = 1,
	ReadWrite = 2,
	NoAccess  = 99
};

typedef struct s_data_info
{
	char name[50];
	char type[20];
	char max_val[20];
	char min_val[20];
	char valNames[6][20];
	char val[20];
	time_t updated;
	AccessLevel access;
} t_data_info;

class best_data{
	private:
		std::map<String, s_data_info> data;
	public:
	// === $$$$$ >> Veri Yönetimi << $$$$$ ===
		// Yeni bir data ekle (mevcutsa bir şey yapmaz)
		void add_data(
			const String& name,
			const String& type,
			const String& max_val,
			const String& min_val,
			const String& default_val,
			const AccessLevel& access,
			char valNames[6][20] = nullptr);
		// Var olan bir datanın değerini güncelle (veya eksikse oluştur)
		void push_data(const String& name, const String& val);
	
	// === $$$$$ >> Tekil Veri Erişimleri << $$$$$ ===
		String get_type(const String& name);
		String get_max(const String& name);
		String get_min(const String& name);
		String get_value(const String& name);
		String get_updated(const String& name);
		AccessLevel get_access(const String& name);

	// === $$$$$ >> JSON Dönüşümleri << $$$$$ ===
		// Basit JSON formatında tüm verileri döndür
		String get_json();
		// Detaylı JSON bilgisi döndür (type, max_val, min_val ve updated)
		String get_json_info();
};