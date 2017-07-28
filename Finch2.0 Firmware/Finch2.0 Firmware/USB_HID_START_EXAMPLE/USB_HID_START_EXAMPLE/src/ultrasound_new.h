/*
 * ultrasound_new.h
 *
 * Created: 6/22/2017 1:33:21 PM
 *  Author: raghu
 */ 


#ifndef ULTRASOUND_NEW_H_
#define ULTRASOUND_NEW_H_

void configure_ultrasound();
void configure_pins();
void configure_modules();
void configure_callbacks();
void configure_tc4();
void configure_echo_extint();
void configure_TC4_callback();
void configure_ext_callback();
void setup_ultrasound();
char* get_ultrasound_distance();

struct tc_module tc_instance4;


#define TRIG_PIN_LOW 0
#define TRIG_PIN_HIGH 1

#define ECHO_PIN_EIC PIN_PA15A_EIC_EXTINT15
#define ECHO_PIN PIN_PA15
#define ECHO_PIN_MUX PINMUX_PA15A_EIC_EXTINT15
#define TRIG_PIN PIN_PA03
#define ECHO_EIC_LINE 15
#define EXTINT_CLOCK_SOURCE      GCLK_GENERATOR_0




#endif /* ULTRASOUND_NEW_H_ */