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

#include"INIT.h"
#include"xlcd2.h"

LCD_PORT LCD_P;
LCD_DATA LD;

/********************************************************************************************
* Function Name:          XLCDInit(void)													*
* Description:            This function initializes the LCD and syncs with Microcontroller. *
* Input parameters:       void 																*
* Return value:           void																*
********************************************************************************************/
void INIT_XLCD(void)
{
	XLCDDelay15ms(); 
	LCD_D0_D = 0;
	LCD_D1_D = 0;
	LCD_D2_D = 0;
	LCD_D3_D = 0;
	LCD_RS_D = 0;
	LCD_EN_D = 0;
	LCD_BL_D = 0;
	LCD_RW_D = 0;
	
	LCD_D0_P = 0;
	LCD_D1_P = 0;
	LCD_D2_P = 0;
	LCD_D3_P = 0;
	LCD_RS_P = 0;
	LCD_EN_P = 0;
	LCD_BACKLIGHT = 1;				//	Back Light ON

	XLCDDelay15ms(); 

	TOPORT(0b00000011);
	TOPORT(0b00000011);
	TOPORT(0b00000011);
	TOPORT(0b00000010);

	XLCDCommand(0b00101000);    	//Function Set :- Data lines:4;5X8 Dots
   	XLCDCommand(0b00001000);        //display off  :- Display Off;Cursor Off;
   	XLCDClear();
   	XLCDCommand(0b00000110);    	//if cursor inc and no display shift
	XLCDCommand(0b00001111);    	//display on cursor on blink on	        
	XLCDClear();
}

/********************************************************************************************
* Function Name:          XLCDCommand(void)													*
* Description:            Initializes LCD in command mode.									*
* Input parameters:       cmd / 8 bit data.													*
* Return value:           void																*
********************************************************************************************/
void XLCDCommand(unsigned char cmd)
{	
	XLCDDelay4ms();
	LCD_EN_P = 0;
	LCD_RS_P = 0;
	LD.DATA = cmd;
	TOPORT(LD.UN);
	TOPORT(LD.LN);
}

/********************************************************************************************
* Function Name:          XLCDPut(void)														*
* Description:            Initializes LCD in data mode.										*
* Input parameters:       cmd / 8 bit data.													*
* Return value:           void																*
********************************************************************************************/	
void XLCDPut(char data)
{
	XLCDDelay4ms();
	LCD_EN_P = 0;
	LCD_RS_P = 1;
	LD.DATA = data;
	TOPORT(LD.UN);
	TOPORT(LD.LN);
}

/********************************************************************************************
* Function Name:          TOPORT(BYTE DT)													*
* Description:            Sends 4 bytes of data to port pins.								*
* Input parameters:       BYTE DT															*
* Return value:           void																*
********************************************************************************************/
void TOPORT(BYTE DT)
{
	LCD_P.PORT = DT;
	LCD_D0_P = LCD_P.D0;	
	LCD_D1_P = LCD_P.D1;	
	LCD_D2_P = LCD_P.D2;	
	LCD_D3_P = LCD_P.D3;	
	XLCD_Clock();
}

/********************************************************************************************
* Function Name:          XLCDPutRamString(const char* string)								*
* Description:            Puts a string to LCD.												*
* Input parameters:       Constant character string pointer.								*
* Return value:           void																*
********************************************************************************************/
void XLCDPutRamString(const char *string)
{
    while(*string)                          // Write data to LCD up to null
    {   
        XLCDPut(*string);                   // Write character to LCD
        string++;                           // Increment buffer
    }
    return;
}

/********************************************************************************************
* Function Name:          XLCDDelay15ms(void)												*
* Description:            Time Delay														*
* Input parameters:       void																*
* Return value:           void																*
********************************************************************************************/
void XLCDDelay15ms (void)
{
   	char i;
   	for(i=0;i<4;i++)
   	{
   		XLCDDelay4ms();
   	}
   	return;
}

