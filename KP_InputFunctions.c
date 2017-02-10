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
 
 
#include <reset.h>
#include"24lc256.h"
#include"Ex_EEPROM.h"
#include"flags.h"
#include"INIT.h"
#include"KP_InputFunctions.h"
#include"RTC.h"
#include"main.h"
#include"StringManipulated.h"
#include"timer.h"
#include"XLCD2.h"
#include "uart.h"

#define AUTOTIME	0		// Auto mode settings
#define RTCTIME		1		// For RTC time edit
#define ShowScreen(x,y,z) Xaxis = x, Yaxis = y, Zaxis = z

unsigned char EnteredPW[5];
unsigned char CursorPos = 0, MaxCursorPos = 0;
unsigned char position = AUTO;
unsigned char auto_dis_lcd = 0, Temp_No_of_Shifts = 0, No_of_Sifts = 0, Temp_No_of_Sifts = 0;
unsigned int  Start_address = 0;
unsigned char KPKGS[8], KPHBuff[4], KPMBuff[4], KPKGSFEED[12], KPFEED[18],RenteredPW[5], KPDATE[6];
unsigned char *pkgs = 0, *pEnteredPW = 0, *pHour =0,  *pMin =0,   *pfeed = 0, *pdate = 0;
unsigned char temp_feedtype = 0, temp_ar_mode = 0;
unsigned char  temp_kgs_feed_dis = 0;
LCDflags Lcd = {0b00000000000000000000};

extern char Line1Buff[20];
extern unsigned char Yaxis, Xaxis, Zaxis, KP_Data;
extern unsigned char ExstingPW[5], MasterPW[5];
extern unsigned char system_mode, manual_mode_on, No_of_Shifts, feed_type,ar_mode;
extern unsigned char total_feed_in_kgs;	// total feeding in kgs
extern unsigned char left_feed_in_kgs;	// at present left feed in kgs
extern unsigned char  autorefill_feed;
extern unsigned char cycle_cycle_mins;	// cycle to cycle interval in mins
extern unsigned char Sec_Per_Kg;
extern unsigned char kgs_per_cycle;			// kgs per cycle
extern unsigned char start_cycles;			
extern unsigned char Data_Mod, data_write,Auto_Plus_load, Feeder_ON;

extern Time SystemTime, ONTime, OFFTime;
extern Time AUTO_ONTime[7];
extern TCF	TCF_in_Kg;						// Total_Crop_Feed_in_Kg
extern TCF	TCF_in_Kg_3;
extern TCF	TCF_in_Kg_3p;
extern TCF	TCF_in_Kg_4;
extern TCF	TCF_in_Kg_4s;
extern TCF	TCF_in_Kg_5;
extern TCF	TCF_in_Kg_6;


                                   /*12345678901234567890*/
static const char MENU[] 			=	"        MENU        ";  // Main Menu Name
static const char MMENU[]			=	"    MASTER MENU     ";  // Main Menu Name
static const char SPAC[] 			=	"                    ";  // Black Spcace
static const char Ero[] 			=	"        Error       ";

static const char PAM_[]			=	"  MASTER PASSWORD   ";
static const char PAU_[] 			=   "   ENTER PASSWORD   ";
static const char PE_[]				=	"        ____        ";
static const char PWDE[] 			=	"   WRONG PASSWORD   ";

static const char OPER_MODE[] 		=	"   OPERATION MODE  >";	// Menuscreen7 - Set Auto or Manual or AutoPlus
static const char AUTOPLUSSET[]		=	"<  AUTO+ SETTINGS  >";	// Menuscreen6 - Set Auto Plus Setting
static const char AUTOSET[]		 	=	"<   AUTO SETTINGS  >";	// Menuscreen6 - Set Auto Setting
static const char KGS_FEED[]		=	"<    FEED REFILL   >"; // Menuscreen5 - Enter Refilled Feed
static const char KGS_PER_CYCLE[]	=	"<   NO OF KGS/CYC  >"; // Menuscreen4 - Enter Kgs/Cyc
static const char CYCLE_INTERVAL[]	=	"<  CYCLE INTERVAL  >"; // Menuscreen3 - Cyc 2 Cyc Interval
static const char TOTAL_CROP[] 		=	"<  TOTAL CROP FEED >"; // Menuscreen2 - Total crop feed.
static const char CHANGEPWD[] 		=   "<  CHANGE PASSWORD  "; // Menuscreen1 - Set Change Password
// For Master Screen
static const char SETTIME[]			=   "      SET TIME     >"; // Masterscreen1 - Set Time Screen
static const char SETDATE[]			=	"<     SET DATE     >"; // Masterscreen2 - Set Date Screen
static const char SETFEEDFLOW[] 	= 	"<   NO OF SEC/KG   >";	// Masterscreen3 - Set Feed Flow
static const char AUTORELOAD[]		= 	"<    AUTO RELOAD   >";	// Masterscreen4 - Enable/Disable Auto Reload
static const char STARTUPFEED[]		=	"<   STARTUP FEED   >";	// Masterscreen5 - Startup feed details
static const char RESET_TCF_KGS[]	=	"< RESET CROP FEED  >";	// Masterscreen6 - Reset Crop Feed
static const char RSTMENUPWD[]		= 	"<  RESET PASSWORD  >";	// Masterscreen7 - Reset Menu Password.
static const char DFSET[]			=	"< DEFAULT SETTINGS  "; // Masterscreen8 - Restore to default Setting
// For Menu Screen 7
static const char OPER_MODE1[] 		=	"   OPERATION MODE   ";	// heading for menuscreen1 option
static const char MANUAL_[]			=	"       MANUAL      >"; // Select Manual
static const char AUTO_[] 			=	"<       AUTO       >";	// select Auto
static const char AUTOPLUS_[]		= 	"<       AUTO+       ";	// select Auto Plus
// for Menuscreen 6
	// AutoPlus/Auto Settings
static const char SHIFTS_NO[]		=	"    No. OF SHIFTS   "; // Option 1 Set No of shifts
static const char SHIFT_NO[] 		=	"     SHIFT:         ";
static const char ON_TIME[]			=	"  ON TIME - __:__   ";
static const char SHIFT_KGS[]		=	"   SHIFT KGs: __    ";
// for Menuscreen 5 
static const char FEED_3[]			= 	"   FEED TYPE : 3   >";
static const char FEED_3S[]			= 	"<  FEED TYPE : 3P  >";
static const char FEED_4[]			= 	"<  FEED TYPE : 4   >";
static const char FEED_4S[]			= 	"<  FEED TYPE : 4S  >";
static const char FEED_5[]			= 	"<  FEED TYPE : 5   >";
static const char FEED_5S[]			= 	"<  FEED TYPE : 6    ";
static const char APFEEDNO[]		=	" S 1  2  3  4  5  6 ";
static const char APFEED_I[]		=	"  __+__+__+__+__+__ ";
static const char XX[]				=	"XX";
static const char INPUT_KGS_T[]		=	"        + ___ KGS   ";	// Total No of Kgs
static const char INPUT_KGS_T_AP[]	=	"      + ___/    KGS ";	// Total No of Kgs for AutoPlus mode
static const char KGS_FEED_T[]		=	"   NO OF KGS FEED   "; // No of Kgs/Cycle
static const char INVALID_KGS[]		=	"  EXCEEDING MAX KG  ";
static const char INVALID_KGS1[]	=	"   NO KGs ENTERED   ";
// For Menu Screen 4
static const char KGS_PER_CYCLE1[]	=	" NO OF KGS/CYC __   "; 
// For Menu Screen 3
static const char CYCLE_INIT[]		= 	"  CYC-CYC INTERVAL  ";
static const char INPUT_MINS[] 		=	"       __ Mins      ";
// For Menu Screen 5
static const char TOTAL_CROP1[] 	=	" TOTAL CROP      Kg ";
static const char TCF_KGS_TYPE[]	=	" TYPE            Kg ";
// For Menu Screen 6
static const char NEWPW[]			=	" ENTER NEW PASSWORD ";	
static const char RENEWPW[]			=	"  RE-ENTER PASSWORD ";
static const char MISPW[]			=	"  PASSWORD MISMATCH ";
// For Master Screen 1
static const char SETTIME1[] 		=	"      SET TIME      ";
static const char Time_[] 			=   "   HH:MM -  __:__   ";
static const char INVALID_T[] 		=	"    INVALID TIME    ";
// For Master Screen 2
static const char SETDATE1[]		=	"      SET DATE      ";
static const char Date_[]			=	"DD:MM:YY - __.__.__ ";
static const char INVALID_D[]		=	"    INVALID DATE    ";
// For Master Screen 3
static const char SEC_KG[]			=	"   FEED FLOW TYPE   ";
static const char INPUT_SECS_R[]	=	"     : ___ Secs/Kg >";
static const char INPUT_SECS_LR[]	=	"<    : ___ Secs/Kg >";
static const char INPUT_SECS_L[]	=	"<    : ___ Secs/Kg  ";
// For Master Screen 4
static const char AUTORELOAD1[] 	= 	"     AUTO RELOAD    ";
static const char EN_[] 			=	"       ENABLE      >"; // Option 1 
static const char DIS_[]			=	"<      DISABLE      "; // Option 2
// For Master Screen 5
static const char STARTCYCS[]		=	"  CONT. CYC's :     ";
// For Master Screen 6
static const char CLEARFEED[]	=	"     CLEAR FEED     ";
// For Master Menu 7
static const char RSTMENUPWD_[] = 	"   RESET PASSWORD   ";
// For Master Screen 8
static const char DFSET1[] 		= 	"   RESTORE SETTING  ";

static const char ENTER_CANCEL[] =	"  ENTER     CANCEL  ";
static const char SAVE_CANCEL[] =   "   SAVE     CANCEL  ";

/********************************************************************************************
* Function Name:  PasswordEnter																*
* Description:    Checks and validates Master/LCD paswword, once validated navigates to the	*
*				  corresponding screen.	 													*
* Parameters:     unsigned char																*
* Return Value:   void																		*
********************************************************************************************/
void  PasswordEnter( unsigned char PswdType )
{
	if( Lcd.Z0 == 0)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC); 
		XLCDL2home();
		if(PswdType == 0)			XLCDPutRamString(PAU_); 
		else if(PswdType == 1)		XLCDPutRamString(PAM_); 
		XLCDL4home();
		XLCDPutRamString(SPAC); 
		XLCDL3home();
		XLCDPutRamString(PE_);                               
		MoveLeft(12);
		Lcd.Z0 = 1, KP_Data = 0, CursorPos =0;
		XLCD_BLINKOON();
		pEnteredPW = EnteredPW;
	}
	else if(Lcd.Z0 == 1)
	{	
		if(KP_Data > 47 && KP_Data < 58 && CursorPos < 4)
		{
			CursorPos++;				// Counting the number of characters
			XLCDPut('*');				// Masking the key by '*'  
			*pEnteredPW++ = KP_Data;
			if(CursorPos == 4)			XLCD_CURSOR_OFF();
			KP_Data =0;
		}
		else if(KP_Data == 'E' && CursorPos == 4)
		{
			if(!PswdType)	
			{
				if(MemoryCMP(ExstingPW,EnteredPW))
					ShowScreen(1,7,0);
				else
				{
					WrongPassword();					// Display wrong passowrd screen.
					ShowScreen(0,0,0);
				}
			}
			else if(PswdType)
			{	
				if( MemoryCMP(MasterPW,EnteredPW))			
				{
					if(Xaxis == 3 && Yaxis == 0)
						ShowScreen(3,8,0);
					else if (Xaxis == 4 && Yaxis == 1) 
					{
						ShowScreen(Xaxis,Yaxis,0);
						Lcd.Z22 = 0, Lcd.Z23 = 1;
					}
					else if(Xaxis == 4 && Yaxis == 2)
					{
						ShowScreen(Xaxis,Yaxis,0);
						Lcd.Z20 = 0, Lcd.Z21 = 1;
					}
					else if(Xaxis == 4 && Yaxis == 3)
					{
						ShowScreen(Xaxis,Yaxis,0);
						Lcd.Z17 = 0, Lcd.Z18 = 1;
					}	
				}
				else
				{
					WrongPassword();					// Display wrong passowrd screen.
					if(Yaxis == 0)		ShowScreen(0,0,0);
					else if(Yaxis!=0)	ShowScreen(3,Yaxis,0);            
				}
				
			}
			Lcd.Z0 = 0, KP_Data = 0, CursorPos = 0, pEnteredPW = '\0';
			MemoryClear(EnteredPW);
		}
		else if (KP_Data == 'C')
		{
			if( !PswdType )
			{
				Lcd.Z0 = 0, KP_Data =0, pEnteredPW = '\0';               /* Go to the Main Menu if Wrong Password is pressed */
				ShowScreen(0,0,0);
			}
			else if( PswdType )
			{
				Lcd.Z0 = 0, KP_Data =0, pEnteredPW = '\0';               /* Go to the Main Menu if Wrong Password is pressed */
				if(Yaxis != 0)				ShowScreen(3,Yaxis,0);
				else 						ShowScreen(0,0,0);
			}
			MemoryClear(EnteredPW);
			XLCD_BLINKCURSOROFF();
		}
	}
}

