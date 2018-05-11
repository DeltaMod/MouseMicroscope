#include <SPI.h>
#include <avr/pgmspace.h>

// Registers
#define REG_Product_ID                           0x00
#define REG_Revision_ID                          0x01
#define REG_Motion                               0x02
#define REG_Delta_X_L                            0x03
#define REG_Delta_X_H                            0x04
#define REG_Delta_Y_L                            0x05
#define REG_Delta_Y_H                            0x06
#define REG_SQUAL                                0x07
#define REG_Pixel_Sum                            0x08
#define REG_Maximum_Pixel                        0x09
#define REG_Minimum_Pixel                        0x0a
#define REG_Shutter_Lower                        0x0b
#define REG_Shutter_Upper                        0x0c
#define REG_Frame_Period_Lower                   0x0d
#define REG_Frame_Period_Upper                   0x0e
#define REG_Configuration_I                      0x0f
#define REG_Configuration_II                     0x10
#define REG_Frame_Capture                        0x12
#define REG_SROM_Enable                          0x13
#define REG_Run_Downshift                        0x14
#define REG_Rest1_Rate                           0x15
#define REG_Rest1_Downshift                      0x16
#define REG_Rest2_Rate                           0x17
#define REG_Rest2_Downshift                      0x18
#define REG_Rest3_Rate                           0x19
#define REG_Frame_Period_Max_Bound_Lower         0x1a
#define REG_Frame_Period_Max_Bound_Upper         0x1b
#define REG_Frame_Period_Min_Bound_Lower         0x1c
#define REG_Frame_Period_Min_Bound_Upper         0x1d
#define REG_Shutter_Max_Bound_Lower              0x1e
#define REG_Shutter_Max_Bound_Upper              0x1f
#define REG_LASER_CTRL0                          0x20
#define REG_Observation                          0x24
#define REG_Data_Out_Lower                       0x25
#define REG_Data_Out_Upper                       0x26
#define REG_SROM_ID                              0x2a
#define REG_Lift_Detection_Thr                   0x2e
#define REG_Configuration_V                      0x2f
#define REG_Configuration_IV                     0x39
#define REG_Power_Up_Reset                       0x3a
#define REG_Shutdown                             0x3b
#define REG_Inverse_Product_ID                   0x3f
#define REG_Motion_Burst                         0x50
#define REG_SROM_Load_Burst                      0x62
#define REG_Pixel_Burst                          0x64
#define ADNS_INIT_COMPLETE                       9
#define ADNS_FRAME_SIZE                          900

//const unsigned short adnsFirmwareLength = 3070;
const int adnsSlaveSelectPinNo = 10;
byte adnsInitComplete = 0;
byte adns_frame_data[900];

extern prog_uchar adnsFirmwareData[];

void adns_com_begin() {
	digitalWrite(adnsSlaveSelectPinNo, LOW);
}

void adns_com_end() {
	digitalWrite(adnsSlaveSelectPinNo, HIGH);
}

byte adns_read_reg(byte address, bool last = true) {
	adns_com_begin();

	// send address of the register, with MSBit = 0 to indicate it's a read
	SPI.transfer(address & 0x7f);
	delayMicroseconds(100); // tSRAD
	// read data
	byte data = SPI.transfer(0);

	if (last) {
		delayMicroseconds(1); // tSCLK-NCS for read operation is 120ns
		adns_com_end();
		delayMicroseconds(19); //  tSRW/tSRR (=20us) minus tSCLK-NCS
	}

	return data;
}

void adns_write_reg(byte address, byte data, bool last = true) {
	adns_com_begin();

	//send address of the register, with MSBit = 1 to indicate it's a write
	SPI.transfer(address | 0x80);
	//sent data
	SPI.transfer(data);

	if (last) {
		delayMicroseconds(20); // tSCLK-NCS for write operation
		adns_com_end();
		delayMicroseconds(100); // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be shortened, but is looks like a safe lower bound
	}
}

