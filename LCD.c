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
#include"24lc256.h"
#include"main.h"
#include"XLCD2.h"
#include"Ex_EEPROM.h"
#include"uart.h"
#include"KP_InputFunctions.h"
#include"flags.h"
#include"timer.h"
#include"lcd.h"
#include"StringManipulated.h"

#define     ShowScreen(x,y,z) Xaxis = x, Yaxis = y, Zaxis = z

const unsigned char  keypad[]   = {0x41,0x42,0x44,0x48,0x81,0x82,0x84,0x88,0x11,0x12,0x14,0x18,0x21,0x22,0x24,0x28};
const unsigned char character[] = {'2' ,'5' ,'8' ,'0' ,'3' ,'6' ,'9' ,'C' ,'M' ,'U' ,'D' ,'E' ,'1' ,'4' ,'7' ,'S' };
KEYs KP_Port;
unsigned char Yaxis = 0, Xaxis = 0, Zaxis = 0, KP_Data = 0;
unsigned char flag_masterscreen = 0, flag_shiftsscreen = 0, Manual_Key_Hold_Count = 0;
unsigned char Down_Arrow_Count = 0;
unsigned int TimeOut = 0;
char Line1Buff[20];
static unsigned char ShiftScreen = 0, ShiftScreen1 = 0;
/* Prototype Functions */
static void KeyPadScan( void );

extern unsigned char Compile_Date[25];
extern unsigned char system_mode, auto_dis_lcd;			// system in auto or manual
extern unsigned char total_feed_in_kgs;		// total feeding in kgs
extern unsigned char left_feed_in_kgs;			// at present left feed in kgs
extern unsigned char kgs_per_cycle;			// kgs per cycle
extern unsigned char cycle_cycle_mins, ar_mode;		// cycle to cycle interval in mins
extern unsigned char No_of_Shifts,Present_Auto_Plus_shift_count, En_Display_Lcd, Auto_Plus_load;
extern LCDflags Lcd;
extern Time SystemTime, StartTime;
extern Time ONTime, OFFTime, AUTO_ONTime[7], AUTO_OFFTime[7];
extern Time Auto_Next_ONTime, Auto_Next_OFFTime, Prev_OFF_Time;
extern unsigned char KPKGSFEED[12];
void LCDTask(void)
{
	MemoryClear((unsigned char*)Line1Buff);
	LCD_BACKLIGHT = 1;						//	Back Light ON;
	KeyPadScan();
	if(flag_masterscreen == 8)
	{
		ShowScreen(3,0,0);
		flag_masterscreen = 0;
	}
	if(Down_Arrow_Count == 16)				//	Clear Feed left in Kgs Manually
	{
		Down_Arrow_Count = 0;
		left_feed_in_kgs = 0;
		Char_WriteEEPROM_ex(left_feed_in_kgs, EE_LEFT_FEED);
	}
	if(flag_shiftsscreen == 8)
	{
		ShowScreen(5,0,0);
		flag_shiftsscreen = 0;
	}
	switch(Xaxis)
	{
		case 0:	MainScreen();
				break;
		case 1:	if(Yaxis == 0)
				{
					PasswordEnter(0);
					break;
				}
				else if(Yaxis!=0)
				{
					MenuScreens();
					break;
				}
		case 2:	CustomMenuScreens();
				break;
		case 3:	if(Yaxis == 0)
				{
					PasswordEnter(1);
					break;
				}
				else
				{
	 				MasterScreens();
					break;
				}
		case 4: CustomMasterScreens();
				break;
		case 5: displayautoplustime();
				break;
		case 6: displayautoplustime1();
				break;
		default:ErrorScreen();
	}
}

void INIT_LED(void)
{
	DDR_LED_MRUN	= Output;			//	LED ON Board	
 	DDR_LED_RUN		= Output;			//	Toggle MCLR RUN Status LED on LED Board	
 	DDR_LED_AUTO	= Output;			//	Toggle Auto Status LED on LED Board
 	DDR_LED_MANL	= Output;			//	Toggle Manual Status LED on LED Board
 	DDR_LED_AUTOP	= Output;			//	Toggle Error Status LED on LED Board	
}