/********************************************************************************************
* Function Name:          XLCDDelay4ms(void)												*
* Description:            Time Delay														*
* Input parameters:       void																*
* Return value:           void																*
********************************************************************************************/
void XLCDDelay4ms (void)
{
    int i;
  	for(i=0;i<512;i++)			//	10000 replaced by Ravi
   	{
   		Nop();
   	}
   	return;
}

/********************************************************************************************
* Function Name:          XLCDDelay(unsigned int t)											*
* Description:            Time Delay in defined ms.											*
* Input parameters:       integer t															*
* Return value:           void																*
********************************************************************************************/
void XLCDDelayms(unsigned int t)
{
	int i;
    while(t--)
		for(i=0;i<1275;i++)			//1275 loops is considered to be 1ms approx.
			Nop();
}

/********************************************************************************************
* Function Name:          XLCD_Clock(void)													*
* Description:            One Clock Cycle.													*
* Input parameters:       void																*
* Return value:           void																*
********************************************************************************************/
void XLCD_Clock(void)
{
	LCD_EN_P = 1; 
    Nop();
    Nop();
    LCD_EN_P = 0; 	
}


/********************************************************************************************
* Function Name:          LCD_Write(char* string, char line)								*
* Description:            Place the string in particular line of LCD accordingly.			*
* Input parameters:       char string array, line number where to be placed.				*
* Return value:           void																*
********************************************************************************************/
void LCD_Write(char *string, char line)
{
	if (line == 1)
	{
		XLCDL1home();
		XLCDPutRamString(string);
	}
	else if (line == 2)
	{
		XLCDL2home();
		XLCDPutRamString(string);
	}
	else if (line == 3)
	{
		XLCDL3home();
		XLCDPutRamString(string);
	}
	else if (line == 4)
	{
		XLCDL4home();
		XLCDPutRamString(string);
	}
}

/********************************************************************************************
* Function Name:          PutsXLCD															*
* Description:            This function writes a string of characters into LCD controller	*
*						  It stops transmission before the null character(which is not sent)*
* Input parameters:       char  *buffer														*
* Return value:           void																*
********************************************************************************************/
void PutsXLCD(char *buffer)
{
    while(*buffer != '\0')
    {
        while(BusyXLCD());		// wait for command completion
        WriteDataXLCD(*buffer); // calling another function
        buffer++;				// to write each char to the lcd module
    }
    while(BusyXLCD());			// wait for command completion
} 

/********************************************************************************************
* Function Name:             BusyXLCD														*
* Description:               This function returns the busy status of the LCD Controller.	*
*							 The Busy flag is MS bit when the Instruction Reg is read		*
* Input parameters:          none															*
* Return value:              char- returns 1 if Busy flag is set, else 0					*
********************************************************************************************/
char BusyXLCD(void)
{
	unsigned char i = 0;
	for(i = 0; i < 40; i++)
		Nop();
    return 0;
} 

/********************************************************************************************
* Function Name:            WriteDataXLCD													*
* Description:              This function writes a data byte into the DDRam/CGRam of the LCD*
*							controller.The user must first check the busy  status of LCD 	*
*							controller  by calling the BusyXLCD() function.The data written	*
*							can go to the DDRam or CGRam depending on the previous			*
*							SetXXRamAddr( char) that was  called.							*
* Input parameters:         char data														*
* Return value:             void															*
********************************************************************************************/
void WriteDataXLCD( char data)
{
	unsigned char i = 0;
    LCD_RS_P = 1;  /* select Data Reg */
	LCD_D3_P = (unsigned int)((data & 0x08)>>3);
	LCD_D2_P = (unsigned int)((data & 0x04)>>2);
	LCD_D1_P = (unsigned int)((data & 0x02)>>1);
	LCD_D0_P = (unsigned int)((data & 0x01)>>0);

    LCD_EN_P = 1;
	for(i = 0; i < 64; i++)
		Nop();
    LCD_EN_P = 0;
	for(i = 0; i < 24; i++)
		Nop();
    LCD_RS_P = 0; 
}  

