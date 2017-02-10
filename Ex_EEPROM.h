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
 
#ifndef	_eeprom_h
	#define _eeprom_h

	#define	EE_Compile_Len		0x0001
	#define	EE_Compile_Date		0x0002  // 30 byte array
	#define LCD_KEY			  	0x0021	// - 0x000b	// 4 bytes
	#define	EE_SYS_MODE			0x0025	// auto+ / auto / manual mode address
	/* For AutoPlus Mode */
	#define EE_Shifts			0x0026	// No of Shifts
	#define EE_ONTIME_HH_1		0x0027
	#define EE_ONTIME_MM_1		0x0028
	#define EE_ONTIME_HH_2		0x0029
	#define EE_ONTIME_MM_2		0x002A
	#define EE_ONTIME_HH_3		0x002B
	#define EE_ONTIME_MM_3		0x002C
	#define EE_ONTIME_HH_4		0x002D
	#define EE_ONTIME_MM_4		0x002E
	#define EE_ONTIME_HH_5		0x002F
	#define EE_ONTIME_MM_5		0x0030
	#define EE_ONTIME_HH_6		0x0031
	#define EE_ONTIME_MM_6		0x0032
	#define EE_ONTIME_HH_7		0x0033
	#define EE_ONTIME_MM_7		0x0034
	// on and off times used on reset
	#define EE_ONTIME_HH		0x0035
	#define	EE_ONTIME_MM		0x0036
	#define	EE_ONTIME_SS		0x0037
	#define	EE_OFFTIME_HH		0x0038
	#define	EE_OFFTIME_MM		0x0039
	#define	EE_OFFTIME_SS		0x003A
	
	#define	EE_KGS_PER_CYCLE	0x003B
	#define	EE_CYCLE_INTERVAL	0x003C
	#define EE_LEFT_FEED		0x003D
	#define	EE_TOTAL_FFED_KGS	0x003E
	#define EE_AUTOREFILL_FEED	0x003F	// Auto Refill Feed
	#define EE_FEEDTYPE			0x0040	// Feed Type
	#define EE_KGS_SHIFT		0x0041	// 12 bytes 
	// total crop feed details 								
	#define	EE_TCF_KGS_BYTE1	0x004D
	#define	EE_TCF_KGS_BYTE2	0x004E
	#define	EE_TCF_KGS_BYTE3	0x004F
	#define	EE_TCF_KGS_BYTE4	0x0050
	#define EE_TCF_KGS_BYTE1_3	0x0051
	#define EE_TCF_KGS_BYTE2_3	0x0052
	#define EE_TCF_KGS_BYTE3_3	0x0053
	#define EE_TCF_KGS_BYTE4_3	0x0054
	#define EE_TCF_KGS_BYTE1_3p	0x0055
	#define EE_TCF_KGS_BYTE2_3p	0x0056
	#define EE_TCF_KGS_BYTE3_3p	0x0057
	#define EE_TCF_KGS_BYTE4_3p	0x0058
	#define EE_TCF_KGS_BYTE1_4	0x0059
	#define EE_TCF_KGS_BYTE2_4	0x005A
	#define EE_TCF_KGS_BYTE3_4	0x005B
	#define EE_TCF_KGS_BYTE4_4	0x005C
	#define EE_TCF_KGS_BYTE1_4s	0x005D
	#define EE_TCF_KGS_BYTE2_4s	0x005E
	#define EE_TCF_KGS_BYTE3_4s	0x005F
	#define EE_TCF_KGS_BYTE4_4s	0x0060
	#define EE_TCF_KGS_BYTE1_5	0x0061
	#define EE_TCF_KGS_BYTE2_5	0x0062
	#define EE_TCF_KGS_BYTE3_5	0x0063
	#define EE_TCF_KGS_BYTE4_5	0x0064
	#define EE_TCF_KGS_BYTE1_6	0x0065
	#define EE_TCF_KGS_BYTE2_6	0x0066
	#define EE_TCF_KGS_BYTE3_6	0x0067
	#define EE_TCF_KGS_BYTE4_6	0x0068
	#define	EE_SEC_TO_KGS_T		0x0069		// 18 bytes

	#define EE_AR_MODE			0x0080		//	Auto Reload
	#define EE_START_CYCLES		0x0081
//	#define	EE_FEED_ERROR		0x0082
	#define	EE_Count_ON_Cycles	0x0083		// 2 bytes
 	#define	EE_Count_ON_Cycles1	0x0084		// 2 bytes
	#define	EE_FEEDER_ON		0x0085
	#define EE_MAN_ON_SET		0x0086
	// Reset condition storing values.
	#define TRAP_val			0x0087
	#define IOPUWR_val			0x0088
	#define EXTR_val			0x0089
	#define SWDT_val			0x008A
	#define WDTO_val			0x008B
	#define BOR_val				0x008C
	#define POR_val				0x008D	

#endif
