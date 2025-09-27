#include <Arduino.h>
#include "user.hpp"
#include "../best_data/best_data.hpp"

extern best_data dataManager;
void user_loop()
{
	Serial.println(">> Kullanıcı döngüsü çalışıyor...");
	// ---- ReadOnly veriler ----
	dataManager.push_data("Temperature", String(random(0, 51)));   // 0-50 °C
	dataManager.push_data("Humidity",    String(random(0, 101)));  // 0-100 %
	dataManager.push_data("ErrorCode",   String(random(0, 256)));  // 0-255
	dataManager.push_data("PowerState",  String(random(0, 2)));    // 0 veya 1

	// ---- ReadWrite veriler ----
//	dataManager.push_data("OperatingMode", String(random(0, 6)));  // 0-5
//	dataManager.push_data("FanSpeed",      String(random(0, 7)));  // 0-6
//	dataManager.push_data("SleepMode",     String(random(0, 4)));  // 0-3

	delay(100);
}