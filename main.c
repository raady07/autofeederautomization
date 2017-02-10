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
#include"flags.h"
#include"I2C.h"
#include"INIT.h"
#include"KP_InputFunctions.h"
#include"lcd.h"
#include"main.h"
#include"RTC.h"
#include"StringManipulated.h"
#include"timer.h"
#include"uart.h"
#include"xlcd2.h"
#include"reset.h"

_FOSC( CSW_FSCM_OFF & XT_PLL4 );
_FWDT( WDTPSB_10 & WDTPSA_512 & WDT_ON );		// WDT = 2ms * prescalar A * prescalar B // WDT = 2*10*512/1000 = 10.2 sec.
_FBORPOR( PWRT_64 & PBOR_OFF & BORV_45 & MCLR_EN );
_FGS( CODE_PROT_ON );

unsigned char Compile_Time[9] 	= __TIME__;
unsigned char Compile_Date[25] 	= __DATE__;
unsigned char ReadBuff[30];
unsigned char ExstingPW[5], MasterPW[5] = "8722";
unsigned char En_Display_Lcd = 0;
unsigned char system_mode 		= 0;
unsigned char No_of_Shifts 		= 0;
unsigned char feed_type 		= 0, Kgs_Per_Shift 	= 10, Sec_Per_Kg = 0;
unsigned char total_feed_in_kgs = 0, left_feed_in_kgs 	= 0, autorefill_feed = 0;;
unsigned char kgs_per_cycle 	= 0;
unsigned char cycle_cycle_mins 	= 0;
unsigned char ar_mode 			= 0;
unsigned char start_cycles 		= 0;
unsigned char Feeder_ON 		= 0;
unsigned char manual_mode_on 	= 0;
unsigned char *ptrdummy, *ptrdummy1;
unsigned int  Tics_ON = 0, Tics_OFF = 0;
unsigned char Data_Mod 			= 0;						// detect data modifications.
unsigned char Auto_Plus_load 	= 0;
unsigned char Feed_error_led	= 0;
unsigned char Present_Auto_Plus_shift_count = 0;
unsigned int  dummysec = 0, flag_tics_on = 0, flag_tics_off = 0;
unsigned char flag_modify_feed  = 0, flag_show_parameters = 0, flag_onandoff = 0;
unsigned char flag_fastblink	= 0, flag_slowblink = 0;

Time SystemTime, TimeOnRTC, ONTime, OFFTime;
Time Auto_Next_ONTime, Auto_Next_OFFTime, Prev_OFF_Time;
Time AUTO_ONTime[7], AUTO_OFFTime[7];
TCF	 TCF_in_Kg;
TCF	 TCF_in_Kg_3, TCF_in_Kg_3p;
TCF	 TCF_in_Kg_4, TCF_in_Kg_4s;
TCF	 TCF_in_Kg_5, TCF_in_Kg_6;

extern unsigned char everysec, Every_Minute, data_write, Manual_Key_change;
extern unsigned char Sec, Min, Hour, DOW, Date, Month, Year;
extern unsigned char KPFEED[18],KPKGSFEED[12],KPKGS[8];
extern unsigned char flag_day_change,cnt_ledtask;
extern unsigned int Start_address;

int	main(void)
{
	unsigned char i = 0;
	Time temp_on;
	temp_on.Value = 0;
	InitAllPheriperals();
	if(total_feed_in_kgs > 0)
	{
		putsUART1((unsigned int*)"\tKgs/Cyc: ");
		PCPutInt1(kgs_per_cycle); 
		putsUART1((unsigned int*)"\tLeft Feed: ");
		PCPutInt1(left_feed_in_kgs); 
		putsUART1((unsigned int*)"\tTotal Feed: ");
		PCPutInt1(total_feed_in_kgs); 
		putsUART1((unsigned int*)" Kgs\n\r");
	}

	if(!ONorOFF(ONTime, OFFTime))
	{
		//putsUART1((unsigned int*)"\n\r Feeder OFF");
		Switch_OFF_FEEDER();
	}
	resetmonitoring();
	RCONbits.SWDTEN = 1;
	while(1)
	{
		if(flag_day_change == 1)
		{
			if(ar_mode == 1)
			{
				autoreloadfunction();
			}
			if(system_mode == MANUAL)
			{
				OFFTime.Value = ONTime.Value;
				Char_WriteEEPROM_ex(OFFTime.Hours, 	EE_OFFTIME_HH);
				Char_WriteEEPROM_ex(OFFTime.Minutes,EE_OFFTIME_MM);
				Char_WriteEEPROM_ex(OFFTime.Seconds,EE_OFFTIME_SS);
				manual_mode_on = 0;
				Char_WriteEEPROM_ex(manual_mode_on,EE_MAN_ON_SET);
			}
			flag_day_change = 0;
		}
		if(++cnt_ledtask>= 25)
		{
			LED_Task();
			cnt_ledtask	= 0;
		}
		if(everysec >= 1)
		{
			flag_slowblink	= 1;
			everysec = 0;
			if(Data_Mod == 1)
			{
				Data_Mod = 0, data_write = 1;
				if(system_mode == AUTOPLUS)								// End else if
			 	{
					for(i = 0; i < No_of_Shifts; i++)
					{
						Kgs_Per_Shift = ConvertSTRING_Int(&KPKGSFEED[i*2],2);
						if(Kgs_Per_Shift > total_feed_in_kgs)
							Kgs_Per_Shift = total_feed_in_kgs;
						AUTO_OFFTime[i] = Compute_Off_Time(AUTO_ONTime[i]);
					}
				}
				else if(system_mode == AUTO)
				{
					Kgs_Per_Shift 	= total_feed_in_kgs;
					AUTO_OFFTime[6] = Compute_Off_Time(AUTO_ONTime[6]);
				}
				Tics_ON 		= Sec_Per_Kg * kgs_per_cycle;  // value is in Seconds.
				Tics_OFF		= 60 * (int)(cycle_cycle_mins);
				dummysec 		= 0;
				flag_modify_feed= 0;
				ptrdummy = (unsigned char*)&dummysec;
				String_WriteEEPROM_ex(EE_Count_ON_Cycles,ptrdummy,2);
				ptrdummy1= (unsigned char*)&flag_modify_feed;
				String_WriteEEPROM_ex(EE_Count_ON_Cycles1,ptrdummy1,2);
				Feed_error_led = 0;
			}
			decidemodeONOFFtime();
			CheckONTime();
			if(Feeder_ON == 1)			
				Control_Feeding();
		}
		if(En_Display_Lcd == 1)
		{
			flag_fastblink	= 1;
			LCDTask();
			En_Display_Lcd = 0;
		}
		__asm__ ("CLRWDT");			// clearing the WDT.
	}
}

