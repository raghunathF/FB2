/*
 * finch_adc.h
 *
 * Created: 2/25/2016 2:18:25 PM
 *  Author: Tom
 */ 

void configure_adc(void);
uint8_t adc_start_read_result(const enum adc_positive_input analogPin);
void configure_adc_temp(void);
float calculate_temperature(uint16_t raw_code);
void load_calibration_data(void);
float convert_dec_to_frac(uint8_t val);

/* TEMP_LOG Base Address*/
#define TEMP_LOG_READ_NVM1					(*(RwReg*)NVMCTRL_TEMP_LOG)
/* TEMP_LOG Next Address*/
#define TEMP_LOG_READ_NVM2					(*(RwReg*)NVMCTRL_TEMP_LOG + 4)
#define ADC_TEMP_SAMPLE_LENGTH				4
#define INT1V_VALUE_FLOAT					1.0
#define INT1V_DIVIDER_1000					1000.0
#define ADC_12BIT_FULL_SCALE_VALUE_FLOAT	4095.0

#define LINE_FOLLOWER ADC_POSITIVE_INPUT_PIN2
#define BATT_MTR ADC_POSITIVE_INPUT_PIN10



#define LEFT_LIGHT ADC_POSITIVE_INPUT_PIN11
#define RIGHT_LIGHT ADC_POSITIVE_INPUT_PIN3


#define CTR_LINE ADC_POSITIVE_INPUT_PIN2
#define BATT_MTR ADC_POSITIVE_INPUT_PIN10