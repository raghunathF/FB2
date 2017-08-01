/*
 * sound.c
 *
 
 *  Author: Raghunath
 */ 

#include <asf.h>
#include <dac.h>
#include "sound.h"



#define PI 3.1415
#define AUDIO_SDB PIN_PB23

static volatile  uint8_t waveform_type = sine;
static volatile uint16_t waveform_volume = 0; //0x3ff highest analog value to be generated
volatile static uint16_t waveform_frequency = 5000;  //1KHz


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



struct speaker_timer* calculate_frequency(void)
{	
	static struct speaker_timer* I_1 = NULL;
	static first_time = true;
	uint8_t prescalar_value = 1;
	if(first_time == true)
	{
		I_1 = malloc(sizeof(struct speaker_timer));
		first_time = false;
	}
	
	if((waveform_frequency <= 10000) & (waveform_frequency > 2941))
	{
		I_1->speaker_prescalar =  TC_CLOCK_PRESCALER_DIV1;
		prescalar_value = PRESCALAR_1 ; 
	}  
	else if((waveform_frequency <= 2941) & (waveform_frequency > 1470))
	{
		I_1->speaker_prescalar =  TC_CLOCK_PRESCALER_DIV2;
		prescalar_value = PRESCALAR_2 ; 
	}
	else if((waveform_frequency <= 1470) & (waveform_frequency > 732))
	{
		I_1->speaker_prescalar =  TC_CLOCK_PRESCALER_DIV4;
		prescalar_value = PRESCALAR_4 ; 	
	}
	else if((waveform_frequency <= 732) & (waveform_frequency > 366))
	{
		I_1->speaker_prescalar =  TC_CLOCK_PRESCALER_DIV8;
		prescalar_value = PRESCALAR_8 ; 
	}
	else if((waveform_frequency>= 366) & (waveform_frequency < 183))
	{
		I_1->speaker_prescalar =  TC_CLOCK_PRESCALER_DIV16;
		prescalar_value = PRESCALAR_32 ; 	
	} 
	else 
	{
		I_1->speaker_prescalar =  TC_CLOCK_PRESCALER_DIV64;
		prescalar_value = PRESCALAR_64 ; 	
	}
	I_1->speaker_period = (OPTIMIZE_COUNT_CALCULATION/((prescalar_value)*(waveform_frequency)));
	
	return I_1; 
}

void disable_TC5_callback()
{
	tc_disable_callback(&tc_instance5, TC_CALLBACK_OVERFLOW);
}

void disable_dac_callbacks()
{
	disable_TC5_callback();
}


//waveform type      -- 0 -- Ramp
//					 -- 1 -- Triangular
//					 -- 2 -- Sine Wave
//waveform_volume    -- Maximum value -- 03ff--1023
//					 -- Minimum value -- yet to be determined and should be greater than number of divisions 

//static uint16_t waveform_test[64];

static uint16_t* calculate_wave() 
{
	
	//volatile static uint16_t waveform_sine[NO_DATA_SINE];
	volatile static uint16_t waveform_test[NO_DATA_SINE];
	static uint16_t* waveform_sine = NULL;
	static uint16_t* waveform_ramp = NULL;
	static uint16_t* waveform_tri = NULL;
	static uint16_t* waveform_square = NULL;
	static uint16_t* waveform_current = NULL;
	static uint16_t waveform_amplitude = 0;
	int i=0;
	static bool first_run = true;
	
	
	waveform_amplitude = waveform_volume*MAX_AMPLITUDE;
	
	if(first_run == true )
	{
		waveform_sine = malloc(NO_DATA_SINE*sizeof(waveform_sine));
		waveform_ramp = malloc(NO_DATA_RAMP*sizeof(waveform_ramp));
		waveform_tri = malloc(NO_DATA_TRI*sizeof(waveform_tri));
		waveform_square = malloc(NO_DATA_TRI*sizeof(waveform_square));
		first_run = false;
	}
	
	if(waveform_type == square )
	{
		waveform_current = waveform_square;
		for(i=0;i<(NO_DATA_SQUARE/2);i++)
		{
			*waveform_square = waveform_amplitude;
			waveform_square++;
		}
		for(i=(NO_DATA_SQUARE/2);i<NO_DATA_SQUARE;i++)
		{
			*waveform_square = 0;
			waveform_square++;
		}
		waveform_square = waveform_current;
		
	}
	else if(waveform_type == ramp )
	{	
		waveform_current = waveform_ramp;
		for(i=0;i<NO_DATA_RAMP;i++)
		{
			*waveform_ramp = (i*waveform_amplitude) / NO_DATA_RAMP;
			 waveform_ramp++;
		}
		waveform_ramp = waveform_current;
	}
	else if(waveform_type == triangular)
	{
		waveform_current = waveform_tri;
		for(i=0;i<(NO_DATA_TRI/2);i++)
		{
			*waveform_tri = (i*waveform_amplitude*2) / NO_DATA_TRI;
			waveform_tri++;
		}
		for(i= (NO_DATA_TRI/2);i<NO_DATA_TRI;i++)
		{
			*waveform_tri = ((NO_DATA_TRI-i)*(waveform_amplitude*2))/NO_DATA_TRI;
			waveform_tri++;
		}
		waveform_tri = waveform_current; 
		
	}
	else if(waveform_type == sine)
	{
		waveform_current = waveform_sine;
		for(i=0;i<NO_DATA_SINE;i++)
		{
			//waveform_sine[i] = sin((i*2*PI)/NO_DATA_SINE);
			waveform_test[i] =   (uint16_t)(sin((i*2*PI)/NO_DATA_SINE)*(waveform_amplitude/2) + (waveform_amplitude/2));
			*waveform_sine =   (uint16_t)(sin((i*2*PI)/NO_DATA_SINE)*(waveform_amplitude/2) + (waveform_amplitude/2));
			waveform_sine++;
		}
		waveform_sine = waveform_current; 
		
	}
	return waveform_current;
}




