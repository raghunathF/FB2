/*
 * encoders.c
 *
 * Created: 7/3/2017 8:45:15 AM
 *  Author: RaghunathReddy Jangam
 */ 

#include <asf.h>
#include <i2c_common.h>
#include <i2c_master.h>
#include "encoders.h"

static uint8_t bufferReceive_encoder[6];

void get_data_encoders(struct i2c_master_module instance)
{ 
	struct i2c_master_packet packet = {
		.address     = ENCODER_ADD,
		.data_length = 6,
		.data        = bufferReceive_encoder,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code  = 0x0,
	};
	
	// Read all the registers of the encoder
	while(i2c_master_read_packet_wait_no_stop(&instance, &packet) != STATUS_OK);
	
}

// Helper function to just execute a simple write
void write_data_encoders(uint8_t *bufferSend, uint8_t length, struct i2c_master_module *const module)
{
	struct i2c_master_packet packet = {
		.address     = ENCODER_ADD,
		.data_length= length,
		.data        = bufferSend,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code  = 0x0,
	};
	do {
		error_code = i2c_master_write_packet_wait(module, &packet);
		
	}
	while (error_code != STATUS_OK);
	
}

void config_I2C_SERCOM4()
{
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 40000;
	// Fast mode Baud rate
	config_i2c_master.baud_rate        = I2C_MASTER_BAUD_RATE_400KHZ;
	config_i2c_master.start_hold_time  = I2C_MASTER_START_HOLD_TIME_400NS_800NS;
	/* Initialize and enable device with config. */
	do 
	{
		error_code = i2c_master_init(&i2c_master_encoder_right, RIGHT_ENCODER_SERCOMM_4, &config_i2c_master);
	}
	while(error_code != STATUS_OK);
	i2c_master_enable(&i2c_master_encoder_right);
}

void config_I2C_SERCOM3()
{
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout = 40000;
	// Fast mode Baud rate
	config_i2c_master.baud_rate        = I2C_MASTER_BAUD_RATE_400KHZ;
	config_i2c_master.start_hold_time  = I2C_MASTER_START_HOLD_TIME_400NS_800NS;
	/* Initialize and enable device with config. */
	do
	{
		error_code = i2c_master_init(&i2c_master_encoder_left, LEFT_ENCODER_SERCOMM_3, &config_i2c_master);
	}
	while(error_code != STATUS_OK);
	i2c_master_enable(&i2c_master_encoder_left);
}


void initialize_left_encoder()
{
	unsigned char bufferSend[4];
	// Disable the temperature sensor, Parity test disabled, enable the interrupt, FAST mode
	bufferSend[0] = 0b00000000;
	bufferSend[1] = 0b00000110;
	bufferSend[2] = 0b00000000;
	bufferSend[3] = 0b10000000;  
	write_data_encoders(bufferSend, 4 , &i2c_master_encoder_left);
	
}

void initialize_right_encoder()
{
	unsigned char bufferSend[4];
	// Disable the temperature sensor, Parity test disabled, enable the interrupt, FAST mode
	bufferSend[0] = 0b00000000;
	bufferSend[1] = 0b00000110;
	bufferSend[2] = 0b00000000;
	bufferSend[3] = 0b10000000;
	write_data_encoders(bufferSend, 4 , &i2c_master_encoder_right);
	
}

void config_left_encoder()
{
	config_I2C_SERCOM3(); //have to change
	initialize_left_encoder();
}

void config_right_encoder()
{
	config_I2C_SERCOM4(); 
	initialize_right_encoder();
	
}
void config_encoders_modules()
{
	config_right_encoder();
	config_left_encoder();
}
void configure_encoders()
{
	config_encoders_modules();
}

void read_encoders()
{
	
	volatile int16_t x_left,y_left,z_left;
	volatile int16_t temp;
	bool conversion_complete;
	
	x_left = 0;
	y_left = 0;
	z_left = 0;
	get_data_encoders(i2c_master_encoder_right);
	
	temp = bufferReceive_encoder[5] & 0x0010;
	conversion_complete = temp<<5;
	
	if(conversion_complete == 0 )
	{
		x_left = bufferReceive_encoder[0];
		x_left = x_left << 4 ;
		temp = bufferReceive_encoder[4] & 0x00F0;
		temp = temp >> 4;
		x_left = x_left + temp ;
		temp  = x_left & 0x0800;
		x_left = x_left & 0x07FF;
		if(temp > 0 )
		{
			x_left = x_left- 2048;
		}
	
		y_left = bufferReceive_encoder[1];
		y_left = y_left << 4 ;
		temp = bufferReceive_encoder[4] & 0x000F;
		y_left = y_left + temp ;
		temp  = y_left & 0x0800;
		y_left = y_left & 0x07FF;
		if(temp > 0 )
		{
			y_left = y_left- 2048;
		}
	
		z_left = bufferReceive_encoder[2];
		z_left = z_left << 4 ;
		temp = bufferReceive_encoder[5] & 0x000F;
		z_left = z_left + temp ;
		
		temp  = z_left & 0x0800;
		z_left = z_left & 0x07FF;
		if(temp > 0 )
		{
			z_left = z_left- 2048;
		}
	}
	
}