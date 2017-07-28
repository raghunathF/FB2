/*
 * serial_ble.c
 *
 * Created: 7/15/2017 4:53:50 PM
 *  Author: raghu
 */ 
#include <asf.h>
#include <stdio.h>
#include "serial_ble.h"

#define MAX_RX_BUFFER_LENGTH   1
volatile uint8_t rx_buffer[MAX_RX_BUFFER_LENGTH];
#define MAX_LIMIT_RING_BUFFER 255

extern	volatile uint8_t ring_buffer[MAX_LIMIT_RING_BUFFER];
extern volatile uint8_t tail_ring_buffer;
extern volatile uint8_t head_ring_buffer;
extern volatile bool received_data_updated;



void serial_ble_usart_configure()
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = 250000;
	config_usart.mux_setting = BLE_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = BLE_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = BLE_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = BLE_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = BLE_SERCOM_PINMUX_PAD3;
	//! [setup_change_config]
	//! [setup_set_config]
	while (usart_init(&usart_ble_instance,BLE_SERCOM, &config_usart) != STATUS_OK) 
	{
	}
	usart_enable(&usart_ble_instance);
}

void serial_ble_enable()
{
	//! [setup_enable_callbacks]
	//usart_enable_callback(&usart_ble_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_ble_instance, USART_CALLBACK_BUFFER_RECEIVED);
	//uint8_t string[] = "Hello World!\r\n";
	//usart_write_buffer_wait(&usart_ble_instance, string, sizeof(string));
	usart_read_buffer_job(&usart_ble_instance,(uint8_t *)rx_buffer, MAX_RX_BUFFER_LENGTH);
}

void serial_ble_modules_configure()
{
	serial_ble_usart_configure();	
}

void usart_read_callback(struct usart_module *const usart_module)
{
	uint16_t received_data;
	received_data_updated = true;
	received_data = (usart_ble_instance.hw->USART.DATA.reg & SERCOM_USART_DATA_MASK);
	*(ring_buffer + head_ring_buffer) = (uint8_t)received_data;
	head_ring_buffer++;
	received_data_updated = true;
}

void configure_usart_ble_callbacks(void)
{
	//usart_register_callback(&usart_ble_instance,usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_ble_instance,usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
}
void serial_ble_callbacks_configure()
{
	configure_usart_ble_callbacks();
}

void serial_ble_configure()
{
	//serial_ble_pins_configure();
	serial_ble_modules_configure();
	serial_ble_callbacks_configure();
	serial_ble_enable();
}
