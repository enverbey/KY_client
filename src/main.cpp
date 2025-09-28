#include <Arduino.h>
#include "../lib/system/system.hpp"
#include "../lib/user/user.hpp"

void setup() {
	system_setup();
	user_setup();
}

void loop() {	
	system_loop();
	user_loop();
}