#include "TinySoftWire.h"

//Initialize Class Variables
uint8_t TinySoftWire::rxBuffer[RX_BUFFER_LENGTH];
uint8_t TinySoftWire::rxBufferIndex = 0;
uint8_t TinySoftWire::rxBufferLength = 0;

//~~Transmission begin + end~~

/*
Begin a new transmission to the given address
*/
inline bool TinySoftWire::beginTransmission(uint8_t address)
{
	iic_start();
	return iic_send(address << 1);
}

/*
End the current transmission, sending a stop condition
*/
inline void TinySoftWire::endTransmission(void)
{
	iic_stop();
}

//~~Writing to slaves~~

/*
Write data to the current slave (call only AFTER beginTransmission)
*/
inline bool TinySoftWire::write(uint8_t data)
{
	return iic_send(data);
}

//~~reading from slaves~~

/*
Request data from the a slave with the given address.
Only if sendStop is true, a stop condition is send after the request (thus releasing the I2C bus)
[!]Returns the number of bytes received by the slave (<<this is not implemented, so make sure the number of bytes you request matches the number the slave returns)
*/
inline void TinySoftWire::requestFrom(uint8_t address, uint8_t count, bool sendStop)
{
	// clamp to buffer length
	if (count > RX_BUFFER_LENGTH)
	{
		count = RX_BUFFER_LENGTH;
	}

	//read number of bytes into buffer
	for (uint8_t cnt = 0; cnt < count; cnt++)
	{
		rxBuffer[cnt] = iic_receive(cnt == count - 1);//send ack on last byte
	}

	//send stop condition
	if (sendStop)
	{
		iic_stop();
	}

	// set rx buffer iterator vars
	rxBufferIndex = 0;
	rxBufferLength = count;
}

/*
Returns the number of bytes available in the receive buffer
*/
inline uint8_t TinySoftWire::available(void)
{
	return rxBufferLength - rxBufferIndex;
}

/*
reads a byte that was received from a slave (after calling requestFrom)
*/
inline uint8_t TinySoftWire::read(void)
{
	int value = -1;

	// get each successive byte on each call
	if (rxBufferIndex < rxBufferLength)
	{
		value = rxBuffer[rxBufferIndex];
		++rxBufferIndex;
	}

	return value;
}

//    _____       _                        _   ______                _   _                 
//   |_   _|     | |                      | | |  ____|              | | (_)                
//     | |  _ __ | |_ ___ _ __ _ __   __ _| | | |__ _   _ _ __   ___| |_ _  ___  _ __  ___ 
//     | | | '_ \| __/ _ \ '__| '_ \ / _` | | |  __| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//    _| |_| | | | ||  __/ |  | | | | (_| | | | |  | |_| | | | | (__| |_| | (_) | | | \__ \
//   |_____|_| |_|\__\___|_|  |_| |_|\__,_|_| |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
//
/*
Send Start Condition
*/
void TinySoftWire::iic_start(void)
{
	W_SDA_HIGH;
	W_SCL_HIGH;
	while (!R_SDA || !R_SCL) {};//TODO: timeout??
	W_SDA_LOW;
	I2C_DELAY;
	W_SCL_LOW;
	I2C_DELAY;
}

/*
Send Stop Condition
*/
void TinySoftWire::iic_stop(void)
{
	W_SCL_HIGH;
	I2C_DELAY;
	W_SDA_HIGH;
	I2C_DELAY;
	while (!R_SDA || !R_SCL) {};//TODO: timeout??
}

/*
Send a single byte, returns the ack
*/
bool TinySoftWire::iic_send(uint8_t byte)
{
	// Send 8 Bits
	for (int8_t bit = 7; bit >= 0; bit--)
	{
		if (byte & 128)
		{
			W_SDA_HIGH;
		}
		else
		{
			W_SDA_LOW;
		}
		I2C_DELAY;
		W_SCL_HIGH;
		I2C_DELAY;
		W_SCL_LOW;
		byte = (byte << 1) & 0xFF;
	}

	// Receive ACK    
	W_SDA_HIGH;
	I2C_DELAY;
	W_SCL_HIGH;
	I2C_DELAY;
	while (!R_SCL) {};//TODO: timeout??
	bool ack = (R_SDA == 0);
	W_SCL_LOW;
	I2C_DELAY;
	return ack;
}

/*
Receive a byte and send a ACK (or NAK)
*/
uint8_t TinySoftWire::iic_receive(bool ack)
{
	uint8_t byte = 0;
	// Receive 8 Bits
	W_SDA_HIGH;
	for (int8_t bit = 7; bit >= 0; bit--)
	{
		W_SCL_HIGH;
		I2C_DELAY;
		byte = (byte << 1) & 0xFF;
		if (R_SDA)
		{
			byte |= 1;
		}
		W_SCL_LOW;
		I2C_DELAY;
	}

	// Send ACK
	if (ack)
	{
		W_SDA_LOW;
	}
	I2C_DELAY;
	W_SCL_HIGH;
	I2C_DELAY;
	W_SCL_LOW;
	I2C_DELAY;
	W_SDA_HIGH;
	return byte;
}