void decidemodeONOFFtime(void)
{
	if(system_mode == AUTOPLUS)
    {
		Decide_AutoPlus_Shift_Position();
	}
    else if(system_mode == AUTO)
    {
		if(data_write == 1)
		{
			data_write 		= 0;
			ONTime.Value	= AUTO_ONTime[6].Value;
			OFFTime.Value 	= AUTO_OFFTime[6].Value;
		}	
    }
    else if(system_mode == MANUAL)
    {									
	    if(manual_mode_on == TRUE)							// Check hardware Manual Push Button Is pressed
	    {	
		    if(Manual_Key_change == 1)
			{
				Manual_Key_change = 0;
				if(left_feed_in_kgs > 0)
				{
		    		ONTime.Value = SystemTime.Value;
					Kgs_Per_Shift = left_feed_in_kgs;
					OFFTime = Compute_Off_Time(ONTime);
					Char_WriteEEPROM_ex(ONTime.Hours, 	EE_ONTIME_HH);
					Char_WriteEEPROM_ex(ONTime.Minutes, EE_ONTIME_MM);
					Char_WriteEEPROM_ex(ONTime.Seconds,	EE_ONTIME_SS);
					Char_WriteEEPROM_ex(OFFTime.Hours, 	EE_OFFTIME_HH);
					Char_WriteEEPROM_ex(OFFTime.Minutes,EE_OFFTIME_MM);
					Char_WriteEEPROM_ex(OFFTime.Seconds,EE_OFFTIME_SS);
				}
			}	
	    }
		else if(manual_mode_on == FALSE)		// Assigning off time for feeder in manual.
		{
			if(Manual_Key_change == 1)
			{	
				Manual_Key_change	= 0;
				//Feeder_ON	= 0;
				//Switch_OFF_FEEDER();
				if(left_feed_in_kgs > 0)
				{
					OFFTime.Value = SystemTime.Value;
					Char_WriteEEPROM_ex(Feeder_ON,		EE_FEEDER_ON);
					Char_WriteEEPROM_ex(OFFTime.Hours, 	EE_OFFTIME_HH);
					Char_WriteEEPROM_ex(OFFTime.Minutes,EE_OFFTIME_MM);
					Char_WriteEEPROM_ex(OFFTime.Seconds,EE_OFFTIME_SS);
					Char_WriteEEPROM_ex(left_feed_in_kgs,EE_LEFT_FEED);
				}
			}
		}
   }			
}

void CheckONTime(void)
{
	if(ONorOFF(ONTime,OFFTime))
	{
		flag_onandoff = 1;
		if(left_feed_in_kgs > 0)
		{
			Feed_error_led	= 0;
			if(Feeder_ON == 0)
			{			
				Feeder_ON 	= 1;
				dummysec 	= 0, flag_modify_feed = 0;
				Char_WriteEEPROM_ex(Feeder_ON,		EE_FEEDER_ON);
				Switch_ON_Feeder();
			} 
			else 
				Feeder_ON = 1;
		}
		else if(left_feed_in_kgs <= 0)
		{
			left_feed_in_kgs = 0;
			Feed_error_led = 1;		// Feed Error signal high.
		}
	}
	else
	{
		flag_onandoff 	= 0;
		Feed_error_led 	= 0;
		if(Feeder_ON == 1)
		{			
			Feeder_ON 	= 0;
			Char_WriteEEPROM_ex(Feeder_ON,		EE_FEEDER_ON);
			if(( Relay_1 == 1) || ( Relay_2 == 1 ) || ( Relay_3 == 1 ))
			{
				Switch_OFF_FEEDER();
			}
			if(left_feed_in_kgs <= 1)
			{
				left_feed_in_kgs = 0;
				Char_WriteEEPROM_ex(left_feed_in_kgs,EE_LEFT_FEED);		
			}
			dummysec = 0, flag_modify_feed = 0;
			ptrdummy = (unsigned char*)&dummysec;
			String_WriteEEPROM_ex(EE_Count_ON_Cycles,ptrdummy,2);
			ptrdummy1= (unsigned char*)&flag_modify_feed;
			String_WriteEEPROM_ex(EE_Count_ON_Cycles,ptrdummy1,2);
		} 
		else 
			Feeder_ON = 0;
	}

}

