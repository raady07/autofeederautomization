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
 
#ifndef __UART_H
	#define _UART1_H
	#include<P30F5011.h>
	#include<string.h>
	
	void INIT_UART1(void); 					// UART intialize
	char BusyUART1(void); 					// UART Busy.
	void CloseUART1(void);					// UART disable.
	void putsUART1(unsigned int *buffer);	// Print string through UART.
	unsigned int ReadUART1(void);			// Read string from UART buffer.
	void WriteUART1(unsigned char data);	// Print character through UART.
	char DataRdyUART1(void);				// Data Ready UART.
	void PCPutInt1(unsigned long int i);
	void PCPutInt2(signed int i);
#endif 
