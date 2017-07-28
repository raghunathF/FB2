/*
 * motors.h
 *
 * Created: 3/2/2016 10:02:53 AM
 *  Author: Tom
 */ 

#include <asf.h>

#ifndef MOTORS_H_
#define MOTORS_H_

void config_motors(void);
void set_motor_left(uint8_t dir, uint8_t speed);
void set_motor_right(uint8_t dir, uint8_t speed);
void turn_off_motors(void);

#define LEFT_MOTOR_WO 1
#define RIGHT_MOTOR_WO 0
#define LEFT_MOTOR_CH 1
#define RIGHT_MOTOR_CH 0

struct tcc_module tcc_instance0;

#endif /* MOTORS_H_ */