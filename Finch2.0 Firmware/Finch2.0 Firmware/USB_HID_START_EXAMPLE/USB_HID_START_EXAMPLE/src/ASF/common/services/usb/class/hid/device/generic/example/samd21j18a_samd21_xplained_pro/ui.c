/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <dac.h>
#include "ui.h"
#include "finch_adc.h"
#include "orb.h"
#include "sound.h"
#include "motors.h"
#include "accel.h"
#include "ir.h"
#include "ultrasound_new.h"
#include "led_left_right.h"

static uint8_t ui_hid_report[UDI_HID_REPORT_OUT_SIZE];
int count = 0;

void ui_init(void)
{
	/* Initialize LEDs */
	//LED_On(LED_0_PIN);
}

void ui_powerdown(void)
{
	//LED_Off(LED_0_PIN);
}


void ui_wakeup_enable(void)
{

}

void ui_wakeup_disable(void)
{

}

void ui_wakeup(void)
{
	//LED_On(LED_0_PIN);
}

void ui_process(uint16_t framenumber)
{
	/*
	bool b_btn_state;
	static bool btn0_last_state = false;
	static uint8_t cpt_sof = 0;

	// Blink LED
	if (ui_b_led_blink) {
		if ((framenumber % 1000) == 0) {
			LED_On(LED_0_PIN);
		}
		if ((framenumber % 1000) == 500) {
			LED_Off(LED_0_PIN);
		}
	}

	// Scan process running each 40ms
	cpt_sof++;
	if (cpt_sof < 40) {
		return;
	}
	cpt_sof = 0;

	// Scan buttons
	b_btn_state = !port_pin_get_input_level(BUTTON_0_PIN);
	if (b_btn_state != btn0_last_state) {
		ui_hid_report[0] = b_btn_state;
		udi_hid_generic_send_report_in(ui_hid_report);
		btn0_last_state = b_btn_state;
	}*/
}

