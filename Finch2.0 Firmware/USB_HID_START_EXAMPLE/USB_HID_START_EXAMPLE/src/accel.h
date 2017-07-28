/*
 * accel.h
 *
 * Created: 3/3/2016 2:06:06 PM
 *  Author: Tom
 */ 


#ifndef ACCEL_H_
#define ACCEL_H_

#define ACCEL_ADDRESS 0x4C // Need to add 1 for reads
#define TIMEOUT 10

struct i2c_master_module i2c_master_instance;

uint8_t bufferReceive[4];

enum status_code value;
uint8_t calc_baud; 

void config_accel(void);
void get_accel_data(void);
void write_data(uint8_t *bufferSend, uint8_t length);




#endif /* ACCEL_H_ */