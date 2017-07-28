/*
 * power_enable.h
 *
 * Created: 6/8/2017 2:10:53 PM
 *  Author: raghu
 */ 


#ifndef POWER_ENABLE_H_
#define POWER_ENABLE_H_

void configure_power_enable_pins(void);
void enable_power_ic(void);


#define  ENABLE_PIN_PWIC PIN_PA18
#define  ENABLE_PIN_5V PIN_PA14

#define  ENABLE_PWIC 1
#define  DISABLE_PWIC 0

#define  ENABLE_5V 1
#define  DISABLE_5V 0




#endif /* POWER_ENABLE_H_ */