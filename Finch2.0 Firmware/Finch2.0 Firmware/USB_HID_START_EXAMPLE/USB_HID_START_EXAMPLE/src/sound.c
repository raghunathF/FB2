/*
 * sound.c
 *
 * Created: 3/1/2016 10:31:07 AM
 *  Author: Tom
 */ 

#include <asf.h>
#include <dac.h>
#include "sound.h"




#define AUDIO_SDB PIN_PB23

static uint8_t waveform_type = ramp;
static uint16_t count_timer = 0;
static uint16_t waveform_volume = highest_volume; //0x3ff highest analog value to be generated
static uint16_t waveform_frequency = 1000;  //1KHz

void configure_dac(void)
{

	struct dac_config config_dac;
	dac_get_config_defaults(&config_dac);
	config_dac.reference = DAC_REFERENCE_AVCC;
	dac_init(&dac_instance, DAC, &config_dac);
	//dac_enable(&dac_instance);
	
}

void configure_dac_channel(void)
{
	
	struct dac_chan_config config_dac_chan;
	dac_chan_get_config_defaults(&config_dac_chan);
	dac_chan_set_config(&dac_instance, DAC_CHANNEL_0, &config_dac_chan);
	dac_chan_enable(&dac_instance, DAC_CHANNEL_0);
	
}

void configure_buzzer_pins()
{
	// Enable output on the I/O line
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	//port_pin_set_config(PIN_PA02, &config_port_pin);
	port_pin_set_config(AUDIO_SDB, &config_port_pin);

}

void calculate_initial_count()
{

	count_timer = OPTIMIZE_COUNT/waveform_frequency;
	count_timer = MAX_COUNT_VALUE - count_timer; 

}


void disable_TC5_callback()
{
	tc_disable_callback(&tc_instance5, TC_CALLBACK_OVERFLOW);
}


void disable_dac_callbacks()
{
	disable_TC5_callback();
}

void disable_speaker()
{
	disable_dac_callbacks();
	port_pin_set_output_level(AUDIO_SDB, false);	
}

void write_sound(uint8_t waveform_type_temp,uint16_t waveform_frequency_temp,uint16_t waveform_volume_temp) 
{
	//Ugly
	waveform_frequency = waveform_frequency_temp;
	waveform_type = waveform_type_temp;
	waveform_volume = waveform_volume_temp;
	if((waveform_frequency > 0) && (waveform_volume > 0))
	{
		 update_sound = true;
		 calculate_initial_count(waveform_frequency_temp);
		 enable_speaker();	
	}
	else
	{
		disable_speaker();
	}
}



void configure_tc5()
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT; //16
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64; //64
	config_tc.counter_16_bit.value =  0xA9D5; // 1KHz ramp waveform
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;
	tc_init(&tc_instance5, TC5, &config_tc);
}

void TC5_OV_callback(struct tc_module *const module_inst)
{
	//Update the DAC buffer value
	static int i = 1;
	static int count = 0 ;
	
	static uint8_t local_waveform = 0;
	static uint16_t local_count = 0 ;
	static uint16_t local_volume = 0; 
	tc_stop_counter(&tc_instance5);
	static bool order= true;
	//port_pin_set_output_level(PIN_PA08,1);
	
	if(update_sound)
	{
		local_waveform = waveform_type;
		local_count = count_timer;
		local_volume = waveform_volume;
		i = 0;  
		update_sound = false;
	}
	dac_chan_write(&dac_instance, DAC_CHANNEL_0, i);
	
	if (local_waveform == ramp )
	{
		i = i+30;
		if(i>= local_volume)
		{
			i=0;
		}
	}
	else if(local_waveform == triangular)
	{
		if(order == true)
		{
			i = i + 60;
			if(i>= local_volume)
			{
				i = local_volume;
				order = false;
			}
			
		}
		else
		{
			i = i - 60;
			if(i<=0)
			{
				i = 0;
				order = true;
			}		
		}
	}
	tc_set_count_value(&tc_instance5, local_count);
	//port_pin_toggle_output_level(PIN_PA08);
	//port_pin_set_output_level(PIN_PA08,0);
	tc_start_counter(&tc_instance5);
//	else if(i == 0)
//	{
//		order = true;
//	}
//	if(order)
//	{
//		i++;
//	}
// 	else
//	{
//		i--;
//	}
}

void configure_TC5_callback()
{
	tc_register_callback(&tc_instance5, TC5_OV_callback,TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance5, TC_CALLBACK_OVERFLOW);
}


void configure_dac_callbacks()
{
	configure_TC5_callback();
}


void enable_speaker()
{
	
	configure_dac_callbacks();
	tc_enable(&tc_instance5);
	dac_enable(&dac_instance);
	port_pin_set_output_level(AUDIO_SDB, true);
	
}
void configure_buzzer_modules()
{
	configure_dac();
	configure_dac_channel();
	configure_tc5();
}
void configure_buzzer()
{
	configure_buzzer_pins();
	configure_buzzer_modules();
	//enable_speaker();	
}