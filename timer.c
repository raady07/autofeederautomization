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

#include"24lc256.h"
#include"Ex_EEPROM.h"
#include"timer.h"
#include"flags.h"
#include"INIT.h"
#include"RTC.h"
#include"main.h"

unsigned char ten_ms_cnt	= 0;
unsigned char everysec 		= 0, Every_Minute = 1;
unsigned char lcd_cont 		= 0;
unsigned char data_write	= 0, Manual_Key_change = 0;
unsigned char cnt_ledtask 	= 0, flag_day_change = 0;
unsigned char TimerDelay_flag_10ms = 0;
extern unsigned char system_mode, Manual_Key_Hold_Count, En_Display_Lcd;
extern Time SystemTime;

/********************************************************************************************
* Function Name     : INIT_TIMER                                         					*
* Description       : This function initializes the Timer1 and raise interrupt with time 	*
* 					  calculated in T1_TIME.  												*
* Parameters        : None                                               					*
* Return Value      : void.    																*
********************************************************************************************/
void  INIT_TIMER(void)
{
	T1CON    		= 0;				//  Ensure Timer 1 is in reset state
	IFS0bits.T1IF  	= 0;				//  Reset Timer 1 interrupt flag
	IPC0bits.T1IP  	= 7;				//  Set Timer1 interrupt priority level to 7
	IEC0bits.T1IE  	= 1;				//  Enable Interrrupt w.r.t Timer 1
	T1CONbits.TCKPS	= 1;				// Set Timer Input Clock Prescale Select bits (1:8)
	T1CONbits.TSIDL = 0;				// Continue Timer operation in Idle mode
	T1CONbits.TCS  	= 0;				//  Enable Internal clock (FOSC/4)
	PR1    			= __T1_TIME;		//  set Timer 1 period register for 10ms
	T1CONbits.TON  	= 1;				//  Enable Timer 1
}

/* Timer Interrupt */
void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt(void)
{
	IFS0bits.T1IF = 0;					// reset Timer 1 interrupt flag
	TimerDelay_flag_10ms = 1;
	cnt_ledtask++;
    if(++ten_ms_cnt>=100)
	{
		ten_ms_cnt 		= 0;
		everysec++;
		Clock();
		LED_RUN 		= !LED_RUN;				//	Toggle MCLR RUN Status LED
	}
	if(++lcd_cont > 33)								// for every 330ms
	{
		lcd_cont 		= 0;
		En_Display_Lcd 	= 1;
		LED_MRUN 		= !LED_MRUN;			//	Toggle MCLR RUN Status LED
	}
	if( (system_mode == MANUAL) && (KEY_MANL) )			// RF6
	{
		Manual_Key_Hold_Count++;
		if(Manual_Key_Hold_Count >= 200)
		{
			Manual_Key_Hold_Count = 0; 		
			Manual_Key_change = 1;
			manual_mode_on = !manual_mode_on;
			Char_WriteEEPROM_ex(manual_mode_on,EE_MAN_ON_SET);
		}
	}
}

/********************************************************************************************
* Function Name     : Clock                                      							*
* Description       : This function starts timer and updates with structure TIME		 	*
* Parameters        : void                                  								*
* Return Value      : void.    																*
********************************************************************************************/
void Clock(void)
{
	if ( SystemTime.Seconds < 59 )          	// is cummulative seconds < 59?
	{
		SystemTime.Seconds++;               	// yes, so increment seconds
	}
	else                         				// else seconds => 59     
	{
		Every_Minute = 1;
		SystemTime.Seconds = 0x00;          	// reset seconds
		if ( SystemTime.Minutes < 59 )      	// is cummulative minutes < 59?
		{
			SystemTime.Minutes++;           	// yes, so updates minutes  
		}
		else                     				// else minutes => 59
		{
			SystemTime.Minutes = 0x00;      	// reset minutes
			if (SystemTime.Hours < 23)    	// is cummulative hours < 23
			{
				Read_RTC();
				Sync_with_RTCTime();
			}
			else
			{
				SystemTime.Hours = 0x00;    	// Reset time
			}		  
		}
	}
	if(SystemTime.Hours == 0 && SystemTime.Minutes == 0 && SystemTime.Seconds == 0)
	{
		flag_day_change = 1;   	
		printf("day changed");
	}
}

/********************************************************************************************
* Function Name     : TimerDelay_10ms                                      					*
* Description       : This function creates a multiple of 10 msec with entered delay	 	*
* Parameters        : unsigned int delay                                  					*
* Return Value      : void.    																*
********************************************************************************************/
void TimerDelay_10ms(unsigned long int delay)
{
	TimerDelay_flag_10ms = 0;
	while(delay)
	{
		if(TimerDelay_flag_10ms == 1)
		{
			TimerDelay_flag_10ms = 0;
			delay--;
		}
	}
}

