/*-------------------------------------------------------------------------
 * Developed for Enersys Energy Solutions Private Limited, Hyderabad, Indiaresearch
 * research on Auto Feeder. 
 * If you use the code cite the paper
 * Appana, D. K., Alam, M. W., & Basnet, B. (2016) "A Novel Design of Feeder System 
 * for Aqua Culture Suitable for Shrimp Farming," International Journal of Hybrid 
 * Information Technology, 9(4), 199-212.
 * http://www.sersc.org/journals/IJHIT/vol9_no4_2016/18.pdf
 * (c) Copyright 2016 Enersys Energy Solutions, All rights reserved
 *-------------------------------------------------------------------------*/
 
#include"I2C.h"
#include"24lc256.h"

ee_address add;

unsigned char Char_ReadEEPROM_ex(unsigned short int Address)
{
	unsigned char data = 0;
	WaitI2C(1000);											// bus free time 4.7 ms is required for every new transmission
	add.value = Address;
	IdleI2C();
	StartI2C();
	IdleI2C();
	MasterWriteI2C(EEPROM_WRITE_ID);       					// Write Control byte
	IdleI2C();
	MasterWriteI2C(add.msb);								// write MSB Address
   	IdleI2C();
	MasterWriteI2C(add.lsb);								// write LSB Address
	IdleI2C();
	RestartI2C();
   	IdleI2C();
	MasterWriteI2C(EEPROM_READ_ID);       					// Read Control byte
	IdleI2C();
    data = MasterReadI2C();
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
	IdleI2C();
	return data;
}

unsigned char String_ReadEEPROM_ex(unsigned short int Address, unsigned char *ptr, unsigned short int no_of_bytes)
{
	if(no_of_bytes > 64)
		return 0;	
	WaitI2C(10000);											// bus free time 4.7 ms is required for every new transmission
	add.value = Address;
	IdleI2C();
	StartI2C();
	IdleI2C();
	MasterWriteI2C(EEPROM_WRITE_ID);       					// Write Control byte
	IdleI2C();
	MasterWriteI2C(add.msb);								// write MSB Address
   	IdleI2C();
	MasterWriteI2C(add.lsb);								// write LSB Address
	IdleI2C();
	RestartI2C();
   	IdleI2C();
	MasterWriteI2C(EEPROM_READ_ID);       						// Read Control byte
	IdleI2C();
   	while( no_of_bytes > 1 )
   	{
	    *ptr++ = MasterReadI2C();
		IdleI2C();
    	Nop();
		AckI2C();
		IdleI2C();
		no_of_bytes--;
		*ptr = '\0';
	}	
    *ptr++ = MasterReadI2C();
    *ptr = '\0'; 
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
	IdleI2C();
	return 1;
}

void Char_WriteEEPROM_ex(unsigned char data, unsigned short int Address)
{
	EEPROM_WP_DS;
	WaitI2C(10000);										// bus free time 4.7 ms is required for every new transmission
	add.value = Address;
	IdleI2C();
	StartI2C();
	IdleI2C();
	MasterWriteI2C(EEPROM_WRITE_ID);       				// Write Control byte
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(add.msb);								// write MSB Address
   	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(add.lsb);								// write LSB Address
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(data);       						// Write data byte
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	IdleI2C();
	StopI2C();
	IdleI2C();
	EEPROM_WP_EN;
}

void String_WriteEEPROM_ex(unsigned short int Address, unsigned char *ptr, unsigned char no_of_bytes)
{
	EEPROM_WP_DS;
	WaitI2C(10000);										// bus free time 4.7ms is required for every new transmission
	add.value = Address;
	IdleI2C();
	StartI2C();
	IdleI2C();
	MasterWriteI2C(EEPROM_WRITE_ID);       				// Write Control byte
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(add.msb);								// write MSB Address
   	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(add.lsb);								// write LSB Address
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	while(no_of_bytes--)
	{
		MasterWriteI2C(*ptr++);       						// Write data byte
		IdleI2C();
	    while(I2CSTATbits.ACKSTAT);
	}
	StopI2C();
	IdleI2C();
	EEPROM_WP_EN;
}
