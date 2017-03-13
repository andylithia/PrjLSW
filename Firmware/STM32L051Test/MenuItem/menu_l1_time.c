#include "menu_l1_time.h"
#include "HCMS29xx_Util.h"

extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef gtime;
extern RTC_DateTypeDef gdate;

extern uint8_t testBuffer[4*8];
extern m_HCMS29xx_VirtualBuffer_Create(gVirtualBuffer,8);

extern int gShadeEnable;

extern menu_t* currentMenu;
extern menu_t menu_l2_apps;

extern uint8_t MFA_allowInterrupt;

int testVal=0;
int modeFlag=0;		// 0: time  1: date	  2: year
int editFlag=0;
int editSelector=0;	// 0: minute/day  other: hour/month

int exitFlag=0;

mfa_t mfa_l1_time={
	NULL,
	menu_l1_time_enter,
	menu_l1_time_exit,
	menu_l1_time_k3,
	menu_l1_time_k4,
	menu_l1_time_k5,
	menu_l1_time_k6,
	NULL, // menu_l1_time_accel,
	menu_l1_time_loop,
	NULL,
	NULL,
	NULL
};

menu_t menu_l1_time={ 
	1,
	1,
	NULL,
	&mfa_l1_time
};

void menu_l1_time_enter(void* arg){
	HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x00);
	exitFlag=0;
}
void menu_l1_time_exit(void* arg){
	gShadeEnable=0;
	exitFlag=1;
}
void menu_l1_time_k3(void* arg){
	// +
	if(editFlag){
		HAL_RTC_GetTime(&hrtc, &gtime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &gdate, RTC_FORMAT_BIN);
		switch(modeFlag){
			
			case 0:
			if(editSelector) {
				// time:hour
				gtime.Hours=(gtime.Hours>=23)?0:gtime.Hours+1;
			}
			else {
				// time:minute
				gtime.Minutes=(gtime.Minutes>=59)?0:gtime.Minutes+1;
			}
			HAL_RTC_SetTime(&hrtc, &gtime, RTC_FORMAT_BIN);
			break;
			
			case 1:
			if(editSelector) {
				// date:month
				gdate.Month=(gdate.Month>=12)?1:gdate.Month+1;
			}
			else {
				// date:day
				if(gdate.Month==2){
					if(gdate.Year%4)
						gdate.Date=(gdate.Date>=28)?1:gdate.Date+1;
					else
						gdate.Date=(gdate.Date>=29)?1:gdate.Date+1;
				}
				else if(gdate.Month==1|gdate.Month==3|gdate.Month==5| \
					gdate.Month==7|gdate.Month==8|gdate.Month==10|gdate.Month==12) {
					gdate.Date=(gdate.Date>=31)?1:gdate.Date+1;
				}
				else {
					gdate.Date=(gdate.Date>=30)?1:gdate.Date+1;
				}
			}
			HAL_RTC_SetDate(&hrtc, &gdate, RTC_FORMAT_BIN);
			break;
			
			case 2:
			gdate.Year=(gdate.Year>=99)?0:gdate.Year+1;
			HAL_RTC_SetDate(&hrtc, &gdate, RTC_FORMAT_BIN);
			break;
		}
	}
	else {
		modeFlag=(modeFlag==2)?0:modeFlag+1;
	}
}
void menu_l1_time_k4(void* arg){
	// Select
	if(editFlag){
		// Change current digit
		editSelector=!editSelector;
		if(modeFlag!=2) {
			// Time and Date Settings
			if(editSelector)
				HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x03);
			else 
				HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x0C);
		}
		else {
			// Year Settings
			editSelector=0;
			HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x0C);
		}
	}
	else{
		// Enter edit mode.
		editFlag=!editFlag;
		gShadeEnable=1;
		editSelector=0;
		HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x0C);
		
	}
}
void menu_l1_time_k5(void* arg){
	// Back
	if(editFlag) {
		editFlag=!editFlag;
		gShadeEnable=0;
		editSelector=0;
	}
	else {
		// enter l2 app menu
		currentMenu=menu_enter(currentMenu, &menu_l2_apps, 0, 0);
	}
}
void menu_l1_time_k6(void* arg){
	// -
	if(editFlag){
		HAL_RTC_GetTime(&hrtc, &gtime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &gdate, RTC_FORMAT_BIN);
		switch(modeFlag){
			
			case 0:
			if(editSelector) {
				// time:hour
				gtime.Hours=(gtime.Hours==0)?23:gtime.Hours-1;
			}
			else {
				// time:minute
				gtime.Minutes=(gtime.Minutes==0)?59:gtime.Minutes-1;
			}
			HAL_RTC_SetTime(&hrtc, &gtime, RTC_FORMAT_BIN);
			break;
			
			case 1:
			if(editSelector) {
				// date:month
				gdate.Month=(gdate.Month<=1)?12:gdate.Month-1;
			}
			else {
				// date:day
				if(gdate.Month==2){
					if(gdate.Year%4)
						gdate.Date=(gdate.Date<=1)?28:gdate.Date-1;
					else
						gdate.Date=(gdate.Date<=1)?29:gdate.Date-1;
				}
				else if(gdate.Month==1|gdate.Month==3|gdate.Month==5| \
					gdate.Month==7|gdate.Month==8|gdate.Month==10|gdate.Month==12) {
					gdate.Date=(gdate.Date<=1)?31:gdate.Date-1;
				}
				else {
					gdate.Date=(gdate.Date<=1)?30:gdate.Date-1;
				}
			}
			HAL_RTC_SetDate(&hrtc, &gdate, RTC_FORMAT_BIN);
			break;
			
			case 2:
			gdate.Year=(gdate.Year==0)?99:gdate.Year-1;
			HAL_RTC_SetDate(&hrtc, &gdate, RTC_FORMAT_BIN);
			break;
		}

	}
	else {
		modeFlag=(modeFlag==0)?2:modeFlag-1;
	}
}

void menu_l1_time_loop(void* arg){
	HAL_RTC_GetTime(&hrtc, &gtime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gdate, RTC_FORMAT_BIN);
	char testStr[5];
	switch(modeFlag){
	
	case 0:
		sprintf(testStr, "%02d%02d", gtime.Hours, gtime.Minutes); break;
	case 1:
		sprintf(testStr, "%2d%2d", gdate.Month, gdate.Date); break;
	case 2:
		sprintf(testStr, "20%02d", gdate.Year); break;
	}
	if(!exitFlag)
		HCMS29xx_VirtualBuffer_StringBuilder(testStr,4,gVirtualBuffer);
}