void adns_upload_firmware() {
	// send the firmware to the chip, cf p.18 of the datasheet
	Serial.println("; Uploading firmware...");

	//	1. Select the 3 K bytes SROM size at Confiuration_IV register, address 0x39

	// set the configuration_IV register in 3k firmware mode
	adns_write_reg(REG_Configuration_IV, 0x02); // bit 1 = 1 for 3k mode, other bits are reserved

	//	2. Write 0x1d to SROM_Enable register for initializing

	// write 0x1d in SROM_enable reg for initializing
	adns_write_reg(REG_SROM_Enable, 0x1d);

	//	3. Wait for one frame

	// wait for more than one frame period
	delay(5); // assume that the frame rate is as low as 100fps... even if it should never be that low

	//	4. Write 0x18 to SROM_Enable register again to start SROM downloading

	// write 0x18 to SROM_enable to start SROM download
	adns_write_reg(REG_SROM_Enable, 0x18);

	//	5. Write SROM fie into SROM_Load_Burst register, 1 st data must start with SROM_Load_Burst register address.
	//     All the SROM data must be downloaded before SROM start running.

	// write the SROM file (=firmware data)
	adns_com_begin();
	SPI.transfer(REG_SROM_Load_Burst | 0x80); // write burst destination adress
	delayMicroseconds(15);

	// send all bytes of the firmware
	unsigned char c;
	for (unsigned short i = 0; i < adnsFirmwareLength; i++) {
		c = (unsigned char) pgm_read_byte(adnsFirmwareData + i);
		SPI.transfer(c);
		delayMicroseconds(15);
	}

	adns_com_end();
}

void adnsPowerUp() {
	// set pin i/o direction.
	pinMode(adnsSlaveSelectPinNo, OUTPUT);

	// set spi communication.
	SPI.begin();
	SPI.setDataMode(SPI_MODE3);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(64);

	//	1. Apply power to VDD5/VDD3 and VDDIO in any order
	//	2. Drive NCS high, and then low to reset the SPI port.

	adns_com_end(); // ensure that the serial port is reset
	adns_com_begin(); // ensure that the serial port is reset
	adns_com_end(); // ensure that the serial port is reset

	//	3. Write 0x5a to Power_Up_Reset register (address 0x3a).

	adns_write_reg(REG_Power_Up_Reset, 0x5a); // force reset

	//	4. Wait for at least 50ms time.

	delay(50); // wait for it to reboot

	//	5. Read from registers 0x02, 0x03, 0x04, 0x05 and 0x06 (or read these same 5 bytes from burst motion register) one time
	//	   regardless of the motion pin state.

	// read registers 0x02 to 0x06 (and discard the data)
	adns_read_reg(REG_Motion);
	adns_read_reg(REG_Delta_X_L);
	adns_read_reg(REG_Delta_X_H);
	adns_read_reg(REG_Delta_Y_L);
	adns_read_reg(REG_Delta_Y_H);

	//	6. SROM download.

	// upload the firmware
	adns_upload_firmware();

	delay(5);

	//	7. Enable laser by setting Forced_Disable bit (Bit-0) of LASER_CTRL0 register (address 0x20) to 0.

	//enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
	// reading the actual value of the register is important because the real
	// default value is different from what is said in the datasheet, and if you
	// change the reserved bytes (like by writing 0x00...) it would not work.
	byte laser_ctrl0 = adns_read_reg(REG_LASER_CTRL0);
	adns_write_reg(REG_LASER_CTRL0, laser_ctrl0 & 0xf0);

	delay(1);

	Serial.println("; Optical Chip Initialized.");

	adnsInitComplete = ADNS_INIT_COMPLETE;
}

void adns_frame_capture() {
	//	1. Reset the chip by writing 0x5a to Power_Up_Reset register (address 0x3a).

	Serial.println("; Step 1 - Force Reset.");

	adns_write_reg(REG_Power_Up_Reset, 0x5a); // force reset

	delay(50); // wait for it to reboot

	//	2. Enable laser by setting Forced_Disable bit (Bit-0) of LASER_CTRL) register to 0.

	Serial.println("; Step 2 - Enabling Laser.");

	//enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
	// reading the actual value of the register is important because the real
	// default value is different from what is said in the datasheet, and if you
	// change the reserved bytes (like by writing 0x00...) it would not work.
	byte laser_ctrl0 = adns_read_reg(REG_LASER_CTRL0);
	adns_write_reg(REG_LASER_CTRL0, laser_ctrl0 & 0xf0);

	delay(1);
}

