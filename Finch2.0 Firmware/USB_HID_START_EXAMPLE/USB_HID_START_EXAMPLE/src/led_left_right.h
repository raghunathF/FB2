/*
 * led_left_right.h
 *
 * Created: 6/12/2017 5:28:11 PM
 *  Author: raghu
 */ 


#ifndef LED_LEFT_RIGHT_H_
#define LED_LEFT_RIGHT_H_

void configure_tc_callbacks(void);
void configure_rgb_led(void);
void set_led_right_new(uint8_t red, uint8_t green, uint8_t blue);
void set_led_left_new(uint8_t red, uint8_t green, uint8_t blue);

#define L_RLED_PIN PIN_PA07
#define L_GLED_PIN PIN_PA06
#define L_BLED_PIN PIN_PB22

#define R_RLED_PIN PIN_PB11
#define R_GLED_PIN PIN_PB10
#define R_BLED_PIN PIN_PA27


#endif /* LED_LEFT_RIGHT_H_ */