void INIT_KEYPAD(void)
{
	ADCON1bits.ADON		=	0;			//	Disable ADC		
	ADPCFG				=	0xFFFF;		//	CONFIGURE PORTB as DIGITAL IO
	DCICON1bits.DCIEN	=	0;			//	DCI Diabled
	DDR_KEY_MANL		=	Input;		//  manual key

	DDR_KP_R1 = Output;
	DDR_KP_R2 = Output;
	DDR_KP_R3 = Output;
	DDR_KP_R4 = Output;
	DDR_KP_C1 = Input;
	DDR_KP_C2 = Input;
	DDR_KP_C3 =	Input;
	DDR_KP_C4 = Input;
}	

static void KeyPadScan( void )
{
    unsigned char ii =0, i =0, cKeyPad =0, KP_V = 0;
	Write_KeyPad(0xF0);                //init/re-init port 
	LCD_BACKLIGHT = 1;					//	Back Light ON
	KP_V = Read_KeyPad();
	if(KP_V!= 0xF0)
	{
		for(i=0;i<4;i++)
		{
			Write_KeyPad(0x80>>i);
			for(ii=0;ii<16;ii++)
			{
				KP_V = Read_KeyPad();
				if(KP_V == keypad[ii])
				{ 			                           
					KP_Data = cKeyPad = character[ii];
					printf("keypad:%c\n",KP_Data);
					i=4,TimeOut =0;
					break;
				}
			}
		}
	}
	/**********************Operations on Manual Key Button ***********************************************/
	if(cKeyPad == 'E' && KEY_MANL )
	{
		if(En_Display_Lcd == 1)		flag_masterscreen++;
	}
	if(cKeyPad != 'E' && KEY_MANL && Xaxis == 0 && system_mode == AUTOPLUS)
	{
		if(En_Display_Lcd == 1)		flag_shiftsscreen++;
	}
	/*********************** Operation on Manual Key Button - End ****************************************/

	if(cKeyPad == 'M' && Xaxis == 0)	Xaxis = 1;
	if(cKeyPad == 'D' && Xaxis == 0)
	{
		if(En_Display_Lcd == 1)		Down_Arrow_Count++;
	}		
	
	else if(cKeyPad == 'U' && Xaxis == 1)
	{
		if(Yaxis > 0 && Yaxis < 7)		Yaxis++;
	}	
	else if(cKeyPad == 'D' && Xaxis == 1)
	{
		if(Yaxis > 1 && Yaxis < 8)		Yaxis--;
	}

	else if(cKeyPad == 'U' && Xaxis == 3)
	{
		if(Yaxis > 0 && Yaxis < 8)		Yaxis++;
	}
	else if(cKeyPad == 'D' && Xaxis == 3)
	{
		if(Yaxis > 1 && Yaxis < 9)		Yaxis--;
	}
	if( Yaxis!=0)	TimeOut++;
	if(TimeOut >= 540)
	{
		Xaxis = 0, Yaxis = 0, Zaxis = 0, TimeOut =0, LCD_BACKLIGHT = 1, auto_dis_lcd = 0,
		Lcd.Z0	= 0, Lcd.Z1	 = 0, Lcd.Z2  = 0, Lcd.Z3  = 0,
		Lcd.Z4  = 0, Lcd.Z5	 = 0, Lcd.Z6  = 0, Lcd.Z7  = 0,
		Lcd.Z8  = 0, Lcd.Z9  = 0, Lcd.Z10 = 0, Lcd.Z11 = 0,
		Lcd.Z12 = 0, Lcd.Z13 = 0, Lcd.Z14 = 0, Lcd.Z15 = 0,
		Lcd.Z16 = 0, Lcd.Z17 = 0, Lcd.Z18 = 0, Lcd.Z19 = 0,
		Lcd.Z20 = 0, Lcd.Z21 = 0, Lcd.Z22 = 0, Lcd.Z23 = 0;
	}
	if(KP_Data == 'M')		KP_Data = 0;
	cKeyPad = 0;
}

