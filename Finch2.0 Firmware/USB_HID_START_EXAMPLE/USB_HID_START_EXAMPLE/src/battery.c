/*
 * battery.c
 *
 * Created: 7/27/2017 1:47:23 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "finch_adc.h"
#include "led_left_right.h"
#include "ultrasound_new.h"
#include "orb.h"
#include "battery.h"


void configure_batt_status_pins()
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_DOWN;
	port_pin_set_config(BATT_STATUS , &config_port_pin);
}

void check_batterylevel_initial()
{
	uint8_t low_battery_adc = 0;
	uint8_t i=0;
	bool fade_increase = false;
	low_battery_adc = adc_start_read_result(BATT_MTR);
	while(low_battery_adc < LOW_BATTERY_THRESHOLD)
	{
		low_battery_adc = adc_start_read_result(BATT_MTR);
		if(fade_increase == false)
		{
			set_led_left_new(RED_MAX-i, 0, 0);
			set_led_right_new(RED_MAX-i,0, 0);
			i++;
			if(i > RED_MAX)
			{
				i=0;
				fade_increase = true;
			}
			
		}
		else
		{
			set_led_left_new(i, 0, 0);
			set_led_right_new(i,0, 0);
			i++;
			if(i > RED_MAX)
			{
				i=0;
				fade_increase = false;
			}
			
		}
		delay_cycles_ms(3);
		//motors_stop();
		//turn_off_motors();
	}
	set_led_left_new(20, 20, 20);
	
}

void configure_batt_status()
{
	configure_batt_status_pins();
}