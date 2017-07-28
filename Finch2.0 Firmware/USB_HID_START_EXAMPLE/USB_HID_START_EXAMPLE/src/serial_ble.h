/*
 * serial_ble.h
 *
 * Created: 7/16/2017 12:34:22 PM
 *  Author: raghu
 */ 

#ifndef SERIAL_BLE_H_
#define SERIAL_BLE_H_


//
#define BLE_SERCOM              SERCOM2
#define BLE_SERCOM_MUX_SETTING  USART_RX_3_TX_2_XCK_3
#define BLE_SERCOM_PINMUX_PAD0  PINMUX_UNUSED
#define BLE_SERCOM_PINMUX_PAD1  PINMUX_UNUSED
#define BLE_SERCOM_PINMUX_PAD2  PINMUX_PA10D_SERCOM2_PAD2
#define BLE_SERCOM_PINMUX_PAD3  PINMUX_PA11D_SERCOM2_PAD3
#define BLE_SERCOM_DMAC_ID_TX   SERCOM2_DMAC_ID_TX
#define BLE_SERCOM_DMAC_ID_RX   SERCOM2_DMAC_ID_RX



struct usart_module usart_ble_instance;

void serial_ble_configure();


#endif /* SERIAL_BLE_H_ */