void Control_Feeding(void)
{	
	dummysec++;
	if(flag_modify_feed >= (Sec_Per_Kg))	
		flag_modify_feed = 0;
	if(flag_modify_feed >= (Sec_Per_Kg - 1))
	{
		left_feed_in_kgs--;
		Char_WriteEEPROM_ex(left_feed_in_kgs,EE_LEFT_FEED);
	}
	if(dummysec <= Tics_ON)
	{
		flag_modify_feed++;
		printf("dummysec %d < Run %d, modify %d, left feed %d\n",dummysec, Tics_ON, flag_modify_feed,left_feed_in_kgs );
		if(( Relay_1 == 0) || ( Relay_2 == 0 ) || ( Relay_3 == 0 ))
				Switch_ON_Feeder();
	}
	else if((dummysec > Tics_ON) &&(dummysec < Tics_ON+Tics_OFF))
	{
		flag_modify_feed = 0;
		printf("dummysec %d < Idle %d, modify %d, left feed %d\n",dummysec,Tics_ON + Tics_OFF, flag_modify_feed,left_feed_in_kgs );
		if(( Relay_1 == 1) || ( Relay_2 == 1 ) || ( Relay_3 == 1 ))
				Switch_OFF_FEEDER();
	}
	else if(dummysec >= Tics_ON+Tics_OFF)		dummysec = 0;
	
	if(SystemTime.Seconds == 0 || SystemTime.Seconds == 15 || SystemTime.Seconds == 30 || SystemTime.Seconds == 45 )
	{
		ptrdummy = (unsigned char*)&dummysec;
		String_WriteEEPROM_ex(EE_Count_ON_Cycles,ptrdummy,2);
		ptrdummy1= (unsigned char*)&flag_modify_feed;
		String_WriteEEPROM_ex(EE_Count_ON_Cycles1,ptrdummy1,2);
	}
	
}

void INIT_RELAY(void)
{
	DDR_Relay_1	=	Output;	
	DDR_Relay_2	=	Output;	
	DDR_Relay_3	=	Output;	
	DDR_Relay_4	=	Output;	
	DDR_LATCH_1	=	Output;
}

void Sync_with_RTCTime(void)
{
	SystemTime.Value 	= 0;
	SystemTime.Seconds 	= Sec;
	SystemTime.Minutes 	= Min;
	SystemTime.Hours 	= Hour;
}	
void makingallzero(void)
{
	unsigned char i = 0;
	ONTime.Value			= 0;
	OFFTime.Value 			= 0;	
	Auto_Next_ONTime.Value 	= 0;
	Auto_Next_OFFTime.Value = 0;
	Prev_OFF_Time.Value 	= 0;
	for(i = 0; i < 7; i++)
		AUTO_ONTime[i].Value	= 0;
	total_feed_in_kgs = 0, left_feed_in_kgs 	= 0;
}

