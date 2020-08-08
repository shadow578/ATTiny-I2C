/*
BitBang I2C implementation that can (hopefully) be used on a attiny13, with a api interface similar (although cut down) to the default arduino Wire.h library

Set the SDA/SCL PORT and PIN by defining SOFT_SDA_PIN and SOFT_SCL_PIN before including this library (default is SDA=PB4 and SDA=PB3) (pins can be all pins in PORTB)
eg.:
#define SOFT_SDA_PIN PB4
#define SOFT_SCL_PIN PB3

#include "TinySoftWire.h"
*/
#ifndef _TINYSOFTWIRE_h
#define _TINYSOFTWIRE_h

//~~includes and basic defines~~
#include "Arduino.h"
#include <pins_arduino.h> 
#define RX_BUFFER_LENGTH 8
#define I2C_DELAY _delay_us(4);

//~~Pin config defines~~
#ifndef SOFT_SDA_PIN
#pragma message("SOFT_SDA_PIN not defined, defauling to PB3!")
#define SOFT_SDA_PIN PB4
#endif

#ifndef SOFT_SCL_PIN
#pragma message("SOFT_SCL_PIN not defined, defauling to PB3!")
#define SOFT_SCL_PIN PB3
#endif

//~~SDA/SCL pin write / read macros
#define W_SDA_LOW	{PORTB &= ~_BV(SOFT_SDA_PIN); DDRB |= _BV(SOFT_SDA_PIN);}
#define W_SDA_HIGH	{PORTB |= _BV(SOFT_SDA_PIN); DDRB &= ~_BV(SOFT_SDA_PIN);}
#define R_SDA		(PINB &= _BV(SOFT_SDA_PIN))
#define W_SCL_LOW	{PORTB &= ~_BV(SOFT_SCL_PIN); DDRB |= _BV(SOFT_SCL_PIN);}
#define W_SCL_HIGH	{PORTB |= _BV(SOFT_SCL_PIN); DDRB &= ~_BV(SOFT_SCL_PIN);}
#define R_SCL		(PINB &= _BV(SOFT_SCL_PIN))

class TinySoftWire
{
public:
	//~~Transmission begin + end~~

	/*
	Begin a new transmission to the given address
	*/
	static inline bool beginTransmission(uint8_t address);

	/*
	End the current transmission, sending a stop condition
	*/
	static inline void endTransmission(void);

	//~~Writing to slaves~~

	/*
	Write data to the current slave (call only AFTER beginTransmission)
	*/
	static inline bool write(uint8_t data);

	//~~reading from slaves~~

	/*
	Request data from the a slave with the given address. 
	Only if sendStop is true, a stop condition is send after the request (thus releasing the I2C bus)
    Returns the number of bytes received by the slave (<<this is not implemented, so make sure the number of bytes you request matches the number the slave returns)
	*/
	static inline void requestFrom(uint8_t address, uint8_t count, bool sendStop);

	/*
	Returns the number of bytes available in the receive buffer
	*/
	static inline uint8_t available(void);

	/*
	reads a byte that was received from a slave (after calling requestFrom)
	*/
	static inline uint8_t read(void);
private:
	//~~buffers for receiving from slaves~~ 
	static uint8_t rxBuffer[];
	static uint8_t rxBufferIndex;
	static uint8_t rxBufferLength;

	//~~Internal functions~~
	/*
	Send Start Condition
	*/
	static void iic_start(void);

	/*
	Send Stop Condition
	*/
	static void iic_stop(void);

	/*
	Send a single byte, returns the ack
	*/
	static bool iic_send(uint8_t byte);

	/*
	Receive a byte and send a ACK (or NAK)
	*/
	static uint8_t iic_receive(bool ack);
};
#endif