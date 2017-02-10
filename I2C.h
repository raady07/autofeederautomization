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
 
 #ifndef __I2C_H
	#define __I2C_H
	#include	<P30F5011.h>
	
	#define __I2C_ON        0b1000001000100000
	#define __I2C_OFF       0x7FFF 					//	I2C module disabled 
	
	void INIT_I2C(void);
	void OpenI2C(unsigned int config1, unsigned int config2);
	void StartI2C(void);
	void RestartI2C(void);
	void StopI2C(void);
	void IdleI2C(void);
	unsigned char MasterReadI2C(void);
	char MasterWriteI2C(unsigned char data_out);
	void NotAckI2C(void);
	void AckI2C(void);
	void WaitI2C(unsigned int cnt);
	void ConfigIntI2C(unsigned int config);
	void CloseI2C(void);
	char DataRdyI2C(void);
	unsigned int MastergetsI2C(unsigned int length, unsigned char * rdptr, unsigned int i2c_data_wait);
	unsigned int MasterputsI2C(unsigned char * wrptr);
#endif
