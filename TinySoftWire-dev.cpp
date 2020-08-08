#include "TinySoftWire-dev.h"

// Send a START ore repeated start signal
void i2c_start(void)
{
	SDA_HIGH;
	SCL_HIGH;
	while (!SDA || !SCL) {};
	SDA_LOW;
	I2C_DELAY;
	SCL_LOW;
	I2C_DELAY;
}

// Send a STOP signal
void i2c_stop(void)
{
	SCL_HIGH;
	I2C_DELAY;
	SDA_HIGH;
	I2C_DELAY;
	while (!SDA || !SCL) {};//TODO: timeout??
}

// Send one byte and receive the ACK
bool i2c_send(uint8_t byte)
{
	// Send 8 Bits
	for (int8_t bit = 7; bit >= 0; bit--)
	{
		if (byte & 128)
		{
			SDA_HIGH;
		}
		else
		{
			SDA_LOW;
		}
		I2C_DELAY;
		SCL_HIGH;
		I2C_DELAY;
		SCL_LOW;
		byte = (byte << 1) & 0xFF;
	}
	// Receive ACK    
	SDA_HIGH;
	I2C_DELAY;
	SCL_HIGH;
	I2C_DELAY;
	while (!SCL) {};//TODO: timeout??
	bool ack = (SDA == 0);
	SCL_LOW;
	I2C_DELAY;
	return ack;
}

//like wire lib
bool i2c_beginTransmission(uint8_t address)
{
	i2c_start();
	return i2c_send(address << 1);
}

// Receive one byte, then send the ACK (or NAK)
uint8_t i2c_receive(bool ack)
{
	uint8_t byte = 0;
	// Receive 8 Bits
	SDA_HIGH;
	for (int8_t bit = 7; bit >= 0; bit--)
	{
		SCL_HIGH;
		I2C_DELAY;
		byte = (byte << 1) & 0xFF;
		if (SDA)
		{
			byte |= 1;
		}
		SCL_LOW;
		I2C_DELAY;
	}
	// Send ACK
	if (ack)
	{
		SDA_LOW;
	}
	I2C_DELAY;
	SCL_HIGH;
	I2C_DELAY;
	SCL_LOW;
	I2C_DELAY;
	SDA_HIGH;
	return byte;
}

uint8_t i2c_communicate(uint8_t slave_address, uint8_t* send_data, uint8_t send_bytes, uint8_t* rcv_data, uint8_t rcv_bytes)
{
	uint8_t rcv_count = 0;

	slave_address = (slave_address << 1) & 0xFF;

	if (send_bytes > 0)
	{
		i2c_start();
		bool ack = i2c_send(slave_address); // write mode
		if (!ack) goto error;
		while (send_bytes > 0)
		{
			ack = i2c_send(*send_data);
			if (!ack) goto error;
			send_data++;
			send_bytes--;
		}
		i2c_stop();
	}

	if (rcv_bytes > 0)
	{
		i2c_start();

		bool ack = i2c_send(slave_address + 1); // write mode
		if (!ack) goto error;
		while (rcv_bytes > 0)
		{
			if (rcv_bytes == 1)
			{
				*rcv_data = i2c_receive(false);
			}
			else
			{
				*rcv_data = i2c_receive(true);
			}
			rcv_data++;
			rcv_bytes--;
			rcv_count++;
		}

		i2c_stop();
	}
	return rcv_count;

error:
	// Sende STOP
	i2c_stop();
	return 0;
}
