/*
 * ultrasound.h
 *
 * Created: 12/2/2016 11:14:33 AM
 *  Author: Tom
 */ 


#ifndef ULTRASOUND_H_
#define ULTRASOUND_H_

#define ECHO_PIN_EIC PIN_PA15A_EIC_EXTINT15
#define ECHO_PIN PIN_PA15
#define ECHO_PIN_MUX PINMUX_PA15A_EIC_EXTINT15
#define TRIG_PIN PIN_PA03
#define ECHO_EIC_LINE 15
#define EXTINT_CLOCK_SOURCE      GCLK_GENERATOR_0


struct tc_module tc_instance3;

volatile uint16_t rising_edge;
volatile uint16_t pulse_width;

void configure_tc3(void);
void tc_callback_trig(
struct tc_module *const module_inst);

void configure_echo_channel(void);
void configure_extint_callbacks(void);
void echo_detection_callback(void);

#endif /* ULTRASOUND_H_ */