void CheckForFirstTime(void)
{    	
	unsigned char Compile_len = 0;				// stores the length of the compile date and time
	unsigned char Read_compile_len = 0;			// stores the length of the compile date and time from eeprom
	unsigned char firsttime = 0, i = 0;			// stores the value of weather controller running for first time or not
	
	strcat((char*)Compile_Date,(const char*)Compile_Time);	// string attach for compile date and time
	Compile_len = strlen((const char*)Compile_Date);		// read compile date and time string length
	Read_compile_len = Char_ReadEEPROM_ex(EE_Compile_Len);	// read compile date and time string length from eeprom
	if(Read_compile_len != 0xFF)				// if it not for first time
	{
		String_ReadEEPROM_ex(EE_Compile_Date, ReadBuff, Read_compile_len);
		if(strcmp((const char*)ReadBuff, (const char*)Compile_Date) == 0)		
			firsttime = 1;						// compile version is same
		else
			firsttime = 0;						// compile version is different
	}
	else										// if it is for fist time	// for new board the value is always 0xff
		firsttime = 0;
	makingallzero();							// making all values to Zero
   	if(firsttime != 1)							// if it is for fist time
   	{	
		Char_WriteEEPROM_ex(Compile_len,  EE_Compile_Len);					// read compile date and time length
		String_WriteEEPROM_ex(EE_Compile_Date, Compile_Date, Compile_len);	// read compile date and time string
		strcpy((char*)ExstingPW,LCD_PASSWORD);    							// Write Default Password
		String_WriteEEPROM_ex(LCD_KEY, 	 (unsigned char*)LCD_PASSWORD,4);	
		system_mode			= DFLT_SYS_MODE;
		Char_WriteEEPROM_ex(DFLT_SYS_MODE,	EE_SYS_MODE);					// write default system mode
		ONTime.Hours 		= DFLT_ONTIME_HH;
		Char_WriteEEPROM_ex(DFLT_ONTIME_HH,	EE_ONTIME_HH);					// write default ontime hours value
		Char_WriteEEPROM_ex(DFLT_ONTIME_MM,	EE_ONTIME_MM);					// write default ontime minutes value
		Char_WriteEEPROM_ex(DFLT_OFFTIME_HH,EE_OFFTIME_HH);					// write default off time hours value
		Char_WriteEEPROM_ex(DFLT_OFFTIME_MM,EE_OFFTIME_MM);					// write default offtime minutes value
		No_of_Shifts		= DFLT_NOOFSHIFTS;
		Char_WriteEEPROM_ex(DFLT_NOOFSHIFTS,EE_Shifts);						// write default shifts -used in auto+.
		AUTO_ONTime[0].Hours	= DFLT_ONTIME_HH_1;
		AUTO_ONTime[1].Hours	= DFLT_ONTIME_HH_2;		
		AUTO_ONTime[2].Hours	= DFLT_ONTIME_HH_3;
		AUTO_ONTime[3].Hours	= DFLT_ONTIME_HH_4;
		AUTO_ONTime[4].Hours	= DFLT_ONTIME_HH_5;
		AUTO_ONTime[5].Hours 	= DFLT_ONTIME_HH_6;
		AUTO_ONTime[6].Hours	= DFLT_ONTIME_HH_7;
		Start_address = EE_ONTIME_HH_1;
		for(i = 0; i < 7; i++)												// For auto and auto+ modes
		{
			Char_WriteEEPROM_ex(AUTO_ONTime[i].Hours,	Start_address++);	// write default ontime hours value
			Char_WriteEEPROM_ex(AUTO_ONTime[i].Minutes,	Start_address++);	// write default ontime minutes value
		}
		Char_WriteEEPROM_ex(DFLT_TOTAL_FFED_KGS,	EE_TOTAL_FFED_KGS);		// write default total feed kgs value
		TCF_in_Kg.Value		= DFLT_TOTAL_FFED_KGS;
		TCF_in_Kg_3.Value	= DFLT_TOTAL_FFED_KGS;
		TCF_in_Kg_3p.Value	= DFLT_TOTAL_FFED_KGS;
		TCF_in_Kg_4.Value	= DFLT_TOTAL_FFED_KGS;
		TCF_in_Kg_4s.Value	= DFLT_TOTAL_FFED_KGS;
		TCF_in_Kg_5.Value	= DFLT_TOTAL_FFED_KGS;
		TCF_in_Kg_6.Value	= DFLT_TOTAL_FFED_KGS;
		Start_address 		= EE_TCF_KGS_BYTE1;
		for(i = 0; i < 28; i++)
			Char_WriteEEPROM_ex(0, Start_address++);						// write default value as 0 for total crop feed in kgs
		total_feed_in_kgs	= DFLT_TOTAL_FFED_KGS;
		Char_WriteEEPROM_ex(DFLT_TOTAL_FFED_KGS, EE_TOTAL_FFED_KGS);		// write default total feed in kgs
		left_feed_in_kgs	= DFLT_LEFT_FEED_KGS;
		Char_WriteEEPROM_ex(DFLT_LEFT_FEED_KGS, EE_LEFT_FEED);				// write default left feed in kgs
		kgs_per_cycle 		= DFLT_KGS_PER_CYCLE;
		Char_WriteEEPROM_ex(DFLT_KGS_PER_CYCLE,		EE_KGS_PER_CYCLE);		// write default kgs per cycle value
		cycle_cycle_mins 	= DFLT_CYCLE_INTERVAL;
		Char_WriteEEPROM_ex(DFLT_CYCLE_INTERVAL,	EE_CYCLE_INTERVAL);		// write default cycle interval
		Char_WriteEEPROM_ex(DFLT_AR_MODE,			EE_AR_MODE);			// default auto relaod mode
		strcpy((char*)KPFEED,DFLT_FEEDFLOW);
 		String_WriteEEPROM_ex(EE_SEC_TO_KGS_T,KPFEED, 18);					// default feed flow
		feed_type 			= DFLT_FEEDTYPE;
		Char_WriteEEPROM_ex(DFLT_FEEDTYPE,			EE_FEEDTYPE);			// write default feed type
		Sec_Per_Kg 			= SEC2KGS_3p;									// 45 secs as default feed type "3"		
		strcpy((char*)KPKGSFEED,(char *)DFLT_KGS_PER_SHIFT);
		String_WriteEEPROM_ex(EE_KGS_SHIFT, KPKGSFEED, 12);					// kgs / shift in auto+ mode array.
		Char_WriteEEPROM_ex(2,EE_START_CYCLES);								// default start cycles.
		Char_WriteEEPROM_ex(0, EE_FEEDER_ON);								// write feeder on status default status off
		Char_WriteEEPROM_ex(0, EE_MAN_ON_SET);
		autorefill_feed		= total_feed_in_kgs;
		Char_WriteEEPROM_ex(autorefill_feed,EE_AUTOREFILL_FEED);
		ptrdummy = (unsigned char*)&dummysec;
		String_WriteEEPROM_ex(EE_Count_ON_Cycles,ptrdummy,2);
		ptrdummy1= (unsigned char*)&flag_modify_feed;
		String_WriteEEPROM_ex(EE_Count_ON_Cycles1,ptrdummy1,2);
		Char_WriteEEPROM_ex(0, TRAP_val);
		Char_WriteEEPROM_ex(0, IOPUWR_val);
		Char_WriteEEPROM_ex(0, EXTR_val);
		Char_WriteEEPROM_ex(0, SWDT_val);
		Char_WriteEEPROM_ex(0, WDTO_val);
		Char_WriteEEPROM_ex(0, BOR_val);
		Char_WriteEEPROM_ex(0, POR_val);	
    }
	else if(firsttime == 1)
   	{	
		String_ReadEEPROM_ex(LCD_KEY, ExstingPW, 4);						// read LCD password.
		system_mode 		= Char_ReadEEPROM_ex(EE_SYS_MODE);				// read system mode.
		No_of_Shifts		= Char_ReadEEPROM_ex(EE_Shifts);				// read no of shifts - used in auto+
		Start_address 		= EE_ONTIME_HH_1;
		for(i = 0; i < 7; i++)												// for Auto and Auto+ modes
		{
			AUTO_ONTime[i].Hours   	= Char_ReadEEPROM_ex(Start_address++);	// read ontime hours value
			AUTO_ONTime[i].Minutes 	= Char_ReadEEPROM_ex(Start_address++);	// read ontime minutes value
		}
		TCF_in_Kg.byte1		= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE1);
		TCF_in_Kg.byte2		= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE2);
		TCF_in_Kg.byte3		= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE3);
		TCF_in_Kg.byte4		= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE4);
		TCF_in_Kg_3.byte1	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE1_3);
		TCF_in_Kg_3.byte2	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE2_3);
		TCF_in_Kg_3.byte3	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE3_3);
		TCF_in_Kg_3.byte4	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE4_3);
		TCF_in_Kg_3p.byte1	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE1_3p);
		TCF_in_Kg_3p.byte2	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE2_3p);
		TCF_in_Kg_3p.byte3	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE3_3p);
		TCF_in_Kg_3p.byte4	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE4_3p);
		TCF_in_Kg_4.byte1	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE1_4);
		TCF_in_Kg_4.byte2	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE2_4);
		TCF_in_Kg_4.byte3	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE3_4);
		TCF_in_Kg_4.byte4	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE4_4);
		TCF_in_Kg_4s.byte1	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE1_4s);
		TCF_in_Kg_4s.byte2	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE2_4s);
		TCF_in_Kg_4s.byte3	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE3_4s);
		TCF_in_Kg_4s.byte4	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE4_4s);
		TCF_in_Kg_5.byte1	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE1_5);
		TCF_in_Kg_5.byte2	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE2_5);
		TCF_in_Kg_5.byte3	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE3_5);
		TCF_in_Kg_5.byte4	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE4_5);
		TCF_in_Kg_6.byte1	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE1_6);
		TCF_in_Kg_6.byte2	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE2_6);
		TCF_in_Kg_6.byte3	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE3_6);
		TCF_in_Kg_6.byte4	= Char_ReadEEPROM_ex(EE_TCF_KGS_BYTE4_6);
		total_feed_in_kgs 	= Char_ReadEEPROM_ex(EE_TOTAL_FFED_KGS);	// total left feed
		left_feed_in_kgs	= Char_ReadEEPROM_ex(EE_LEFT_FEED);			// left feed.
		kgs_per_cycle		= Char_ReadEEPROM_ex(EE_KGS_PER_CYCLE);		// read default kgs per cycle value
		cycle_cycle_mins 	= Char_ReadEEPROM_ex(EE_CYCLE_INTERVAL);	// cycle to cycle interval.
		ar_mode				= Char_ReadEEPROM_ex(EE_AR_MODE);			// auto relaod mode.
		String_ReadEEPROM_ex(EE_SEC_TO_KGS_T, KPFEED, 18);				// feedflow array
		feed_type			= Char_ReadEEPROM_ex(EE_FEEDTYPE);			// feed type
		if(feed_type == 0)		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[0], 3);
		else if(feed_type == 1)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[3], 3);
		else if(feed_type == 2)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[6], 3);
		else if(feed_type == 3)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[9], 3);
		else if(feed_type == 4)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[12],3);
		else if(feed_type == 5)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[15],3);
		String_ReadEEPROM_ex(EE_KGS_SHIFT, KPKGSFEED, 12);				// kgs/ shift in auto+ mode
		start_cycles		= Char_ReadEEPROM_ex(EE_START_CYCLES);
		Feeder_ON 			= Char_ReadEEPROM_ex(EE_FEEDER_ON);
		manual_mode_on		= Char_ReadEEPROM_ex(EE_MAN_ON_SET);
		autorefill_feed		= Char_ReadEEPROM_ex(EE_AUTOREFILL_FEED);	
		ptrdummy = (unsigned char*)&dummysec;
		String_ReadEEPROM_ex(EE_Count_ON_Cycles,ptrdummy,2);
		ptrdummy1= (unsigned char*)&flag_modify_feed;
		String_ReadEEPROM_ex(EE_Count_ON_Cycles1,ptrdummy1,2);
   	}
	SystemTime.Res 	= 0;
   	ONTime.Res 		= 0;
   	OFFTime.Res		= 0;
   	TimeOnRTC.Res 	= 0;
	Tics_ON 		= Sec_Per_Kg * kgs_per_cycle;  // value is in Seconds.
	Tics_OFF		= 60 * (int)(cycle_cycle_mins);
	if(system_mode == AUTOPLUS)								
   	{
		for(i = 0; i < No_of_Shifts; i++)
		{
			Kgs_Per_Shift = ConvertSTRING_Int(&KPKGSFEED[i*2],2);
			if(Kgs_Per_Shift > total_feed_in_kgs)
				Kgs_Per_Shift = total_feed_in_kgs;
			AUTO_OFFTime[i] = Compute_Off_Time(AUTO_ONTime[i]);
		}
		Decide_AutoPlus_Shift_Position();
	}
	else if(system_mode == AUTO)
	{
		Kgs_Per_Shift 		= total_feed_in_kgs;
		AUTO_OFFTime[6] 	= Compute_Off_Time(AUTO_ONTime[6]);
		ONTime.Hours 		= AUTO_ONTime[6].Hours;
		ONTime.Minutes 		= AUTO_ONTime[6].Minutes;
		ONTime.Seconds		= AUTO_ONTime[6].Seconds;
		OFFTime.Hours 		= AUTO_OFFTime[6].Hours;
		OFFTime.Minutes 	= AUTO_OFFTime[6].Minutes;
		OFFTime.Seconds		= AUTO_OFFTime[6].Seconds;
	}
	else if(system_mode == MANUAL)
	{
		Kgs_Per_Shift 	= left_feed_in_kgs;
		if(Feeder_ON == 1)
		{
			ONTime.Value 	= SystemTime.Value;
			OFFTime 		= Compute_Off_Time(ONTime);
			Char_WriteEEPROM_ex(ONTime.Hours,	EE_ONTIME_HH);
			Char_WriteEEPROM_ex(ONTime.Minutes,	EE_ONTIME_MM);
			Char_WriteEEPROM_ex(ONTime.Seconds,	EE_OFFTIME_SS);
			Char_WriteEEPROM_ex(OFFTime.Hours,	EE_OFFTIME_HH);
			Char_WriteEEPROM_ex(OFFTime.Minutes,EE_OFFTIME_MM);
			Char_WriteEEPROM_ex(OFFTime.Seconds,EE_OFFTIME_SS);
			dummysec		= 0;
			flag_modify_feed= 0;
		}
		else
		{
			ONTime.Hours 		= Char_ReadEEPROM_ex(EE_ONTIME_HH);
			ONTime.Minutes 		= Char_ReadEEPROM_ex(EE_ONTIME_MM);
			ONTime.Seconds 		= Char_ReadEEPROM_ex(EE_ONTIME_SS);
			OFFTime.Hours 		= Char_ReadEEPROM_ex(EE_ONTIME_HH);
			OFFTime.Minutes 	= Char_ReadEEPROM_ex(EE_ONTIME_MM);
			OFFTime.Seconds		= Char_ReadEEPROM_ex(EE_OFFTIME_SS);
		}
	}
	printresetvalues();	
}

