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
#include"I2C.h"

/********************************************************************************************
* Function Name:  i2cinit																	*
* Description:    This routine initialize i2c protocol 										*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void INIT_I2C(void)
{
	DDR_EEPROM_WP_EN = 0;		// Write protect EEPROM.
	DDR_RTC_EN		 = 0;		// RTC_INIT
	DDR_SCL_EN		 = 0;		// SCL
	DDR_SDA_EN		 = 0;		// SDA
	CloseI2C();
	OpenI2C(__I2C_ON,__I2C_BRG);		
}	

/********************************************************************************************
* Function Name:  OpenI2C																	*
* Description:    This function configures the I2C module for enable bit,disable slew rate  *
*				  SM bus input levels, SCL release,Intelligent Peripheral Management 		*
*				  Interface enable, sleep mode, general call enable,acknowledge data bit, 	*
*                 acknowledge sequence enable, receive enable, stop condition enable, 		*
*				  restart condition enable and start condition enable. The Baud rate value	*
*				  is also configured.														*
* Parameters:     unsigned int : config1													*
*                 unsigned int : config2													*
* Return Value:   void																		*
********************************************************************************************/

void OpenI2C(unsigned int config1, unsigned int config2)
{
    I2CBRG = config2;
    I2CCON = config1;
}

/********************************************************************************************
* Function Name:  StartI2C																	*
* Description:    This routine generates Start condition during master mode.				*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/

void StartI2C(void)
{
     I2CCONbits.SEN = 1;		// Initiate Start on SDA and SCL pins
}

/********************************************************************************************
* Function Name:  RestartI2C																*
* Description:    This routine generates restart condition during master mode.				*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/

void RestartI2C(void)
{ 
    I2CCONbits.RSEN = 1;		// Initiate restart on SDA and SCL pins
}

/********************************************************************************************
* Function Name:  RestartI2C																*
* Description:    This routine generates stop condition during master mode.					*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/

void StopI2C(void)
{
     I2CCONbits.PEN = 1;		// Initiate Stop on SDA and SCL pins
}


/********************************************************************************************
* Function Name:  RestartI2C																*
* Description:    This routine generates Idle condition during master mode.					*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/

void IdleI2C(void)
{
    /* Wait until I2C Bus is Inactive */
    while(I2CCONbits.SEN || I2CCONbits.PEN || I2CCONbits.RCEN || I2CCONbits.ACKEN || I2CSTATbits.TRSTAT);	
}

/********************************************************************************************
* Function Name:  MasterReadI2C																*
* Description:    This routine reads a single byte from the I2C Bus. To enable master 		*
*				  receive,RCEN bit is set.The RCEN bit is checked until it is cleared.		*
*				  When cleared, the receive register is full and it's contents are returned.*
* Parameters:     void																		*
* Return Value:   unsigned char																*
********************************************************************************************/

unsigned char MasterReadI2C(void)
{
    I2CCONbits.RCEN = 1;
    while(I2CCONbits.RCEN);
    I2CSTATbits.I2COV = 0;
    return(I2CRCV);
}

/********************************************************************************************
* Function Name:  MasterWriteI2C															*
* Description:    This routine is used to write a byte to the I2C bus. The input parameter 	*
* 				  data_out is written to the I2CTRN register. If IWCOL bit is set,write 	*
*				  collision has occured and -1 is returned, else 0 is returned.				*
* Parameters:     unsigned char : data_out													*
* Return Value:   unsigned int																*
********************************************************************************************/

char MasterWriteI2C(unsigned char data_out)
{
    I2CTRN = data_out;

    if(I2CSTATbits.IWCOL)        // If write collision occurs,return -1.
        return -1;
    else
    {
		/* wait until write cycle is complete */
        while(I2CSTATbits.TRSTAT);  	
        	IdleI2C();          // ensure module is idle
		/* test for ACK condition received*/
        if (I2CSTATbits.ACKSTAT)	return (-2);
	    else 						return (0);              
    }
}

/********************************************************************************************
* Function Name:  NotAckI2C																	*
* Description:    This routine generates not acknowledge condition during master receive.	*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/

void NotAckI2C(void)
{
    I2CCONbits.ACKDT = 1;
    I2CCONbits.ACKEN = 1;
}

/********************************************************************************************
* Function Name:  AckI2C																	*
* Description:    This routine generates acknowledge condition during master receive.		*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/
void AckI2C(void)
{
	I2CCONbits.ACKDT = 0;
	I2CCONbits.ACKEN = 1;
}

/********************************************************************************************
* Function Name:  WaitI2C																	*
* Description:    This routine holds I2C bus a couple of NOP w.r.t the parameter cnt		*
* Parameters:     unsigned int : cnt														*
* Return Value:   void																		*
********************************************************************************************/
void WaitI2C(unsigned int cnt)
{
	while(--cnt)
	{
		asm( "nop" );
		asm( "nop" );
	}
}

