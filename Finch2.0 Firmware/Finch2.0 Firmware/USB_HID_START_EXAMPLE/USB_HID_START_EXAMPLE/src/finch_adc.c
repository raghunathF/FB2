/*
 * finch_adc.c
 *
 * Created: 2/25/2016 2:18:03 PM
 *  Author: Tom
 */ 

#include <asf.h>
#include <adc.h>
#include "finch_adc.h"

/*  The following variables have referred with respect to device data sheet 
	Equation1 and Equation1b on section "Temperature Sensor Characteristics" 
	of Electrical Characteristics */

struct adc_module adc_instance;

float coarse_temp; /* Coarse value of the temperature - tempC */ 
float fine_temp;   /* Finer value of the temperature - tempF */

float tempR;       /* Production Room Temperature value read from NVM memory - tempR */
float tempH;	   /* Production Hot Temperature value read from NVM memory - tempH */
float INT1VR;      /* Room temp 2�s complement of the internal 1V reference value - INT1VR */
float INT1VH;	   /* Hot temp 2�s complement of the internal 1V reference value - INT1VR */
uint16_t ADCR;     /* Production Room Temperature ADC Value read from NVM memory - ADCR */
uint16_t ADCH;     /* Production Hot Temperature ADC Value read from NVM memory - ADCH */

float VADCR;	   /* Room Temperature ADC voltage - VADCR */
float VADCH;	   /* Hot Temperature ADC voltage - VADCH */

void configure_adc()
{
	struct adc_config conf_adc;
	
	adc_get_config_defaults(&conf_adc);
	
	//conf_adc.clock_source = GCLK_GENERATOR_1;
	conf_adc.reference = ADC_REFERENCE_INTVCC0;
	conf_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
	conf_adc.positive_input = LEFT_LIGHT;
	conf_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
	conf_adc.resolution = ADC_RESOLUTION_8BIT;
	conf_adc.left_adjust = true;
	
	adc_init(&adc_instance, ADC, &conf_adc);
	
	adc_enable(&adc_instance);
}


uint8_t adc_start_read_result(const enum adc_positive_input analogPin)
{

	uint16_t adc_result = 0;
	//configure_adc(analogPin);
	adc_set_positive_input(&adc_instance, analogPin);
	
	adc_start_conversion(&adc_instance);
	while((adc_get_status(&adc_instance) & ADC_STATUS_RESULT_READY) != 1);
	
	adc_read(&adc_instance, &adc_result);
	
	return adc_result;
}


/**
* \brief Temperature Calculation.
* This function calculate fine temperature using Equation1 and Equation
* 1b as mentioned in data sheet section "Temperature Sensor Characteristics"
* of Electrical Characteristics.
*
*/

void configure_adc_temp(void)
{
	struct adc_config conf_adc;
	
	adc_get_config_defaults(&conf_adc);
	
	//conf_adc.clock_source = GCLK_GENERATOR_1;
	conf_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
	conf_adc.reference = ADC_REFERENCE_INT1V;
	conf_adc.positive_input = ADC_POSITIVE_INPUT_TEMP;
	conf_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
	conf_adc.sample_length = ADC_TEMP_SAMPLE_LENGTH;
	
	adc_init(&adc_instance, ADC, &conf_adc);
	
	ADC->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(2) | ADC_AVGCTRL_SAMPLENUM_4;
	
	adc_enable(&adc_instance);
}

float calculate_temperature(uint16_t raw_code)
{
	uint8_t result;
	float VADC;      /* Voltage calculation using ADC result for Coarse Temp calculation */
	float VADCM;     /* Voltage calculation using ADC result for Fine Temp calculation. */
	float INT1VM;    /* Voltage calculation for reality INT1V value during the ADC conversion */
	
	VADC = ((float)raw_code * INT1V_VALUE_FLOAT)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
	
	/* Coarse Temp Calculation by assume INT1V=1V for this ADC conversion */
	coarse_temp = tempR + (((tempH - tempR)/(VADCH - VADCR)) * (VADC - VADCR));
	
	/* Calculation to find the real INT1V value during the ADC conversion */
	INT1VM = INT1VR + (((INT1VH - INT1VR) * (coarse_temp - tempR))/(tempH - tempR));
	
	VADCM = ((float)raw_code * INT1VM)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
	
	/* Fine Temp Calculation by replace INT1V=1V by INT1V = INT1Vm for ADC conversion */
	result = 128 + (tempR + (((tempH - tempR)/(VADCH - VADCR)) * (VADCM - VADCR)));
	
	return result;
}