int adns_get_frame_size() {
	return ADNS_FRAME_SIZE;
}

byte* adns_get_frame() {
	//	3. Write 0x93 to Frame_Capture register (address 0x12).

	Serial.println("; Step 3.");

	adns_write_reg(REG_Frame_Capture, 0x93);

	//	4. Write 0xc5 to Frame_Capture register.

	Serial.println("; Step 4.");

	adns_write_reg(REG_Frame_Capture, 0xc5);

	//	5. Wait for two frames.

	Serial.println("; Step 5.");

	// wait for more than one frame period
	delay(10); // assume that the frame rate is as low as 100fps... even if it should never be that low

	//	6. Check for fist pixel by reading bit zero of Motion register. If = 1, fist pixel is available.

	Serial.println("; Step 6.");

	byte firstByte;

	do {
		firstByte = adns_read_reg(REG_Motion, false);
	} while ((firstByte & 1) == 0);

	delayMicroseconds(100);

	//	7. Continue read from Pixel_Burst register until all 900 pixels are transferred.

	SPI.transfer(REG_Pixel_Burst);
	delayMicroseconds(100); // tSRAD

	for (int i = 0; i < ADNS_FRAME_SIZE; i++) {
		adns_frame_data[i] = SPI.transfer(0);
		delayMicroseconds(15);
	}

	adns_com_end(); // ensure that the serial port is reset

	//	8. Continue step 3-7 to capture another frame.

	return adns_frame_data;
}

void adns_display_registers() {
	typedef struct _tagRegInfo {
		int address;
		const char* name;
	} RegInfo;

	RegInfo regInfo[] = {
			{ 0x00, "Product_ID                  " },	//
			{ 0x0F, "Coinfiguration_I            " },	//
			{ 0x10, "Coinfiguration_II           " },	//
			{ 0x1a, "Frame_Period_Max_Bound_Lower" },	//
			{ 0x1b, "Frame_Period_Max_Bound_Upper" },	//
			{ 0x1c, "Frame_Period_Min_Bound_Lower" },	//
			{ 0x1d, "Frame_Period_Min_Bound_Upper" },	//
			{ 0x1e, "Shutter_Max_Bound_Lower     " },	//
			{ 0x1f, "Shutter_Max_Bound_Upper     " },	//
			{ 0x2A, "SROM_Version                " },	//
			{ 0x3F, "Inverse_Product_ID          " },	//
			{ 0x02, "Motion                      " },       //    
                        { 0x20, "REG_LASER_CTRL0             " }};

	digitalWrite(adnsSlaveSelectPinNo, LOW);

	int index = 0;
	for (index = 0; index < 13; index++) {
		SPI.transfer(regInfo[index].address);
		delay(1);
		Serial.print("; ");
		Serial.print(regInfo[index].address, HEX);
		Serial.print(",\t");
		Serial.print(regInfo[index].name);
		Serial.print(" =\t");
		byte value = SPI.transfer(0);
		Serial.print(value, HEX);
		Serial.print(",\t");
		Serial.println(value, BIN);
		delay(1);
	}

	digitalWrite(adnsSlaveSelectPinNo, HIGH);
}

long adns_get_position() {
	long pos = 0;

	if (adnsInitComplete == ADNS_INIT_COMPLETE) {
		digitalWrite(adnsSlaveSelectPinNo, LOW);
		pos += (int) adns_read_reg(REG_Delta_X_L);
		pos <<= 16;
		pos += (int) adns_read_reg(REG_Delta_Y_L);
		digitalWrite(adnsSlaveSelectPinNo, HIGH);
	}

	return pos;
}
    

