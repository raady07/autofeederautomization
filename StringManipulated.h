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
 
#ifndef _stringmaipualted_h
	#define _stringmaipualted_h
	
	void MemoryClear(unsigned char *ptr);
	unsigned char ConvertSTRING_CHAR(unsigned char *ptr);
	unsigned int ConvertSTRING_Int(unsigned char *ptr, unsigned char noofbytes);
	unsigned char stringlength(unsigned char *ptr);
	void StringCopy(unsigned char *Source, unsigned char *Destination);
	unsigned char MemoryCMP(unsigned char *Source, unsigned char *Destination);
	void ClearBuffer(unsigned char *ptr, int length);
#endif
