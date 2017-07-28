/*
 * accel.c
 *
 * Created: 3/3/2016 2:05:50 PM
 *  Author: Tom
 */ 

#include <asf.h>
#include <i2c_common.h>
#include <i2c_master.h>
#include "accel.h"



void config_accel(void)
{
	unsigned char bufferSend[4];
	
	// Make sure our accidental i2c port is turned off
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(PIN_PA23, &config_port_pin);
	port_pin_set_config(PIN_PA22, &config_port_pin);
	
	/* Initialize config structure and software module. */
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	/* Change buffer timeout to something longer. */
	config_i2c_master.buffer_timeout = 40000;
	// Change to 400 KHz
	config_i2c_master.baud_rate        = I2C_MASTER_BAUD_RATE_100KHZ;
	config_i2c_master.start_hold_time  = I2C_MASTER_START_HOLD_TIME_400NS_800NS;
	/* Initialize and enable device with config. */
	do {
		value = i2c_master_init(&i2c_master_instance, SERCOM4, &config_i2c_master);
	}
	while(value != STATUS_OK); 
	
	i2c_master_enable(&i2c_master_instance);
	
	
	
	 	
	// Start by putting the accelerometer in standby mode
	bufferSend[0] = 0x00000000;
	bufferSend[1] = 0b00000110;  // set interrupt to push-pull, mode to standby (mode and Ton are both 0), auto-sleep to enable
	bufferSend[2] = 0b00000000;
	bufferSend[3] = 0b00000000;
	write_data(bufferSend, 4);
	
	/* 
	// Set sleep mode to off by setting sleep counter to 0
	bufferSend[0] = 0x05;
	bufferSend[1] = 0b00000000;
	write_data(bufferSend, 2);
	 
	// Set interrupts for tap and shake detection
	bufferSend[0] = 0x06;
	bufferSend[1] = 0b11100100;  // set interrupts to occur with a tap or shake (PDINT, SHINTX/Y/Z)
	write_data(bufferSend, 2);
	 
	// Set filter rate to 8 samples, set number of samples to 120 in waking state, 32 in sleeping (hopefully we avoid sleep)
	bufferSend[0] = 0x08;
	bufferSend[1] = 0b11100000;
	write_data(bufferSend, 2);
	 
	// Set tap detection thresholds - 8 counts, and only detect taps on z-axis
	bufferSend[0] = 0x09;
	bufferSend[1] = 0b01101000;
	write_data(bufferSend, 2);


	// Set tap debounce to 16 detections, roughly 8 ms
	bufferSend[0] = 0x0A;
	bufferSend[1] = 0b00010000;
	write_data(bufferSend, 2);
	 
	// Change acclerometer mode to active waking state (enable it), enable push/pull on interrupt line
	bufferSend[0] = 0x07;
	bufferSend[1] = 0b01010001;
	write_data(bufferSend, 2);
	*/
	get_accel_data();
}

void get_accel_data(void)
{
	
	struct i2c_master_packet packet = {
		.address     = ACCEL_ADDRESS,//_R,
		.data_length = 4,
		.data        = bufferReceive,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code  = 0x0,
	};
	/*
	unsigned char bufferSend[1];
	bufferSend[0] = 0x00;
	write_data(bufferSend, 1);
	*/
	// Read the resulting four bytes (x, y, z, and status)
	while(i2c_master_read_packet_wait_no_stop(&i2c_master_instance, &packet) !=
	STATUS_OK);
	
}

// Helper function to just execute a simple write
void write_data(uint8_t *bufferSend, uint8_t length)
{
	struct i2c_master_packet packet = {
		.address     = ACCEL_ADDRESS,
		.data_length = length,
		.data        = bufferSend,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code  = 0x0,
	};
	do {
		calc_baud = i2c_master_write_packet_wait(&i2c_master_instance, &packet);
		
	}
	while (calc_baud != STATUS_OK);
	
}