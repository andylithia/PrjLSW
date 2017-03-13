#include "menu_l3_RPN.h"
#include "HCMS29xx_Util.h"

#define __ABS(x) ((x<0)?-(x):x)

extern uint8_t testBuffer[4*8];
extern m_HCMS29xx_VirtualBuffer_Create(gVirtualBuffer,8);

extern int gShadeEnable;
extern menu_t* currentMenu;
extern menu_t menu_l2_apps;

// x y z t (lx ly)
double	RPN_stack[6];
char	RPN_IOBuffer[3*4+1]="00000000+000";

uint8_t menuLevel=0; // 0.Stack View  1.L0  2.L1
uint8_t viewSection=0;
uint8_t cursorPos=0;
uint8_t listL0Pos=0;
uint8_t listL1Pos=0;

uint8_t RPNFlags=0;	

uint8_t IOFlag=1;	// 0.Don't care IOBuffer  1.Process IOBuffer

mfa_t mfa_l3_RPN={
	NULL,
	menu_l3_RPN_enter,
	menu_l3_RPN_exit,
	menu_l3_RPN_k3,
	menu_l3_RPN_k4,
	menu_l3_RPN_k5,
	menu_l3_RPN_k6,
	NULL, // menu_l1_time_accel,
	menu_l3_RPN_loop,
	NULL,
	NULL,
	NULL
};

menu_t menu_l3_RPN={ 
	3,
	3,
	NULL,
	&mfa_l3_RPN
};

RPN_operator_t RPN_opList_ctrl[]={	
				{1, "CANx",			&RPN_cancel},
				{1, "DUPx",			&RPN_dup},
				{1, "x\x8E\x8Dy",	&RPN_swap_xy},
				{1, "R\x8F",		&RPN_rd},
				{1, "CHS ",			&RPN_chs},
				{1, "DROP",			&RPN_drop},
				{1, "R\x90",		&RPN_ru},
				{1, "UNDO",			&RPN_undo},
				{1, "CL x",			&RPN_clx},
				{1, "CL A",			&RPN_cla}
};

RPN_operator_t RPN_opList_nums[]={
				{1, "0", &RPN_numeric},
				{1, "1", &RPN_numeric},
				{1, "2", &RPN_numeric},
				{1, "3", &RPN_numeric},
				{1, "4", &RPN_numeric},
				{1, "5", &RPN_numeric},
				{1, "6", &RPN_numeric},
				{1, "7", &RPN_numeric},
				{1, "8", &RPN_numeric},
				{1, "9", &RPN_numeric},
};

RPN_operator_t RPN_opList_oper[]={
				{1, "+",	&RPN_add},
				{1, "-",	&RPN_sub},
				{1, "*",	&RPN_mul},
				{1, "/",	&RPN_div},
				{1, "%",	&RPN_mod},
				{1, "\x83",	&RPN_sqrt},
				{1, "y^x",	&RPN_pow_yx},
				{0, "Lg10",	&RPN_lg10},
				{1, "Lg e",	&RPN_lge},
				{0, "Lgxy", &RPN_lgxy},
				{0, "e^x",	&RPN_pow_ex},
				{1, "Sin",	&RPN_sin},	
				{0, "Cos",	&RPN_cos},
				{0, "Tan",	&RPN_tan},
				{0, "aSin",	&RPN_asin},
				{0, "aCos",	&RPN_acos},
				{0, "aTan",	&RPN_atan}
};
	
char RPN_conf_deg[]="Deg ";
char RPN_conf_log[]="Log ";
char RPN_conf_tri[]="Tri ";

RPN_operator_t RPN_opList_conf[]={
				{1, RPN_conf_deg,	&RPN_cfg_deg},
				{1, RPN_conf_log, &RPN_cfg_log},
				{1, RPN_conf_tri, &RPN_cfg_trig},
				{1, "Int ", &RPN_cfg_integer},
//				{1, "DLi ", &RPN_cfg_dualLine}
};
static RPN_operatorList_t operatorList[4]={
				{"Ctrl", sizeof(RPN_opList_ctrl)/sizeof(RPN_operator_t), RPN_opList_ctrl},
				{"Nums", sizeof(RPN_opList_nums)/sizeof(RPN_operator_t), RPN_opList_nums},
				{"Oper", sizeof(RPN_opList_oper)/sizeof(RPN_operator_t), RPN_opList_oper},
				{"Conf", sizeof(RPN_opList_conf)/sizeof(RPN_operator_t), RPN_opList_conf}
};

int RPN_atoi(char* charBuffer,int sectionLength){
	uint8_t negativeFlag=0;
	int output=0;
	int position=0;
	if(charBuffer[0]=='-'){
		negativeFlag=1;
		position++;
	}
	else if(charBuffer[0]=='+')
		position++;
	for(;position<sectionLength;position++)
		output=output*10+(charBuffer[position]-'0');
	if(negativeFlag)
		output=-output;
	return output;
}