void LED_Task(void)
{
	if(Feed_error_led == 1)
	{
		onallleds();
	}
	else
	{
		if(flag_onandoff == 1)
		{
			if((flag_modify_feed == 0) && (flag_slowblink == 1))
			{
				blinkleds();
				flag_slowblink = 0;
			}
			else if((flag_modify_feed != 0)&& (flag_fastblink == 1))
			{
				blinkleds();
				flag_fastblink = 0;
			}
		}
		else 
			noblinkleds();
	}
}

void blinkleds(void)
{	
	if(system_mode == AUTOPLUS)
	{
		LED_AUTOP	= !LED_AUTOP;
		LED_AUTO	= 0;
		LED_MANL	= 0;
	}
	else if(system_mode == AUTO)
	{
		LED_AUTOP	= 0;
		LED_AUTO	= !LED_AUTO;
		LED_MANL	= 0;
	}
	else if(system_mode == MANUAL)
	{
		LED_AUTOP	= 0;
		LED_AUTO	= 0;
		LED_MANL	= !LED_MANL;
	}
}

void noblinkleds(void)
{
	if(system_mode == AUTOPLUS)
	{
		LED_AUTOP	= 1;
		LED_AUTO	= 0;
		LED_MANL	= 0;
	}
	else if(system_mode == AUTO)
	{
		LED_AUTOP	= 0;
		LED_AUTO	= 1;
		LED_MANL	= 0;
	}
	else if(system_mode == MANUAL)
	{
		LED_AUTOP	= 0;
		LED_AUTO	= 0;
		LED_MANL	= 1;
	}
}

