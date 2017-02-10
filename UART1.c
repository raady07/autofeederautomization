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
#include"uart.h"

/********************************************************************************************
* Function Name     : INIT_UART1                                         					*
* Description       : This Function will configure and initialize UART						*
* Parameters        : None                                               					*
* Return Value      : void.    																*
********************************************************************************************/
void INIT_UART1(void)  				
{
	DDR_UART1TX		 	= 0;    		// Configure RF3 as Output for UART1 Tx .
	DDR_UART1RX			= 1;			// Configure RF2 as Input for UART1 Rx.
	
    /********************* CONFIGURE UART1 Baud Rate Generator Register ********************/
    U1BRG    = __UARTBAUD; 						
								
    /************************* CONFIGURE UART1MODE Register ********************************/
    U1MODEbits.USIDL	= 0;    		// Continue operation in Idle mode
    U1MODEbits.ALTIO	= 0;    		// UART communicates usingalternate UxTX and UxRX I/O pins
    U1MODEbits.WAKE 	= 1;    		// Enable Wake up
    U1MODEbits.LPBACK	= 0;    		// Disable Loop Back- 0 / enable for rec transmit -1.
    U1MODEbits.ABAUD	= 0;    		// Disable Auto Baud
    U1MODEbits.PDSEL	= 0;    		// 8 Bit Data, No Parity
    U1MODEbits.STSEL	= 0;    		// 1 Stop Bit
    
    IPC2bits.U1TXIP		= 6;			// Tx priority level.
	IPC2bits.U1RXIP		= 7;			// Rx priority level.

    U1MODEbits.UARTEN	= 1;    		// Enable UART
    
    IEC0bits.U1TXIE 	= 0; 			// Disabled TX interrupt.
    IEC0bits.U1RXIE		= 0;			// Disabled Rx interrupt.
         
    /********************* CONFIGURE UART1 STATUS & CONTROL Register ***********************/
    U1STAbits.UTXISEL	= 0;    		// Interrupt when a character is transferred to the Transmit Shift register
    U1STAbits.UTXBRK 	= 0;    		// Operate U1Tx Normally on Transmit Break Bit
    U1STAbits.UTXEN  	= 1;    		// Enable U1 Tranmission
    U1STAbits.URXISEL 	= 3;    		// Interrupt flag bit is set when Receive Buffer is 3/4 full (i.e., has 3 data characters)
 }

/********************************************************************************************
* Function Name     : BusyUART1                                           					*
* Description       : This function checks if the UART is busy and flags ready for Tx/Rx.	*
* Parameters        : unsigned int * address of the string buffer to be transmitted			*  
* Return Value      : None                                                 					*
********************************************************************************************/    
char BusyUART1(void)
{  
         return(!U1STAbits.TRMT);
}           
     
/********************************************************************************************
* Function Name     : CloseUART1                                           					*
* Description       : This function stop and closes the UART functionality					*
* Parameters        : None																	*  
* Return Value      : None                                                 					*
********************************************************************************************/ 
void CloseUART1(void)
{  
    U1MODEbits.UARTEN = 0;
	
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;
	
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;
}

/********************************************************************************************
* Function Name     : DataRdyUART1                                           				*
* Description       : This function checks URXDA and flags if the UART is ready				*
* Parameters        : None																	*  
* Return Value      : unsigned int                                                 			*
********************************************************************************************/ 
char DataRdyUART1(void)
{
    return(U1STAbits.URXDA);
}

/********************************************************************************************
* Function Name     : putsUART1                                           					*
* Description       : This function puts the data string to be transmitted in to the		*
*                     transmit buffer (till NULL character).Operation is done by filling up	*
*					  UxTXREG.																*
* Parameters        : unsigned int * address of the string buffer to be transmitted			*  
* Return Value      : None                                                 					*
********************************************************************************************/    
void putsUART1(unsigned int *buffer)
{
    char * temp_ptr = (char *) buffer;

    /* transmit till NULL character is encountered */

    if(U1MODEbits.PDSEL == 3)        /* check if TX is 8bits or 9bits */
    {
        while(*buffer != '\0') 
        {
            while(U1STAbits.UTXBF); /* wait if the buffer is full */
            U1TXREG = *buffer++;    /* transfer data word to TX reg */
        }
    }
    else
    {
        while(*temp_ptr != '\0')
        {
            while(U1STAbits.UTXBF);  /* wait if the buffer is full */
            U1TXREG = *temp_ptr++;   /* transfer data byte to TX reg */
        }
    }
}

/********************************************************************************************
* Function Name     : ReadUART1                                           					*
* Description       : This function returns the contents of UxRXREG buffer					*
* Parameters        : None																	*  
* Return Value      : unsigned int                                                 			*
********************************************************************************************/  
unsigned int ReadUART1(void){
	if(U1MODEbits.PDSEL == 3)
          return (U1RXREG);
    else
          return (U1RXREG & 0xFF);
}     
     
/********************************************************************************************
* Function Name     : WriteUART1                                           					*
* Description       : This function writes data into the UxTXREG for that to be Tx'd		*
* Parameters        : None																	*  
* Return Value      : unsigned int                                                 			*
********************************************************************************************/ 
void WriteUART1(unsigned char data)
{
    if(U1MODEbits.PDSEL == 3)
    {
        while(U1STAbits.UTXBF); /* wait if the buffer is full */
        U1TXREG = data;
    }   
    else
	{
        while(U1STAbits.UTXBF); /* wait if the buffer is full */
        U1TXREG = data & 0xFF;
 	}   
}

void PCPutInt1(unsigned long int i)
{
	unsigned char j[10],x = 0;

	if( i == 0 )
		WriteUART1('0');
	else
	{
		while( i )
		{
			j[x++] = i%10;
			i = i/10;
		}
		while( x-- )
			WriteUART1( j[x] + 48 );
	}
}

