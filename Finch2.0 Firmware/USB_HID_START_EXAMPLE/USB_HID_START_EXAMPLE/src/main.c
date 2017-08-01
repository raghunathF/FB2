/**
 * \file
 *
 * \brief Main functions for Generic example
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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
#include <delay.h>
#include <system.h>
#include "conf_usb.h"
#include "ui.h"
#include "finch_adc.h"
#include "orb.h"
#include "sound.h"
#include "ir.h"
#include "accel.h"
#include "global.h"
//#include "ultrasound.h"
#include "ultrasound_new.h"
#include "power_enable.h"
#include "led_left_right.h"
#include "encoders.h"
#include "serial_ble.h"
#include "command_length_macros.h"
#include "battery.h"

#define MAX_TIME_OUT 250 

static volatile bool main_b_generic_enable = false;

#define MAX_REC_BYTE 20


#define MAX_TRANSMIT_VALUE 20
#define ACCL_TRANSMIT_LENGTH 5
#define SENSORS_TRANSMIT_LENGTH 11
#define L_TRANSMIT_LENGTH 3
#define LF_TRANSMIT_LENGTH 2
#define ULTRA_TRANSMIT_LENGTH 3
#define  TEST_TRANSMIT_COUNT 2
#define SENSORS_GET_LEN 1
#define VERSION_TRANSMIT_LENGTH 5
#define BATTERY_TRANSMIT_LENGTH 2

#define  FW_VERSION_MSB 0x00
#define  FW_VERSION_LSB 0x01

#define  HW_VERSION_MSB 0x00
#define  HW_VERSION_LSB 0x01

#define  MAX_COUNT_BROADCAST 1

#define  ALL_SET_LEN 14
/*! \brief Main function. Execution starts here.*/

void config_test_pins()
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA08, &config_port_pin);
}

void serial_receive_bytes(uint8_t count_receive ,uint8_t* receive_data)
{
	uint8_t i =0;
	while((head_ring_buffer - tail_ring_buffer) < count_receive )
	{
		if(serial_timeout)
		{
			serial_timeout = false;
			return;
		}
	}
	for(i=1;i<=count_receive;i++)
	{
		receive_data[i] = ring_buffer[tail_ring_buffer];
		tail_ring_buffer++;
	}
}





