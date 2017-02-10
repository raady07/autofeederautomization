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

#ifndef _timer_h
	#define _timer_h
	void INIT_TIMER(void);
	void Clock(void);
	void TimerDelay_10ms(unsigned long int delay);
	void LED_Task(void);
		
	/*
	|---------------------------------------|
	|	Reserved | Hrs 	|	Mins  |	Secs	|
	|	MSB		 |		|		  | LSB		|
	|---------------------------------------|
	*/
	typedef union  _Time
	{
	  unsigned long int Value;
		 struct
		{
		  unsigned char Seconds;
		  unsigned char Minutes;
		  unsigned char Hours;
		  unsigned char Res;								//	Reserved
	     };
	} Time;
#endif	