//DMA initialization
void configure_dma_resource_dac(struct dma_resource *resource)
{
	struct dma_resource_config config;
	dma_get_config_defaults(&config);
	config.peripheral_trigger = TC5_DMAC_ID_OVF;
	config.trigger_action = DMA_TRIGGER_ACTON_BEAT;
	dma_allocate(resource, &config);
}

void setup_transfer_descriptor_dac(DmacDescriptor *descriptor)
{
	uint16_t* waveform=NULL;
	static bool first_time = true;
	struct dma_descriptor_config descriptor_config;
	dma_descriptor_get_config_defaults(&descriptor_config);
	waveform = calculate_wave();
	descriptor_config.beat_size = DMA_BEAT_SIZE_HWORD;
	descriptor_config.dst_increment_enable = false;
	descriptor_config.block_transfer_count = NO_DATA_SINE;
	descriptor_config.source_address = (uint32_t)waveform +  NO_DATA_SINE*2;
	//descriptor_config.block_transfer_count = sizeof(start_up_song);
	//descriptor_config.source_address = (uint32_t)start_up_song +  sizeof(start_up_song);
	descriptor_config.destination_address = (uint32_t)(&dac_instance.hw->DATA.reg);
	descriptor_config.next_descriptor_address = (uint32_t)descriptor;

	if(first_time == true)
	{
		dma_descriptor_create(descriptor, &descriptor_config);
		first_time = false;
	}
	else
	{
		//dma_suspend_job( &resource_DAC);
		dma_descriptor_create(descriptor, &descriptor_config);
		dma_start_transfer_job(&resource_DAC);
		//dma_resume_job( &resource_DAC);
	}
	
}

void setup_prescalar(struct speaker_timer* I_1)
{
	volatile uint16_t temp = 0;
	tc_disable(&tc_instance5);

	tc_instance5.hw->COUNT8.PER.reg	=  (uint8_t)I_1->speaker_period ;
	temp = tc_instance5.hw->COUNT8.CTRLA.reg ;
	temp = ((uint16_t)I_1->speaker_prescalar) | (temp & PRESCALAR_MASK); 
	tc_instance5.hw->COUNT8.CTRLA.reg =  temp;
	while (tc_is_syncing(&tc_instance5)) {
		/* Wait for sync */
	}
	
}

void speaker_update()
{
	static bool speaker_enable = false;
	struct speaker_timer* I_1=NULL;
	
	waveform_type      =   buzz_type;
	waveform_frequency =   buzz_frequency;
	waveform_volume    =   buzz_volume;
	
	if((waveform_frequency > 0) && (waveform_volume > 0))
	{
		I_1 = calculate_frequency() ;
		setup_transfer_descriptor_dac(&descriptor_DAC);
		setup_prescalar(I_1);
		dma_start_transfer_job(&resource_DAC);
		enable_speaker();
		if(speaker_enable == false)
		{
			//tc_set_top_value (&tc_instance5,(uint32_t)I_1->speaker_period)
			//enable_speaker();
			speaker_enable = true;
		}
	}
	else
	{
		speaker_enable = false;
		disable_speaker();
	}

	
}



//
void configure_tc5(struct speaker_timer* I_1)
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT; //16
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = I_1->speaker_prescalar; //64
	config_tc.counter_8_bit.period = I_1->speaker_period; // 10KHz waveform
	config_tc.count_direction = TC_COUNT_DIRECTION_UP;
	tc_init(&tc_instance5, TC5, &config_tc);
}

/*
void configure_TC5_callback()
{
	tc_register_callback(&tc_instance5, TC5_OV_callback,TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance5, TC_CALLBACK_OVERFLOW);
}
*/

void configure_dac_callbacks()
{
	configure_TC5_callback();
}

void disable_speaker()
{
	//Disable the all the modules which are involved in speaker which work independently
	port_pin_set_output_level(AUDIO_SDB, false);
	tc_disable(&tc_instance5);
	dac_disable(&dac_instance);

}

void enable_speaker()
{
	static bool first_time = true; 
	//configure_dac_callbacks();
	tc_enable(&tc_instance5);
	dac_enable(&dac_instance);
	port_pin_set_output_level(AUDIO_SDB, true);
	if(first_time == true)
	{
		dma_start_transfer_job(&resource_DAC);
		first_time = false;
	}
	//dma_start_transfer_job(&resource_DAC);
}

void configure_dma_buzzer()
{
	configure_dma_resource_dac(&resource_DAC);
	setup_transfer_descriptor_dac(&descriptor_DAC);
	dma_add_descriptor(&resource_DAC, &descriptor_DAC);
}

void configure_buzzer_modules()
{
	struct speaker_timer* I_2 = NULL;
	configure_dac();
	configure_dac_channel();
	I_2 = calculate_frequency();
	configure_tc5(I_2);
	configure_dma_buzzer();
}

void configure_buzzer()
{
	configure_buzzer_pins();
	configure_buzzer_modules();
	enable_speaker();	
}