#ifndef _TINYSOFTWIRE-DEV_h
#define _TINYSOFTWIRE-DEV_h
#include <util/delay.h> 
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

//pins are SCL=PB3 (D11) and SDA=PB4 (D12)

#define SCL_LOW      { PORTB &= ~(1<<PB3); DDRB |=  (1<<PB3); }
#define SCL_HIGH     { PORTB |=  (1<<PB3); DDRB &= ~(1<<PB3); }
#define SCL          ( PINB &= (1<<PB3) )

#define SDA_LOW      { PORTB &= ~(1<<PB4); DDRB |=  (1<<PB4); }
#define SDA_HIGH     { PORTB |=  (1<<PB4); DDRB &= ~(1<<PB4); }
#define SDA          ( PINB &= (1<<PB4) )

#define I2C_DELAY    _delay_us(4);

uint8_t i2c_communicate(uint8_t slave_address, uint8_t* send_data, uint8_t send_bytes, uint8_t* rcv_data, uint8_t rcv_bytes);


void i2c_start(void);

bool i2c_beginTransmission(uint8_t address);

void i2c_stop(void);

bool i2c_send(uint8_t byte);

uint8_t i2c_receive(bool ack);
#endif