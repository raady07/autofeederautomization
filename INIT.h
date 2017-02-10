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
 
 #ifndef __INIT_H
	#define __INIT_H

	#include 	<P30F5011.h>
	#include    <string.h>
	#include    <ctype.h>
	#include    <stdlib.h>
	#include    <stdio.h>

	/********************************Setting*************************************************
	* Fosc(Internal FRC) 			: 7.37 Mhz												*
	* Fosc(External)				: 8 Mhz													*
	* Fcy = (Fosc/4)*PLL																	*
	****************************************************************************************/
	#define	__FCY			8000000					// 8MHZ
	#define	__FSCL			100000					// 100KHZ

	/*********************Configruration Bits Setting****************************************
	*	Primary Oscillator 			: FRC w/ PLL 4x											*
	*	Oscillator Source			: Internal Fast RC										*
	*	Clock Switching & Monitor	: Sw Disable, Mon Disabled								*
	*	Watchdog Timer				: Disabled												*
	*	POR Timer Value				: 64 ms													*
	*	Brownout Voltage			: 2.0 V													*
	****************************************************************************************/

	#define LCD_PASSWORD		"0000"
	#define MASTER_PASSWORD		"8722"

	/****************************************************************************************
	* CONFIGURE UART1 : Baud Rate Generator Register										*
   	* Desired Baud Rate : 9600				* 												*
	* Formula :  UxBRG = (FCY / (16 * Baud Rate)) – 1  										*
	* ------------------------------------------------------------------------------------- *
	* I2CBRG Calculations																	*
	* Formula : I2CBRG = [( Fcy / Fscl )-( Fcy / 1,111,111 ) ] - 1							*
	* ------------------------------------------------------------------------------------- *
	* Timer Delay Calculations used:														*
	* 1. Timer Frequency Ftimer = Fcy														*
	* 2. Using a Prescalar 1:8, then 														*
	*    Ftimer = Fcy / scalar value 														*
	* 3. Time Interval Ttimer = 1/Ftimer													*
	* 4. To calculate the value to be filled in Timer rolling over							*
	*    register to generate a sec delay :													*
	*    No of count for sec delay =  sec / Ttimer 											*		
	* --------------------------------------------------------------------------------------*
	* ._________________________.___________________________________________________.		*
	* |							|	/w PLL0		/w PLL4		/w PLL8		/w PLL16	|		*
	* |-------------------------|---------------------------------------------------|		*	
	* | Fcy(Internal FRC)		|	1.835M		7.37M		14.68M		29.36M		|		*
	* | BaudRate				|	10.94		46.98		94.57		190.145		|		*
	* | I2CBRG(Fscl = 100Khz)	|	15.69		67.09		133.58		266.17		|		*
	* | I2CBRG(Fscl = 400Khz)	|	1.93		10.79		22.48		45.97		|		*
	* | Timer:10ms,scalar 1:8														|		*
	* | PR1 					|	2303.12		9212.5		18435		36850		|		*
	* | Timer:128th sample of 20ms, scalar 1:8										|		*
	* | PR3						|	35.986		143.945		287.89		575.78		|		*	
	* | Timer:128th sample of 20ms, scalar 1:1										|		*
	* | PR3						|	287.89		1151.56		2303.125	4606.25		|		*
	* |-------------------------|---------------------------------------------------|		*
	* | Fcy(External Osc)		|	2M			8M			16M			32M			|		*
	* | BaudRate				|	12.02		51.08		103.16		207.33		|		*
	* | I2CBRG(Fscl = 100Khz)	|	17.19		71.79		144.59		290.19		|		*
	* | I2CBRG(Fscl = 400Khz)	|	2.19		11.79		24.59		50.19		|		*
	* | PR1 					|	2500		10000		20000		40000		|		*	
	* |_________________________|___________________________________________________|		*
   	****************************************************************************************/
	
	#define __UARTBAUD	51
	#define __I2C_BRG	17
	#define __T1_TIME	10000
	// For reset monitoring.
	#define Sleep()  {__asm__ volatile ("pwrsav #0");} 
	#define Idle()   {__asm__ volatile ("pwrsav #1");} 

	//#define _T1_TIME	2303
	//#define __I2C_BRG	16
	//#define _UARTBAUD	11

	/***************************UART's******************************************************/
	#define DDR_UART1TX			TRISFbits.TRISF3		    		// Configure RF3 as Output for UART1 Tx .
	#define DDR_UART1RX			TRISFbits.TRISF2					// Configure RF2 as Input for UART1 Rx.
	/***************************LED'S******************************************************/
	#define DDR_LED_MRUN		TRISGbits.TRISG12					//	LED ON Board
	#define DDR_LED_RUN			TRISGbits.TRISG13					//	Toggle MCLR RUN Status LED on LED Board	
	#define DDR_LED_MANL		TRISGbits.TRISG15					//	Toggle Auto Status LED on LED Board
	#define DDR_LED_AUTO		TRISCbits.TRISC1					//	Toggle Manual Status LED on LED Board
	#define DDR_LED_AUTOP		TRISCbits.TRISC2					//	Toggle Error Status LED on LED Board							

	#define LED_MRUN			LATGbits.LATG12
	#define LED_RUN				LATGbits.LATG13			
	#define LED_MANL			LATGbits.LATG15			
	#define LED_AUTO			LATCbits.LATC1			
	#define LED_AUTOP			LATCbits.LATC2			
	/***************************LATCH and Load CONFIGURATIONS*****************************/
	#define	DDR_LATCH_1			TRISFbits.TRISF1
	#define	Latch_1				LATFbits.LATF1
	
	#define	DDR_Relay_1			TRISBbits.TRISB8
	#define	DDR_Relay_2			TRISBbits.TRISB9
	#define	DDR_Relay_3			TRISBbits.TRISB10
	#define	DDR_Relay_4			TRISBbits.TRISB11
	
	#define	Relay_1				LATBbits.LATB8
	#define	Relay_2				LATBbits.LATB9
	#define	Relay_3				LATBbits.LATB10
	#define	Relay_4				LATBbits.LATB11
	/***************************Manual Key***********************************************/
	#define	DDR_KEY_MANL		TRISFbits.TRISF6					//	Manual Key
	#define	KEY_MANL			PORTFbits.RF6

	/***************************EEPROM & RTC********************************************/
	#define	DDR_EEPROM_WP_EN	TRISFbits.TRISF0					//  EEPROM Write Protect
	#define DDR_RTC_EN			TRISDbits.TRISD8
	#define DDR_SCL_EN			TRISGbits.TRISG2
	#define DDR_SDA_EN			TRISGbits.TRISG3
	/***************************KEYPAD CONFIGURATIONS***********************************/

	#define DDR_KP_R1			TRISBbits.TRISB0
	#define DDR_KP_R2 			TRISBbits.TRISB1
	#define DDR_KP_R3 			TRISBbits.TRISB2
	#define DDR_KP_R4 			TRISBbits.TRISB3
	#define DDR_KP_C1			TRISBbits.TRISB4
	#define DDR_KP_C2 			TRISBbits.TRISB5
	#define DDR_KP_C3 			TRISBbits.TRISB6
	#define DDR_KP_C4 			TRISBbits.TRISB7
	
	#define KP_R1 				LATBbits.LATB0
	#define KP_R2 				LATBbits.LATB1
	#define KP_R3				LATBbits.LATB2
	#define KP_R4				LATBbits.LATB3
	#define KP_C1 				LATBbits.LATB4
	#define KP_C2				LATBbits.LATB5
	#define KP_C3 				LATBbits.LATB6
	#define KP_C4 				LATBbits.LATB7
	
	#define R_KP_C0				PORTBbits.RB4 
	#define R_KP_C1				PORTBbits.RB5 
	#define R_KP_C2				PORTBbits.RB6
	#define R_KP_C3				PORTBbits.RB7
	#define R_KP_R0				PORTBbits.RB0
	#define R_KP_R1				PORTBbits.RB1
	#define R_KP_R2				PORTBbits.RB2
	#define R_KP_R3				PORTBbits.RB3
	/***************************LCD CONFIGURATIONS*************************************/
	#define LCD_D0_D 			TRISDbits.TRISD0
	#define LCD_D1_D 			TRISDbits.TRISD1
	#define LCD_D2_D 			TRISDbits.TRISD2
	#define LCD_D3_D 			TRISDbits.TRISD3
	#define LCD_RS_D 			TRISDbits.TRISD4
	#define LCD_RW_D 			TRISDbits.TRISD5
	#define LCD_EN_D 			TRISDbits.TRISD6
	#define LCD_BL_D 			TRISDbits.TRISD7
	
	#define LCD_D0_P 			PORTDbits.RD0
	#define LCD_D1_P 			PORTDbits.RD1
	#define LCD_D2_P 			PORTDbits.RD2
	#define LCD_D3_P 			PORTDbits.RD3
	#define LCD_RS_P 			PORTDbits.RD4
	#define LCD_RW_P 			PORTDbits.RD5
	#define LCD_EN_P			PORTDbits.RD6
	#define LCD_BACKLIGHT		PORTDbits.RD7
#endif
