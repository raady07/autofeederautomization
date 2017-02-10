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

#include "I2C.h"
#include "RTC.h"

unsigned char  Sec = 0, Min = 0, Hour = 0, DOW = 0, Date = 0, Month = 0, Year = 0;

/********************************************************************************************
* Function Name:  Read_RTC																	*
* Description:    Read Seconds, Minutes, Hours, Day, Date, Month, Year from RTC				*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void Read_RTC(void)
{
	Nop();
	Nop();
	WaitI2C(3);
	StartI2C();
	IdleI2C();
	MasterWriteI2C(RTC_ID);       						// Write Control byte
	IdleI2C();
	MasterWriteI2C(0x00);								// write LSB Address
   	IdleI2C();
	RestartI2C();
	IdleI2C();
	MasterWriteI2C(RTC_ID | 1);       					// Read Control Byte
	IdleI2C();
    Sec		=	BCD2Bin( MasterReadI2C() );				//	Read Seconds with  BCD format			
	AckI2C();											//	Wait for Acknowledgment
	IdleI2C();	
	Min		=	BCD2Bin( MasterReadI2C() );				//	Read Minutes with  BCD format	
	IdleI2C();
	AckI2C();											//	Wait for Acknowledgment
	IdleI2C();
	Hour	=	BCD2Bin( MasterReadI2C() );				//	Read Hour with  BCD format	
	IdleI2C();
	AckI2C();											//	Wait for Acknowledgment
	IdleI2C();
    DOW 	=	BCD2Bin( MasterReadI2C() );				//	Read Day of the week with  BCD format	
	IdleI2C();
	AckI2C();											//	Wait for Acknowledgment
	IdleI2C();
	Date	=	BCD2Bin( MasterReadI2C() );				//	Read Date with  BCD format	
	IdleI2C();
	AckI2C();											//	Wait for Acknowledgment
	IdleI2C();
	Month	=	BCD2Bin( MasterReadI2C() );				//	Read Month with  BCD format	
	IdleI2C();										
	AckI2C();											//	Wait for Acknowledgment
	IdleI2C();
	Year	=	BCD2Bin( MasterReadI2C() );				//	Read Year(YY) with  BCD format	
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
	IdleI2C();
}


void W_RTC_Time(unsigned char hrs, unsigned char mins, unsigned char secs)
{
	StartI2C();
	IdleI2C();
	MasterWriteI2C(RTC_ID);                      			// Write Control byte along with Device ID
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C( 0x00 );								// write LSB Address	
    IdleI2C();                      
	MasterWriteI2C( D2BCD(secs) );							// write sec Value in D2B format with CH bit=0 at addrs loc 0x00 
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C( D2BCD(mins));							// write min Value in D2B format at addrs loc 0x01   
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(D2BCD(hrs));            				// write hour Value in D2B format at addrs loc 0x02 (Need to consider 12/24 Hr mode)
	IdleI2C();
	StopI2C();
    IdleI2C();
} 		

/********************************************************************************************
* Function Name:  W_RTC_Date																*
* Description:    Edit Day, Date, Month, Year on RTC										*
* Parameters:     unsigned char dd, unsigned char mm, unsigned char yy, unsigned char dow	*
* Return Value:   void																		*
********************************************************************************************/
void W_RTC_Date(unsigned char dd, unsigned char mm, unsigned char yy, unsigned char dow)
{
	StartI2C();
	IdleI2C();
	MasterWriteI2C(RTC_ID);                      		// Write Control byte along with Device ID
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C( 0x03 );								// write LSB Address w.r.t DOW	
    IdleI2C();                      
	MasterWriteI2C( D2BCD(dow) );						// write day 0f week Value in D2B format at addrs loc=03
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C( D2BCD(dd));							// write Date Value in D2B format at addrs loc=04  
	IdleI2C();
    while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(D2BCD(mm));            				// write Month Value in D2B format at addrs loc=05
	IdleI2C();
	 while(I2CSTATbits.ACKSTAT);
	MasterWriteI2C(D2BCD(yy));            				// write Year Value in D2B format at addrs loc=06
	IdleI2C();
	StopI2C();
    IdleI2C();
}

/********************************************************************************************
* Function Name:  BCD2Bin																	*
* Description:    Convert data read from RTC to Binary										*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
unsigned char    BCD2Bin(unsigned char data)
{
	return ((((data & 0xF0 )>>4) * 10) + (data&0xF));	  
} 

/********************************************************************************************
* Function Name:  D2BCD																		*
* Description:    convert decimal to binary from RTC										*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
unsigned char	D2BCD(unsigned char data)
{
	return (((data/10)<< 4) | (data%10));
}		