/********************************************************************************************
* Function Name:  Menuscreen																*
* Description:    on Menu button press and after validating the password shows the Yaxis    *
				  valued screen which can be changed by up and down arrows					*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void MenuScreens(void)
{
	XLCDL1home();
	XLCDPutRamString(SPAC); 						
	XLCDL4home();
	XLCDPutRamString(SPAC); 				
	XLCDL2home();
	XLCDPutRamString(MENU);                         
	XLCDL3home();
	switch(Yaxis)
	{
		case 7:	XLCDPutRamString(OPER_MODE); 								// "Operational Mode"
				break;
		case 6:	if(system_mode == AUTOPLUS)		XLCDPutRamString(AUTOPLUSSET); 	// "Auto Plus Setting"	
				else if(system_mode == AUTO)	XLCDPutRamString(AUTOSET); 		// "Auto Setting"	
				else if(system_mode == MANUAL)
				{
					if(KP_Data == 'D')		Yaxis = 5;
					else if(KP_Data == 'U')	Yaxis = 7;
				}	
				break;
		case 5: XLCDPutRamString(KGS_FEED); 								// "Feed Settings"
				break;
		case 4: XLCDPutRamString(KGS_PER_CYCLE); 							// "Kgs to displace per cycle"
				break;
		case 3: XLCDPutRamString(CYCLE_INTERVAL); 							// "Cycle to Cycle Interval Settings"
				break;
		case 2:	XLCDPutRamString(TOTAL_CROP); 								// "Display TotalCrop Fed"
				break;
		case 1:	XLCDPutRamString(CHANGEPWD);								// "Change User Password"
				break;
		default:ErrorScreen();
	}
	if(KP_Data == 'E')			Xaxis = 2, KP_Data = 0;
	else if(KP_Data == 'C')
	{
		XLCD_BLINKCURSOROFF();
		KP_Data = 0, ShowScreen(0,0,0);
	}
}
/********************************************************************************************
* Function Name:  MasterScreen																*
* Description:    on button press E and Manual Button Enters this Menu						*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void MasterScreens(void)
{
	XLCDL1home();
	XLCDPutRamString(SPAC);
	XLCDL4home();
	XLCDPutRamString(SPAC); 
	XLCDL2home();
	XLCDPutRamString(MMENU); 						
	XLCDL3home();	
	switch(Yaxis)
	{
		case 8:	XLCDPutRamString(SETTIME); 		// "Set Time"
				break;
		case 7:	XLCDPutRamString(SETDATE); 		// "Set Date"
				break;
		case 6: XLCDPutRamString(SETFEEDFLOW); 	// "Set feed flow"
				break;
		case 5: XLCDPutRamString(AUTORELOAD);	// "Set En/Dis AutoReload"
				break;
		case 4: XLCDPutRamString(STARTUPFEED); 	// "Set % percentage of load feed"
				break;
		case 3: XLCDPutRamString(RESET_TCF_KGS);// "Reset Total Crop Feed"
				break;
		case 2: XLCDPutRamString(RSTMENUPWD);	// "Reset Password"
				break;
		case 1: XLCDPutRamString(DFSET);		// Restore to default settings
				break;
		default:ErrorScreen();
	}
	if(KP_Data == 'E')			Xaxis = 4, KP_Data = 0;
	else if(KP_Data == 'C')
	{
		XLCD_BLINKCURSOROFF();
		KP_Data = 0, ShowScreen(0,0,0);
	}
}

/********************************************************************************************
* Function Name:  CustomMenuScreens															*
* Description:    on Entering Menu custom screen which can be changed by up and down arrows	*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void CustomMenuScreens(void)
{
		switch(Yaxis)
		{
			case 7:	AutoManualMode();
					break;
			case 6:	if(system_mode == AUTOPLUS)		AutoPlusModeSetting();
					else if(system_mode == AUTO)	SetTimeScreen(AUTOTIME);
					break;
			case 5:	if(system_mode == AUTOPLUS)		Noof_Kgs_Feed_AP();
					else if(system_mode == AUTO || system_mode == MANUAL)
													Noof_Kgs_Feed();													
					break;
			case 4: Noof_Kgs_Per_Cycle();
					break;
			case 3:	Cycle_Interval();
					break;
			case 2:	Display_Total_Crop();
					break;
			case 1:	ChangePassword();
					break;
			default:ErrorScreen();
		}
}

/********************************************************************************************
* Function Name:  CustomMasterScreen														*
* Description:    Entering Menu custom screen which can be changed by up and down arrows	*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void CustomMasterScreens(void)
{
	switch(Yaxis)
	{
		case 8:	SetTimeScreen(RTCTIME);
				break;
		case 7:	SetDateScreen();
				break;
		case 6:	SecondsPerKG();
				break;
		case 5: AutoReloadScreen();
				break;
		case 4:	StartUpFeedScreen();
				break;
		case 3: Reset_TCF();
				break;
		case 2:	ResetMenuPassword();
				break;
		case 1:	SetHardReset();
				break;
		default:ErrorScreen();
	}
}

/********************************************************************************************
* Function Name:  AutoManualMode															*
* Description:    Decide to switch to Manual or Auto Mode									*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void AutoManualMode(void)
{
	if( Lcd.Z1 == 0 )
	{
		XLCDL1home();
		XLCDPutRamString(OPER_MODE1); 
		XLCDL2home();
		XLCDPutRamString(SPAC); 
		XLCDL3home();
		if(system_mode == AUTOPLUS)			XLCDPutRamString(AUTOPLUS_);
		else if(system_mode == AUTO)		XLCDPutRamString(AUTO_);
		else if(system_mode == MANUAL)		XLCDPutRamString(MANUAL_);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		Lcd.Z1 = 1, KP_Data = 0;
		position = system_mode;
	}
	else if(Lcd.Z1 == 1)
	{
		XLCDL3home();
		if(position == AUTOPLUS)		XLCDPutRamString(AUTOPLUS_);
		else if(position == AUTO)		XLCDPutRamString(AUTO_);
		else if(position == MANUAL)		XLCDPutRamString(MANUAL_);
		if( KP_Data == 'D' )
		{
			KP_Data = 0;
			if(position == MANUAL)			position = AUTO;
			else if(position == AUTO)		position = AUTOPLUS;
		}
		else if( KP_Data == 'U' )
		{
			KP_Data = 0;
			if(position == AUTOPLUS)		position = AUTO;
			else if(position == AUTO)		position = MANUAL;
		}
		else if( KP_Data == 'S' )
		{
			if(system_mode != position)
			{
				system_mode = position;
				Char_WriteEEPROM_ex(system_mode,EE_SYS_MODE);		// writing the changed mode
			 	Switch_OFF_FEEDER();
				//Feeder_ON = 0;
				if(system_mode == AUTOPLUS)
					Decide_AutoPlus_Shift_Position();
				else if(system_mode == MANUAL)	
				{
					ONTime.Value  = 0, OFFTime.Value = 0;
					Char_WriteEEPROM_ex(ONTime.Hours, 	EE_ONTIME_HH);
					Char_WriteEEPROM_ex(ONTime.Minutes, EE_ONTIME_MM);
					Char_WriteEEPROM_ex(ONTime.Seconds, EE_ONTIME_SS);
					Char_WriteEEPROM_ex(OFFTime.Hours, 	EE_OFFTIME_HH);
					Char_WriteEEPROM_ex(OFFTime.Minutes,EE_OFFTIME_MM);
					Char_WriteEEPROM_ex(OFFTime.Seconds,EE_ONTIME_SS);
				}
				manual_mode_on = FALSE;
				Char_WriteEEPROM_ex(manual_mode_on,EE_MAN_ON_SET);
				Data_Mod = 1;
			}
			ShowScreen(1,Yaxis,0), KP_Data =0, Lcd.Z1 = 0;
		}
		else if( KP_Data == 'C' )
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, KP_Data = 0, Lcd.Z1 = 0, ShowScreen(1,Yaxis,0);
		}	
	}
}

/********************************************************************************************
* Function Name:  AutoPlusModeSetting														*
* Description:    Enter shift timing and Kgs/Sec											*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void AutoPlusModeSetting(void)
{
	unsigned char bHour_on =0, bMin_on = 0;
	unsigned char i = 0;
	if(auto_dis_lcd == 0)
	{
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL1home();
		XLCDPutRamString(SPAC); 					// Blank Line.
		XLCDL2home();
		XLCDPutRamString(SHIFTS_NO); 				// Select options
		XLCDL4home();
		XLCDPutRamString(ENTER_CANCEL);
		XLCDL3home();
		XLCDPutRamString(SPAC); 					// Blank Line.
		MoveLeft(10);
		sprintf(Line1Buff,"%d",No_of_Shifts);
		XLCDPutRamString(Line1Buff);
		MoveLeft(1);
		XLCD_BLINKOON();
		auto_dis_lcd = 1, KP_Data = 0;
		Temp_No_of_Shifts = No_of_Shifts;
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if(auto_dis_lcd == 1)
	{
		if(KP_Data > 48 && KP_Data < 56)
		{
			XLCDPut(KP_Data);
			MoveLeft(1);
			Temp_No_of_Shifts = KP_Data - 48;
			KP_Data = 0;
		}
		else if(KP_Data == 'U' && Temp_No_of_Shifts > 0 && Temp_No_of_Shifts < 6)
		{
			KP_Data = 0,Temp_No_of_Shifts++;
			sprintf(Line1Buff,"%d",Temp_No_of_Shifts);
			XLCDPutRamString(Line1Buff);
			MoveLeft(1);
		}
		else if(KP_Data == 'D' && Temp_No_of_Shifts > 1 && Temp_No_of_Shifts < 7)
		{
			KP_Data = 0,Temp_No_of_Shifts--;
			sprintf(Line1Buff,"%d",Temp_No_of_Shifts);
			XLCDPutRamString(Line1Buff);
			MoveLeft(1);
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, auto_dis_lcd = 0, KP_Data = 0, Temp_No_of_Shifts = 0, No_of_Sifts = 0;
			ShowScreen(1,Yaxis,0);
		}
		else if(KP_Data == 'E')
		{
			XLCD_BLINKCURSOROFF();
			auto_dis_lcd = 2, KP_Data = 0, No_of_Sifts = 0;
		}
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if(auto_dis_lcd == 2)
	{	
		MemoryClear(KPHBuff);
		MemoryClear(KPMBuff);
		XLCDL1home();
		XLCDPutRamString(SHIFT_NO); 					// Blank Line.
		MoveLeft(8);
		sprintf(Line1Buff,"%d",No_of_Sifts+1);
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		if(Temp_No_of_Shifts == No_of_Sifts+1)		XLCDPutRamString(SAVE_CANCEL);
		else if(Temp_No_of_Shifts != No_of_Sifts+1)	XLCDPutRamString(ENTER_CANCEL);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL2home();
		XLCDPutRamString(ON_TIME); 
		MoveLeft(8);
		MemoryClear((unsigned char*)Line1Buff);
		sprintf(Line1Buff,"%02d", AUTO_ONTime[No_of_Sifts].Hours);
		XLCDPutRamString(Line1Buff);
		strcpy((char*)KPHBuff,Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		MoveRight(1);
		sprintf(Line1Buff,"%02d", AUTO_ONTime[No_of_Sifts].Minutes);
		XLCDPutRamString(Line1Buff);
		strcpy((char*)KPMBuff,Line1Buff);
		MoveLeft(5);
		XLCD_BLINKOON();
		auto_dis_lcd = 3, KP_Data = 0, CursorPos = 0;
		MemoryClear((unsigned char*)Line1Buff);
		pHour = KPHBuff, pMin = KPMBuff;
	}
	else if(auto_dis_lcd == 3)
	{
		if(KP_Data > 47 && KP_Data < 58 && CursorPos < 4)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			if(CursorPos == 1 || CursorPos == 2)		*pHour++ = KP_Data;
			else if(CursorPos == 3 || CursorPos == 4)	*pMin++  = KP_Data;
			if(CursorPos == 2)		XLCDPut(':');
			if(CursorPos == 4) 		XLCD_BLINKCURSOROFF();
			KP_Data = 0;
		}
		else if(KP_Data == 'U' && CursorPos > 0)
		{
			CursorPos--;
			MoveLeft(1);
			if(CursorPos == 0 || CursorPos == 1)		*pHour--;
			else if(CursorPos == 2 || CursorPos == 3)	*pMin--;
			if(CursorPos == 1)							MoveLeft(1)
			XLCD_BLINKOON();
			KP_Data = 0;
		}
		else if(KP_Data == 'D' && CursorPos < 4)
		{
			CursorPos++;
			MoveRight(1);
			if(CursorPos == 1 || CursorPos == 2)		*pHour++;
			else if(CursorPos == 3 || CursorPos == 4)	*pMin++;
			if(CursorPos == 2)							MoveRight(1)
			else if(CursorPos == 4)						XLCD_BLINKCURSOROFF();
			KP_Data = 0;
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			MemoryClear(KPHBuff); MemoryClear(KPMBuff);
			Data_Mod = 0, auto_dis_lcd = 0, KP_Data = 0, CursorPos = 0,*pHour = '\0', *pMin = '\0';
			ShowScreen(1,Yaxis,0);
		}
		else if(KP_Data == 'E')
		{ 
			if(No_of_Sifts + 1 < Temp_No_of_Shifts)
			{
				bHour_on 	= ConvertSTRING_CHAR(KPHBuff);
				bMin_on		= ConvertSTRING_CHAR(KPMBuff);
				if(bHour_on < 24 && bMin_on < 60)
				{
					AUTO_ONTime[No_of_Sifts].Hours		= bHour_on;
					AUTO_ONTime[No_of_Sifts].Minutes 	= bMin_on;
					AUTO_ONTime[No_of_Sifts].Seconds 	= 0;
					No_of_Sifts++;
				}
				else 
				{
					invalidtime();
				}
				CursorPos = 0, auto_dis_lcd = 2;
			}
			KP_Data = 0;
		}
		else if(KP_Data == 'S')
		{
			if(No_of_Sifts + 1 == Temp_No_of_Shifts)
			{
				bHour_on 	= ConvertSTRING_CHAR(KPHBuff);
				bMin_on		= ConvertSTRING_CHAR(KPMBuff);
				if(bHour_on < 24 && bMin_on < 60)
				{
					AUTO_ONTime[No_of_Sifts].Hours		= bHour_on;
					AUTO_ONTime[No_of_Sifts].Minutes 	= bMin_on;
					AUTO_ONTime[No_of_Sifts].Seconds 	= 0;
					No_of_Shifts = Temp_No_of_Shifts;
					Char_WriteEEPROM_ex(No_of_Shifts,EE_Shifts);
					Start_address = EE_ONTIME_HH_1;
					for(i = 0; i < Temp_No_of_Shifts; i++)
					{
						Char_WriteEEPROM_ex(AUTO_ONTime[i].Hours,	Start_address++);		// write default ontime hours value
						Char_WriteEEPROM_ex(AUTO_ONTime[i].Minutes, Start_address++);		// write default ontime minutes value
					}
					Data_Mod = 1;
				}
				else 
				{
					invalidtime();
				}
				CursorPos = 0, auto_dis_lcd = 0, Temp_No_of_Shifts = 0, No_of_Sifts = 0, pHour = '\0', pMin = '\0';
				ShowScreen(1,Yaxis,0);
				XLCD_BLINKCURSOROFF();
			}
			KP_Data = 0;
		}
	}
}

/********************************************************************************************
* Function Name:  Noof_Kgs_Feed_AP															*
* Description:    Modifiy input feed quantity in Auto Plus setting							*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void Noof_Kgs_Feed_AP(void)
{
	unsigned int temp_kgs_feed = 0, temp_kgs_feed_total = 0, jj = 0, i = 0,arr[6];
	if(Lcd.Z4 == 0 && Lcd.Z5 == 0)
	{
		temp_feedtype = feed_type;
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(ENTER_CANCEL);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL2home();
		if(temp_feedtype == 5)			XLCDPutRamString(FEED_5S);	
		else if(temp_feedtype == 4)		XLCDPutRamString(FEED_5);	
		else if(temp_feedtype == 3)		XLCDPutRamString(FEED_4S);	
		else if(temp_feedtype == 2)		XLCDPutRamString(FEED_4);	
		else if(temp_feedtype == 1)		XLCDPutRamString(FEED_3S);	
		else if(temp_feedtype == 0)		XLCDPutRamString(FEED_3);
		Lcd.Z4 = 1, Lcd.Z5 = 0, KP_Data = 0;
	}
	else if(Lcd.Z4 == 1 && Lcd.Z5 == 0)
	{	
		XLCDL2home();
		if(temp_feedtype == 5)			XLCDPutRamString(FEED_5S);	
		else if(temp_feedtype == 4)		XLCDPutRamString(FEED_5);	
		else if(temp_feedtype == 3)		XLCDPutRamString(FEED_4S);	
		else if(temp_feedtype == 2)		XLCDPutRamString(FEED_4);	
		else if(temp_feedtype == 1)		XLCDPutRamString(FEED_3S);	
		else if(temp_feedtype == 0)		XLCDPutRamString(FEED_3);
		if(KP_Data == 'U' && temp_feedtype > 0)			KP_Data = 0, temp_feedtype--;
		else if(KP_Data == 'D' && temp_feedtype <5)		KP_Data = 0, temp_feedtype++;
		else if(KP_Data == 'E')							KP_Data = 0, Lcd.Z4 = 0, Lcd.Z5 = 1;
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, KP_Data = 0, Lcd.Z4 = 0, Lcd.Z5 = 0, ShowScreen(1,Yaxis,0);
		}
	}
	else if(Lcd.Z4 == 0 && Lcd.Z5 == 1)
	{
		MemoryClear((unsigned char*)Line1Buff);
		MemoryClear((unsigned char *)KPHBuff);
		XLCDL1home();
		XLCDPutRamString(APFEEDNO);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL);
		XLCDL3home();
		XLCDPutRamString(INPUT_KGS_T_AP);
		MoveLeft(18); 
		sprintf(Line1Buff,"%03d + ___/%03d",left_feed_in_kgs,temp_kgs_feed_dis);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		String_ReadEEPROM_ex(EE_KGS_SHIFT, KPKGSFEED, 12);
		XLCDL2home();
		XLCDPutRamString(APFEED_I);
		for(jj = 0; jj < 11; jj= jj+2)
			arr[i++] = ConvertSTRING_Int(&KPKGSFEED[jj],2);
		if(No_of_Shifts == 1)
		{
			MoveLeft(18);
			sprintf(Line1Buff,"%02d", arr[0]);
			XLCDPutRamString(Line1Buff);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			MoveLeft(17);
			MaxCursorPos = 5;
		}
		else if(No_of_Shifts == 2)
		{
			MoveLeft(18);
			sprintf(Line1Buff,"%02d+%02d", arr[0],arr[1]);
			XLCDPutRamString(Line1Buff);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			MoveLeft(17);
			MaxCursorPos = 7;
		}
		else if(No_of_Shifts == 3)
		{
			MoveLeft(18);
			sprintf(Line1Buff,"%02d+%02d+%02d", arr[0], arr[1], arr[2]);
			XLCDPutRamString(Line1Buff);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			MoveLeft(17);
			MaxCursorPos = 9;
		}
		else if(No_of_Shifts == 4)
		{
			MoveLeft(18);
			sprintf(Line1Buff,"%02d+%02d+%02d+%02d", arr[0], arr[1], arr[2], arr[3]);
			XLCDPutRamString(Line1Buff);
			XLCDPut('+');
			XLCDPutRamString(XX);
			XLCDPut('+');
			XLCDPutRamString(XX);
			MoveLeft(17);
			MaxCursorPos = 11;
		}
		else if(No_of_Shifts == 5)
		{
			MoveLeft(18);
			sprintf(Line1Buff,"%02d+%02d+%02d+%02d+%02d", arr[0], arr[1], arr[2], arr[3], arr[4]);
			XLCDPutRamString(Line1Buff);
			XLCDPut('+');
			XLCDPutRamString(XX);
			MoveLeft(17);
			MaxCursorPos = 13;
		}
		else if(No_of_Shifts == 6)
		{
			MoveLeft(18);
			sprintf(Line1Buff,"%02d+%02d+%02d+%02d+%02d+%02d", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
			XLCDPutRamString(Line1Buff);
			MoveLeft(17);
			MaxCursorPos = 15;
		}
		XLCD_BLINKOON();
		pfeed = KPKGSFEED;
		pHour = KPHBuff;
		Lcd.Z4 = 1, Lcd.Z5 = 1, KP_Data = 0, CursorPos = 0;
	}
	else if(Lcd.Z4 == 1 && Lcd.Z5 == 1)
	{	
		MemoryClear((unsigned char *)Line1Buff);
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < MaxCursorPos)
		{ 
			CursorPos++;
			XLCDPut(KP_Data);
			if(No_of_Shifts == 1)
			{
				if(CursorPos <= 2)		*pfeed++ = KP_Data;
				else if(CursorPos > 2)	*pHour++ = KP_Data;
				if(CursorPos == 2)
				{
					MoveLeft(12);
					temp_kgs_feed_dis = ConvertSTRING_Int(&KPKGSFEED[0],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 2)
			{
				if(CursorPos <= 4)		*pfeed++ = KP_Data;
				else if(CursorPos > 4)	*pHour++ = KP_Data;
				if(CursorPos == 2)		XLCDPut('+');
				else if(CursorPos == 4)		
				{
					MoveLeft(15);
					for(jj = 0; jj < 3; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 3)
			{
				if(CursorPos <= 6)		*pfeed++ = KP_Data;
				else if(CursorPos > 6)	*pHour++ = KP_Data;
				if(CursorPos == 2 || CursorPos == 4)
					XLCDPut('+');
				else if(CursorPos == 6)	
				{
					MoveLeft(18);
					for(jj = 0; jj < 5; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 4)
			{
				if(CursorPos <= 8)		*pfeed++ = KP_Data;
				else if(CursorPos > 8)	*pHour++ = KP_Data;
				if(CursorPos == 2 || CursorPos == 4 || CursorPos == 6)
					XLCDPut('+');
				else if(CursorPos == 8)		
				{
					MoveLeft(21);
					for(jj = 0; jj < 7; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 5)
			{
				if(CursorPos <= 10)		*pfeed++ = KP_Data;
				else if(CursorPos > 10)	*pHour++ = KP_Data;
				if(CursorPos == 2 || CursorPos == 4 || CursorPos == 6 || CursorPos == 8)
					XLCDPut('+');
				else if(CursorPos == 10)
				{
					MoveLeft(24);
					for(jj = 0; jj < 9; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 6)
			{
				if(CursorPos <= 12)		*pfeed++ = KP_Data;
				else if(CursorPos > 12)	*pHour++ = KP_Data;
				if(CursorPos == 2 || CursorPos == 4 || CursorPos == 6 || CursorPos == 8 || CursorPos == 10)
					XLCDPut('+');
				else if(CursorPos == 12)	
				{
					MoveLeft(27);
					for(jj = 0; jj < 11; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			if(CursorPos == MaxCursorPos)	XLCD_BLINKCURSOROFF();
			KP_Data = 0;	
		}
		else if(KP_Data == 'U' && CursorPos > 0)
		{
			CursorPos --, KP_Data = 0;
			XLCD_BLINKOON();
			MoveLeft(1);		
			if(No_of_Shifts == 1)
			{
				if(CursorPos <= 1)				pfeed--, temp_kgs_feed_dis = 0;
				else if(CursorPos > 1)			pHour--;
				if(CursorPos == 1)				MoveRight(16)
			}
			else if(No_of_Shifts == 2)
			{
				if(CursorPos <= 3)				pfeed--, temp_kgs_feed_dis = 0;
				else if(CursorPos > 3)			pHour--;
				if(CursorPos == 1)				MoveLeft(1)
				else if(CursorPos == 3)			MoveRight(19)
			}
			else if(No_of_Shifts == 3)
			{
				if(CursorPos <= 5)				pfeed--, temp_kgs_feed_dis = 0;
				else if(CursorPos > 5)			pHour--;
				if(CursorPos == 1 || CursorPos == 3)
												MoveLeft(1)
				else if(CursorPos == 5)			MoveRight(22)
			}
			else if(No_of_Shifts == 4)
			{
				if(CursorPos <= 7)				pfeed--, temp_kgs_feed_dis = 0;
				else if(CursorPos > 7)			pHour--;
				if(CursorPos == 1 || CursorPos == 3 || CursorPos == 5)
												MoveLeft(1)
				else if(CursorPos == 7)			MoveRight(25)
			}
			else if(No_of_Shifts == 5)
			{
				if(CursorPos <= 9)				pfeed--, temp_kgs_feed_dis = 0;
				else if(CursorPos > 9)			pHour--;
				if(CursorPos == 1 || CursorPos == 3 || CursorPos == 5 || CursorPos == 7)
												MoveLeft(1)
				else if(CursorPos == 9)			MoveRight(28)
			}
			else if(No_of_Shifts == 6)
			{
				if(CursorPos <= 11)				pfeed--, temp_kgs_feed_dis = 0;
				else if(CursorPos > 11)			pHour--;
				if(CursorPos == 1 || CursorPos == 3 || CursorPos == 5 || CursorPos == 7 || CursorPos == 9)
												MoveLeft(1)
				if(CursorPos == 11)				MoveRight(31)
			}	
		}
		else if(KP_Data == 'D' && CursorPos < MaxCursorPos)
		{
			MemoryClear((unsigned char *)Line1Buff);
			CursorPos++, KP_Data = 0;
			MoveRight(1);
			if(No_of_Shifts == 1)
			{
				if(CursorPos <= 2)				pfeed++;
				else if(CursorPos > 2)			pHour++;
				if(CursorPos == 2)
				{
					MoveLeft(12);
					temp_kgs_feed_dis = ConvertSTRING_Int(&KPKGSFEED[0],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 2)
			{
				if(CursorPos <= 4)				pfeed++;
				else if(CursorPos > 4)			pHour++;
				if(CursorPos == 2)				MoveRight(1)
				else if(CursorPos == 4)			
				{
					MoveLeft(15);
					for(jj = 0; jj < 3; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 3)
			{
				if(CursorPos <= 6)				pfeed++;
				else if(CursorPos > 6)			pHour++;
				if(CursorPos == 2 || CursorPos == 4)
												MoveRight(1)
				else if(CursorPos == 6)		
				{
					MoveLeft(18);
					for(jj = 0; jj < 5; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 4)
			{
				if(CursorPos <= 8)				pfeed++;
				else if(CursorPos > 8)			pHour++;
				if(CursorPos == 2 || CursorPos == 4 || CursorPos == 6)
												MoveRight(1)
				else if(CursorPos == 8)
				{
					MoveLeft(21);
					for(jj = 0; jj < 7; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 5)
			{
				if(CursorPos <= 10)				pfeed++;
				else if(CursorPos > 10)			pHour++;
				if(CursorPos == 2 || CursorPos == 4 || CursorPos == 6 || CursorPos == 8)
											MoveRight(1)
				else if(CursorPos == 10)
				{
					MoveLeft(24);
					for(jj = 0; jj < 9; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			else if(No_of_Shifts == 6)
			{
				if(CursorPos <= 12)				pfeed++;
				else if(CursorPos > 12)			pHour++;
				if(CursorPos == 2 || CursorPos == 4 || CursorPos == 6 || CursorPos == 8 || CursorPos == 10)
											MoveRight(1)
				else if(CursorPos == 12)	
				{
					MoveLeft(27);
					for(jj = 0; jj < 11; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
					sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
					XLCDPutRamString(Line1Buff);
					MoveLeft(7);
				}
			}
			if(CursorPos == MaxCursorPos)	XLCD_BLINKCURSOROFF();
		}
		else if(KP_Data == 'E')
		{
			temp_kgs_feed_dis = 0;
			if(No_of_Shifts == 1)
			{
				CursorPos = 2;
				temp_kgs_feed_dis = ConvertSTRING_Int(&KPKGSFEED[0],2);
			}
			else if(No_of_Shifts == 2)
			{
				CursorPos = 4;
				for(jj = 0; jj < 3; jj = jj+2)
					temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
			}
			else if(No_of_Shifts == 3)
			{
				CursorPos = 6;
				for(jj = 0; jj < 5; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
			}
			else if(No_of_Shifts == 4)
			{
				CursorPos = 8;
				for(jj = 0; jj < 7; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
			}
			else if(No_of_Shifts == 5)
			{
				CursorPos = 10;

				for(jj = 0; jj < 9; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
			}
			else if(No_of_Shifts == 6)
			{
				CursorPos = 12;
				for(jj = 0; jj < 11; jj = jj+2)
						temp_kgs_feed_dis += ConvertSTRING_Int(&KPKGSFEED[jj],2);
			}
			pfeed = KPKGSFEED + CursorPos;
			KP_Data = 0;
			XLCDL3home();
			MoveRight(12);
			sprintf(Line1Buff,"%03d",temp_kgs_feed_dis);
			XLCDPutRamString(Line1Buff);
			MoveLeft(7);
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();	
			MemoryClear(KPHBuff);
			String_ReadEEPROM_ex(EE_KGS_SHIFT, KPKGSFEED, 12);
			Data_Mod = 0, Lcd.Z4 = 0, Lcd.Z5 = 0, KP_Data = 0, temp_kgs_feed_dis = 0, pfeed = '\0', pHour = '\0';
			ShowScreen(2,Yaxis,0);
		}
		else if(KP_Data == 'S' && CursorPos == MaxCursorPos)
		{
			temp_kgs_feed_total = ConvertSTRING_Int(&KPHBuff[0],3);
			temp_kgs_feed = temp_kgs_feed_dis;
			if(temp_kgs_feed < MAX_KGS && (temp_kgs_feed + left_feed_in_kgs)< MAX_KGS && (left_feed_in_kgs + temp_kgs_feed_total) < MAX_KGS)
			{
				feed_type = temp_feedtype;
				Char_WriteEEPROM_ex(feed_type, EE_FEEDTYPE);
				String_WriteEEPROM_ex(EE_KGS_SHIFT, KPKGSFEED, 12);
				total_feed_in_kgs =  left_feed_in_kgs + temp_kgs_feed_total;
				autorefill_feed		= total_feed_in_kgs;
				Char_WriteEEPROM_ex(autorefill_feed,EE_AUTOREFILL_FEED);
				left_feed_in_kgs = total_feed_in_kgs;
				Char_WriteEEPROM_ex(left_feed_in_kgs,EE_LEFT_FEED);
				TCF_in_Kg.Value += temp_kgs_feed_total;
				Char_WriteEEPROM_ex(total_feed_in_kgs, 	EE_TOTAL_FFED_KGS);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte1, 	EE_TCF_KGS_BYTE1);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte2, 	EE_TCF_KGS_BYTE2);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte3, 	EE_TCF_KGS_BYTE3);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte4, 	EE_TCF_KGS_BYTE4);
				if(temp_kgs_feed_total > 0)
				{	
					String_ReadEEPROM_ex(EE_SEC_TO_KGS_T, KPFEED, 18);
					if(feed_type == 0)
					{
						TCF_in_Kg_3.Value	+= temp_kgs_feed;
						Char_WriteEEPROM_ex(TCF_in_Kg_3.byte1, 	EE_TCF_KGS_BYTE1_3);
						Char_WriteEEPROM_ex(TCF_in_Kg_3.byte2, 	EE_TCF_KGS_BYTE2_3);
						Char_WriteEEPROM_ex(TCF_in_Kg_3.byte3, 	EE_TCF_KGS_BYTE3_3);
						Char_WriteEEPROM_ex(TCF_in_Kg_3.byte4, 	EE_TCF_KGS_BYTE4_3);
						Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[0],3);
					}
					else if(feed_type == 1)
					{
						TCF_in_Kg_3p.Value	+= temp_kgs_feed;
						Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte1, 	EE_TCF_KGS_BYTE1_3p);
						Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte2, 	EE_TCF_KGS_BYTE2_3p);
						Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte3, 	EE_TCF_KGS_BYTE3_3p);
						Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte4, 	EE_TCF_KGS_BYTE4_3p);
						Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[3], 3);
					}
					else if(feed_type == 2)
					{
						TCF_in_Kg_4.Value	+= temp_kgs_feed;
						Char_WriteEEPROM_ex(TCF_in_Kg_4.byte1, 	EE_TCF_KGS_BYTE1_4);
						Char_WriteEEPROM_ex(TCF_in_Kg_4.byte2, 	EE_TCF_KGS_BYTE2_4);
						Char_WriteEEPROM_ex(TCF_in_Kg_4.byte3, 	EE_TCF_KGS_BYTE3_4);
						Char_WriteEEPROM_ex(TCF_in_Kg_4.byte4, 	EE_TCF_KGS_BYTE4_4);
						Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[6], 3);
					}
					else if(feed_type == 3)
					{
						TCF_in_Kg_4s.Value	+= temp_kgs_feed;
						Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte1, 	EE_TCF_KGS_BYTE1_4s);
						Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte2, 	EE_TCF_KGS_BYTE2_4s);
						Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte3, 	EE_TCF_KGS_BYTE3_4s);
						Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte4, 	EE_TCF_KGS_BYTE4_4s);
						Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[9], 3);
					}
					else if(feed_type == 4)
					{
						TCF_in_Kg_5.Value	+= temp_kgs_feed;
						Char_WriteEEPROM_ex(TCF_in_Kg_5.byte1, 	EE_TCF_KGS_BYTE1_5);
						Char_WriteEEPROM_ex(TCF_in_Kg_5.byte2, 	EE_TCF_KGS_BYTE2_5);
						Char_WriteEEPROM_ex(TCF_in_Kg_5.byte3, 	EE_TCF_KGS_BYTE3_5);
						Char_WriteEEPROM_ex(TCF_in_Kg_5.byte4, 	EE_TCF_KGS_BYTE4_5);
						Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[12], 3);
					}
					else if(feed_type == 5)
					{
						TCF_in_Kg_6.Value	+= temp_kgs_feed;
						Char_WriteEEPROM_ex(TCF_in_Kg_6.byte1, 	EE_TCF_KGS_BYTE1_6);
						Char_WriteEEPROM_ex(TCF_in_Kg_6.byte2, 	EE_TCF_KGS_BYTE2_6);
						Char_WriteEEPROM_ex(TCF_in_Kg_6.byte3, 	EE_TCF_KGS_BYTE3_6);
						Char_WriteEEPROM_ex(TCF_in_Kg_6.byte4, 	EE_TCF_KGS_BYTE4_6);
						Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[18], 3);
					}
				}
				temp_kgs_feed_dis = 0;
				Data_Mod = 1;
			}
			else 
			{
				invalidkgs();
			}
			MemoryClear(KPHBuff);
			Lcd.Z4 = 0, Lcd.Z5 = 0, KP_Data = 0, pfeed = '\0', pHour = '\0';
			ShowScreen(1,Yaxis,0);	
		}
	}
}
/********************************************************************************************
* Function Name:  Noof_Kgs_Feed																*
* Description:    Modifiy input feed quantity in Auto and Manual							*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void Noof_Kgs_Feed(void)
{	
	unsigned char feed_kgs = 0;
	if(Lcd.Z4 == 0 && Lcd.Z5 == 0)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(ENTER_CANCEL);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		temp_feedtype = feed_type;
		XLCDL2home();
		if(temp_feedtype == 5)			XLCDPutRamString(FEED_5S);	
		else if(temp_feedtype == 4)		XLCDPutRamString(FEED_5);	
		else if(temp_feedtype == 3)		XLCDPutRamString(FEED_4S);	
		else if(temp_feedtype == 2)		XLCDPutRamString(FEED_4);	
		else if(temp_feedtype == 1)		XLCDPutRamString(FEED_3S);	
		else if(temp_feedtype == 0)		XLCDPutRamString(FEED_3);
		Lcd.Z4 = 1, Lcd.Z5 = 0, KP_Data = 0;
	}
	else if(Lcd.Z4 == 1 && Lcd.Z5 == 0)
	{
		XLCDL2home();
		if(temp_feedtype == 5)			XLCDPutRamString(FEED_5S);	
		else if(temp_feedtype == 4)		XLCDPutRamString(FEED_5);	
		else if(temp_feedtype == 3)		XLCDPutRamString(FEED_4S);	
		else if(temp_feedtype == 2)		XLCDPutRamString(FEED_4);	
		else if(temp_feedtype == 1)		XLCDPutRamString(FEED_3S);	
		else if(temp_feedtype == 0)		XLCDPutRamString(FEED_3);
		if(KP_Data == 'U' && temp_feedtype > 0)			KP_Data = 0, temp_feedtype--;
		else if(KP_Data == 'D' && temp_feedtype <5)		KP_Data = 0, temp_feedtype++;
		else if(KP_Data == 'E')							KP_Data = 0, Lcd.Z4 = 0, Lcd.Z5 = 1;
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, KP_Data = 0, Lcd.Z4 = 0, Lcd.Z5 = 0, ShowScreen(1,Yaxis,0);
		}
	}
	else if(Lcd.Z4 == 0 && Lcd.Z5 == 1)
	{ 
		MemoryClear(KPHBuff);
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL);
		XLCDL2home();
		XLCDPutRamString(INPUT_KGS_T);
		MoveLeft(16); 
		sprintf(Line1Buff,"%3d",left_feed_in_kgs);
		XLCDPutRamString(Line1Buff);
		MoveRight(3);
		XLCD_BLINKOON();
		Lcd.Z4 = 1, Lcd.Z5 = 1, KP_Data = 0, CursorPos = 0;
		pfeed = KPHBuff;
	}
	else if(Lcd.Z4 == 1 && Lcd.Z5 == 1)
	{
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < 3)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			*pfeed++ = KP_Data;
			if(CursorPos == 3)	XLCD_BLINKCURSOROFF();
			KP_Data =0;
		}
		else if(KP_Data == 'U' && CursorPos > 0)
		{
			pfeed--, CursorPos--, KP_Data = 0;
			MoveLeft(1);
			XLCD_BLINKOON();		
		}
		else if(KP_Data == 'D' && CursorPos < 3)
		{
			CursorPos++, pfeed++, KP_Data = 0;
			MoveRight(1);
			if(CursorPos == 2)	XLCD_BLINKCURSOROFF();
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			MemoryClear(KPKGSFEED);
			Data_Mod = 0, Lcd.Z4 = 0, Lcd.Z5 = 0, KP_Data = 0, CursorPos = 0, pfeed = '\0';
			ShowScreen(1,Yaxis,0);
		}
		else if(KP_Data == 'S' && CursorPos == 3)
		{	
			feed_kgs = ConvertSTRING_Int((unsigned char*)&KPHBuff[0],3);
			if( (feed_kgs >= 1) && (feed_kgs <= MAX_KGS) && ((feed_kgs + left_feed_in_kgs) <= MAX_KGS))
			{
				total_feed_in_kgs	= left_feed_in_kgs + feed_kgs;
				autorefill_feed		= total_feed_in_kgs;
				Char_WriteEEPROM_ex(autorefill_feed,EE_AUTOREFILL_FEED);
				left_feed_in_kgs 	= total_feed_in_kgs;
				Char_WriteEEPROM_ex(left_feed_in_kgs,EE_LEFT_FEED);
				TCF_in_Kg.Value += feed_kgs;
				feed_type = temp_feedtype;
				Char_WriteEEPROM_ex(feed_type, EE_FEEDTYPE);
				Char_WriteEEPROM_ex(total_feed_in_kgs, 	EE_TOTAL_FFED_KGS);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte1, 	EE_TCF_KGS_BYTE1);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte2, 	EE_TCF_KGS_BYTE2);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte3, 	EE_TCF_KGS_BYTE3);
				Char_WriteEEPROM_ex(TCF_in_Kg.byte4, 	EE_TCF_KGS_BYTE4);
				String_ReadEEPROM_ex(EE_SEC_TO_KGS_T, KPFEED, 18);
				if(feed_type == 0)
				{
					TCF_in_Kg_3.Value	+= feed_kgs;
					Char_WriteEEPROM_ex(TCF_in_Kg_3.byte1, 	EE_TCF_KGS_BYTE1_3);
					Char_WriteEEPROM_ex(TCF_in_Kg_3.byte2, 	EE_TCF_KGS_BYTE2_3);
					Char_WriteEEPROM_ex(TCF_in_Kg_3.byte3, 	EE_TCF_KGS_BYTE3_3);
					Char_WriteEEPROM_ex(TCF_in_Kg_3.byte4, 	EE_TCF_KGS_BYTE4_3);
					Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[0],3);
				}
				else if(feed_type == 1)
				{
					TCF_in_Kg_3p.Value	+= feed_kgs;
					Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte1, EE_TCF_KGS_BYTE1_3p);
					Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte2, EE_TCF_KGS_BYTE2_3p);
					Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte3, EE_TCF_KGS_BYTE3_3p);
					Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte4, EE_TCF_KGS_BYTE4_3p);
					Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[3], 3);
				}
				else if(feed_type == 2)
				{
					TCF_in_Kg_4.Value	+= feed_kgs;
					Char_WriteEEPROM_ex(TCF_in_Kg_4.byte1, 	EE_TCF_KGS_BYTE1_4);
					Char_WriteEEPROM_ex(TCF_in_Kg_4.byte2, 	EE_TCF_KGS_BYTE2_4);
					Char_WriteEEPROM_ex(TCF_in_Kg_4.byte3, 	EE_TCF_KGS_BYTE3_4);
					Char_WriteEEPROM_ex(TCF_in_Kg_4.byte4, 	EE_TCF_KGS_BYTE4_4);
					Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[6], 3);
				}
				else if(feed_type == 3)
				{
					TCF_in_Kg_4s.Value	+= feed_kgs;
					Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte1, EE_TCF_KGS_BYTE1_4s);
					Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte2, EE_TCF_KGS_BYTE2_4s);
					Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte3, EE_TCF_KGS_BYTE3_4s);
					Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte4, EE_TCF_KGS_BYTE4_4s);
					Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[9], 3);
				}
				else if(feed_type == 4)
				{
					TCF_in_Kg_5.Value	+= feed_kgs;
					Char_WriteEEPROM_ex(TCF_in_Kg_5.byte1, 	EE_TCF_KGS_BYTE1_5);
					Char_WriteEEPROM_ex(TCF_in_Kg_5.byte2, 	EE_TCF_KGS_BYTE2_5);
					Char_WriteEEPROM_ex(TCF_in_Kg_5.byte3, 	EE_TCF_KGS_BYTE3_5);
					Char_WriteEEPROM_ex(TCF_in_Kg_5.byte4, 	EE_TCF_KGS_BYTE4_5);
					Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[12], 3);
				}
				else if(feed_type == 5)
				{
					TCF_in_Kg_6.Value	+= feed_kgs;
					Char_WriteEEPROM_ex(TCF_in_Kg_6.byte1, 	EE_TCF_KGS_BYTE1_6);
					Char_WriteEEPROM_ex(TCF_in_Kg_6.byte2, 	EE_TCF_KGS_BYTE2_6);
					Char_WriteEEPROM_ex(TCF_in_Kg_6.byte3, 	EE_TCF_KGS_BYTE3_6);
					Char_WriteEEPROM_ex(TCF_in_Kg_6.byte4, 	EE_TCF_KGS_BYTE4_6);
					Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[18], 3);
				}
				Data_Mod = 1;
			}
			else
			{
				if(feed_kgs == 0)	invalidkgs_feed();
				else 				invalidkgs();
			}
			MemoryClear(KPHBuff);
			Lcd.Z4 = 0, Lcd.Z5 = 0, KP_Data = 0, CursorPos = 0, pfeed = '\0';
			ShowScreen(1,Yaxis,0);
		}
	}
}

/********************************************************************************************
* Function Name:  Noof_Kgs_Per_Cycle														*
* Description:    Modifiy input feed quantity												*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void Noof_Kgs_Per_Cycle(void)
{
	unsigned char temp = 0;
	if( Lcd.Z2 == 0 )
	{
		MemoryClear((unsigned char *)Line1Buff);
		MemoryClear(KPHBuff);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL);
		XLCDL2home();
		XLCDPutRamString(KGS_PER_CYCLE1); 
		MoveLeft(5);
		XLCDPut((kgs_per_cycle/10) + 48);
		XLCDPut((kgs_per_cycle%10) + 48);
		MoveLeft(2);
		sprintf(Line1Buff,"%02d",kgs_per_cycle);
		strcpy((char*)KPHBuff,Line1Buff);
		Lcd.Z2 = 1, Lcd.Z3 = 0, KP_Data = 0, CursorPos = 0;
		XLCD_BLINKOON();
		pHour = KPHBuff;
		MemoryClear((unsigned char *)Line1Buff);
	}
	else if( Lcd.Z2 == 1 )
	{	
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < 2)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			*pHour++ = KP_Data;
			if(CursorPos == 2)		XLCD_BLINKCURSOROFF();
			KP_Data =0;
			Lcd.Z3 = 1;
		}
		else if(KP_Data == 'U' && CursorPos > 0)
		{
			CursorPos--, *pHour--, KP_Data = 0;
			MoveLeft(1);
			XLCD_BLINKOON();
		}
		else if(KP_Data == 'D' && CursorPos < 2)
		{
			CursorPos++, *pHour++, KP_Data = 0;
			MoveRight(1);
			if(CursorPos == 2) XLCD_BLINKCURSOROFF();
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, KP_Data = 0, Lcd.Z2 = 0, Lcd.Z3 = 0, CursorPos = 0, *pHour = '\0';
			ShowScreen(1,Yaxis,0);
			MemoryClear(KPHBuff);
		}
		else if(KP_Data == 'S')
		{
			if(Lcd.Z3 == 1)
			{
				temp =  ConvertSTRING_CHAR(KPHBuff);
				if(temp > 0)
				{
					kgs_per_cycle = temp;
					Char_WriteEEPROM_ex(kgs_per_cycle,   EE_KGS_PER_CYCLE);
					Data_Mod = 1;
				}
			}
			XLCD_BLINKCURSOROFF();
			ShowScreen(1,Yaxis,0);
			KP_Data = 0, Lcd.Z2 = 0, Lcd.Z3 = 0, CursorPos = 0, *pHour = '\0';
			MemoryClear(KPHBuff);	
		}
	}
}

/********************************************************************************************
* Function Name:  Cycle_Interval															*
* Description:    Modifiy input feed quantity												*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void Cycle_Interval(void)
{
	unsigned char temp = 0;
	if( Lcd.Z6 == 0 )
	{
		MemoryClear((unsigned char *)Line1Buff);
		MemoryClear(KPHBuff);
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL2home();
		XLCDPutRamString(CYCLE_INIT); 
		XLCDL3home();
		XLCDPutRamString(INPUT_MINS);
		MoveLeft(13);
		sprintf(Line1Buff,"%02d",cycle_cycle_mins);
		XLCDPutRamString(Line1Buff);
		MoveLeft(2);
		strcpy((char*)KPHBuff,Line1Buff);
		Lcd.Z6 = 1, KP_Data = 0, CursorPos = 0;
		XLCD_BLINKOON();
		pfeed = KPHBuff;
		MemoryClear((unsigned char *)Line1Buff);
	}
	else if( Lcd.Z6 == 1 )
	{
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < 2)
		{
			CursorPos++, *pfeed++ = KP_Data;
			XLCDPut(KP_Data);
			if(CursorPos == 2)		XLCD_BLINKCURSOROFF();
			KP_Data =0;
		}
		else if(KP_Data == 'U' && CursorPos > 0)
		{
			CursorPos--, *pfeed--, KP_Data = 0;
			MoveLeft(1); 
			XLCD_BLINKOON();
		}
		else if(KP_Data == 'D' && CursorPos < 1)
		{
			CursorPos++, *pfeed++, KP_Data = 0;
			MoveRight(1);
			if(CursorPos == 2)		XLCD_BLINKCURSOROFF();
		}
		else if( KP_Data == 'C' )
		{
			XLCD_BLINKCURSOROFF();
			MemoryClear(KPHBuff);
			Data_Mod = 0, KP_Data = 0,  Lcd.Z6 = 0, *pfeed = '\0', ShowScreen(1,Yaxis,0);
		}
		else if(KP_Data == 'S')
		{
			temp =  ConvertSTRING_CHAR(KPHBuff);
			if(temp >= 1)
			{
				cycle_cycle_mins = temp;
				Char_WriteEEPROM_ex(cycle_cycle_mins,   EE_CYCLE_INTERVAL);
				Data_Mod = 1;
				ShowScreen(1,Yaxis,0), KP_Data = 0,  Lcd.Z6 = 0, *pfeed = '\0';
				XLCD_BLINKCURSOROFF();
			}
			else
			{
				ShowScreen(1,Yaxis,0), KP_Data = 0,  Lcd.Z6 = 0, *pfeed = '\0';
			}
			MemoryClear(KPHBuff);
		}		
		
	}
}

/********************************************************************************************
* Function Name:  Display_Total_Crop														*
* Description:    Display total crop														*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void Display_Total_Crop(void)
{
	unsigned int temp = 0;	
	if(Lcd.Z7 == 0 )
	{
		MemoryClear((unsigned char *)Line1Buff);
		XLCDL1home();
		XLCDPutRamString(TCF_KGS_TYPE);
		MoveLeft(12);
		XLCDPutRamString("3 ");
		temp = TCF_in_Kg_3.Value;
		MoveRight(2);
		sprintf(Line1Buff,"%4d",temp);
		XLCDPutRamString(Line1Buff);

		XLCDL2home();
		XLCDPutRamString(TCF_KGS_TYPE); 
		MoveLeft(12);
		XLCDPutRamString("3P");
		temp = TCF_in_Kg_3p.Value;		
		MoveRight(2);
		sprintf(Line1Buff,"%4d",temp);
		XLCDPutRamString(Line1Buff);

		XLCDL3home();
		XLCDPutRamString(TCF_KGS_TYPE);  
		MoveLeft(12);
		XLCDPutRamString("4S");
		temp = TCF_in_Kg_4s.Value;				
		MoveRight(2);
		sprintf(Line1Buff,"%4d",temp);
		XLCDPutRamString(Line1Buff);
		
		XLCDL4home();
		XLCDPutRamString(TOTAL_CROP1);
		temp = TCF_in_Kg.Value;
		MoveLeft(8);
		sprintf(Line1Buff,"%4d",temp);
		XLCDPutRamString(Line1Buff);
		Lcd.Z7 = 1, KP_Data = 0;
		MemoryClear((unsigned char *)Line1Buff);
	}
	else if(Lcd.Z7 == 1)
	{
		if(KP_Data == 'U')
			KP_Data = 0, Lcd.Z7 = 0;
		else if(KP_Data == 'D')
		{
			XLCDL1home();
			XLCDPutRamString(TCF_KGS_TYPE);
			MoveLeft(12);
			XLCDPutRamString("4 ");
			temp = TCF_in_Kg_4.Value;
			MoveRight(2);
			sprintf(Line1Buff,"%4d",temp);
			XLCDPutRamString(Line1Buff);
	
			XLCDL2home();
			XLCDPutRamString(TCF_KGS_TYPE); 
			MoveLeft(12);
			XLCDPutRamString("5 ");
			temp = TCF_in_Kg_5.Value;		
			MoveRight(2);
			sprintf(Line1Buff,"%4d",temp);
			XLCDPutRamString(Line1Buff);
	
			XLCDL3home();
			XLCDPutRamString(TCF_KGS_TYPE);  
			MoveLeft(12);
			XLCDPutRamString("6 ");
			temp = TCF_in_Kg_6.Value;				
			MoveRight(2);
			sprintf(Line1Buff,"%4d",temp);
			XLCDPutRamString(Line1Buff);
		}
		else if( KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Lcd.Z7 = 0, KP_Data = 0;
			ShowScreen(1,Yaxis,0);
		}
	}
}

/********************************************************************************************
* Function Name:  ChangePassword															*
* Description:    Changes password of Menu													*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void ChangePassword( void )
{
	if( Lcd.Z8 == 0)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(ENTER_CANCEL); 						// Select options
		XLCDL2home();
		XLCDPutRamString(NEWPW); 
		XLCDL3home();
		XLCDPutRamString(PE_); 
		MoveLeft(12);	                                   
		XLCD_BLINKOON();	                   
		Lcd.Z8 = 1, Lcd.Z9 = 0, KP_Data = 0, CursorPos =0;
		pEnteredPW = EnteredPW;              				
	}
	else if(Lcd.Z8 == 1)
	{
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < 4) 
		{
			CursorPos++;
			XLCDPut('*'); 								
			*pEnteredPW++ = KP_Data;
			*pEnteredPW   ='\0';
			if(CursorPos == 4)			XLCD_BLINKCURSOROFF();
			KP_Data =0;
		}
		else if(KP_Data == 'E' && CursorPos == 4)
		{
			if(Lcd.Z9 == 0)
			{
				XLCDL1home();
				XLCDPutRamString(SPAC); 
				XLCDL2home();
				XLCDPutRamString(RENEWPW);
				XLCDL4home();
				XLCDPutRamString(SAVE_CANCEL);
				XLCDL3home();
				XLCDPutRamString(PE_);
				MoveLeft(12);	                                   
				XLCD_BLINKOON();
				Lcd.Z9 = 1, CursorPos = 0;
				pEnteredPW = RenteredPW;
			}
			KP_Data = 0;
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Lcd.Z8 = 0, Lcd.Z9 = 0, KP_Data = 0, ShowScreen(1,Yaxis,0);
			MemoryClear(EnteredPW);
			MemoryClear(RenteredPW);
		}
		else if (KP_Data == 'S' && CursorPos == 4)
		{
			if(Lcd.Z9 == 1)
			{
				if(strcmp((const char*)EnteredPW,(const char*)RenteredPW) == 0)
				{
					strcpy((char*)ExstingPW,(char*)RenteredPW);
					String_WriteEEPROM_ex(LCD_KEY, (unsigned char*)ExstingPW, 4);
				}
				else 
				{
					mismatchpassword();
				}
				Lcd.Z8 =0, Lcd.Z9 = 0, pEnteredPW = 0, ShowScreen(1,Yaxis,0);
				MemoryClear(RenteredPW);
				MemoryClear(EnteredPW);
			}
			KP_Data = 0;
		}
	}
}

/********************************************************************************************
* Function Name:  SetTimeScreen																*
* Description:    Modifiy Time in RTC														*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void SetTimeScreen(unsigned char WHATTIME)
{
	unsigned char bHour =0, bMin =0;
	if( Lcd.Z10 == 0)
	{
		MemoryClear(KPHBuff);
		MemoryClear(KPMBuff);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 					
		XLCDL3home();
		XLCDPutRamString(SPAC);
		switch(WHATTIME)
		{
			case 0:	XLCDL1home();
					XLCDPutRamString(SPAC); 					
					XLCDL2home();
					XLCDPutRamString(ON_TIME);
					MoveLeft(8);
					MemoryClear((unsigned char*)Line1Buff);
					sprintf(Line1Buff,"%02d", AUTO_ONTime[6].Hours);
					XLCDPutRamString(Line1Buff);
					strcpy((char*)KPHBuff,Line1Buff);
					MemoryClear((unsigned char*)Line1Buff);
					MoveRight(1);
					sprintf(Line1Buff,"%02d", AUTO_ONTime[6].Minutes);
					XLCDPutRamString(Line1Buff);
					strcpy((char*)KPMBuff,Line1Buff);
					MoveLeft(5);
					break; 
			case 1: XLCDL1home();
					XLCDPutRamString(SETTIME1); 					
					XLCDL2home();
					XLCDPutRamString(Time_); 
					MoveLeft(8);
					MemoryClear((unsigned char*)Line1Buff);
					sprintf(Line1Buff,"%02d", SystemTime.Hours);
					XLCDPutRamString(Line1Buff);
					strcpy((char*)KPHBuff,Line1Buff);
					MemoryClear((unsigned char*)Line1Buff);
					MoveRight(1);
					sprintf(Line1Buff,"%02d", SystemTime.Minutes);
					XLCDPutRamString(Line1Buff);
					strcpy((char*)KPMBuff,Line1Buff);
					MoveLeft(5);
					break;
		}	 
		Lcd.Z10 = 1, Lcd.Z11 = 0, KP_Data = 0, CursorPos = 0;
		XLCD_BLINKOON();
		pHour = KPHBuff, pMin = KPMBuff;		
	}
	else if (Lcd.Z10 == 1)
	{
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < 4)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			if(CursorPos == 1 || CursorPos == 2 )	*pHour++ = KP_Data;
			else if(CursorPos >= 3)					*pMin++  = KP_Data; 
			if(CursorPos == 2)						XLCDPut(':');
			if(CursorPos == 4)						XLCD_BLINKCURSOROFF();
			Lcd.Z11 = 1, KP_Data = 0;
		}
		else if( KP_Data == 'U' && CursorPos > 0)
		{	
			CursorPos--;
			MoveLeft(1);
			if(CursorPos == 0 || CursorPos == 1)		*pHour--;
			else if(CursorPos == 2 || CursorPos == 3)	*pMin--;
			if(CursorPos == 1)							MoveLeft(1)
			XLCD_BLINKOON();
			KP_Data = 0;
		}	
		else if( KP_Data == 'D' && CursorPos < 4)
		{
			CursorPos++;
			MoveRight(1);
			if(CursorPos == 1 || CursorPos == 2)		*pHour++;
			else if(CursorPos == 3 || CursorPos == 4)	*pMin++;
			if(CursorPos == 2)							MoveRight(1)
			else if(CursorPos == 4)						XLCD_BLINKCURSOROFF();
			KP_Data = 0;
		}	
		else if( KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, Lcd.Z10 = 0, Lcd.Z11 = 0,KP_Data =0, pMin ='\0', pHour ='\0';
			MemoryClear(KPHBuff); MemoryClear(KPMBuff);
			switch(WHATTIME)
			{
				case 0:	ShowScreen(1,Yaxis,0);
						break;
				case 1: ShowScreen(3,Yaxis,0);
						break;
			}
		}
		else if( KP_Data == 'S')
		{
			if(Lcd.Z11 == 1)
			{
				bHour =  ConvertSTRING_CHAR(KPHBuff);
				bMin  =  ConvertSTRING_CHAR(KPMBuff);
				if(bHour < 24 &&  bMin < 60)
				{
					switch(WHATTIME)
					{
						case 0:	AUTO_ONTime[6].Hours = bHour, AUTO_ONTime[6].Minutes = bMin, AUTO_ONTime[6].Seconds = 0;
								Char_WriteEEPROM_ex(bHour,  EE_ONTIME_HH_7);
								Char_WriteEEPROM_ex(bMin,   EE_ONTIME_MM_7);
								break;
						case 1:	SystemTime.Hours = bHour,SystemTime.Minutes = bMin,SystemTime.Seconds = SystemTime.Res = 0;
								W_RTC_Time(SystemTime.Hours, SystemTime.Minutes, 0);
								break;	
					}
				}
				else
				{
					invalidtime();
				}
				Data_Mod = 1;
			}
			XLCD_BLINKCURSOROFF();
			Lcd.Z10 = 0, Lcd.Z11 = 0, KP_Data = 0, pMin ='\0', pHour ='\0';
			MemoryClear(KPHBuff); MemoryClear(KPMBuff);
			switch(WHATTIME)
			{
				case 0:	ShowScreen(1,Yaxis,0);
						break;
				case 1: ShowScreen(3,Yaxis,0);
						break;
			}
		}
	}
}

/********************************************************************************************
* Function Name:  SetDateScreen																*
* Description:    Modify Date in RTC														*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/	
void SetDateScreen(void)
{
	unsigned char bDD = 0, bMM = 0, bYY = 0;
	if(Lcd.Z12 == 0)
	{
		MemoryClear(KPDATE);
		XLCDL1home();
		XLCDPutRamString(SETDATE1); 					
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 					// Select options
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL2home();
		XLCDPutRamString(Date_); 
		MoveLeft(9);
		Lcd.Z12 = 1, Lcd.Z13 = 0, KP_Data = 0, CursorPos = 0;
		XLCD_BLINKOON();
		pdate = KPDATE;
	}
	if(Lcd.Z12 == 1)
	{
		if( KP_Data>47 && KP_Data <58 && CursorPos < 6)
		{
			XLCDPut(KP_Data);
			CursorPos++, pdate++;
			if(CursorPos == 2 || CursorPos == 4)		XLCDPut('.');		
			if(CursorPos == 6)							XLCD_BLINKCURSOROFF();
			Lcd.Z13 = 1, KP_Data = 0;
		}
		else if( KP_Data == 'U' && CursorPos > 0)
		{
			KP_Data = 0, CursorPos--, pdate--;
			MoveLeft(1);
			XLCD_BLINKOON();
			if(CursorPos == 1 || CursorPos == 3)		MoveLeft(1);	
		}
		else if( KP_Data == 'D' && CursorPos < 6)
		{
			KP_Data = 0, CursorPos++, *pdate++; 
			MoveRight(1);
			if(CursorPos == 2 || CursorPos == 4)		MoveRight(1);
			if(CursorPos == 6)							XLCD_BLINKCURSOROFF();
		}
		else if( KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, Lcd.Z12 = 0, Lcd.Z13 = 0, KP_Data =0, pdate='\0';
			MemoryClear(KPDATE);
			ShowScreen(3,Yaxis,0);
		}
		else if( KP_Data == 'S' && CursorPos == 6)
		{
			if(Lcd.Z13 == 1)
			{
				bDD = ConvertSTRING_Int((unsigned char*)&KPDATE[0],2);
				bMM = ConvertSTRING_Int((unsigned char*)&KPDATE[2],2);
				bYY = ConvertSTRING_Int((unsigned char*)&KPDATE[4],2);
				if(bDD <32 && bMM <13 && bYY <100 )
				{
					W_RTC_Date(bDD,bMM,bYY,1);
					Data_Mod = 1;
				}
				else
				{
					invaliddate();
				}
			}
			XLCD_BLINKCURSOROFF();
			MemoryClear(KPDATE);
			Lcd.Z12 = 0, Lcd.Z13 = 0, KP_Data = 0, pdate ='\0', ShowScreen(3,Yaxis,0);
		}
	}	
}

/********************************************************************************************
* Function Name:  SecondsPerKgn																*
* Description:    Modify Spread time for different kind of feeds							*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void SecondsPerKG(void)
{
	unsigned int temp = 0;
	unsigned char n = 0, nint = 0;
	if( Lcd.Z14 == 0 && Lcd.Z15 == 0 )
	{
		String_ReadEEPROM_ex(EE_SEC_TO_KGS_T, KPFEED, 18);
		MemoryClear((unsigned char *)Line1Buff);
		XLCDL1home();
		XLCDPutRamString(SEC_KG);
		XLCDL4home();
		XLCDPutRamString(ENTER_CANCEL);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL2home();
		XLCDPutRamString(INPUT_SECS_R);
		MoveLeft(18);
		XLCDPutRamString("3 ");
		MoveRight(3);
		temp = ConvertSTRING_Int(&KPFEED[0],3);
		sprintf(Line1Buff,"%03d",temp);
		XLCDPutRamString(Line1Buff);
		Lcd.Z14 = 1, Lcd.Z15 = 0, KP_Data = 0, CursorPos = 0, Zaxis = 0;
		MemoryClear((unsigned char *)Line1Buff);
		
	}
	else if(Lcd.Z14 == 1 && Lcd.Z15 == 0)
	{
		if(Zaxis == 0)
		{
			XLCDL2home();
			XLCDPutRamString(INPUT_SECS_R);
			MoveLeft(18);
			XLCDPutRamString("3 ");
			MoveRight(3);
			temp = ConvertSTRING_Int(&KPFEED[0],3);
			pfeed = KPFEED;
		}
		else if(Zaxis == 1)
		{
			XLCDL2home();
			XLCDPutRamString(INPUT_SECS_LR);
			MoveLeft(18);
			XLCDPutRamString("3P");
			MoveRight(3);
			temp = ConvertSTRING_Int(&KPFEED[3],3);
			pfeed = KPFEED + 3;
		}
		else if(Zaxis == 2)
		{
			XLCDL2home();
			XLCDPutRamString(INPUT_SECS_LR);
			MoveLeft(18);
			XLCDPutRamString("4 ");
			MoveRight(3);
			temp = ConvertSTRING_Int(&KPFEED[6],3);
			pfeed = KPFEED + 6;
		}
		else if(Zaxis == 3)
		{
			XLCDL2home();
			XLCDPutRamString(INPUT_SECS_LR);
			MoveLeft(18);
			XLCDPutRamString("4S");
			MoveRight(3);
			temp = ConvertSTRING_Int(&KPFEED[9],3);
			pfeed = KPFEED + 9;
		}
		else if(Zaxis == 4)
		{
			XLCDL2home();
			XLCDPutRamString(INPUT_SECS_LR);
			MoveLeft(18);
			XLCDPutRamString("5 ");
			MoveRight(3);
			temp = ConvertSTRING_Int(&KPFEED[12],3);
			pfeed = KPFEED + 12;
		}
		else if(Zaxis == 5)
		{
			XLCDL2home();
			XLCDPutRamString(INPUT_SECS_L);
			MoveLeft(18);
			XLCDPutRamString("6 ");
			MoveRight(3);
			temp = ConvertSTRING_Int(&KPFEED[15],3);
			pfeed = KPFEED + 15;
		}
		sprintf(Line1Buff,"%03d",temp);
		XLCDPutRamString(Line1Buff);
		if(KP_Data == 'U' && Zaxis > 0 )		Zaxis--, KP_Data = 0;
		else if(KP_Data == 'D' && Zaxis < 5)	Zaxis++, KP_Data = 0;
		else if(KP_Data == 'E')
		{
			XLCDL4home();
			XLCDPutRamString(SAVE_CANCEL);
			XLCDL2home();
			MoveRight(7);
			XLCD_BLINKOON();
			Lcd.Z14 = 1, Lcd.Z15 = 1, KP_Data = 0, CursorPos = 0;
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Lcd.Z14 = 0, Lcd.Z15 = 0, KP_Data = 0;
			ShowScreen(3,Yaxis,0);
		}
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if( Lcd.Z14 == 1 && Lcd.Z15 == 1)
	{
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < 3)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			*pfeed++ = KP_Data;
			if(CursorPos == 3)			XLCD_BLINKCURSOROFF();
			KP_Data =0;
		}
		else if(KP_Data == 'U' && CursorPos > 0)
		{
			CursorPos--, pfeed--, KP_Data = 0;
			XLCD_BLINKOON();
			MoveLeft(1);
		}
		else if(KP_Data == 'D'&& CursorPos < 3)
		{
			CursorPos++, pfeed++, KP_Data = 0;
			if(CursorPos == 3)			XLCD_BLINKCURSOROFF();
			MoveRight(1);
		}
		else if(KP_Data == 'C')
		{
			XLCDL4home();
			XLCDPutRamString(ENTER_CANCEL);
			String_ReadEEPROM_ex(EE_SEC_TO_KGS_T, KPFEED, 18);
			XLCD_BLINKCURSOROFF();
			ShowScreen(4,Yaxis,Zaxis), Data_Mod = 0, Lcd.Z14 = 1, Lcd.Z15 = 0, CursorPos = 0, KP_Data = 0;
		}
		else if( KP_Data == 'S' && CursorPos == 3)
		{
			XLCD_BLINKCURSOROFF();
			Lcd.Z14 = 1, Lcd.Z15 = 0, KP_Data = 0, CursorPos = 0;
			for(n = 0; n < 16; n=n+3)
			{
				temp = ConvertSTRING_Int((unsigned char*)&KPFEED[n],3);
				if( (temp > 0) && (temp < 251));
				else 	nint++;
			}
			if(nint == 0)
			{
				String_WriteEEPROM_ex(EE_SEC_TO_KGS_T,KPFEED,18);
				KP_Data = 0, Lcd.Z14 = 1, Lcd.Z15 = 0;
				if(feed_type == 0)		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[0], 3);
				else if(feed_type == 1)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[3], 3);
				else if(feed_type == 2)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[6], 3);
				else if(feed_type == 3)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[9], 3);
				else if(feed_type == 4)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[12],3);
				else if(feed_type == 5)	Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[15],3);
				Data_Mod = 1;
			}
			XLCDL4home();
			XLCDPutRamString(ENTER_CANCEL);
			ShowScreen(4,Yaxis,Zaxis);
		}
		
	}
}	

/********************************************************************************************
* Function Name:  AutoReloadScreen															*
* Description:    Enable or Disable Auto Reload Functionality								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void AutoReloadScreen(void)
{
	if(Lcd.Z16 == 0)
	{
		XLCDL1home();
		XLCDPutRamString(AUTORELOAD1); 					
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 	
		XLCDL2home();
		XLCDPutRamString(SPAC);
		XLCDL3home();
		if(ar_mode == 1) 		XLCDPutRamString(EN_); 
		else if(ar_mode == 0) 	XLCDPutRamString(DIS_); 
		Lcd.Z16 = 1,temp_ar_mode = ar_mode, KP_Data = 0;
	}
	else if(Lcd.Z16 == 1)
	{
		if(KP_Data == 'U')
		{
			XLCDL3home();
			XLCDPutRamString(EN_); 
			temp_ar_mode = 1, KP_Data = 0;
		}
		else if(KP_Data == 'D')
		{
			XLCDL3home();
			XLCDPutRamString(DIS_); 
			temp_ar_mode = 0, KP_Data = 0;
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, Lcd.Z16 = 0, KP_Data = 0;
			ShowScreen(3,Yaxis,0);
		}
		else if(KP_Data == 'S')
		{
			ar_mode = temp_ar_mode;
			Char_WriteEEPROM_ex(ar_mode,EE_AR_MODE);
			Data_Mod = 1, Lcd.Z16 = 0, KP_Data = 0;
			ShowScreen(3,Yaxis,0);
		}
		
	}
}

/********************************************************************************************
* Function Name:  StartUpFeedScreen															*
* Description:    Input how much percentage of feed at the start							*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void StartUpFeedScreen(void)
{	
	if(Lcd.Z24 == 0)
	{
		MemoryClear(KPHBuff);
		XLCDL1home();
		XLCDPutRamString(SPAC); 					
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 	
		XLCDL3home();
		XLCDPutRamString(SPAC); 	
		XLCDL2home();
		XLCDPutRamString(STARTCYCS);
		MoveLeft(4);
		sprintf(Line1Buff,"%02d",start_cycles);
		XLCDPutRamString(Line1Buff);
		strcpy((char*)KPHBuff,Line1Buff);
		MoveLeft(2);
		Lcd.Z24 = 1, KP_Data = 0, CursorPos = 0;
		XLCD_BLINKOON();
		pfeed = KPHBuff;
		MemoryClear((unsigned char *)Line1Buff);		
	}
	else if(Lcd.Z24 == 1)
	{

		if(KP_Data > 47 && KP_Data < 58 && CursorPos < 2)
		{	
			CursorPos++; *pfeed++ = KP_Data;
			XLCDPut(KP_Data);
			if(CursorPos == 2)		XLCD_BLINKCURSOROFF();
			 KP_Data = 0;
		}
		else if(KP_Data == 'U' && CursorPos > 0)	
		{
			KP_Data = 0,CursorPos--, pfeed--;
			XLCD_BLINKOON();
			MoveLeft(1);
		}
		else if(KP_Data == 'D' && CursorPos < 2)
		{
			KP_Data = 0,CursorPos++, pfeed++;
			MoveRight(1);
			if(CursorPos == 2) 		XLCD_BLINKCURSOROFF();
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Data_Mod = 0, Lcd.Z24 = 0, KP_Data = 0, pfeed = '\0';
			ShowScreen(3,Yaxis,0);
		}
		else if(KP_Data == 'S' && CursorPos == 2)
		{
			XLCD_BLINKCURSOROFF();
			start_cycles = ConvertSTRING_Int((unsigned char*)&KPHBuff[0], 2);
			Char_WriteEEPROM_ex(start_cycles,EE_START_CYCLES);
			Data_Mod = 1, Lcd.Z24 = 0, KP_Data = 0, pfeed = '\0';
			ShowScreen(3,Yaxis,0);
		}
	}
}

/********************************************************************************************
* Function Name:  Reset_TCF																	*
* Description:    Clear Total Crop Feed to Zero												*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void Reset_TCF(void)
{
	unsigned int i = 0;
	if( Lcd.Z17 == 0 && Lcd.Z18 == 0)
		PasswordEnter(1);
	else if(Lcd.Z17 == 0 && Lcd.Z18 == 1)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC); 
		XLCDL2home();
		XLCDPutRamString(CLEARFEED); 
		XLCDL3home();
		XLCDPutRamString(SPAC); 
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		Lcd.Z17 = 1, Lcd.Z18 = 1,KP_Data = 0;
	}
	else if(Lcd.Z17 == 1 && Lcd.Z18 == 1)
	{
		if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			Lcd.Z17 = 0, Lcd.Z18 = 0, KP_Data =0;
			ShowScreen(3,Yaxis,0);

		}
		else if (KP_Data == 'S')
		{
			TCF_in_Kg.Value		= 0;
			TCF_in_Kg_3.Value	= 0;
			TCF_in_Kg_3p.Value	= 0;
			TCF_in_Kg_4.Value	= 0;
			TCF_in_Kg_4s.Value	= 0;
			TCF_in_Kg_5.Value	= 0;
			TCF_in_Kg_6.Value	= 0;
			Start_address 		= EE_TCF_KGS_BYTE1;
			for(i = 0; i < 28; i++)
				Char_WriteEEPROM_ex(0, Start_address);
			left_feed_in_kgs = 0, total_feed_in_kgs = 0;
			Data_Mod = 1;
			Char_WriteEEPROM_ex(total_feed_in_kgs,	EE_TOTAL_FFED_KGS);
			Char_WriteEEPROM_ex(left_feed_in_kgs,	EE_LEFT_FEED);	
			Lcd.Z17 = 0, Lcd.Z18 = 0, KP_Data =0;;
			ShowScreen(3,Yaxis,0);
		}
	}
}

/********************************************************************************************
* Function Name:  ResetMenuPassword															*
* Description:    Rewrite to default Menu Password											*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void ResetMenuPassword(void)
{	
	if(Lcd.Z20 == 0 && Lcd.Z21 == 0)
	{
		PasswordEnter(1);
	}
	else if(Lcd.Z20 == 0 && Lcd.Z21 == 1)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL3home();
		XLCDPutRamString(SPAC); 
		XLCDL2home();
		XLCDPutRamString(RSTMENUPWD_); 
		Lcd.Z20 = 1, Lcd.Z21 = 1, KP_Data = 0;
	}
	else if(Lcd.Z20 == 1 && Lcd.Z21 == 1)
	{
		if(KP_Data == 'S')
		{
			Lcd.Z20 = 0, Lcd.Z21 = 0, KP_Data = 0;
			strcpy((char*)ExstingPW,LCD_PASSWORD);
			String_WriteEEPROM_ex(LCD_KEY, (unsigned char*)LCD_PASSWORD, 4);		 // Writing Default Password - LCD.
			ShowScreen(3,Yaxis,0);
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			ShowScreen(3,Yaxis,0);
			Lcd.Z20 = 0, Lcd.Z21 = 0, KP_Data = 0;
		}
	}
}

/********************************************************************************************
* Function Name:  SetHardReset																*
* Description:    Restore to default														*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void SetHardReset(void)
{
	if(Lcd.Z22 == 0 && Lcd.Z23 == 0)
	{
		PasswordEnter(1);
	}
	else if (Lcd.Z22 == 0 && Lcd.Z23 == 1)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL3home();
		XLCDPutRamString(SPAC); 
		XLCDL2home();
		XLCDPutRamString(DFSET1); 
		Lcd.Z22 = 1, Lcd.Z23 = 1, KP_Data = 0;
	}
	else if(Lcd.Z22 == 1 && Lcd.Z23 == 1)
	{
		if(KP_Data == 'S')
		{
			Lcd.Z22 = 0, Lcd.Z23 = 0, KP_Data = 0;
			Char_WriteEEPROM_ex(0xFF,  EE_Compile_Len);    /* To Check whether System is starting for the first time	*/
			TimerDelay_10ms(5);
			__asm__ volatile ("reset") ;
		}
		else if(KP_Data == 'C')
		{
			XLCD_BLINKCURSOROFF();
			ShowScreen(3,Yaxis,0);
			Lcd.Z22 = 0, Lcd.Z23 = 0, KP_Data = 0;
		}
	}
}

