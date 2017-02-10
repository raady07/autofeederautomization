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

#ifndef _kp_inputfn_h
	#define _kp_inputfn_h

	void PasswordEnter( unsigned char PswdType );
	
	void MenuScreens();
	void MasterScreens(void);
	void CustomMenuScreens(void);
	void CustomMasterScreens(void);
	// Menu screens
	void AutoManualMode(void);
	void AutoPlusModeSetting(void);
	void Noof_Kgs_Feed(void);
	void Noof_Kgs_Feed_AP(void);
	void Noof_Kgs_Per_Cycle(void);
	void Cycle_Interval(void);
	void Display_Total_Crop(void);
	void ChangePassword(void);
	// Master screens 
	void SetTimeScreen(unsigned char WHATTIME);
	void SetDateScreen(void);
	void StartUpFeedScreen(void);
	void AutoReloadScreen(void);
	void SecondsPerKG(void);
	void Reset_TCF(void);
	void ResetMenuPassword(void);
	void SetHardReset(void);
	void autoreloadfunction(void);
	// Error Screens
	void WrongPassword(void);
	void mismatchpassword(void);
	void invalidtime(void);
	void invaliddate(void);
	void invalidkgs(void);
	void invalidkgs_feed(void);
	void ErrorScreen(void);
	extern void Decide_AutoPlus_Shift_Position(void);
	extern void Switch_OFF_FEEDER(void);
#endif	
