#include <Arduino.h>
#include <string.h>

#define BAUD_RATE 9600
#define COMMAND_LENGTH 12

enum Command {
	LOCKED,
	ARMED,
	RELEASING,
	RELEASED, 
	STOP,
	INVALID
};  

enum Command mode;
char data [COMMAND_LENGTH]; 
int numBits; 

void setup() {
	mode = LOCKED;
	Serial.begin(BAUD_RATE); 
}

void switch_mode(Command command) {
	switch (command) {
		case LOCKED:
			mode = LOCKED;
			break;

		case ARMED:
			if (mode == LOCKED) {
				mode = ARMED; 
			}
			break;

		case RELEASING:
			if (mode == ARMED) {
				mode = RELEASING;
			}
			break;

		case RELEASED:
			if (mode == RELEASING) {
				mode = RELEASED;
			}
			break;

		case STOP:
			mode = STOP; 
			break;
			
		default:
			mode = INVALID;
			break;
	}
}

enum Command decode() {
	if (!strcmp(data, "LOCK")) {
		return LOCKED;
	} else if (!strcmp(data, "ARM")) {
		return ARMED;
	} else if (!strcmp(data, "RELS")) {
		return RELEASING; 
	} else if (!strcmp(data, "RELD")) {
		return RELEASED; 
	} else if (!strcmp(data, "STOP")) {
		return STOP; 
	} else {
		return INVALID;
	}
}

void clear_buffer(char* buffer, int size) {
	for (int i = 0; i < size; i++) {
		buffer[i] = '\0'; 
	}
}

void loop() {
	
	// if input detected parse it
	numBits = Serial.available(); 

	if (numBits > 0) {
		for (int i = 0; i < 4; i++) {
			char bit = Serial.read(); 
			if (bit != '\n' && bit != '\r') {
				data[i] = bit; 
			} 
		}
		enum Command command = decode(); 
		clear_buffer(data, COMMAND_LENGTH); 
		switch_mode(command);
	}
	

	Serial.println(mode); 
	delay(500);
}

