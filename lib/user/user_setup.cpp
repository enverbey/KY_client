#include "user.hpp"
#include "../best_data/best_data.hpp"

extern best_data dataManager;
void push_info_to_server();

void user_setup() {
    dataManager.add_data("UnitPower", "int", "1", "0", "0", AccessLevel:ReadWrite);
      // ——— Sadece okunabilir veriler ———
  dataManager.add_data("WANT SEX?", "int", "1",  "0",  "0",  AccessLevel::ReadOnly);
  dataManager.add_data("Temperature", "int", "50",  "0",  "24",  AccessLevel::ReadOnly);
  dataManager.add_data("Humidity",    "int", "100", "0",  "45",  AccessLevel::ReadOnly);
  dataManager.add_data("ErrorCode",   "int", "255", "0",  "0",   AccessLevel::ReadOnly);
  
  dataManager.add_data("PowerState",  "int", "1",   "0",  "1",   AccessLevel::ReadOnly);

  // Yeni sadece okunabilir veriler
  char powerStatusNames[6][20] = {
      "Off",
      "On",
      "",
      "",
      "",
      ""
  };
    dataManager.add_data("PowerStatus", "int", "1", "0", "0", AccessLevel::ReadWrite, powerStatusNames);
    dataManager.add_data("OperatingMode", "int", "5", "0", "5", AccessLevel::ReadWrite);
    dataManager.add_data("FanSpeed", "int", "6", "0", "0", AccessLevel::ReadWrite);
    
    char sleepModeNames[6][20] = {
        "Off",
        "Light Sleep",
        "Deep Sleep",
        "Ultra Deep Sleep",
        "",
        ""
    };
    dataManager.add_data("SleepMode", "int", "3", "0", "0", AccessLevel::ReadWrite, sleepModeNames);

  
    push_info_to_server();
}