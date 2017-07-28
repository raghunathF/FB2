/*
 * ultrasound.c
 *
 * Created: 12/2/2016 11:14:16 AM
 *  Author: Tom
 */ 

#include <asf.h>
#include "ultrasound.h"

void configure_tc3(void)
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	// Set trigger pin to output, echo pin to input
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(TRIG_PIN, &config_port_pin);
	
	port_pin_set_config(PIN_PB03, &config_port_pin);
	port_pin_set_output_level(TRIG_PIN, false);
	
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(ECHO_PIN, &config_port_pin);
	
	
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT; //16
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64; //64
	config_tc.counter_16_bit.value = 18661; // Overflows in 62.5ms
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;

	tc_init(&tc_instance3, TC3, &config_tc);
	tc_enable(&tc_instance3);
	tc_register_callback(&tc_instance3, tc_callback_trig,
	TC_CALLBACK_OVERFLOW);
	
	configure_echo_channel();
	configure_extint_callbacks();
	tc_enable_callback(&tc_instance3, TC_CALLBACK_OVERFLOW);
}

void configure_echo_channel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = ECHO_PIN_EIC;
	config_extint_chan.gpio_pin_mux       = ECHO_PIN_MUX;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_DOWN;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(ECHO_EIC_LINE, &config_extint_chan);
}

void configure_extint_callbacks(void)
{
	extint_register_callback(echo_detection_callback,
	ECHO_EIC_LINE,
	EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(ECHO_EIC_LINE,
	EXTINT_CALLBACK_TYPE_DETECT);
}
void echo_detection_callback(void)
{
	bool pin_state = port_pin_get_input_level(ECHO_PIN);
	if(pin_state)
	{
		rising_edge = tc_get_count_value(&tc_instance3);
	}
	else
	{
		pulse_width = tc_get_count_value(&tc_instance3) - rising_edge;
	}
}

void tc_callback_trig(
struct tc_module *const module_inst)
{
	tc_set_count_value(&tc_instance3, 18661);
	port_pin_set_output_level(TRIG_PIN, true);
	delay_ms(80); //this should actually be a 10 us delay because delay function is mysteriously misconfigured
	port_pin_set_output_level(TRIG_PIN, false);
}