void check()
{
	volatile uint8_t received_value[MAX_REC_BYTE];
	volatile static uint8_t prev_rec_value[MAX_REC_BYTE];
	volatile uint8_t transmit_value[MAX_TRANSMIT_VALUE];
	volatile static uint8_t echo_prev[2];
	volatile static uint8_t echo_current[2];
	volatile int temp=0;
	uint8_t temp_batt_level	= 0;
	uint8_t temp_batt_status = 0;
	static volatile bool broadcast= false;

	volatile static char* echo_distance;
	//usart_read_job(&usart_instance ,)
	uint8_t i =0;
	if(head_ring_buffer != tail_ring_buffer)
	{
		received_value[0] = ring_buffer[tail_ring_buffer];
		tail_ring_buffer++;
		switch(received_value[0])
		{
			//Set all the sensors values
			case 'u':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(ALL_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					if((received_value[1]^prev_rec_value[1]) || (received_value[2]^prev_rec_value[2]) || (received_value[3]^prev_rec_value[3]) || (received_value[4]^prev_rec_value[4]) || (received_value[5]^prev_rec_value[5]) || (received_value[6]^prev_rec_value[6]))
					{
						set_led_left_new(received_value[1], received_value[2], received_value[3]);
						set_led_right_new(received_value[4], received_value[5], received_value[6]);
					}
					if((received_value[7]^prev_rec_value[7]) || (received_value[8]^prev_rec_value[8]) || (received_value[9]^prev_rec_value[9]) || (received_value[10]^prev_rec_value[10]) )
					{
						if(received_value[8] == 0 && received_value[9] == 0)
						{
							turn_off_motors();
						}
						set_motor_left(received_value[7], received_value[8]); // directions and speed are inputs
						set_motor_right(received_value[9], received_value[10]);
					}
					if((received_value[11]^prev_rec_value[11]) || (received_value[12]^prev_rec_value[12]) || (received_value[13]^prev_rec_value[13]) || (received_value[14]^prev_rec_value[14]) )
					{
						buzz_type =  received_value[11];
						buzz_frequency =  (received_value[12]<<8) + received_value[13];
						buzz_volume    =  received_value[14];
						speaker_update();
					}
					for(i=1;i<=ALL_SET_LEN;i++)
					{
						prev_rec_value[i] = received_value[i];
					}
				}
				break;
			//Get all the sensors values
			case 'v':
				//Transmit LDR values
				transmit_value[0] = SENSORS_TRANSMIT_LENGTH-1; // legacy reasons
				transmit_value[1] = adc_start_read_result(LEFT_LIGHT);
				transmit_value[2] = adc_start_read_result(RIGHT_LIGHT);
				
				//accelerometer
				get_accel_data();
				// Checks if the value was read at the same time the register was being updated (invalid result), and if so, repeats the request for data
				while((bufferReceive[0] & 0b01000000) | (bufferReceive[1] & 0b01000000) | (bufferReceive[2] & 0b01000000) | (bufferReceive[3] & 0b01000000)) {
					get_accel_data();
				}
				 
				for(i = 3; i < ACCL_TRANSMIT_LENGTH; i++) {
					transmit_value[i] = bufferReceive[i-1];
					
				}
				
				//Line Follower
				transmit_value[7] = adc_start_read_result(LINE_FOLLOWER);
				
				//Ultrasound
				echo_distance = get_ultrasound_distance();
				transmit_value[9] = *echo_distance++;
				transmit_value[8] = *echo_distance;				
				usart_write_buffer_wait(&usart_ble_instance, transmit_value, SENSORS_TRANSMIT_LENGTH);

				break;
			
			case 'b':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(SENSORS_GET_LEN,received_value);
				count_broadcast = 0;
				switch(received_value[1])
				{
					//Start broadcast
					case 'g':
						
						broadcast = true;
						break;	
					//Stop broadcast
					case 's':
					
						broadcast = false;
						break;
					
					default :
					
						head_ring_buffer = 0;
						tail_ring_buffer = 0;
						break;
					
				}
				break;
			/*
			
			case 'O':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(LEDS_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					set_led_left_new(received_value[1], received_value[2], received_value[3]);
					set_led_right_new(received_value[4], received_value[5], received_value[6]);
				}
				break;
			
			*/
			case 'O':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(LEDS_SET_LEN_TEST,received_value);
				if(serial_timeout == false)
				{
					set_led_left_new(received_value[2], received_value[3], received_value[4]);
					set_led_right_new(received_value[2], received_value[3], received_value[4]);
				}
				
				break;
				
			case 'M':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(MOTORS_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					if(received_value[2] == 0 && received_value[4] == 0)
					{
						turn_off_motors();
					}
					set_motor_left(received_value[1], received_value[2]); // directions and speed are inputs
					set_motor_right(received_value[3], received_value[4]);
				}
				break;
			// Creates an outgoing report with the accelerometer data
			case 'A':
				get_accel_data();
				// Checks if the value was read at the same time the register was being updated (invalid result), and if so, repeats the request for data
				while((bufferReceive[0] & 0b01000000) | (bufferReceive[1] & 0b01000000) | (bufferReceive[2] & 0b01000000) | (bufferReceive[3] & 0b01000000)) {
					get_accel_data();
				}
				transmit_value[0] = ACCL_TRANSMIT_LENGTH-1; // legacy reasons 
				for(i = 1; i < ACCL_TRANSMIT_LENGTH; i++) {
					transmit_value[i] = bufferReceive[i-1];
				}
				usart_write_buffer_wait(&usart_ble_instance, transmit_value, ACCL_TRANSMIT_LENGTH);
				break;
				
			case 'L':
				transmit_value[0] = L_TRANSMIT_LENGTH-1;
				transmit_value[1] = adc_start_read_result(LEFT_LIGHT);
				transmit_value[2] = adc_start_read_result(RIGHT_LIGHT);
				usart_write_buffer_wait(&usart_ble_instance, transmit_value, L_TRANSMIT_LENGTH);
				break;
				
			case 'F':
				transmit_value[0] = LF_TRANSMIT_LENGTH-1;
				transmit_value[1] = adc_start_read_result(LINE_FOLLOWER);
				usart_write_buffer_wait(&usart_ble_instance, transmit_value, LF_TRANSMIT_LENGTH);
				break;
			//Ultrasound sensor -- ascii value 55
			case 'U':
				//Nasty
				echo_distance = get_ultrasound_distance();
				echo_current[0]	= *echo_distance++;
				echo_current[1]	= *echo_distance;
				if((resource_distance_lock == 1) || (echo_current[1] > 0x75))
				{
					transmit_value[0] = ULTRA_TRANSMIT_LENGTH-1;
					transmit_value[2] = echo_prev[0];
					transmit_value[1] = echo_prev[1];
				}
				else
				{
					transmit_value[0] = ULTRA_TRANSMIT_LENGTH-1;
					transmit_value[2] = echo_current[0];
					transmit_value[1] = echo_current[1];
					echo_prev[0]	  = echo_current[0];
					echo_prev[1]	  = echo_current[1];
					
				}
				usart_write_buffer_wait(&usart_ble_instance, transmit_value, ULTRA_TRANSMIT_LENGTH);
				break;
				
			case 'B':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(ULTRASOUND_SET_LEN,received_value);
				if(serial_timeout == false)
				{
					buzz_type =  received_value[1];
					buzz_frequency =  (received_value[2]<<8) + received_value[3];
					buzz_volume    =  received_value[4];
					speaker_update();
				}
				break;
				
			case 's':
				serial_timeout_count = 0;
				serial_timeout = false;
				serial_receive_bytes(SENSORS_GET_LEN,received_value);
				if(serial_timeout == false)
				{
					switch(received_value[1])
					{
						case '1':
							transmit_value[0] = L_TRANSMIT_LENGTH-1;
							transmit_value[1] = adc_start_read_result(LEFT_LIGHT);
							transmit_value[2] = adc_start_read_result(RIGHT_LIGHT);
							usart_write_buffer_wait(&usart_ble_instance, transmit_value, L_TRANSMIT_LENGTH);
							break;
							
						case '2':
						
							get_accel_data();
							while((bufferReceive[0] & 0b01000000) | (bufferReceive[1] & 0b01000000) | (bufferReceive[2] & 0b01000000) | (bufferReceive[3] & 0b01000000)) {
								get_accel_data();
							}
							transmit_value[0] = ACCL_TRANSMIT_LENGTH-1; // legacy reasons
							for(i = 1; i < ACCL_TRANSMIT_LENGTH; i++) 
							{
								transmit_value[i] = bufferReceive[i-1];
							}
							usart_write_buffer_wait(&usart_ble_instance, transmit_value, ACCL_TRANSMIT_LENGTH);
							break;
					
						case '3':
							transmit_value[0] = LF_TRANSMIT_LENGTH-1;
							transmit_value[1] = adc_start_read_result(LINE_FOLLOWER);
							usart_write_buffer_wait(&usart_ble_instance, transmit_value, LF_TRANSMIT_LENGTH);
							break;
							
						case '4':
							echo_distance = get_ultrasound_distance();
							transmit_value[0] = ULTRA_TRANSMIT_LENGTH-1;
							transmit_value[2] = *echo_distance++;
							transmit_value[1] = *echo_distance;
							usart_write_buffer_wait(&usart_ble_instance, transmit_value, ULTRA_TRANSMIT_LENGTH);
							break;
							
						case '5':
							transmit_value[0] = BATTERY_TRANSMIT_LENGTH-1;
							temp_batt_level = adc_start_read_result(BATT_MTR);
							temp_batt_level = temp_batt_level >> 1;
							temp_batt_status = port_pin_get_output_level(BATT_STATUS);
							temp_batt_status = temp_batt_status << 7;
							transmit_value[1] = temp_batt_status | temp_batt_level;
							usart_write_buffer_wait(&usart_ble_instance, transmit_value, BATTERY_TRANSMIT_LENGTH);
							break;
							
						case '6':
							transmit_value[0] = VERSION_TRANSMIT_LENGTH-1;
							transmit_value[1] = FW_VERSION_MSB;
							transmit_value[2] = FW_VERSION_LSB;
							transmit_value[3] = HW_VERSION_MSB;
							transmit_value[4] = HW_VERSION_LSB;
							usart_write_buffer_wait(&usart_ble_instance, transmit_value, VERSION_TRANSMIT_LENGTH);
							break;
							

						default:
							head_ring_buffer = 0;
							tail_ring_buffer = 0;
							break;
					
						}
				}
				break;
				
			case 'E':
			
				transmit_value[0] = BATTERY_TRANSMIT_LENGTH-1;
				temp_batt_level = adc_start_read_result(BATT_MTR);
				temp_batt_level = temp_batt_level >> 1;
				temp_batt_status = port_pin_get_output_level(BATT_STATUS);
				temp_batt_status = temp_batt_status << 7;
				transmit_value[1] = temp_batt_status | temp_batt_level;
				usart_write_buffer_wait(&usart_ble_instance, transmit_value, BATTERY_TRANSMIT_LENGTH);
				break;
				
			
					
			// Returns an incrementing counter - used to measure cycle time and as a keep-alive.
			/*
			case 'z':
				transmit_value[0] = TEST_TRANSMIT_COUNT-1;
				transmit_value[1] = count;
				count++;
				usart_write_buffer_wait(&usart_ble_instance, transmit_value, TEST_TRANSMIT_COUNT);
				break;
			*/

			default:
				head_ring_buffer = 0;
				tail_ring_buffer = 0;
				break;
		}
		
		if((head_ring_buffer == tail_ring_buffer) || (head_ring_buffer > MAX_LIMIT_RING_BUFFER) || (tail_ring_buffer > MAX_LIMIT_RING_BUFFER) || (serial_timeout == true))
		{
			head_ring_buffer = 0;
			tail_ring_buffer = 0;
		}
	 }		//received_data_updated = false;	
	
	 if(broadcast == true)
	 {
		if(count_broadcast > MAX_COUNT_BROADCAST)
		{
			count_broadcast = 0;
			port_pin_set_output_level(PIN_PA08, true);
			//Transmit LDR values
			transmit_value[0] = SENSORS_TRANSMIT_LENGTH-1; // legacy reasons
			transmit_value[1] = adc_start_read_result(LEFT_LIGHT);
			transmit_value[2] = adc_start_read_result(RIGHT_LIGHT);
			
			//accelerometer
			get_accel_data();
			// Checks if the value was read at the same time the register was being updated (invalid result), and if so, repeats the request for data
			while((bufferReceive[0] & 0b01000000) | (bufferReceive[1] & 0b01000000) | (bufferReceive[2] & 0b01000000) | (bufferReceive[3] & 0b01000000)) 
			{
				get_accel_data();
			}
			
			for(i = 3; i < ACCL_TRANSMIT_LENGTH; i++)
			{
				transmit_value[i] = bufferReceive[i-1];	
			}
			
			//Line Follower
			transmit_value[7] = adc_start_read_result(LINE_FOLLOWER);
			
			//Ultrasound
			echo_distance = get_ultrasound_distance();
			echo_current[0]	= *echo_distance++;
			echo_current[1]	= *echo_distance;
			if((resource_distance_lock == 1) || (echo_current[1] > 0x75))
			{
				transmit_value[9] = echo_prev[0];
				transmit_value[8] = echo_prev[1];
			}
			else
			{
				transmit_value[9] = echo_current[0];
				transmit_value[8] = echo_current[1];
				echo_prev[0]	  = echo_current[0];
				echo_prev[1]	  = echo_current[1];
					
			}
			temp_batt_level = adc_start_read_result(BATT_MTR);
			temp_batt_level = temp_batt_level >> 1; 
			temp_batt_status = port_pin_get_output_level(BATT_STATUS);
			temp_batt_status = temp_batt_status << 7;
			transmit_value[10] = temp_batt_status | temp_batt_level;
			//Pin High
			
			usart_write_buffer_wait(&usart_ble_instance, transmit_value, SENSORS_TRANSMIT_LENGTH);
			port_pin_set_output_level(PIN_PA08, false);
			//Pin Low
		}
	 }	 
}
int main(void)
{
	static uint16_t i =0;
	uint8_t received_value[MAX_REC_BYTE];
	
	configure_power_enable_pins();
	enable_power_ic();
	
	board_init(); //Predefined
	system_init(); //Predefined
	
	irq_initialize_vectors();
	cpu_irq_enable();
	//Initialize the sleep manager
	sleepmgr_init();

	delay_init();
	config_test_pins();

	ui_init();
	ui_powerdown();
	
	configure_rgb_led();
	configure_tc_callbacks();
	
	configure_adc();
	config_motors();
	
	configure_batt_status();
	check_batterylevel_initial();

//config_led();
//configure_dac();
//configure_dac_channel();
//config_ir();
//configure_tcc_callbacks();

	configure_buzzer();	
	config_accel(); // Need to do this before USB to get I2C working while allowing USB to use the other two pins of SERCOM3

	configure_ultrasound();
	setup_ultrasound();
//configure_tc3();
	
	configure_encoders();
	//while(1)
	//{
		//read_encoders();
	//}
	calc_baud = (uint8_t)(system_gclk_chan_get_hz(SERCOM0_GCLK_ID_CORE + 1)/10000000);
	// Start USB stack to authorize VBus monitoring
	serial_ble_configure();
	
	udc_start();
	system_interrupt_enable_global();
	
	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	count_broadcast = 0;
	
	while (true) 
	{
		check();
		
	//! [main_loop]
	//! [main_write]
	//dac_chan_write(&dac_instance, DAC_CHANNEL_0, i);
	//! [main_write]

	//! [main_inc_val]
	//if (++i == 0x3FF) {
		//i = 0;
	//}
		
		
	//	port_pin_toggle_output_level(PIN_PB02);
	//	cpu_delay_s(1);
		/*
#ifdef   USB_DEVICE_LOW_SPEED
		// No USB "Keep a live" interrupt available in low speed
		// to scan generic interface then use main loop
		if (main_b_generic_enable) {
			static volatile uint16_t virtual_sof_sub = 0;
			static uint16_t virtual_sof = 0;
			if (sysclk_get_cpu_hz()/50000 ==
				virtual_sof_sub++) {
				virtual_sof_sub = 0;
				static uint16_t virtual_sof = 0;
				ui_process(virtual_sof++);
			}
		}
#else*/
	//	sleepmgr_enter_sleep();
//#endif
	/*for(int i = 0; i < 0xFF; i+=20)
	{
		write_sound(i);
		delay_ms(150);
	}
	for(int i = 0xFF; i > 0; i-=20)
	{
		write_sound(i);
		delay_ms(150);
	}*/
	
	
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_generic_enable)
		return;
	ui_process(udd_get_frame_number());
}

