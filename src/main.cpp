#include <Arduino.h>
#include <string.h>

#define BAUD_RATE 9600
#define COMMAND_LENGTH 4
#define DELAY_MS 500

enum State {
	LOCKED,
	ARMED,
	RELEASING,
	RETRACTING,
	FAULT,
};  

enum Command {
	LOCK,
	ARM,
	RELEASE,
	STOP,
	INVALID
}; 

enum State state;
char data [COMMAND_LENGTH + 1]; 
unsigned long previousMillis; 
unsigned long currentMillis;
int idx = 0;


void setup() {
	state = LOCKED;
	previousMillis = millis();
	Serial.begin(BAUD_RATE); 
}

void handle_command(Command cmd) {
	switch (cmd) {
		case LOCK:
			state = LOCKED;
			break;

		case ARM:
			if (state == LOCKED) {
				state = ARMED; 
			}
			break;

		case RELEASE:
			if (state == ARMED) {
				state = RELEASING;
			}
			break;

		case STOP:
			state = FAULT; 
			break;

		default:
			break;
	}
}

enum Command decode() {
	if (!strcmp(data, "LOCK")) {
		return LOCK;
	} else if (!strcmp(data, "ARMD")) {
		return ARM;
	} else if (!strcmp(data, "RELS")) {
		return RELEASE; 
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

	while (idx < COMMAND_LENGTH && Serial.available()) {
		int r = Serial.read(); 
		if (r < 0) { break; }
		char bit = (char)r;
		if (bit != '\n' && bit != '\r') {
			data[idx++] = bit; 
		} 
	}


	if (idx == COMMAND_LENGTH) {
		idx = 0; 
		// terminate string
		data[COMMAND_LENGTH] = '\0'; 

		// flush buffer at \n
		while (Serial.available()) {
			int r = Serial.read(); 
			char bit = (char)r;

			if (bit == '\n') {
				break;
			}
		}
		
		// decode command
		enum Command cmd = decode(); 

		// change state
		handle_command(cmd);

		// clear buffer
		clear_buffer(data, COMMAND_LENGTH + 1); 
	}

	currentMillis = millis(); 

	if (currentMillis - previousMillis >= DELAY_MS) {
		Serial.println(state); 
		previousMillis = currentMillis; 
	}
	
}

