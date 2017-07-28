/*
 * orb.h
 *
 * Created: 2/29/2016 11:01:17 AM
 *  Author: Tom
 */ 


#ifndef ORB_H_
#define ORB_H_


#define LEFT_MOTOR_WO 0
#define RIGHT_MOTOR_WO 1
#define LEFT_MOTOR_CH 0
#define RIGHT_MOTOR_CH 1

#define LRED_CH 1
#define LGREEN_CH 0
#define LBLUE_CH 0

#define LRED_WO 1
#define LGREEN_WO 0
//#define LBLUE_WO 7

#define LRED_LED PIN_PA06
#define LGREEN_LED PIN_PA07
#define LBLUE_LED PIN_PA17 // This is no longer correct

#define RRED_CH 1
#define RGREEN_CH 2
#define RBLUE_CH 3


#define RRED_WO 5
#define RGREEN_WO 4
//#define RBLUE_WO 1

//#define RRED_LED PIN_PA16 // This is no longer correct
//#define RGREEN_LED PIN_PA04
//#define RBLUE_LED PIN_PA05


#define RBLUE_PIN     PIN_PA27
#define LBLUE_PIN     PIN_PB22

#define RBLUE_ENABLE 1
#define LBLUE_ENABLE 1

#define RBLUE_DISABLE 0
#define LBLUE_DISABLE 0

void config_motors(void);
void set_motor_left(uint8_t dir, uint8_t speed);
void set_motor_right(uint8_t dir, uint8_t speed);
void turn_off_motors(void);



void config_led(void);
void configure_tcc_callbacks(void);

void set_led_left(uint8_t red, uint8_t green, uint8_t blue);
void set_led_right(uint8_t red, uint8_t green, uint8_t blue);


struct tcc_module tcc_instance1;
struct tcc_module tcc_instance0;

// Probably need to use TC 5 also, or just do a software PWM for the LEDs to free up some TC/TCCs

#endif /* ORB_H_ */