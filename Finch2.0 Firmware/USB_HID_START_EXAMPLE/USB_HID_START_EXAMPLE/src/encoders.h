/*
 * encoders.h
 *
 * Created: 7/3/2017 8:45:37 AM
 *  Author: raghu
 */ 


#ifndef ENCODERS_H_
#define ENCODERS_H_

void configure_encoders(void);
void config_encoders_modules(void);
void get_data_encoders(struct i2c_master_module instance);
void write_data_encoders(uint8_t *bufferSend, uint8_t length, struct i2c_master_module *const module);
void config_I2C_SERCOM4(void);
void config_I2C_SERCOM3(void);
void initialize_left_encoder(void);
void initialize_right_encoder(void);
void config_right_encoder(void);
void config_left_encoder(void);
void read_encoders(void);



enum status_code error_code;

struct i2c_master_module i2c_master_encoder_right;
struct i2c_master_module i2c_master_encoder_left;

#define ENCODER_ADD 0x5E
#define LEFT_ENCODER_SERCOMM_3 SERCOM3
#define RIGHT_ENCODER_SERCOMM_4 SERCOM4

#endif /* ENCODERS_H_ */