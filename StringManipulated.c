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

#include"StringManipulated.h"


/********************************************************************************************
* Function Name:  MemoryClear																*
* Description:    This routine clears a pointer pointing to a address location				*
* Parameters:     unsigned char: Address													*
* Return Value:   void																		*
********************************************************************************************/
void MemoryClear(unsigned char *ptr)
{
	while(*ptr)
		*ptr++ = '\0';
}

/********************************************************************************************
* Function Name:  ConvertSTRING_CHAR														*
* Description:    This routine writes a character to a given address location using I2C		*
* Parameters:     unsigned char: Address													*
* Return Value:   void																		*
********************************************************************************************/
unsigned char ConvertSTRING_CHAR(unsigned char* ptr)
{
	unsigned char c = 0,i = 1,len = 0;
	len = stringlength(ptr);

	while(--len)
		i = i*10;
	while(*ptr)
	{
		c = c + i*(*ptr++ - 48);
		i = i/10;
	}
	return c;
}

/********************************************************************************************
* Function Name:  ConvertSTRING_Int															*
* Description:    This routine writes a character to a given address location using I2C		*
* Parameters:     unsigned char: Address													*
* Return Value:   void																		*
********************************************************************************************/
unsigned int ConvertSTRING_Int(unsigned char* ptr, unsigned char noofbytes)
{
	unsigned int c = 0,i = 1;

	while(--noofbytes)		i = i*10;
	while(*ptr)
	{
		c = c + i*(*ptr++ - 48);
		i = i/10;
	}
	return c;
}

/********************************************************************************************
* Function Name:  stringlength																*
* Description:    This routine calculates the length of the string							*
* Parameters:     unsigned int: Address														*
* Return Value:   unsigned char																*
********************************************************************************************/
unsigned char stringlength(unsigned char *ptr)
{
	unsigned char length = 0;

	while(*ptr++)
		length++;
	return length;
}

/********************************************************************************************
* Function Name:  StringCopy 																*
* Description:    This routine copies the array data from source to destination 			*
* Parameters:     unsigned char source, unsigned char destination							*
* Return Value:   void																		*
********************************************************************************************/
void StringCopy(unsigned char *Source, unsigned char *Destination)
{
	while(*Source)
		*Destination++ = *Source++;
	*Destination = '\0';
}

/********************************************************************************************
* Function Name:  MemoryCMP																	*
* Description:    This routine compares two arrays and returns 1 on true					*
* Parameters:     unsigned char source, unsigned char destination							*
* Return Value:   unsigned char																*
********************************************************************************************/
unsigned char MemoryCMP(unsigned char *Source, unsigned char *Destination)
{
	unsigned char compare = 0;

	if( stringlength(Source) == stringlength(Destination))
	{
		while(*Source)
		{
			if(*Source++ != *Destination++)
			 {
				compare = 0;
				break;
			 }
			else compare = 1;
		}
		return compare;
	}
	else return 0;
}

/********************************************************************************************
* Function Name:  Clear Buffer																*
* Description:    This routine is to Clear the Buffer. 										*
* Parameters:     *ptr 	- Buffer to be Cleared, length 	- Lenth of the Buffer				*
* Return Value:   void																		*
********************************************************************************************/
void ClearBuffer(unsigned char *ptr, int length)
{
	length--;
	while(length>=0)
	{
		ptr[length] = '\0';
		length--;
	}	
}

