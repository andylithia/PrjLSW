#include "menu_l3_config.h"
#include "HCMS29xx.h"
#include "HCMS29xx_Util.h"

#define SCROLL_DELAY  200
#define SCROLL_PERIOD 30

extern uint8_t testBuffer[4*8];
extern m_HCMS29xx_VirtualBuffer_Create(gVirtualBuffer,8);

extern int gShadeEnable;

extern menu_t* currentMenu;
extern menu_t menu_l2_apps;

// Intensity control
extern uint8_t peakCurrentControl;
extern uint8_t gPWMControl;
extern uint8_t gSleepDelay;
extern int gSleepDelayActual;

static uint8_t configTmpVar;

static uint8_t currentEntry=0;
static uint8_t state=0;

static uint8_t scrollSegmentPointer=0;
static uint16_t scrollDelayCounter=0;
static uint16_t scrollCounter=0;
static uint8_t scrollFlag=0;
static uint8_t scrollShiftCount=8*4;

static char testStr[5];

/*	configType:
*		|7|6|5|4|3|2|1|0|
*              |   \- Digit '0' enable ?
*              |- 00: Y or N
*              |- 01: Hex 0-F
*              |- 10: Dec 0-9
*              \- 11: Dec 00-99 
*/
typedef uint8_t configType_t;
#define CFG_ZERO_DISABLE	0x00
#define CFG_ZERO_ENABLE		0x01
#define CFG_YN						0x00
#define CFG_0F						0x02
#define CFG_09						0x04
#define CFG_0099					0x06

typedef void(*fp_v_t)(void);

typedef struct configEntry{
	char*					name;
	uint8_t				nameLength;
	uint8_t*			instancep;
	configType_t	rangeType;
	fp_v_t				callBack;
}configEntry_t;

mfa_t mfa_l3_config={
	NULL,
	menu_l3_config_enter,
	menu_l3_config_exit,
	menu_l3_config_k3,
	menu_l3_config_k4,
	menu_l3_config_k5,
	menu_l3_config_k6,
	NULL,
	menu_l3_config_loop,
	NULL,
	NULL,
	NULL
};

menu_t menu_l3_config={
	4,
	3,
	NULL,
	&mfa_l3_config
};

void resetScroll(void);
// Callbacks
void configIntensityCallback(void);
void configScrTimeoutCallback(void);

configEntry_t configEntryList[]={
	{"Intensity   ", 3, &gPWMControl, CFG_ZERO_DISABLE|CFG_0F, &configIntensityCallback},
	{"ScrTimeout  ", 3, &gSleepDelay, CFG_ZERO_DISABLE|CFG_0099, &configScrTimeoutCallback},
	{"aTrigger", 2, NULL, CFG_ZERO_ENABLE|CFG_0F, NULL},
	{"aWakeup ", 2, NULL, CFG_ZERO_ENABLE|CFG_YN, NULL},
	{"AutoSave", 2, NULL, CFG_ZERO_ENABLE|CFG_YN, NULL}
};

const  uint8_t configEntryCount=sizeof(configEntryList)/sizeof(configEntry_t);

void menu_l3_config_enter(void* arg){
	HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0xFF);
	HCMS29xx_VirtualBuffer_StringBuilder(configEntryList[currentEntry].name,4,gVirtualBuffer);
	configTmpVar=*(configEntryList[currentEntry].instancep);
}

void menu_l3_config_exit(void* arg){
	resetScroll();
}

void menu_l3_config_k3(void* arg){
	// +
	if(state){
		// +1
		switch(configEntryList[currentEntry].rangeType&0x0E){
			case CFG_YN:		configTmpVar=!configTmpVar; break;
			case CFG_0F:		configTmpVar=(configTmpVar>=0x0F)?0:configTmpVar+1; break;
			case CFG_09:		configTmpVar=(configTmpVar>=9)?0:configTmpVar+1; break;
			case CFG_0099:	configTmpVar=(configTmpVar>=99)?0:configTmpVar+1; break;
		}
		if(!(configEntryList[currentEntry].rangeType&CFG_ZERO_ENABLE))
			// Correct zero
			if(configTmpVar==0) configTmpVar=1;
	}
	else {
		// Next entry
		currentEntry=(currentEntry>=configEntryCount-1)?0:currentEntry+1;
		resetScroll();
		HCMS29xx_VirtualBuffer_StringBuilder(configEntryList[currentEntry].name,4,gVirtualBuffer);
		
	}
}