void onallleds(void)
{
	LED_AUTOP	= 1;
	LED_AUTO	= 1;
	LED_MANL	= 1;
}

void Decide_AutoPlus_Shift_Position(void)
{
	unsigned char Next_Shift_count = 0;
	unsigned char Prev_shift_count = 0;
	unsigned char shift_count 	   = 0;
	while(shift_count < No_of_Shifts)		// if time is between on and off time.
	{
		if(ONorOFF(AUTO_ONTime[shift_count],AUTO_OFFTime[shift_count]))
		{
			Auto_Plus_load = 1;
			break;
		}	
		shift_count++;
	}
	if(shift_count == No_of_Shifts)			// if time is between off and on time.
	{
		shift_count = 0;
		while(shift_count < No_of_Shifts)
		{
			Next_Shift_count = shift_count + 1;
			if(Next_Shift_count >= No_of_Shifts)
				Next_Shift_count = 0;
			if(ONorOFF(AUTO_OFFTime[shift_count], AUTO_ONTime[Next_Shift_count]))
			{
				Auto_Plus_load = 0;
				break;
			}	
			shift_count++;
		}
	}
	else
	{
		Next_Shift_count = shift_count + 1;
	}
	
	if(shift_count >= No_of_Shifts)
		shift_count = 0;	
	if(Next_Shift_count >= No_of_Shifts)
		Next_Shift_count = 0;
	if(shift_count == No_of_Shifts - 1)
		Prev_shift_count = No_of_Shifts - 1;
	else Prev_shift_count = shift_count;
	if(left_feed_in_kgs == 0)
		Auto_Plus_load = 0;	

	Present_Auto_Plus_shift_count = shift_count+1;			

	if((Auto_Plus_load == 0) && (shift_count == No_of_Shifts - 1))
			Present_Auto_Plus_shift_count = 0;

	ONTime.Value 			= AUTO_ONTime[shift_count].Value;
	OFFTime.Value			= AUTO_OFFTime[shift_count].Value;
	Prev_OFF_Time.Value 	= AUTO_OFFTime[Prev_shift_count].Value;
	Auto_Next_ONTime.Value	= AUTO_ONTime[Next_Shift_count].Value;
	if(No_of_Shifts == 1)
	{
		Auto_Next_ONTime.Value 	= AUTO_ONTime[0].Value;
		Prev_OFF_Time.Value 	= AUTO_OFFTime[0].Value;
	}	
	ONTime.Res 			= 0;
   	OFFTime.Res 		= 0;
	Prev_OFF_Time.Res	= 0;
	Auto_Next_ONTime.Res= 0;
}

