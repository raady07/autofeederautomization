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
#ifndef _lcd_h
	#define _lcd_h
	typedef union _KEYs
	{
		unsigned char _KEYPORT;
		struct
		{
			unsigned C0:1;
			unsigned C1:1;
			unsigned C2:1;
			unsigned C3:1;
			unsigned R0:1;
			unsigned R1:1;
			unsigned R2:1;
			unsigned R3:1;
		};
	} KEYs;

	void Write_KeyPad(unsigned char chr);
	unsigned char Read_KeyPad(void);
	
	void LCDTask(void);
	void INIT_LED(void);
	void INIT_KEYPAD(void);
	void MainScreen(void);
	void LCD_StartUP_Screen(void);
	void displayautoplustime(void);
	void displayautoplustime1(void);
#endif