/********************************************************************************************
* Function Name:  Wrongpassword																*
* Description:    Display if the entered password is not valid								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void autoreloadfunction(void)
{
	Data_Mod = 1;
	left_feed_in_kgs = autorefill_feed;
	Char_WriteEEPROM_ex(left_feed_in_kgs,EE_LEFT_FEED);
	TCF_in_Kg.Value += left_feed_in_kgs;
	Char_WriteEEPROM_ex(TCF_in_Kg.byte1, EE_TCF_KGS_BYTE1);
	Char_WriteEEPROM_ex(TCF_in_Kg.byte2, EE_TCF_KGS_BYTE2);
	Char_WriteEEPROM_ex(TCF_in_Kg.byte3, EE_TCF_KGS_BYTE3);
	Char_WriteEEPROM_ex(TCF_in_Kg.byte4, EE_TCF_KGS_BYTE4);
	String_ReadEEPROM_ex(EE_SEC_TO_KGS_T, KPFEED, 18);
	if(feed_type == 0)
	{
		TCF_in_Kg_3.Value	+= left_feed_in_kgs;
		Char_WriteEEPROM_ex(TCF_in_Kg_3.byte1, EE_TCF_KGS_BYTE1_3);
		Char_WriteEEPROM_ex(TCF_in_Kg_3.byte2, EE_TCF_KGS_BYTE2_3);
		Char_WriteEEPROM_ex(TCF_in_Kg_3.byte3, EE_TCF_KGS_BYTE3_3);
		Char_WriteEEPROM_ex(TCF_in_Kg_3.byte4, EE_TCF_KGS_BYTE4_3);
		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[0],3);
	}
	else if(feed_type == 1)
	{
		TCF_in_Kg_3p.Value	+= left_feed_in_kgs;
		Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte1,	EE_TCF_KGS_BYTE1_3p);
		Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte2, EE_TCF_KGS_BYTE2_3p);
		Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte3, EE_TCF_KGS_BYTE3_3p);
		Char_WriteEEPROM_ex(TCF_in_Kg_3p.byte4, EE_TCF_KGS_BYTE4_3p);
		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[3], 3);
	}
	else if(feed_type == 2)
	{
		TCF_in_Kg_4.Value	+= left_feed_in_kgs;
		Char_WriteEEPROM_ex(TCF_in_Kg_4.byte1,	EE_TCF_KGS_BYTE1_4);
		Char_WriteEEPROM_ex(TCF_in_Kg_4.byte2, 	EE_TCF_KGS_BYTE2_4);
		Char_WriteEEPROM_ex(TCF_in_Kg_4.byte3, 	EE_TCF_KGS_BYTE3_4);
		Char_WriteEEPROM_ex(TCF_in_Kg_4.byte4, 	EE_TCF_KGS_BYTE4_4);
		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[6], 3);
	}
	else if(feed_type == 3)
	{
		TCF_in_Kg_4s.Value	+= left_feed_in_kgs;
		Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte1, EE_TCF_KGS_BYTE1_4s);
		Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte2, EE_TCF_KGS_BYTE2_4s);
		Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte3, EE_TCF_KGS_BYTE3_4s);
		Char_WriteEEPROM_ex(TCF_in_Kg_4s.byte4, EE_TCF_KGS_BYTE4_4s);
		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[9], 3);
	}
	else if(feed_type == 4)
	{
		TCF_in_Kg_5.Value	+= left_feed_in_kgs;
		Char_WriteEEPROM_ex(TCF_in_Kg_5.byte1, 	EE_TCF_KGS_BYTE1_5);
		Char_WriteEEPROM_ex(TCF_in_Kg_5.byte2, 	EE_TCF_KGS_BYTE2_5);
		Char_WriteEEPROM_ex(TCF_in_Kg_5.byte3, 	EE_TCF_KGS_BYTE3_5);
		Char_WriteEEPROM_ex(TCF_in_Kg_5.byte4, 	EE_TCF_KGS_BYTE4_5);
		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[12], 3);
				}
	else if(feed_type == 5)
	{
		TCF_in_Kg_6.Value	+= left_feed_in_kgs;
		Char_WriteEEPROM_ex(TCF_in_Kg_6.byte1, 	EE_TCF_KGS_BYTE1_6);
		Char_WriteEEPROM_ex(TCF_in_Kg_6.byte2, 	EE_TCF_KGS_BYTE2_6);
		Char_WriteEEPROM_ex(TCF_in_Kg_6.byte3, 	EE_TCF_KGS_BYTE3_6);
		Char_WriteEEPROM_ex(TCF_in_Kg_6.byte4, 	EE_TCF_KGS_BYTE4_6);
		Sec_Per_Kg = ConvertSTRING_Int((unsigned char*)&KPFEED[18], 3);
	}
	
}
/********************************************************************************************
* Function Name:  Wrongpassword																*
* Description:    Display if the entered password is not valid								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void WrongPassword(void)
{
	XLCD_BLINKCURSOROFF();
	XLCDL1home();
	XLCDPutRamString(SPAC); 						// Blank Line.
	XLCDL2home();
	XLCDPutRamString(PWDE); 						// "Enter Passowrd"
	XLCDL4home();
	XLCDPutRamString(SPAC); 						// Blank Line
	XLCDL3home();
	XLCDPutRamString(SPAC);                         // Line to Enter Password
	TimerDelay_10ms(100);
}

/********************************************************************************************
* Function Name:  mismatchpassword															*
* Description:    Display if the entered passwords are different on password change menu	*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void mismatchpassword(void)
{
	XLCD_BLINKCURSOROFF();
	XLCDL1home();
	XLCDPutRamString(SPAC); 						// Blank Line.
	XLCDL2home();
	XLCDPutRamString(MISPW); 						// "Mismatch Passowrd"
	XLCDL4home();
	XLCDPutRamString(SPAC); 						// Blank Line
	XLCDL3home();
	XLCDPutRamString(SPAC);                         // Line to Enter Password
	TimerDelay_10ms(100);
}

/********************************************************************************************
* Function Name:  invalidtime																*
* Description:    Display if the entered values are not valid								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void invalidtime(void)
{
	XLCD_BLINKCURSOROFF();
	XLCDL1home();
	XLCDPutRamString(SPAC); 						// Blank Line.
	XLCDL2home();
	XLCDPutRamString(INVALID_T); 					// "Invalid Time"
	XLCDL4home();
	XLCDPutRamString(SPAC); 						// Blank Line
	XLCDL3home();
	XLCDPutRamString(SPAC);                         // Line to Enter Password
	TimerDelay_10ms(100);
}

/********************************************************************************************
* Function Name:  invaliddate																*
* Description:    Display if the entered date are not valid.								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void invaliddate(void)
{
	XLCD_BLINKCURSOROFF();
	XLCDL1home();
	XLCDPutRamString(SPAC); 							// Blank Line.
	XLCDL2home();
	XLCDPutRamString(INVALID_D); 						// "Invalid Time"
	XLCDL4home();
	XLCDPutRamString(SPAC); 							// Blank Line
	XLCDL3home();
	XLCDPutRamString(SPAC);                         	// Line to Enter Password
	TimerDelay_10ms(100);
}

/********************************************************************************************
* Function Name:  invalidkgs																*
* Description:    Display if the entered values are not valid								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void invalidkgs(void)
{
	XLCD_BLINKCURSOROFF();
	XLCDL1home();
	XLCDPutRamString(SPAC); 						// Blank Line.
	XLCDL2home();
	XLCDPutRamString(INVALID_KGS); 					// "Invalid Time"
	XLCDL4home();
	XLCDPutRamString(SPAC); 						// Blank Line
	XLCDL3home();
	XLCDPutRamString(SPAC);                         // Line to Enter Password
	TimerDelay_10ms(100);
}

/********************************************************************************************
* Function Name:  invalidkgs_feed															*
* Description:    Display if the entered values are not valid								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void invalidkgs_feed(void)
{
	XLCD_BLINKCURSOROFF();
	XLCDL1home();
	XLCDPutRamString(SPAC); 						// Blank Line.
	XLCDL2home();
	XLCDPutRamString(INVALID_KGS1); 					// "Invalid Feed"
	XLCDL4home();
	XLCDPutRamString(SPAC); 						// Blank Line
	XLCDL3home();
	XLCDPutRamString(SPAC);                         // Line to Enter Password
	TimerDelay_10ms(100);
}
/********************************************************************************************
* Function Name:  Errorscreen																*
* Description:    Display the System Faults by the error screen								*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void ErrorScreen(void)
{
	XLCD_BLINKCURSOROFF();
	XLCDL2home();
	XLCDPutRamString(Ero); 								// Error Display
	XLCDL1home();
	XLCDPutRamString(SPAC); 							// Line 1 Blank Line
	XLCDL3home();
	XLCDPutRamString(SPAC); 							// Line 3 Blank Line
	XLCDL4home();
	XLCDPutRamString(SPAC);                         	// Line 4 Blank Line
}
