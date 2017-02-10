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
 
 #ifndef	_24lc256_h
	#define _24lc256_h
	#define	EEPROM_WP_EN	LATFbits.LATF0 = 0
	#define	EEPROM_WP_DS	LATFbits.LATF0 = 1
	#define EEPROM_ID		0xA0
	#define	EEPROM_READ_ID	0xA1
	#define	EEPROM_WRITE_ID	0xA0
	typedef union ee_address{
		unsigned short int value;
		struct
		{
			unsigned char lsb;
			unsigned char msb;
		};	
	}ee_address;
	unsigned char Char_ReadEEPROM_ex(unsigned short int Address);
	unsigned char String_ReadEEPROM_ex(unsigned short int Address, unsigned char *ptr, unsigned short int no_of_bytes);
	void Char_WriteEEPROM_ex(unsigned char data, unsigned short int Address);
	void String_WriteEEPROM_ex(unsigned short int Address, unsigned char *ptr, unsigned char no_of_bytes);
#endif