void main_remotewakeup_enable(void)
{
	ui_wakeup_enable();
}

void main_remotewakeup_disable(void)
{
	ui_wakeup_disable();
}

bool main_generic_enable(void)
{
	main_b_generic_enable = true;
	return true;
}

void main_generic_disable(void)
{
	main_b_generic_enable = false;
}

void main_hid_set_feature(uint8_t* report)
{
	/*if (report[0] == 0xAA && report[1] == 0x55
			&& report[2] == 0xAA && report[3] == 0x55) {
		// Disconnect USB Device
		udc_stop();
		ui_powerdown();
	}*/
}




/**
 * \mainpage ASF USB Device HID Generic
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device HID Generic
 * on Atmel MCU with USB module.
 * The application note AVR4905 provides information about this implementation.
 *
 * \section startup Startup
 * The example uses the buttons or sensors available on the board
 * to simulate a standard generic.
 * After loading firmware, connect the board (EVKxx,Xplain,...) to the USB Host.
 * When connected to a USB host system this application provides a HID generic
 * application in the Unix/Mac/Windows operating systems.
 * This example uses the native HID driver for these operating systems.
 *
 * A Atmel PC tool allows to communicate with the HID generic device.
 * This document gives information on integrating the Atmel USB HID DLL functions.
 * Simple code examples that demonstrate different types of implementation are given.
 * http://www.atmel.com/dyn/resources/prod_documents/doc7645.pdf
 * The PC tool is available here:
 * http://www.atmel.com/dyn/resources/prod_documents/AVR153.zip
 * Note: Use the PID 0x2402 in tool graphical interface,
 * and the button Firmware Upgrade only disconnects the USB device.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and HID modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/hid/
 *   <br>services/usb/class/hid/generic/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c         implement of user's interface (buttons, leds)
 */
