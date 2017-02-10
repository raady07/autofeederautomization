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
 
#ifndef RTC_H
	#define RTC_H
	#define RTC_ID	0xD0
	void Read_RTC(void);
	void W_RTC_Time(unsigned char hrs, unsigned char mins, unsigned char secs);
	void W_RTC_Date(unsigned char dd, unsigned char mm, unsigned char yy, unsigned char dow);
	unsigned char BCD2Bin(unsigned char data);
	unsigned char D2BCD(unsigned char data);
#endif