/********************************************************************************************
* Function Name:  ConfigIntI2C																*
* Description:    This routine enables/disables the SI2C & MI2C interrupts and sets their	*
*				  priorities.																*
* Parameters:     unsigned int : config														*
* Return Value:   void																		*
********************************************************************************************/

void ConfigIntI2C(unsigned int config)
{                
     /* clear the MI2C & SI2C Interrupts */
     _SI2CIF = 0;
     _MI2CIF = 0;

     _SI2CIP = (config & 0x0007);       /* set the SI2C priority */
     _MI2CIP = (config & 0x0070) >> 4;  /* set the MI2C priority */

     _SI2CIE = (config & 0x0008)>> 3;   /* enable/disable SI2C Int */
     _MI2CIE = (config & 0x0080) >> 7;  /* enable/disable MI2C Int */
}

/********************************************************************************************
* Function Name:  CloseI2C																	*
* Description:    This routine disables the I2C module by clearing the I2CEN bit in I2CCON	*
*				  register.The MI2C and SI2C interrupts are disabled and the corresponding 	*
*				  IF flags are cleared.														*
* Parameters:     void																		*
* Return Value:   void																		*
********************************************************************************************/

void CloseI2C(void)
{
	/* clear the I2CEN bit */
	I2CCONbits.I2CEN = 0;

	/* clear the SI2C & MI2C Interrupt enable bits */
	_SI2CIE = 0;
	_MI2CIE = 0;

	/* clear the SI2C & MI2C Interrupt flag bits */
	_SI2CIF = 0;
	_MI2CIF = 0;
}

/********************************************************************************************
* Function Name:  DataRdyI2C																*
* Description:    This routine provides the status whether the receive buffer is full by 	*
*				  returning the RBF bit.													*
* Parameters:     void																		*
* Return Value:   RBF bit status															*
********************************************************************************************/
char DataRdyI2C(void)
{
     return I2CSTATbits.RBF;
}

/********************************************************************************************
* Function Name:  MastergetsI2C																*
* Description:    This routine reads predetermined data string length from the I2C bus.		*
* Parameters:     unsigned int    : length													*
*                 unsigned char * : rdptr													*
* Return Value:   unsigned int																*
********************************************************************************************/

unsigned int MastergetsI2C(unsigned int length, unsigned char * rdptr, unsigned int i2c_data_wait)
{
    int wait = 0;
    while(length)                    /* Receive length bytes */
    {
        I2CCONbits.RCEN = 1;
        while(!DataRdyI2C())
        {
            if(wait < i2c_data_wait)
                wait++ ;                 
            else
            return(length);          /* Time out,return number of byte/word to be read */
        }
        wait = 0;
        *rdptr = I2CRCV;             /* save byte received */
        rdptr++;
        length--;
        if(length == 0)              /* If last char, generate NACK sequence */
        {
            I2CCONbits.ACKDT = 1;
            I2CCONbits.ACKEN = 1;
        }
        else                         /* For other chars,generate ACK sequence */
        {
            I2CCONbits.ACKDT = 0;
            I2CCONbits.ACKEN = 1;
        }
            while(I2CCONbits.ACKEN == 1); /* Wait till ACK/NACK sequence is over */
    }
    /* return status that number of bytes specified by length was received */
    return 0;    
}

/********************************************************************************************
* Function Name:  MasterputsI2C																*
* Description:    This routine is used to write out a string to the I2C bus.If write 		*
*				  collision occurs,-3 is sent.If Nack is received, -2 is sent.If string is 	*
*				  written and null char reached, 0 is returned.								*
* Parameters:     unsigned char * : wrptr													*
* Return Value:   unsigned int 																*
********************************************************************************************/

unsigned int MasterputsI2C(unsigned char * wrptr)
{
    while(*wrptr)                           //transmit data until null char
    {
        if(MasterWriteI2C(*wrptr) == -1)	// write a byte
        return -3;                          //return with write collison error

        while(I2CSTATbits.TBF);             //Wait till data is transmitted.

        IdleI2C();
        wrptr++;
    }
    return 0;			
}
