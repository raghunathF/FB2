/*
 * power_enable.c
 *
 * Created: 6/8/2017 2:08:16 PM
 *  Author: raghunath
 */ 
#include <asf.h>
#include "power_enable.h" 


void enable_power_ic(void)
{
	port_pin_set_output_level(ENABLE_PIN_PWIC,ENABLE_PWIC);
	port_pin_set_output_level(ENABLE_PIN_5V,ENABLE_5V);
}


void configure_power_enable_pins(void)
{

	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(ENABLE_PIN_PWIC, &config_port_pin);
	port_pin_set_config(ENABLE_PIN_5V, &config_port_pin);


	
}