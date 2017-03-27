#include "menu_l2_apps.h"
#include "HCMS29xx_Util.h"

#define SCROLL_DELAY  800
#define SCROLL_PERIOD 30

extern menu_t menu_l1_time;

extern RTC_HandleTypeDef hrtc;

extern uint8_t testBuffer[4*8];
extern m_HCMS29xx_VirtualBuffer_Create(gVirtualBuffer,8);

extern int gShadeEnable;

extern menu_t* currentMenu;
extern menu_t menu_l1_time;

extern menu_t menu_l3_RPN;
extern menu_t menu_l3_mastermind;
extern menu_t menu_l3_config;
/*
extern menu_t menu_l3_config;
extern menu_t menu_l3_stimer;
extern menu_t menu_l3_sgen;
*/

appEntry_t entryList[]={
	{"RPN Calc", 2, &menu_l3_RPN},
	{"MstrMind", 2, &menu_l3_mastermind},
	{"StopTimr", 2 ,NULL},
	{"SignGen ", 2, NULL},
	{"NoteView", 2, NULL},
	{"Configs ", 2, &menu_l3_config}
};

int entryCount= sizeof(entryList)/sizeof(appEntry_t);

int currentEntry=0;

static uint8_t scrollSegmentPointer=0;
static uint16_t scrollDelayCounter=0;
static uint16_t scrollCounter=0;
static uint8_t scrollFlag=0;
static uint8_t scrollShiftCount=8*4;

int appScrollFill=5;

mfa_t mfa_l2_apps={
	NULL,
	menu_l2_apps_enter,
	menu_l2_apps_exit,
	menu_l2_apps_k3,
	menu_l2_apps_k4,
	menu_l2_apps_k5,
	menu_l2_apps_k6,
	NULL, // menu_l1_time_accel,
	menu_l2_apps_loop,
	NULL,
	NULL,
	NULL
};

menu_t menu_l2_apps={ 
	2,
	2,
	NULL,
	&mfa_l2_apps
};

void menu_l2_apps_enter(void* arg){
	HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0xFF);
	HCMS29xx_VirtualBuffer_StringBuilder(entryList[currentEntry].name,4,gVirtualBuffer);
}

void menu_l2_apps_exit(void* arg){
	currentEntry=0;

	scrollSegmentPointer=0;
	scrollDelayCounter=0;
	scrollCounter=0;
	scrollFlag=0;
	scrollShiftCount=4*8;
}

void menu_l2_apps_k3(void* arg){
	currentEntry=(currentEntry==entryCount-1)?0:currentEntry+1;
	
	scrollSegmentPointer=0;
	scrollDelayCounter=0;
	scrollCounter=0;
	scrollFlag=0;
	scrollShiftCount=4*8;
	
	HCMS29xx_VirtualBuffer_StringBuilder(entryList[currentEntry].name,4,gVirtualBuffer);
}

void menu_l2_apps_k4(void* arg){
	if(entryList[currentEntry].instancep!=NULL){
		currentMenu=menu_enter(currentMenu, entryList[currentEntry].instancep, 0, 0);
	}
}

void menu_l2_apps_k5(void* arg){
	// Return to time screen
	currentMenu=menu_enter(currentMenu, &menu_l1_time, 0, 0);
}

void menu_l2_apps_k6(void* arg){
	currentEntry=(currentEntry==0)?entryCount-1:currentEntry-1;
	
	scrollSegmentPointer=0;
	scrollDelayCounter=0;
	scrollCounter=0;
	scrollFlag=0;
	scrollShiftCount=4*8;
	
	HCMS29xx_VirtualBuffer_StringBuilder(entryList[currentEntry].name,4,gVirtualBuffer);
}
// void menu_l1_time_accel(void* arg);

void menu_l2_apps_loop(void* arg){
	if(scrollDelayCounter>SCROLL_DELAY){
		scrollFlag=1;
	}
	else
		scrollDelayCounter++;
	
	if(scrollFlag)
		scrollCounter++;
	
	if(scrollShiftCount==8*4){
		scrollSegmentPointer=(scrollSegmentPointer>=entryList[currentEntry].nameLength-1)? \
				0:scrollSegmentPointer+1;
		HCMS29xx_VirtualBuffer_StringBuilder(entryList[currentEntry].name+(scrollSegmentPointer)*4,4,gVirtualBuffer+9*4);
			
		scrollDelayCounter=0;
		scrollShiftCount=0;
		scrollFlag=0;
	}
	
	if(scrollCounter>SCROLL_PERIOD){
		HCMS29xx_VirtualBuffer_LeftShift(gVirtualBuffer, 8);
		scrollShiftCount++;
		scrollCounter=0;
	}
	
	
}
