/*
 * led_left_right.c
 *
 * Created: 6/9/2017 11:37:15 AM
 *  Author: raghu
 */ 
#include <asf.h>
#include "led_left_right.h"



#define RGB_LED_ENABLE 1
#define RGB_LED_DISABLE 0
#define MAX_INTENSITY 255

#define VALUE_LED_OFF 0 

static uint8_t N_valid_compares = 6;

uint8_t time_array[6]; 
uint8_t time_array_ID;

uint8_t led_array[6];
uint8_t led_array_ID;

uint8_t temp_time_array[6]; 
uint8_t temp_led_array[6];

bool update_LED = false; 

void configure_tc(void);
void configure_tc_callbacks(void);
void tc_callback_to_toggle_led(
struct tc_module *const module_inst);
void transfer_temp();
void setup_output_levels_leds_off();
void setup_output_levels_leds_on();

struct tc_module tc_instance;

//Sad used a bubble sort , six elements so did not spend much time on writing a  beautiful code

//! [callback_funcs]
void tc_callback_PWM(
struct tc_module *const module_inst)
{
	//	tc_disable(&tc_instance);
	//	port_pin_toggle_output_level(PIN_PA07);
	//	tc_enable(&tc_instance);
	static uint8_t compare_value=0;
	static uint8_t compare_value_last=0;
	static uint8_t compare_value_current=0;
	static bool first_time = true;
	volatile uint32_t count;
	//count = tc_get_count_value(&tc_instance);
	//tc_stop_counter(&tc_instance);
	//count = count +1;
	//Enable the LED
	if(first_time == false)
	{
		port_pin_set_output_level(led_array[led_array_ID++],RGB_LED_DISABLE);
		 
		if(time_array_ID < N_valid_compares-1 )
		{
			compare_value_last = time_array[time_array_ID];
			compare_value_current = time_array[++time_array_ID];
			while(compare_value_last == compare_value_current && time_array_ID <= N_valid_compares - 1)
			{
				//Enable the LED
				port_pin_set_output_level(led_array[led_array_ID++],RGB_LED_DISABLE);
				compare_value_last = time_array[time_array_ID];
				compare_value_current = time_array[++time_array_ID];
			}
			if(compare_value_current != 255)
			{
				tc_set_inital_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value_last);
				tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value_current);
			}
			else
			{
				tc_set_inital_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value_last);
				tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
			}
		}
		else
		{
			tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
		}
		
	}
	else
	{
		first_time = false;
		tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
	}
	//tc_start_counter(&tc_instance);
	
}

void tc_callback_OF(
struct tc_module *const module_inst)
{
	//tc_stop_counter(&tc_instance);
	//tc_disable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	//port_pin_toggle_output_level(L_RLED_PIN);
	//tc_enable(&tc_instance);
	uint8_t compare_value=0;
	setup_output_levels_leds_off();
	//tc_disable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
	tc_clear_interrupts(&tc_instance);
	if(update_LED == true)
	{
		tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
		transfer_temp();
		update_LED = false;
	}
	time_array_ID = 0;
	led_array_ID  = 0;
	
	compare_value = time_array[0];
	if(compare_value != 255)
	{
		tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value);
	}
	else
	{
		tc_disable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
	}
	tc_set_inital_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
	
	//tc_start_counter(&tc_instance);
	//tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
}

void increasing_sort_tag()
{
	uint8_t i,j ,temp;
	uint8_t N=6;
	for(i=0; i< N-1 ;i++)
	{
		for(j=0;j< N-i-1;j++)
		{
			if(temp_time_array[j]>temp_time_array[j+1])
			{
				temp = temp_time_array[j];
				temp_time_array[j] = temp_time_array[j+1];
				temp_time_array[j+1]= temp;
				
				temp = temp_led_array[j];
				temp_led_array[j] = temp_led_array[j+1];
				temp_led_array[j+1] = temp;
				
			}
		}
	}
}

void transfer_temp()
{
	uint8_t i;
	N_valid_compares = 0;
	for(i=0;i<=5;i++)
	{
		if(temp_time_array[i] != 255)
		{
			N_valid_compares++;
		}
		time_array[i] = temp_time_array[i] ;
		led_array[i]  = temp_led_array[i];
	}
}

void initializing_led_pins()
{
	
	temp_led_array[0]  = PIN_PA07;
	temp_led_array[1]  = PIN_PA06;
	temp_led_array[2]  = PIN_PB22;
	temp_led_array[3]  = PIN_PB11;
	temp_led_array[4]  = PIN_PB10;
	temp_led_array[5]  = PIN_PA27;
}

void set_led_left_new(uint8_t red, uint8_t green, uint8_t blue)
{
	temp_time_array[0] = MAX_INTENSITY - red;
	temp_time_array[1] = MAX_INTENSITY - green;
	temp_time_array[2] = MAX_INTENSITY - blue;
	//initializing_led_pins();
	
	//increasing_sort_tag();
	//update_LED = true;

}