void menu_l3_config_k4(void* arg){
	// SEL
	if(state){
		// Confirm edit
		if(configEntryList[currentEntry].callBack!=NULL)
			(void)(configEntryList[currentEntry].callBack)();
		else if (configEntryList[currentEntry].instancep!=NULL)
			*(configEntryList[currentEntry].instancep)=configTmpVar;
		state=0;
		resetScroll();
		HCMS29xx_VirtualBuffer_StringBuilder(configEntryList[currentEntry].name,4,gVirtualBuffer);
	}
	else {
		// Enter edit mode
		state=1;
		configTmpVar=*(configEntryList[currentEntry].instancep);
	}
}

void menu_l3_config_k5(void* arg){
	// BACK
	if(state){
		// Discard changes
		state=0;
		resetScroll();
		HCMS29xx_VirtualBuffer_StringBuilder(configEntryList[currentEntry].name,4,gVirtualBuffer);
	}
	else
		if(currentEntry!=0) currentEntry=0;
		// Return to apps menu
		else currentMenu=menu_enter(currentMenu, &menu_l2_apps, 0, 0);
}

void menu_l3_config_k6(void* arg){
	if(state){
		// -1
		switch(configEntryList[currentEntry].rangeType&0x0E){
			case CFG_YN:		configTmpVar=!configTmpVar; break;
			case CFG_0F:
			case CFG_09:
			case CFG_0099:	configTmpVar=(configTmpVar<=0)?0:configTmpVar-1; break;
		}
		if(!(configEntryList[currentEntry].rangeType&CFG_ZERO_ENABLE))
			// Correct zero
			if(configTmpVar==0) 
				switch (configEntryList[currentEntry].rangeType&0x0E){
					case CFG_YN:		configTmpVar=!configTmpVar; break;
					case CFG_0F:		configTmpVar=0x0F; break;
					case CFG_09:		configTmpVar=9; break;
					case CFG_0099:	configTmpVar=99; break;
			}
	}
	else {
		// Next entry
		currentEntry=(currentEntry<=0)?configEntryCount-1:currentEntry-1;
		resetScroll();
		HCMS29xx_VirtualBuffer_StringBuilder(configEntryList[currentEntry].name,4,gVirtualBuffer);
	}
}

void menu_l3_config_loop(void* arg){
	if(state){
		switch(configEntryList[currentEntry].rangeType&0x0E){
			case CFG_YN: 
				if(configTmpVar) sprintf(testStr, ":Y  ");
				else sprintf(testStr, ":N  ");
				break;
			case CFG_09:
				sprintf(testStr, ":%d  ", configTmpVar); break;
			case CFG_0F:
				sprintf(testStr, ":%X  ", configTmpVar); break;
			case CFG_0099:
				sprintf(testStr, ":%02d ", configTmpVar); break;
		}
		HCMS29xx_VirtualBuffer_StringBuilder(testStr,4,gVirtualBuffer);
	}
	else {
		if(scrollDelayCounter>SCROLL_DELAY)
			scrollFlag=1;
		else
			scrollDelayCounter++;
		
		if(scrollFlag)
			scrollCounter++;
		
		if(scrollShiftCount==8*4){
			scrollSegmentPointer=(scrollSegmentPointer>=configEntryList[currentEntry].nameLength-1)? \
					0:scrollSegmentPointer+1;
			HCMS29xx_VirtualBuffer_StringBuilder(configEntryList[currentEntry].name+(scrollSegmentPointer)*4,4,gVirtualBuffer+9*4);
				
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
}

void resetScroll(void){
	scrollSegmentPointer=0;
	scrollDelayCounter=0;
	scrollCounter=0;
	scrollFlag=0;
	scrollShiftCount=4*8;
}
	
void configIntensityCallback(void){
	HCMS29xx_SendCWR((0x1<<6)|(peakCurrentControl<<4)|configTmpVar);
	gPWMControl=configTmpVar;
}

void configScrTimeoutCallback(void){
	// *64
	gSleepDelayActual=((int)gSleepDelay)<<6;
}
