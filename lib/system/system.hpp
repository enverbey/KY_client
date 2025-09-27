#pragma once
#include "../best_wifi/best_wifi.hpp"
#include "../best_web_server/best_web_server.hpp"

// 0: OFF, 1: ON
extern uint8_t startStream;
extern uint32_t streamOnTimeout;
extern uint32_t streamOffTimeout;
extern std::atomic<unsigned long> last_data_push;

extern BestWiFi wifi;
extern	std::atomic<unsigned long> last_heart_beat;

void push_info_to_server();
void register_gateway();
void push_data_to_server();

// gateway'a register olmak için
void system_loop();
void system_setup();