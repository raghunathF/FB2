/*
 * sound.h
 *
 * Created: 3/1/2016 10:31:30 AM
 *  Author: Tom
 */ 

#include <asf.h>
#include <dac.h>

#ifndef SOUND_H_
#define SOUND_H_

struct dac_module dac_instance;
struct tc_module tc_instance5;

void configure_tc5();
void configure_buzzer_modules();
void configure_buzzer();
void configure_dac(void);
void configure_dac_channel(void);
void write_sound(uint8_t,uint16_t,uint16_t);
void configure_TC5_callback();
void enable_speaker();
void disable_speaker();


enum sound_waveforms{square,ramp,triangular,sine};
	
#define GCLK0_FREQUENCY (48000000UL) //48MHz clock

#define highest_volume 0x3FF
#define NO_STEPS  34
#define TC5_PRESCALAR  64
#define MAX_COUNT_VALUE 0xFFFF

#define OPTIMIZE_COUNT (GCLK0_FREQUENCY/(NO_STEPS*TC5_PRESCALAR))


static bool update_sound = false;




#endif /* SOUND_H_ */