/*
 * orb.c
 *
 * Created: 2/29/2016 11:00:50 AM
 *  Author: Tom
 */ 

#include <asf.h>
#include "orb.h"

//Should be transferred later on 
#define MIN_INTENSITY 255 



bool motor_enable = false;
uint8_t old_left_dir = 1;
uint8_t old_right_dir = 1;
//bool blue_on = false;

void config_led(void)
{
	
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC1);
	
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1024;
	config_tcc.counter.period = 0x00FF;
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	
	
	//PIN_PA07E_TCC1_WO0 is the the output PA07 on which Red LED is connected
	//PINMUX_PA07E_TCC1_WO0 is the mux which sets the PA07 output
	config_tcc.compare.match[LRED_CH] = 255;
	config_tcc.pins.enable_wave_out_pin[LRED_WO] = true;
	config_tcc.pins.wave_out_pin[LRED_WO]        = PIN_PA07E_TCC1_WO1;
	config_tcc.pins.wave_out_pin_mux[LRED_WO]    = PINMUX_PA07E_TCC1_WO1;
	
	//PIN_PA06E_TCC1_WO0 is the the output PA06 on which Green LED is connected
	//PINMUX_PA06E_TCC1_WO0 is the mux which sets the PA06 output
	config_tcc.compare.match[LGREEN_CH] = 255;
	config_tcc.pins.enable_wave_out_pin[LGREEN_WO] = true;
	config_tcc.pins.wave_out_pin[LGREEN_WO]        = PIN_PA06E_TCC1_WO0;
	config_tcc.pins.wave_out_pin_mux[LGREEN_WO]    = PINMUX_PA06E_TCC1_WO0;
	
	
	tcc_init(&tcc_instance1, TCC1, &config_tcc);
	tcc_enable(&tcc_instance1);
	
	
	//Setting Right Blue LED and Left Blue LED as outputs and initializing them to zero
	
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(RBLUE_PIN, &config_port_pin);
	port_pin_set_config(LBLUE_PIN, &config_port_pin);
	port_pin_set_output_level(RBLUE_PIN,RBLUE_ENABLE);
	port_pin_set_output_level(LBLUE_PIN,LBLUE_ENABLE);
	
}

void set_led_left(uint8_t red, uint8_t green, uint8_t blue)
{
	red   = MIN_INTENSITY - red;
	green = MIN_INTENSITY - green;
	blue  = MIN_INTENSITY - blue;
	 
	tcc_set_compare_value(&tcc_instance1, LRED_CH, red);
	tcc_set_compare_value(&tcc_instance1, LGREEN_CH, green);
	tcc_set_compare_value(&tcc_instance0, LBLUE_CH,  blue);
}

void set_led_right(uint8_t red, uint8_t green, uint8_t blue)
{
	red   = MIN_INTENSITY - red;
	green = MIN_INTENSITY - green;
	blue  = MIN_INTENSITY - blue;
	
	tcc_set_compare_value(&tcc_instance0, RRED_CH, red);
	tcc_set_compare_value(&tcc_instance0, RGREEN_CH, green);
	tcc_set_compare_value(&tcc_instance0, RBLUE_CH, blue);
}


static void tcc_callback_pwm_rb(
struct tcc_module *const module_inst)
{
	port_pin_set_output_level(RBLUE_PIN,RBLUE_DISABLE);
}



static void tcc_callback_pwm_lb(
struct tcc_module *const module_inst)
{
	port_pin_set_output_level(LBLUE_PIN,LBLUE_DISABLE);
}

static void tcc_callback_pwm_of(
struct tcc_module *const module_inst)
{
	port_pin_set_output_level(LBLUE_PIN,LBLUE_ENABLE);
	port_pin_set_output_level(RBLUE_PIN,RBLUE_ENABLE);
}