char* RPN_double2Disp(double input, char outBuffer[3*4+1]){
	// Obtain base-10 exponent and 9-digit mantissa(and sign) 
	int exp=(log10(fabs(input)));
	int man9=input/pow(10,exp-8);
	
	// Reset output buffer
	for(int i=0;i<4*3;i++)
		outBuffer[i]='0';
	
	// Place mantissa and sign
	sprintf(outBuffer,"%d",man9);
	
	// Remove \0 from mantissa part to ensure proper render
	for(int i=1;i<8;i++)
		if(outBuffer[i]=='\0')
			outBuffer[i]='0';
	
	int absExp=__ABS(exp);
	if (absExp>999)
		// Exponent too large or too small
		sprintf(outBuffer+8,"OutR");
	else{
		outBuffer[8]=(absExp==exp)?'+':'-';
		sprintf(outBuffer+9,"%03d",__ABS(exp));
	}
	return outBuffer;
}

double RPN_disp2Double(char* charBuffer){
	return ((double)RPN_atoi(charBuffer,8))*pow(10,RPN_atoi(charBuffer+8, 4)-7);
}

void menu_l3_RPN_enter(void* arg){
	RPN_stack[0]=RPN_disp2Double("12345678+011");
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
	
}
void menu_l3_RPN_exit(void* arg){
	gShadeEnable=0;
	menuLevel=0; // 0.Stack View  1.L0  2.L1
	viewSection=0;
	cursorPos=0;
}
void menu_l3_RPN_k3(void* arg){
	// +
	switch (menuLevel){
	case 0:
		cursorPos=(cursorPos==3)?0:cursorPos+1;
		break;
	case 1:
		listL0Pos=(listL0Pos==sizeof(operatorList)/sizeof(RPN_operatorList_t)-1)? \
			0:listL0Pos+1;
		break;
	case 2:
		do
			listL1Pos=(listL1Pos==operatorList[listL0Pos].operatorListLength-1)?0:listL1Pos+1;
		while(!operatorList[listL0Pos].operatorList[listL1Pos].aviliableFlag);
		break;
	}
	
}
void menu_l3_RPN_k4(void* arg){
	// SEL
	switch(menuLevel){
	case 0:
		menuLevel=1;
		break;
	case 1:
		menuLevel=2;
		break;
	case 2:
		(void)(operatorList[listL0Pos].operatorList[listL1Pos].instance)();
		menuLevel=0;
		break;
	}
}
void menu_l3_RPN_k5(void* arg){
	// BACK
	switch(menuLevel){
	case 0:
		currentMenu=menu_enter(currentMenu, &menu_l2_apps, 0, 0);
		break;
	case 1:
		if(listL0Pos!=0)
			listL0Pos=0;
		else
			menuLevel=0;
		break;
	case 2:
		if(listL1Pos!=0)
			listL1Pos=0;
		else
			menuLevel=1;
		break;
	}
}
void menu_l3_RPN_k6(void* arg){
	// -
	switch(menuLevel){
	case 0:
		viewSection=(viewSection==2)?0:viewSection+1;
		break;
	case 1:
		listL0Pos=(listL0Pos==0)? \
			sizeof(operatorList)/sizeof(RPN_operatorList_t)-1:listL0Pos-1;
		break;
	case 2:
		do
			listL1Pos=(listL1Pos==0)?operatorList[listL0Pos].operatorListLength-1:listL1Pos-1;
		while(!operatorList[listL0Pos].operatorList[listL1Pos].aviliableFlag);
		break;
		
	}
}
// void menu_l1_time_accel(void* arg);
void menu_l3_RPN_loop(void* arg){
	switch(menuLevel){
	case 0:
		// Stack View
		gShadeEnable=1;
		HCMS29xx_VirtualBuffer_StringBuilder((RPN_IOBuffer+(viewSection*4)), 4, gVirtualBuffer);
		if((cursorPos==0)&&((viewSection==2)||(viewSection==0&&RPN_IOBuffer[0]=='-')))
			cursorPos=1;
		HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x00);
		gVirtualBuffer[cursorPos*9+8]=0x03;
		break;
	case 1:
		// L0 List view
		gShadeEnable=0;
		HCMS29xx_VirtualBuffer_StringBuilder(operatorList[listL0Pos].name, 4, gVirtualBuffer);
		break;
	case 2:
		gShadeEnable=0;
		HCMS29xx_VirtualBuffer_StringBuilder("    ", 4, gVirtualBuffer);
		HCMS29xx_VirtualBuffer_StringBuilder(operatorList[listL0Pos].operatorList[listL1Pos].name, 4, gVirtualBuffer);
		break;
	}
}



