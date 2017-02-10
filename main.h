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

#include "timer.h"
#define	DFLT_SYS_MODE		1		// 2-auto+, 1-auto, 0-manual
#define DFLT_RUN_SHIFT		6		// for auto mode , 0-5 for auto+ mode
// Default ON Time(1-6 for autoplus, 7 for auto, 8 for Manual)
#define DFLT_ONTIME_HH_1	6
#define DFLT_ONTIME_HH_2	9
#define DFLT_ONTIME_HH_3	12
#define DFLT_ONTIME_HH_4	15
#define DFLT_ONTIME_HH_5	18
#define DFLT_ONTIME_HH_6	21
#define DFLT_ONTIME_HH_7	6
//default times
#define DFLT_ONTIME_HH		6
#define	DFLT_ONTIME_MM		0
#define	DFLT_ONTIME_SS		0
#define	DFLT_OFFTIME_HH		0
#define	DFLT_OFFTIME_MM		0
#define	DFLT_OFFTIME_SS		0

#define	DFLT_TOTAL_FFED_KGS	0		// default total feed 0 kgs 
#define	DFLT_LEFT_FEED_KGS	0		// deafault left feed 0 kgs.
#define DFLT_NOOFSHIFTS		5		// default shifts 
#define	DFLT_KGS_PER_CYCLE	1		// default kgs per cycle 01
#define	DFLT_CYCLE_INTERVAL	5		// default cyc 2 cyc interval 5 mins 
#define DFLT_AR_MODE		0		// default auto reload mode - 0 (disable)
#define DFLT_FEEDFLOW		"030045060075090105" // deault feed flow for tpye 3,3P,4,4S,5 and 6.
#define DFLT_FEEDTYPE		1		// default feed type "3p"
#define	SEC2KGS_3			30
#define	SEC2KGS_3p			45
#define	SEC2KGS_4s			60
#define	SEC2KGS_4			75
#define	SEC2KGS_5			90
#define	SEC2KGS_6			105
#define DFLT_KGS_PER_SHIFT	"101010101010"	// default kgs per shift in auto+mode 0 -5 10 kgs each

#define	MAX_KGS				250

#define AUTOPLUS			2
#define	AUTO				1
#define	MANUAL				0
#define TRUE                1
#define FALSE               0
#define ENABLE				1
#define DISABLE				0
#define INPUT 				1
#define OUTPUT				0
#define HIGH				1
#define LOW					0
#define	ON					1
#define OFF					0

#define True				1
#define False				0
#define	Enable				1
#define	Disable				0
#define	Output				0
#define	Input				1
#define	High				1
#define	Low					0

/*******************************************************************************************************/

extern unsigned char ExstingPW[5];
extern unsigned char system_mode;			// system in auto or manual
extern unsigned char manual_mode_on;

/*
|-------------------------------------------|
|	byte4    | byte3 	| byte2   |	byte1	|
|	MSB		 |		    |		  | LSB		|
|-------------------------------------------|
*/

typedef	union TCF_{
	unsigned long int Value;				// Total_Crop_Feed_in_Kg
	struct{
		unsigned char byte1;		// lsb
		unsigned char byte2;
		unsigned char byte3;
		unsigned char byte4;		// msb		
	};
}TCF;

void INIT_RELAY(void);
void Print_RTC(void);
void Sync_with_RTCTime(void);
void makingallzero(void);
void CheckForFirstTime(void);
void CheckONTime(void);
void Control_Feeding(void);
Time Compute_Off_Time(Time COMPUTETime);
unsigned char ONorOFF(Time on_time_, Time off_time_);
void Show_Parameters(void);
void InitAllPheriperals(void);
void Switch_OFF_FEEDER(void);
void Switch_ON_Feeder(void);
void decidemodeONOFFtime(void);
void blinkleds(void);
void noblinkleds(void);
void onallleds(void);
void Decide_AutoPlus_Shift_Position(void);
void Auto_Plus_Load_Control(void);
void Convert_Sec_To_Time(unsigned long int sec, Time *temp);
void resetmonitoring(void);
void printresetvalues(void);