void set_led_right_new(uint8_t red, uint8_t green, uint8_t blue)
{
	temp_time_array[3] = MAX_INTENSITY - red;
	temp_time_array[4] = MAX_INTENSITY - green;
	temp_time_array[5] = MAX_INTENSITY- blue;
	initializing_led_pins();
	
	increasing_sort_tag();
	update_LED = true;
}

void initializing_leds()
{

	temp_time_array[0] = 254;//Left -- R
	temp_time_array[1] = 254;//Left  -- G
	temp_time_array[2] = 254;//Left  -- B
	temp_time_array[3] = 254;//Right -- R
	temp_time_array[4] = 254;//Right  -- G
	temp_time_array[5] = 254;//Right -- B

}

void setup_output_levels_leds_on()
{
	
	port_pin_set_output_level(L_RLED_PIN,RGB_LED_DISABLE);
	port_pin_set_output_level(L_GLED_PIN,RGB_LED_DISABLE);
	port_pin_set_output_level(L_BLED_PIN,RGB_LED_DISABLE);
	port_pin_set_output_level(R_RLED_PIN,RGB_LED_DISABLE);
	port_pin_set_output_level(R_GLED_PIN,RGB_LED_DISABLE);
	port_pin_set_output_level(R_BLED_PIN,RGB_LED_DISABLE);

}

void setup_output_levels_leds_off()
{
	port_pin_set_output_level(L_RLED_PIN,RGB_LED_ENABLE);
	port_pin_set_output_level(L_GLED_PIN,RGB_LED_ENABLE);
	port_pin_set_output_level(L_BLED_PIN,RGB_LED_ENABLE);
	port_pin_set_output_level(R_RLED_PIN,RGB_LED_ENABLE);
	port_pin_set_output_level(R_GLED_PIN,RGB_LED_ENABLE);
	port_pin_set_output_level(R_BLED_PIN,RGB_LED_ENABLE);
	
}

void setup_outputs_leds()
{
	
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(L_RLED_PIN, &config_port_pin);
	port_pin_set_config(L_GLED_PIN, &config_port_pin);
	port_pin_set_config(L_BLED_PIN, &config_port_pin);
	port_pin_set_config(R_RLED_PIN, &config_port_pin);
	port_pin_set_config(R_GLED_PIN, &config_port_pin);
	port_pin_set_config(R_BLED_PIN, &config_port_pin);
	setup_output_levels_leds_off();

}


void configure_rgb_led(void)
{	
	struct tc_config rgb_led_config;
	tc_get_config_defaults(&rgb_led_config);
	//rgb_led_config.clock_source = GCLK_GENERATOR_1;
	rgb_led_config.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	rgb_led_config.counter_size = TC_COUNTER_SIZE_8BIT; 
	rgb_led_config.counter_8_bit.period = 0XFF;
	rgb_led_config.counter_8_bit.compare_capture_channel[0] = 0;
	
	tc_init(&tc_instance, TC3, &rgb_led_config);
	tc_enable(&tc_instance);
	
	initializing_leds();
	initializing_led_pins();
	increasing_sort_tag();
	transfer_temp();
	setup_outputs_leds();
}

void configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance, tc_callback_OF,
	TC_CALLBACK_OVERFLOW);
	tc_register_callback(&tc_instance, tc_callback_PWM,
	TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
	
}

void switch_off_LEDS()
{
	set_led_left_new(VALUE_LED_OFF,VALUE_LED_OFF,VALUE_LED_OFF);
	set_led_right_new(VALUE_LED_OFF,VALUE_LED_OFF,VALUE_LED_OFF);
	
}

/*
void tc_callback_PMW(
struct tc_module *const module_inst)
{
	uint8_t compare_value=0;
	uint8_t compare_value_last=0;
	uint8_t compare_value_current=0;
	static bool first_time = true;
    tc_disable(&tc_instance);
	//Enable the LED
	if(first_time == false)
	{
		port_pin_set_output_level(led_array[led_array_ID++],RGB_LED_DISABLE);
		if(time_array_ID < 5)
		{
		 compare_value_last = time_array[time_array_ID];
		 compare_value_current = time_array[++time_array_ID];
	
		 while(compare_value_last == compare_value_current && time_array_ID <= 6)
		 {
			//Enable the LED
			port_pin_set_output_level(led_array[led_array_ID++],RGB_LED_DISABLE);
			compare_value_last = time_array[time_array_ID];
			compare_value_current = time_array[++time_array_ID];
		 }
		 tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value_current);
		}
		 else
		 {
			tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, 0);
		 }

	}
	else
	{
		first_time = false;
		
	}
	tc_enable(&tc_instance);
}



void tc_callback_OF(
 struct tc_module *const module_inst)
  {
	uint8_t compare_value=0;
	tc_disable(&tc_instance);
	setup_output_levels_leds_off();
	if(update_LED == true)
	{
	transfer_temp();
	update_LED = false;
	}
	time_array_ID = 0;
	led_array_ID  = 0;
	compare_value = time_array[0];
	tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, compare_value);
	tc_enable(&tc_instance);
  }
*/



 
