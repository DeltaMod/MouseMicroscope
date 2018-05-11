#include <Arduino.h>

byte testctr = 0;
unsigned long currTime;
unsigned long timer;
unsigned long pollTimer;
volatile byte movementflag = 0;

void adnsPowerUp();
void adns_display_registers();
long adns_get_Position();

void adns_frame_capture();
int adns_get_frameSize();
byte* adns_get_frame();

int convertTwosComplement(int b) {
	//Convert from 2's complement
	if (b & 0x80) {
		b = -1 * ((b ^ 0xff) + 1);
	}

	return b;
}

void setup() {
	Serial.begin(57600);
  
	Serial.println("; ADNS version 0.4 (57600 baud)");

	Serial.println("; Power Up.");

	adnsPowerUp();

	Serial.println("; Display Registers.");

	adns_display_registers();

	delay(1000);

	Serial.println("; Start Frame Capture.");

	// start frame capture.
	adns_frame_capture();

	Serial.println("READY");
}

void test_sensor_position() {
	long pos = adns_get_position();

	long x = pos >> 16;
	long y = pos & 0xffff;

	x = convertTwosComplement(x);
	y = convertTwosComplement(y);

	if (x != 0 || y != 0) {
		Serial.print("x = ");
		Serial.print(x);
		Serial.print(" | ");
		Serial.print("y = ");
		Serial.println(y);
	}
}

void test_frame_capture() {
	byte* frame = adns_get_frame();

	int frameSize = adns_get_frame_size();
        
	for (int i = 0; i < frameSize; i++) {
		Serial.print(frame[i], DEC);

		if (i % 30 == 29) {
			Serial.println("");
		} else {
			Serial.print("\t");
		}
	}

	Serial.println("READY");
}

void loop() {
	if (Serial.available()) {
		Serial.read();

		test_frame_capture();
	}
}

void loop1() {
	currTime = millis();

	if (currTime > timer) {
		Serial.println(testctr++);
		timer = currTime + 200;
	}

	if (currTime > pollTimer) {
		test_frame_capture();

		pollTimer = currTime + 10;
	}
}