Time Compute_Off_Time(Time COMPUTETime)
{
	unsigned long int 	ONTIME_Secs			= 0;				//	Variable to hold ONTIME converted to seconds 
	unsigned long int 	OFFTIME_Secs		= 0;
	unsigned long int	Pre_OFFTIME_Secs	= 0;				//	Variable to hold Computed OFF Time in seconds
	unsigned int 		TIME4ON_Cycle		= 0;
	unsigned long int 	jjj					= 0;
	unsigned int		No_ON_Cycles		= 0;
	unsigned long int 	Idle_TIME			= 0;
	unsigned char		OFF_HH				= 0;
	unsigned char		OFF_MM				= 0;
	unsigned char		OFF_SS				= 0;
	unsigned int		Cyc2Cyc_interval	= 0;
	
	unsigned long int temp_HH_	=	0;
	unsigned long int temp_MM_	=	0;
	unsigned long int temp_SS_	=	0;
	Time TempOffTime;
	temp_HH_	=	(COMPUTETime.Hours);
	temp_MM_	=	(COMPUTETime.Minutes);
	temp_SS_	=	(COMPUTETime.Seconds);
	/*
		changes by Dileep: due to addition of auto+ mode 
		Note: 	In auto+ mode we are defining a particular amount of feed so added Kgs_Per_Shift, so we need to calculate
				based on that. where as in Auto and Manual mode the left_feed_in_kgs is the Kgs_per_shift.
		No_ON_Cycles		= (left_feed_in_kgs/kgs_per_cycle) ; 
		if(left_feed_in_kgs%kgs_per_cycle == 0)
		Pre_OFFTIME_Secs	=	(long int)(No_ON_Cycles*Cyc2Cyc_interval)+(long int)((long int)(TIME4ON_Cycle)*(long int)(No_ON_Cycles)) + (left_feed_in_kgs%kgs_per_cycle)*Sec_Per_Kg;

		Reason: main.c: In function 'Compute_Off_Time':				main.c:661: warning: statement with no effect		for( OFFTIME_Secs; OFFTIME_Secs > 0; OFFTIME_Secs-- )
	*/
	Cyc2Cyc_interval	= (int)cycle_cycle_mins * 60;								//	Convert Cycle to Cycle Interval Min to Seconds
	ONTIME_Secs			= (long int) ( (long int)(temp_HH_ * 3600) + (long int)( temp_MM_ * 60 ) + (long int)temp_SS_ );			//	ONTIME in Seconds
	TIME4ON_Cycle		= (kgs_per_cycle * Sec_Per_Kg);								//	To compute time for one cycle in Seconds
	No_ON_Cycles		= (Kgs_Per_Shift/kgs_per_cycle) ; 							//	To Compute No of cycles 
	if(No_ON_Cycles > 0)
		Idle_TIME			=	( (No_ON_Cycles - 1)*( Cyc2Cyc_interval ) )	;		//	To Compute Total idle time in seconds
	else
		Idle_TIME = 0;
	if(Kgs_Per_Shift%kgs_per_cycle == 0)
		Pre_OFFTIME_Secs	=	(long int)(Idle_TIME)	+	(long int)( (long int)(TIME4ON_Cycle) * (long int)(No_ON_Cycles)  );     //	To Compute total seconds to comple feeding
	else
		Pre_OFFTIME_Secs	=	(long int)(No_ON_Cycles*Cyc2Cyc_interval)+(long int)((long int)(TIME4ON_Cycle)*(long int)(No_ON_Cycles)) + (Kgs_Per_Shift % kgs_per_cycle)*Sec_Per_Kg;
	OFFTIME_Secs		=	ONTIME_Secs	+	Pre_OFFTIME_Secs;																		//	To Compute OFF Time in Seconds
	for(jjj = OFFTIME_Secs; jjj > 0; jjj-- )
	{
		if(OFF_SS >= 59)
		{
			OFF_SS = 0;
			if(OFF_MM >= 59)
			{
				OFF_MM = 0;
				if(OFF_HH >= 23)
				{
					OFF_HH = 0;
				}
				else
					OFF_HH++;
			}
			else
				OFF_MM++;
		}
		else
		{
			OFF_SS++;    	
		}  
	}	
	TempOffTime.Seconds	=	OFF_SS;
	TempOffTime.Minutes	=	OFF_MM;
	TempOffTime.Hours	=	OFF_HH;	
	TempOffTime.Res		= 	0;
	return TempOffTime;
}

unsigned char ONorOFF(Time on_time_, Time off_time_)
{
	SystemTime.Res	=	0;								//	Time Union MSB 
	on_time_.Res	=	0;								//	Time Union MSB 
	off_time_.Res	=	0;								//	Time Union MSB 
	if((SystemTime.Seconds == 0) && (flag_show_parameters == 0))
	{
		flag_show_parameters = 1;
		Show_Parameters();				
	}	
	else 
		flag_show_parameters = 0;
	if(on_time_.Value == off_time_.Value)
		return 0;	
	if(on_time_.Value < off_time_.Value)  				// On/Off are on the same day        
   	{
    	if( (SystemTime.Value >= on_time_.Value) && ( SystemTime.Value < off_time_.Value))
	   		return 1;
	   	else
	    	return 0;
	}
	else                              					// If On/Off are on different days 
	{
		if( (SystemTime.Value >= off_time_.Value) && ( SystemTime.Value < on_time_.Value))
	   		return 0;
	   	else
	   		return 1;
   } 
}

