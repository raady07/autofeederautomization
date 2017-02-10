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
#ifndef __LCD_H
	#define __LCD_H
	#include <P30F5011.h>
	
	#define	LCD_20_4
	typedef unsigned char BYTE;
	
	typedef union _LCD_PORT
	{
		BYTE PORT;
		struct
		{
			unsigned D0:1;
			unsigned D1:1;
			unsigned D2:1;
			unsigned D3:1;
			unsigned Dummy:12;
		};
	}LCD_PORT;
	
	typedef union _LCD_DATA
	{
		BYTE DATA;
		struct
		{
			unsigned LN:4;		//UPPER NIBBLE
			unsigned UN:4;		//LOWER NIBBLE
		};
	}LCD_DATA;
	
	#ifdef	LCD_16_4
		#define XLCDL1home()    	   			XLCDCommand(0x80)
		#define XLCDL2home()     	   			XLCDCommand(0xC0)
		#define XLCDL3home()    	   			XLCDCommand(0x90)
		#define XLCDL4home()     	   			XLCDCommand(0xD0)
	#endif
		
	#ifdef	LCD_20_4
		#define XLCDL1home()    	   			XLCDCommand(0x80)
		#define XLCDL2home()     	   			XLCDCommand(0xC0)
		#define XLCDL3home()    	   			XLCDCommand(0x94)
		#define XLCDL4home()     	   			XLCDCommand(0xD4)
	#endif	
	
	#define XLCDClear()     	   			XLCDCommand(0b00000001)		//0x01
	#define XLCD_RETURNHOME()  				XLCDCommand(0b00000010)		//0x02
	#define XLCD_BLINKOON()        			XLCDCommand(0b00001111)		//0x0F
	#define XLCD_BLINKOFF()         		XLCDCommand(0b00001110)		//0x0E
	#define XLCD_BLINKCURSOROFF()      		XLCDCommand(0b00001100)		//0x0C
	#define XLCD_CURSOR_OFF()				XLCDCommand(0x0C)
	#define XLCD_CRIGHTSHIFT()  			XLCDCommand(0b00010100)		//0x14
	#define XLCD_BACKSPACE()       			XLCDCommand(0b00010000)		//0x10
	#define XLCD_DRIGHTSHIFT()  			XLCDCommand(0b00011100)		//0x1C
	#define XLCD_DLEFTSHIFT()     			XLCDCommand(0b00011000)		//0x18

	void INIT_XLCD(void);                                // To initialise the LCD
	void XLCDPut(char data);                            // To put dtat to be displayed
	void XLCDPutRamString( const char *string);         // To display data string in RAM
	void XLCDCommand(unsigned char cmd);                // To send commands to LCD           
	void XLCDDelay15ms(void);
	void XLCDDelay4ms(void);
	void XLCDDelay(void);
	void XLCDDelayms(unsigned int t);
	void XLCD_Clock(void);
	void LCD_Write(char *string, char line);
	void PutsXLCD(char *buffer);
	char BusyXLCD(void);
	void WriteDataXLCD( char data);
	
	
		
	#define	MoveRight(x)	{ unsigned char _dcnt = x; \
				              while(_dcnt-- != 0) \
					            XLCDCommand(0b00010100); }
	
	#define	MoveLeft(x)	{ unsigned char _dcnt = x; \
				              while(_dcnt-- != 0) \
					            XLCDCommand(0b00010000); }
	
	
	
	
	void TOPORT(BYTE DT);
#endif