void Write_KeyPad(unsigned char chr)
{
	KP_Port._KEYPORT = chr;
	KP_C1	= KP_Port.C0; 
	KP_C2	= KP_Port.C1;  
	KP_C3	= KP_Port.C2;
	KP_C4	= KP_Port.C3;
	KP_R1	= KP_Port.R0;
	KP_R2	= KP_Port.R1;
	KP_R3	= KP_Port.R2;
	KP_R4	= KP_Port.R3;
}

unsigned char Read_KeyPad(void)
{
	KP_Port.C0 = R_KP_C0;
	KP_Port.C1 = R_KP_C1;
	KP_Port.C2 = R_KP_C2;
	KP_Port.C3 = R_KP_C3;
	KP_Port.R0 = R_KP_R0;
	KP_Port.R1 = R_KP_R1;
	KP_Port.R2 = R_KP_R2;
	KP_Port.R3 = R_KP_R3;
	return KP_Port._KEYPORT;
}

/********************************************************************************************
* Function Name:  LCD_StartUP_Screen														*
* Description:    Pluggedin power displays, startup flash screen. Stays for few seconds		*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void LCD_StartUP_Screen(void)
{
	XLCD_BLINKCURSOROFF();
	MemoryClear((unsigned char*)Line1Buff);
	TimerDelay_10ms(100);
	XLCDL1home();
	sprintf(Line1Buff,"ENERSYS ENERGY      ");
	XLCDPutRamString(Line1Buff);
	XLCDL2home();
	sprintf(Line1Buff, "           SOLUTIONS");
	XLCDPutRamString(Line1Buff);
	XLCDL3home();
	sprintf(Line1Buff, " AUTO FEEDER  V 4.0 ");
	XLCDPutRamString(Line1Buff);
	XLCDL4home();
	sprintf(Line1Buff," DATE: %s ",Compile_Date);
	XLCDPutRamString(Line1Buff);
	TimerDelay_10ms(300);
	ShowScreen(0,0,0);
	MemoryClear((unsigned char*)Line1Buff);
}

/********************************************************************************************
* Function Name:  MainScreen																*
* Description:    if Xaxis = 0 turns up this screen, displays current mode, Motor run info	*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void MainScreen(void )
{
	XLCDL1home();
	if(system_mode == AUTOPLUS)
		sprintf(Line1Buff, " AUTO+ %d/%d %02d:%02d:%02d ", Present_Auto_Plus_shift_count, No_of_Shifts, SystemTime.Hours, SystemTime.Minutes, SystemTime.Seconds);
	else if(system_mode == AUTO)
		sprintf(Line1Buff, " AUTO      %02d:%02d:%02d ", SystemTime.Hours, SystemTime.Minutes, SystemTime.Seconds);
	else if(system_mode == MANUAL)
		sprintf(Line1Buff, " MANUAL    %02d:%02d:%02d ", SystemTime.Hours, SystemTime.Minutes, SystemTime.Seconds);
	XLCDPutRamString(Line1Buff);
	MemoryClear((unsigned char*)Line1Buff);
	XLCDL2home();
	if(ar_mode == 1)
		sprintf(Line1Buff, " FEED KGs: R/%03d/%03d  ", left_feed_in_kgs, total_feed_in_kgs);		
	else 
		sprintf(Line1Buff, " FEED KGs: %03d/%03d    ", left_feed_in_kgs, total_feed_in_kgs);		
	XLCDPutRamString(Line1Buff);
	MemoryClear((unsigned char*)Line1Buff);
	if( ShiftScreen <= 10)
	{	
		XLCDL3home();
		sprintf(Line1Buff, "  KGS/CYC: %02d  KGs  ", kgs_per_cycle);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);

		XLCDL4home();
		if(system_mode ==AUTOPLUS)
		{
			if(Auto_Plus_load == 1)
				sprintf(Line1Buff, " ON  TIME: %02d:%02d:%02d ", ONTime.Hours, ONTime.Minutes, ONTime.Seconds );
			else if(Auto_Plus_load == 0)
				sprintf(Line1Buff, " PREV OFF: %02d:%02d:%02d ", Prev_OFF_Time.Hours, Prev_OFF_Time.Minutes, Prev_OFF_Time.Seconds);
		}
		else
			sprintf(Line1Buff, " ON  TIME: %02d:%02d:%02d ", ONTime.Hours, ONTime.Minutes, ONTime.Seconds);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	} 
	else if((ShiftScreen >10) && (ShiftScreen <=20))
	{
		XLCDL3home();
		sprintf(Line1Buff, "  CYC-CYC: %02d Mins  ", cycle_cycle_mins);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);

		XLCDL4home();
		if(system_mode == AUTOPLUS)
		{
			if(Auto_Plus_load == 1)
				sprintf(Line1Buff, " OFF TIME: %02d:%02d:%02d ", OFFTime.Hours, OFFTime.Minutes, OFFTime.Seconds);
			else if(Auto_Plus_load == 0)
				sprintf(Line1Buff, " NEXT ON : %02d:%02d:%02d ", Auto_Next_ONTime.Hours, Auto_Next_ONTime.Minutes, Auto_Next_ONTime.Seconds);
		}
		else
			sprintf(Line1Buff, " OFF TIME: %02d:%02d:%02d ", OFFTime.Hours, OFFTime.Minutes, OFFTime.Seconds);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	ShiftScreen++;
	if( ShiftScreen > 20)		ShiftScreen = 0;
	XLCD_BLINKCURSOROFF();
}

void displayautoplustime(void)
{
	unsigned char dis	= 0;
	if( ShiftScreen1 <= 20)
	{
		XLCDL1home();
		sprintf(Line1Buff, "      Shift  1      ");
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL2home();
		sprintf(Line1Buff," ON  TIME: %02d:%02d:%02d ", AUTO_ONTime[0].Hours, AUTO_ONTime[0].Minutes, AUTO_ONTime[0].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff," OFF TIME: %02d:%02d:%02d ", AUTO_OFFTime[0].Hours, AUTO_OFFTime[0].Minutes, AUTO_OFFTime[0].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		dis = ConvertSTRING_Int(&KPKGSFEED[0],2);
		XLCDL4home();
		sprintf(Line1Buff," KGs/Shft: %02d KGs   ",dis);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if( ShiftScreen1 > 20 && ShiftScreen1 <= 40)
	{
		XLCDL1home();
		sprintf(Line1Buff, "      Shift  2      ");
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL2home();
		sprintf(Line1Buff," ON  TIME: %02d:%02d:%02d ", AUTO_ONTime[1].Hours, AUTO_ONTime[1].Minutes, AUTO_ONTime[1].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff," OFF TIME: %02d:%02d:%02d ", AUTO_OFFTime[1].Hours, AUTO_OFFTime[1].Minutes, AUTO_OFFTime[1].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		dis = ConvertSTRING_Int(&KPKGSFEED[2],2);
		XLCDL4home();
		sprintf(Line1Buff," KGs/Shft: %02d KGs   ",dis);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if( ShiftScreen1 > 40 && ShiftScreen1 <= 60)
	{
		XLCDL1home();
		sprintf(Line1Buff, "      Shift  3      ");
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL2home();
		sprintf(Line1Buff," ON  TIME: %02d:%02d:%02d ", AUTO_ONTime[2].Hours, AUTO_ONTime[2].Minutes, AUTO_ONTime[2].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff," OFF TIME: %02d:%02d:%02d ", AUTO_OFFTime[2].Hours, AUTO_OFFTime[2].Minutes, AUTO_OFFTime[2].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		dis = ConvertSTRING_Int(&KPKGSFEED[4],2);
		XLCDL4home();
		sprintf(Line1Buff," KGs/Shft: %02d KGs   ",dis);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if( ShiftScreen1 > 60 && ShiftScreen1 <= 80)
	{
		XLCDL1home();
		sprintf(Line1Buff, "      Shift  4      ");
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL2home();
		sprintf(Line1Buff," ON  TIME: %02d:%02d:%02d ", AUTO_ONTime[3].Hours, AUTO_ONTime[3].Minutes, AUTO_ONTime[3].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff," OFF TIME: %02d:%02d:%02d ", AUTO_OFFTime[3].Hours, AUTO_OFFTime[3].Minutes, AUTO_OFFTime[3].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		dis = ConvertSTRING_Int(&KPKGSFEED[6],2);
		XLCDL4home();
		sprintf(Line1Buff," KGs/Shft: %02d KGs   ",dis);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if( ShiftScreen1 > 80 && ShiftScreen1 <= 100)
	{
		XLCDL1home();
		sprintf(Line1Buff, "      Shift  5      ");
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL2home();
		sprintf(Line1Buff," ON  TIME: %02d:%02d:%02d ", AUTO_ONTime[4].Hours, AUTO_ONTime[4].Minutes, AUTO_ONTime[4].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff," OFF TIME: %02d:%02d:%02d ", AUTO_OFFTime[4].Hours, AUTO_OFFTime[4].Minutes, AUTO_OFFTime[4].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		dis = ConvertSTRING_Int(&KPKGSFEED[8],2);
		XLCDL4home();
		sprintf(Line1Buff," KGs/Shft: %02d KGs   ",dis);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	else if( ShiftScreen1 > 100 && ShiftScreen1 <= 120)
	{
		XLCDL1home();
		sprintf(Line1Buff, "      Shift  6      ");
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL2home();
		sprintf(Line1Buff," ON  TIME: %02d:%02d:%02d ", AUTO_ONTime[5].Hours, AUTO_ONTime[5].Minutes, AUTO_ONTime[5].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff," OFF TIME: %02d:%02d:%02d ", AUTO_OFFTime[5].Hours, AUTO_OFFTime[5].Minutes, AUTO_OFFTime[5].Seconds );		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		dis = ConvertSTRING_Int(&KPKGSFEED[10],2);
		XLCDL4home();
		sprintf(Line1Buff," KGs/Shft: %02d KGs   ",dis);
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	ShiftScreen1++;
	if(No_of_Shifts == 1)
	{
		if( ShiftScreen1 > 20)	ShiftScreen1 = 0,ShowScreen(6,0,0);
	}
	else if(No_of_Shifts == 2)
	{
		if( ShiftScreen1 > 40)	ShiftScreen1 = 0,ShowScreen(6,0,0);
	}
	else if(No_of_Shifts == 3)
	{
		if( ShiftScreen1 > 60)	ShiftScreen1 = 0,ShowScreen(6,0,0);
	}
	else if(No_of_Shifts == 4)
	{
		if( ShiftScreen1 > 80)	ShiftScreen1 = 0,ShowScreen(6,0,0);
	}
	else if(No_of_Shifts == 5)
	{
		if( ShiftScreen1 > 100)	ShiftScreen1 = 0,ShowScreen(6,0,0);
	}
	else if(No_of_Shifts == 6)
	{
		if( ShiftScreen1 > 120)	ShiftScreen1 = 0,ShowScreen(6,0,0);
	}
}

void displayautoplustime1(void)
{
	unsigned char ijk = 0;
	unsigned int  wgt	= 0;
	ShiftScreen1++;
	if( ShiftScreen1 > 0 && ShiftScreen1 <= 20)
	{
		XLCDL1home();
		sprintf(Line1Buff,"                    ");		
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		XLCDL2home();
		sprintf(Line1Buff,"   SHIFTS/DAY : %02d  ", No_of_Shifts);		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
		for(ijk = 0; ijk < ((No_of_Shifts*2)-1); ijk=ijk+2)
		{
			wgt += ConvertSTRING_Int(&KPKGSFEED[ijk],2);
		}
		XLCDL3home();
		sprintf(Line1Buff,"      KGs/DAY : %03d ", wgt);		
		XLCDPutRamString(Line1Buff);
		MemoryClear((unsigned char*)Line1Buff);
	}
	if( ShiftScreen1 > 20)		ShiftScreen1 = 0, ShowScreen(0,0,0);
}
