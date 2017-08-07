
/*
 * global.h
 *
 * Created: 7/20/2017 1:46:44 PM
 *  Author: raghu
 */ 

#define MAX_LIMIT_RING_BUFFER 255


volatile uint8_t buzz_type=0;
volatile uint16_t buzz_frequency=0;
volatile uint16_t buzz_volume=0;

volatile uint8_t ring_buffer[MAX_LIMIT_RING_BUFFER];
volatile uint8_t tail_ring_buffer= 0;
volatile uint8_t head_ring_buffer= 0;
volatile bool received_data_updated = false;

volatile bool serial_timeout = false;
volatile uint8_t serial_timeout_count = 0;
volatile uint8_t count_broadcast = 0;
volatile bool resource_distance_lock= false;