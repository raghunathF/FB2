/*
 * ir.h
 *
 * Created: 3/1/2016 11:04:24 AM
 *  Author: Tom
 */ 


#ifndef IR_H_
#define IR_H_

void config_ir(void);
uint8_t read_left_ir(void);
uint8_t read_right_ir(void);

#define OBSTACLE_SENSE PIN_PB10 // MAY WANT TO CHANGE THIS IF WE GO INTERRUPT BASED
#define LEFT_IR PIN_PA19
#define RIGHT_IR PIN_PA18

#endif /* IR_H_ */