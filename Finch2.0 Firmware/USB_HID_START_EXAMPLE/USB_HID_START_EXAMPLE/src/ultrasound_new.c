/*
 * ultrasound_new.c
 *
 * Created: 6/22/2017 1:32:54 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include "finch_adc.h"
#include "led_left_right.h"
#include "ultrasound_new.h"
#include "orb.h"
#include "battery.h"
volatile static uint32_t distance_16b=0;


void configure_pins()
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	// Set trigger pin to output, initialize to trigger pin to zero
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(TRIG_PIN, &config_port_pin);
	port_pin_set_output_level(TRIG_PIN, TRIG_PIN_LOW);
	
	//echo pin to input
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(ECHO_PIN, &config_port_pin);
}

void configure_tc4()
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT; //16
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64; //64
	//config_tc.counter_16_bit.value = 18661; // Overflows in 62.5ms
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;
	tc_init(&tc_instance4, TC4, &config_tc);
	tc_enable(&tc_instance4);
}

void configure_echo_extint()
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = ECHO_PIN_EIC;
	config_extint_chan.gpio_pin_mux       = ECHO_PIN_MUX;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_DOWN;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(ECHO_EIC_LINE, &config_extint_chan);
}

void setup_ultrasound()
{
	//Trigger High-Low 10us
	port_pin_set_output_level(TRIG_PIN, TRIG_PIN_HIGH);
	delay_us(100); 
	port_pin_set_output_level(TRIG_PIN, TRIG_PIN_LOW);
	//Initialize the counter
	tc_set_count_value(&tc_instance4, 0);
	//Enable the rising edge and overflow interrupt
	//tc_enable_callback(&tc_instance4, TC_CALLBACK_OVERFLOW);
}

void TC4_OV_callback(struct tc_module *const module_inst)
{
	//stop timer
	//tc_stop_counter(&tc_instance4);
	//
	setup_ultrasound();
	//start timer
	//tc_start_counter(&tc_instance4);
}


void new_echo_detection_callback(void)
{
	static bool interrupt_edge = true;
	static uint16_t one_minute_count = 0;
	static uint8_t low_battery_adc = 0;
	bool fade_increase = false;
	uint8_t i =0;
	//stop timer
	//tc_stop_counter(&tc_instance4);
	//disable echo interrupt
	
	//tc_set_count_value(&tc_instance4, 0);
	one_minute_count = one_minute_count + 1;
	serial_timeout_count = serial_timeout_count + 1;
	count_broadcast = count_broadcast + 1; 
	
	if(interrupt_edge == true )
	{
		tc_set_count_value(&tc_instance4, 0);
		interrupt_edge = false;
		if(serial_timeout_count > MAX_SERIAL_TIMEOUT)
		{
			serial_timeout = true;
			serial_timeout_count = 0;
		}
		if(one_minute_count > 690)
		{
			one_minute_count = 0;	 //One Minute 
			check_batterylevel_initial();
		}	
	}
	else
	{
		interrupt_edge = true;
		resource_distance_lock  = true;
		distance_16b = tc_get_count_value(&tc_instance4);
		resource_distance_lock = false;
	}
	//extint_chan_disable_callback(ECHO_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
	//read counter value
	//distance_16b = tc_get_count_value(&tc_instance4);
	//start timer
	//tc_start_counter(&tc_instance4);
}

char* get_ultrasound_distance()
{
	char* distance_ultrasound= NULL;
	distance_ultrasound= &distance_16b;
	return distance_ultrasound;
}

void configure_TC4_callback()
{
	tc_register_callback(&tc_instance4, TC4_OV_callback,TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance4, TC_CALLBACK_OVERFLOW);
}

void configure_ext_callback()
{
	extint_register_callback(new_echo_detection_callback,ECHO_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(ECHO_EIC_LINE,EXTINT_CALLBACK_TYPE_DETECT);
}

void configure_callbacks()
{
	configure_TC4_callback();
	configure_ext_callback();
	
}
void configure_modules()
{
	configure_tc4();
	configure_echo_extint();
}

void configure_ultrasound()
{
	configure_pins();
	configure_modules();
	configure_callbacks();
}