void ui_handle_report(uint8_t *report)
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;	
	
	uint8_t buzz_type;
	uint16_t buzz_frequency;
	uint16_t buzz_volume;

	char* echo_distance;
	
	int i;
	count++;
	if(count > 255)
		count = 0;
		

	// HID Reports are 8 bytes long. The first byte specifies the function of that report (set motors, get light sensor values, etc).
	switch(report[0]) {
		// If O, set the LED using bytes 1-3 of the HID report
		// ASCII value for O --
		case 'O':
			red = report[1];
			green = report[2];
			blue = report[3];
			set_led_left_new(red, green, blue);
			set_led_right_new(red, green, blue);
			/*initial_orb = 0; // Get out of color fade mode
			exit_count = 0;  // Reset time-out counter*/
			if(red != 0) {
				//LED_On(LED_0_PIN);
			}
			else {
				//LED_Off(LED_0_PIN);
			}
			
			break;
		// If 'L', create an outgoing report with the light sensor values
		case 'L':
			/*lightSensorVals = read_light_sensors(); // returns both values as a 16-bit int, high-byte is left, low-byte is right
			leftLightSensor = lightSensorVals>>8;
			rightLightSensor = lightSensorVals - (leftLightSensor<<8);*/
			ui_hid_report[0] = adc_start_read_result(LEFT_LIGHT);
			ui_hid_report[1] = adc_start_read_result(RIGHT_LIGHT);
		// Get out of idle state and stop LED from color-fading
		/*if(initial_orb) {
			initial_orb = 0;
			set_orb0(0,0,0);
		}
		exit_count = 0; */// Reset time-out counter
		break;
		// If 'T', create an outgoing report with temperature
		case 'T':
			system_voltage_reference_enable(SYSTEM_VOLTAGE_REFERENCE_TEMPSENSE);
			configure_adc_temp();
			load_calibration_data();
			ui_hid_report[0] = calculate_temperature(adc_start_read_result(ADC_POSITIVE_INPUT_TEMP));
			configure_adc();
		/*
		if(initial_orb) {
			initial_orb = 0;
			set_orb0(0,0,0);
		}
		exit_count = 0;*/
		break;
		// If 'M', use bytes 1-4 to set motor speed and direction
		case 'M':
		/*if(initial_orb) {
			initial_orb = 0;
			set_orb0(0,0,0);
		}*/
		if(report[2] == 0 && report[4] == 0)
		{
			turn_off_motors();
		}
		
		set_motor_left(report[1], report[2]); // directions and speed are inputs
		set_motor_right(report[3], report[4]);
		/*exit_count = 0;*/
		break;
		// Use bytes 1-4 to set buzzer frequency and duration
		case 'B':
		/*buzz_duration = (report[1]<<8) + report[2];
		buzz_frequency = (report[3]<<8) + report[4];
		
		if(initial_orb) {
			initial_orb = 0;
			set_orb0(0,0,0);
		}
		chirp(buzz_duration, buzz_frequency);
		exit_count = 0;*/
		buzz_type =  report[1];
		buzz_frequency =  (report[2]<<8) + report[3];
		buzz_volume    =  (report[4]<<8) + report[5];
		write_sound(buzz_type,buzz_frequency,buzz_volume);
		
		break;
		// Creates an outgoing report with the accelerometer data
		case 'A':
		get_accel_data();
		// Checks if the value was read at the same time the register was being updated (invalid result), and if so, repeats the request for data
		while((bufferReceive[0] & 0b01000000) | (bufferReceive[1] & 0b01000000) | (bufferReceive[2] & 0b01000000) | (bufferReceive[3] & 0b01000000)) {
			get_accel_data();
		}
		ui_hid_report[0] = 153; // legacy reasons 
		for(i = 1; i < 5; i++) {
			ui_hid_report[i] = bufferReceive[i-1];
		}
		//ui_hid_report[5] = 0;//errorCode; // Returns the error code for debugging purposes.
		/*if(initial_orb) {
			initial_orb = 0;
			set_orb0(0,0,0);
		}
		exit_count = 0;*/
		break;
		// Returns the left and right IR values
		case 'I':
		ui_hid_report[0] = read_left_ir();
		ui_hid_report[1] = read_right_ir();
		
		/*
		if(initial_orb) {
			initial_orb = 0;
			set_orb0(0,0,0);
		}
		exit_count = 0;*/
		break;
		// Fast way to turn everything off
		case 'X':
		/*
		turn_off_motors();
		turn_off_buzz();
		initial_orb = 0;
		set_orb0(0,0,0);
		exit_count = 0;*/
		break;
		// Fast way to turn everything off AND go to idle state
		case 'R':/*
		turn_off_motors();
		turn_off_buzz();
		initial_orb = 1;
		red = 0;
		green = 0;
		blue = 254;
		redUp = 1;
		greenUp = 0;
		blueUp = 0;
		exit_count = 0;*/
		break;
		//Line Follower
		case 'F':
		ui_hid_report[0] = adc_start_read_result(LINE_FOLLOWER);
		break;
		
		//Ultrasound sensor --ascii value 55
		case 'U':
		echo_distance = get_ultrasound_distance();
		ui_hid_report[0] = *echo_distance++;
		ui_hid_report[1] = *echo_distance;
		break;
		
		
		// Returns an incrementing counter - used to measure cycle time and as a keep-alive.
		case 'z':
		ui_hid_report[0] = count;
		count++;
		if(count > 255) {
			count = 0;
		}
		//exit_count = 0;
		break;
		default:
		break;
	}
	report[0] = 0x00;
	// Sets last byte of outgoing report to last byte of incoming report so an outgoing report can be matched to its incoming request
	ui_hid_report[7]= report[7];
	udi_hid_generic_send_report_in(ui_hid_report);
	
	/*
	if (report[0]=='1') {
		// A led must be on
		switch(report[1]) {
			case '1':
			ui_b_led_blink = false;
			LED_On(LED_0_PIN);
			break;
		}
	} else {
		// A led can blink
		switch(report[1]) {
			case '1':
			ui_b_led_blink = true;
			break;
		}
	}*/
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAMD21-XPlain:
 * - LED0 blinks when USB host has checked and enabled HID generic interface
 * - HID events LED1 can make LED0 always on or blinking
 * - Event buttons are linked to SW0.
 *
 */