void configure_tcc_callbacks(void)
{
	//Overflow Interrupt
	//! [setup_register_callback]
	tcc_register_callback(
	&tcc_instance0,
	tcc_callback_pwm_of,
	(enum tcc_callback)(TCC_CALLBACK_OVERFLOW));
	
	//! [setup_enable_callback]
	tcc_enable_callback(&tcc_instance0,
	(enum tcc_callback)(TCC_CALLBACK_OVERFLOW));
	

	//! [setup_enable_callback]
	//#define LBLUE_CH 0
	//! [setup_register_callback]
	tcc_register_callback(
	&tcc_instance0,
	tcc_callback_pwm_lb,
	(enum tcc_callback)(TCC_CALLBACK_CHANNEL_0 + 0));
	//! [setup_register_callback]
	//! [setup_enable_callback]
	tcc_enable_callback(&tcc_instance0,
	(enum tcc_callback)(TCC_CALLBACK_CHANNEL_0 + 0));
	//! [setup_enable_callback]
	
	//#define RBLUE_CH 3
	//! [setup_register_callback]
	tcc_register_callback(
	&tcc_instance0,
	tcc_callback_pwm_rb,
	(enum tcc_callback)(TCC_CALLBACK_CHANNEL_3 + 0));
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tcc_enable_callback(&tcc_instance0,
	(enum tcc_callback)(TCC_CALLBACK_CHANNEL_3 + 0));
	//! [setup_enable_callback]
	
}
//! [setup]


void config_motors(void)
{
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC0);
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1024;
	config_tcc.counter.period = 0x00FF;
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc.compare.wave_polarity[LEFT_MOTOR_CH] = TCC_WAVE_POLARITY_0;
	config_tcc.compare.wave_polarity[RIGHT_MOTOR_CH] = TCC_WAVE_POLARITY_0;
	config_tcc.compare.match[LEFT_MOTOR_CH] = 0;
	config_tcc.pins.enable_wave_out_pin[LEFT_MOTOR_WO] = true;
	config_tcc.pins.wave_out_pin[LEFT_MOTOR_WO]        = PIN_PA05E_TCC0_WO1; //this is no longer correct--- should be now
	config_tcc.pins.wave_out_pin_mux[LEFT_MOTOR_WO]    = PINMUX_PA05E_TCC0_WO1; // this is no longer correct --- should be now
	config_tcc.compare.match[RIGHT_MOTOR_CH] = 0;
	config_tcc.pins.enable_wave_out_pin[RIGHT_MOTOR_WO] = true;
    config_tcc.pins.wave_out_pin[RIGHT_MOTOR_WO]        = PIN_PA04E_TCC0_WO0; // this is no longer correct --- should be now
	config_tcc.pins.wave_out_pin_mux[RIGHT_MOTOR_WO]    = PINMUX_PA04E_TCC0_WO0; // this is no longer correct --- should be now
	
	tcc_init(&tcc_instance0, TCC0, &config_tcc);
	tcc_enable(&tcc_instance0);
	//tcc_enable(&tcc_instance0);
	// Ugly, but this configures the blue part of the left full color LED and all of the right one
	//config_tcc.compare.match[LBLUE_CH] = 0;
	//config_tcc.pins.enable_wave_out_pin[LBLUE_WO] = true;
	//config_tcc.pins.wave_out_pin[LBLUE_WO]        = PIN_PA17F_TCC0_WO7; // this is no longer correct - should do sw control for LEDs
	//config_tcc.pins.wave_out_pin_mux[LBLUE_WO]    = PINMUX_PA17F_TCC0_WO7; // this is no longer correct - should do sw control for LEDs
	
	
	//config_tcc.compare.match[RRED_CH] = 1;
	//config_tcc.pins.enable_wave_out_pin[RRED_WO] = true;
	//config_tcc.pins.wave_out_pin[RRED_WO]        = PIN_PB11F_TCC0_WO5;
	//config_tcc.pins.wave_out_pin_mux[RRED_WO]    = PINMUX_PB11F_TCC0_WO5;
	
	/*
	config_tcc.compare.match[RGREEN_CH] = 255;
	config_tcc.pins.enable_wave_out_pin[RGREEN_WO] = true;
	config_tcc.pins.wave_out_pin[RGREEN_WO]        = PIN_PB10F_TCC0_WO4;
	config_tcc.pins.wave_out_pin_mux[RGREEN_WO]    = PINMUX_PB10F_TCC0_WO4;
	*/
	

	
	//config_tcc.compare.match[RBLUE_CH] = 0;
	
	//config_tcc.pins.enable_wave_out_pin[RBLUE_WO] = true;
	//config_tcc.pins.wave_out_pin[RBLUE_WO]        = PIN_PA05E_TCC0_WO1;
	//config_tcc.pins.wave_out_pin_mux[RBLUE_WO]    = PINMUX_PA05E_TCC0_WO1;
	
	/*
	// Setup DIO for interfacing with motor driver
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);

	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	// set fault to input
	port_pin_set_config(PIN_PA28, &config_port_pin);
	
	// Set PWM pins to output
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA20, &config_port_pin);
	port_pin_set_config(PIN_PA21, &config_port_pin);

	// Set Nsleep to output, then set it low to keep the chip in sleep mode until we actually use it
	port_pin_set_config(PIN_PA27, &config_port_pin);
	// disable the chip and set motor enable to false for now
	port_pin_set_output_level(PIN_PA27, false);
	port_pin_set_output_level(PIN_PA20, true);
	port_pin_set_output_level(PIN_PA21, true);
	motor_enable = false;
	*/
	
}






