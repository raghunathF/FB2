/*
 * ir.c
 *
 * Created: 3/1/2016 11:04:06 AM
 *  Author: Tom
 */ 

#include <asf.h>
#include "ir.h"

void config_ir(void)
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(OBSTACLE_SENSE, &config_port_pin);

	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;

	port_pin_set_config(LEFT_IR, &config_port_pin);
	port_pin_set_config(RIGHT_IR, &config_port_pin);
	
	port_pin_set_output_level(LEFT_IR, false);
	port_pin_set_output_level(RIGHT_IR, false);
	
	// Need to go interrupt based like the old Finch, but measure the receive high pulse length instead of just whether the receiver is high or low
	
}

uint8_t read_left_ir(void)
{
	return port_pin_get_input_level(OBSTACLE_SENSE);
}

uint8_t read_right_ir(void)
{
	return port_pin_get_input_level(OBSTACLE_SENSE);
}