/**
* \brief Calibration Data.
* This function extract the production calibration data information from
* Temperature log row content and store it variables for temperature calculation
*
*/
void load_calibration_data(void)
{
	volatile uint32_t val1;				/* Temperature Log Row Content first 32 bits */
	volatile uint32_t val2;				/* Temperature Log Row Content another 32 bits */
	uint8_t room_temp_val_int;			/* Integer part of room temperature in �C */
	uint8_t room_temp_val_dec;			/* Decimal part of room temperature in �C */
	uint8_t hot_temp_val_int;			/* Integer part of hot temperature in �C */
	uint8_t hot_temp_val_dec;			/* Decimal part of hot temperature in �C */
	int8_t room_int1v_val;				/* internal 1V reference drift at room temperature */
	int8_t hot_int1v_val;				/* internal 1V reference drift at hot temperature*/
	
	uint32_t *temp_log_row_ptr = (uint32_t *)NVMCTRL_TEMP_LOG;
	
	val1 = *temp_log_row_ptr;
	temp_log_row_ptr++;
	val2 = *temp_log_row_ptr;
	
	room_temp_val_int = (uint8_t)((val1 & NVMCTRL_FUSES_ROOM_TEMP_VAL_INT_Msk) >> NVMCTRL_FUSES_ROOM_TEMP_VAL_INT_Pos);
	
	room_temp_val_dec = (uint8_t)((val1 & NVMCTRL_FUSES_ROOM_TEMP_VAL_DEC_Msk) >> NVMCTRL_FUSES_ROOM_TEMP_VAL_DEC_Pos);
	
	hot_temp_val_int = (uint8_t)((val1 & NVMCTRL_FUSES_HOT_TEMP_VAL_INT_Msk) >> NVMCTRL_FUSES_HOT_TEMP_VAL_INT_Pos);
	
	hot_temp_val_dec = (uint8_t)((val1 & NVMCTRL_FUSES_HOT_TEMP_VAL_DEC_Msk) >> NVMCTRL_FUSES_HOT_TEMP_VAL_DEC_Pos);
	
	room_int1v_val = (int8_t)((val1 & NVMCTRL_FUSES_ROOM_INT1V_VAL_Msk) >> NVMCTRL_FUSES_ROOM_INT1V_VAL_Pos);
	
	hot_int1v_val = (int8_t)((val2 & NVMCTRL_FUSES_HOT_INT1V_VAL_Msk) >> NVMCTRL_FUSES_HOT_INT1V_VAL_Pos);
	
	ADCR = (uint16_t)((val2 & NVMCTRL_FUSES_ROOM_ADC_VAL_Msk) >> NVMCTRL_FUSES_ROOM_ADC_VAL_Pos);
	
	ADCH = (uint16_t)((val2 & NVMCTRL_FUSES_HOT_ADC_VAL_Msk) >> NVMCTRL_FUSES_HOT_ADC_VAL_Pos);
	
	tempR = room_temp_val_int + convert_dec_to_frac(room_temp_val_dec);
	
	tempH = hot_temp_val_int + convert_dec_to_frac(hot_temp_val_dec);
	
	INT1VR = 1 - ((float)room_int1v_val/INT1V_DIVIDER_1000);
	
	INT1VH = 1 - ((float)hot_int1v_val/INT1V_DIVIDER_1000);
	
	VADCR = ((float)ADCR * INT1VR)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
	
	VADCH = ((float)ADCH * INT1VH)/ADC_12BIT_FULL_SCALE_VALUE_FLOAT;
}

/**
* \brief Decimal to Fraction Conversation.
* This function converts the decimal value into fractional
* and return the fractional value for temperature calculation
*/
float convert_dec_to_frac(uint8_t val)
{
	if (val < 10)
	{
		return ((float)val/10.0);
	}
	
	else if (val <100)
	{
		return ((float)val/100.0);
	}
	
	else
	{
		return ((float)val/1000.0);
	}
}