// PWM is on BIN1/BIN2 - PA13 and PA21
// Depending on direction, set other pin to standard output and set it to low
void set_motor_left(uint8_t dir, uint8_t speed)
{
	if(motor_enable == false)
	{
		// enable the chip
		port_pin_set_output_level(PIN_PA27, true);
		
		// set flag to true
		motor_enable = true;
	}
	if(old_left_dir != dir)
	{
		struct system_pinmux_config pin_config;
		struct port_config config_port_pin;
		config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
		system_pinmux_get_config_defaults(&pin_config);
		pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
		// need to configure PWM with opposite pins
		if(dir == 0)
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA21F_TCC0_WO7;
			system_pinmux_pin_set_config(PIN_PA21F_TCC0_WO7, &pin_config);
			port_pin_set_config(PIN_PA13, &config_port_pin);
			port_pin_set_output_level(PIN_PA13, true);
		}
		else
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA13F_TCC0_WO7;
			system_pinmux_pin_set_config(PIN_PA13F_TCC0_WO7, &pin_config);
			port_pin_set_config(PIN_PA21, &config_port_pin);
			port_pin_set_output_level(PIN_PA21, true);
		}
	}
	tcc_set_compare_value(&tcc_instance0, LEFT_MOTOR_CH, speed);
	//tcc_enable(&tcc_instance0);
	old_left_dir = dir;
}

// PWM is on AIN1/AIN2 - PA12 and PA20
void set_motor_right(uint8_t dir, uint8_t speed)
{
	if(motor_enable == false)
	{
		// enable the chip
		port_pin_set_output_level(PIN_PA27, true);
		// enable PWM
		// set flag to true
		motor_enable = true;
	}
	if(old_right_dir != dir)
	{
		struct system_pinmux_config pin_config;
		struct port_config config_port_pin;
		pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
		config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
		system_pinmux_get_config_defaults(&pin_config);
		// need to configure PWM with opposite pins
		if(dir == 0)
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA20F_TCC0_WO6;
			system_pinmux_pin_set_config(PIN_PA20F_TCC0_WO6, &pin_config);
			port_pin_set_config(PIN_PA12, &config_port_pin);
			port_pin_set_output_level(PIN_PA12, true);
		}
		else
		{
			pin_config.mux_position = (uint8_t)PINMUX_PA12F_TCC0_WO6;
			system_pinmux_pin_set_config(PIN_PA12F_TCC0_WO6, &pin_config);
			port_pin_set_config(PIN_PA20, &config_port_pin);
			port_pin_set_output_level(PIN_PA20, true);
		}
	}
	tcc_set_compare_value(&tcc_instance0, RIGHT_MOTOR_CH, speed);
	old_right_dir = dir;
}

void turn_off_motors()
{
	
	// disable the chip to save power - this is no longer an option as we needed the sleep mode i/o pin for something else
	// port_pin_set_output_level(PIN_PA27, false); 
	// Turn PWM to 0, no need to turn everything off
	set_motor_left(1,0);
	set_motor_right(1,0);
	
	// Set flag to false
	motor_enable = false;
}