void Switch_OFF_FEEDER(void)
{
	Latch_1 	= 	Enable;								//	Enable Latch
	Relay_1		=	Low;
	Relay_2		=	Low;
	Relay_3		=	Low;
	TimerDelay_10ms(1);
	Latch_1		= 	Disable;							//	Disable Latch	
}

void Switch_ON_Feeder(void)
{
	Latch_1 	= 	Enable;								// Enable Latch
	Relay_1		=	High;
	Relay_2		=	High;
	Relay_3		=	High;
	TimerDelay_10ms(1);
	Latch_1 	= 	Disable;							// Disable Latch
}

void Show_Parameters(void)
{
	putsUART1((unsigned int*)"\r\nSystem:");
	PCPutInt1(SystemTime.Hours);
	WriteUART1(':');
	PCPutInt1(SystemTime.Minutes);
	WriteUART1(':');
	PCPutInt1(SystemTime.Seconds);
	WriteUART1(':');
	PCPutInt1(SystemTime.Value);
	putsUART1((unsigned int*)"\r\n");
	putsUART1((unsigned int*)"ONTime:");
	PCPutInt1(ONTime.Hours);
	WriteUART1(':');
	PCPutInt1(ONTime.Minutes);
	WriteUART1(':');
	PCPutInt1(ONTime.Seconds);
	WriteUART1(':');
	PCPutInt1(ONTime.Value);
	putsUART1((unsigned int*)"\r\n");
	putsUART1((unsigned int*)"OFFTime:");
	PCPutInt1(OFFTime.Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime.Minutes);
	WriteUART1(':');
	PCPutInt1(OFFTime.Seconds);	
	WriteUART1(':');
	PCPutInt1(OFFTime.Value);
	putsUART1((unsigned int*)"\r\n");
	putsUART1((unsigned int*)"Feed Left:");
	PCPutInt1(left_feed_in_kgs);
	putsUART1((unsigned int*)"Kgs");
	putsUART1((unsigned int*)"\r\n");
}

void InitAllPheriperals(void)
{		
	INIT_KEYPAD();									// Initialize Keypad
	INIT_XLCD();									// Initialize LCD
	INIT_TIMER();									// Initialise Timer
	INIT_UART1();									// Initialize UART
	INIT_I2C();										// Initialize I2C
	INIT_LED();										// Initialize LED
	INIT_RELAY();									// Initialize Relays
	EEPROM_WP_EN;									// Enable EEPROM Write Protect to avoid mishandling eeprom data
	LCD_StartUP_Screen();
	Read_RTC();
	Sync_with_RTCTime();
    CheckForFirstTime();
	putsUART1((unsigned int*)"\n\rSYSTEM READY");
}

void Convert_Sec_To_Time(unsigned long int sec, Time *temp)
{
	temp->Value =0;
	temp->Hours = sec/3600;
	temp->Minutes = (sec%3600)/60;
	temp->Seconds = (sec%3600)%60;
}

void resetmonitoring(void)
{
	unsigned char j = 0;			
	if(RCONbits.EXTR)								// MCLR pin reset
	{
		j = Char_ReadEEPROM_ex(EXTR_val);
		j++;
		Char_WriteEEPROM_ex(j,EXTR_val);
		RCONbits.EXTR = 0;
	}
	if(RCONbits.SWDTEN)								// Software WDT Enable is not cleared.
	{											
		j = Char_ReadEEPROM_ex(SWDT_val);
		j++;
		Char_WriteEEPROM_ex(j,SWDT_val);
	}
	if(RCONbits.WDTO)								// WatchDog Timer TimeOut Reset
	{												
		j = Char_ReadEEPROM_ex(WDTO_val);
		j++;
		Char_WriteEEPROM_ex(j,WDTO_val);
		RCONbits.WDTO = 0;	
	}
	if(RCONbits.BOR && RCONbits.POR == 0)			// Brown Out Reset
	{
		j = Char_ReadEEPROM_ex(BOR_val);
		j++;
		Char_WriteEEPROM_ex(j,BOR_val);
		RCONbits.BOR = 0;	
	}
	if(RCONbits.POR)								// Power on Reset, when power on reset occurs clear all reset flags.
	{
		j = Char_ReadEEPROM_ex(POR_val);
		j++;
		Char_WriteEEPROM_ex(j,POR_val);
		RCONbits.POR = 0;
		RCONbits.BOR = 0;	
	}
	if(RCONbits.TRAPR)								// Trap Conflict detected.
	{
		j = Char_ReadEEPROM_ex(TRAP_val);
		j++;
		Char_WriteEEPROM_ex(j,TRAP_val);
		RCONbits.TRAPR = 0;
	}	
	if(RCONbits.IOPUWR)								// Illegal Opcode detected.
	{
		j = Char_ReadEEPROM_ex(IOPUWR_val);
		j++;
		Char_WriteEEPROM_ex(j,IOPUWR_val);
		RCONbits.TRAPR = 0;
	}	
}

void printresetvalues(void)
{
	printf("TRAP Reset			:%d\n",	Char_ReadEEPROM_ex(TRAP_val));
	printf("Illegal Code Reset	:%d\n", Char_ReadEEPROM_ex(IOPUWR_val));
	printf("Ext.Pin Reset		:%d\n",	Char_ReadEEPROM_ex(EXTR_val));
	printf("SWDT Reset			:%d\n",	Char_ReadEEPROM_ex(SWDT_val));
	printf("WDTO Reset			:%d\n",	Char_ReadEEPROM_ex(WDTO_val));
	printf("Brownout Reset		:%d\n", Char_ReadEEPROM_ex(BOR_val));
	printf("Power Reset			:%d\n",	Char_ReadEEPROM_ex(POR_val));	
}
