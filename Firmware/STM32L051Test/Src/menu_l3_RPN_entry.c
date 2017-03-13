#include "menu_l3_RPN.h"

#define BIT(X)	(0x1<<X)
// I'm bita lazy XD
#define SF(X,Y)	X|=BIT(Y)
#define CF(X,Y)	X&=~BIT(Y)
#define XF(X,Y)	X^=BIT(Y)
#define FS(X,Y)	(X&BIT(Y))

#define d2r(x) (3.1415926*(x)/180.0)
#define r2d(x) (180.0*(x)/3.1415926)

// x y z t (lx ly)
extern double	RPN_stack[6];
extern char		RPN_IOBuffer[3*4+1];

extern RPN_operator_t RPN_opList_oper[17];
extern RPN_operator_t RPN_opList_conf[4];
extern RPN_operatorList_t operatorList[4];
extern uint8_t menuLevel; // 0.Stack View  1.L0  2.L1
extern uint8_t viewSection;
extern uint8_t cursorPos;
extern uint8_t listL0Pos;
extern uint8_t listL1Pos;	

extern uint8_t RPNFlags;

/* -- Internal -- */
void RPN_i_drop(void){
	RPN_stack[1]=RPN_stack[2];
	RPN_stack[2]=RPN_stack[3];
}

void RPN_i_savexy(void){
	RPN_stack[5]=RPN_stack[1];
	RPN_stack[4]=RPN_stack[0];
}

void RPN_i_savex(void){
	RPN_stack[4]=RPN_stack[0];
}
/* -- Control -- */
void RPN_cancel(void){
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_dup(void){
	RPN_i_savexy();
	
	RPN_stack[3]=RPN_stack[2];
	RPN_stack[2]=RPN_stack[1];
	RPN_stack[1]=RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
}
void RPN_swap_xy(void){
	RPN_i_savexy();
	// Xchg xy
	RPN_stack[0]=RPN_stack[1];
	RPN_stack[1]=RPN_disp2Double(RPN_IOBuffer);
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_rd(void){
	RPN_i_savexy();
	
	RPN_double2Disp(RPN_stack[1], RPN_IOBuffer);
	RPN_stack[0]=RPN_stack[1];
	RPN_stack[1]=RPN_stack[2];
	RPN_stack[2]=RPN_stack[3];
	RPN_stack[3]=RPN_disp2Double(RPN_IOBuffer);
}
void RPN_chs(void){
	RPN_i_savexy();
	
	if(viewSection==2) {
		RPN_IOBuffer[8]=(RPN_IOBuffer[8]=='+')?'-':'+';
		RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	}
	else {
		RPN_stack[0]=-RPN_disp2Double(RPN_IOBuffer);
		RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
	}
}
void RPN_drop(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_stack[1];
	RPN_stack[1]=RPN_stack[2];
	RPN_stack[2]=RPN_stack[3];
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_ru(void){
	RPN_i_savexy();
	
	RPN_double2Disp(RPN_stack[3], RPN_IOBuffer);
	RPN_stack[3]=RPN_stack[2];
	RPN_stack[2]=RPN_stack[1];
	RPN_stack[1]=RPN_stack[0];
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
}
void RPN_undo(void){
	// Restore xy
	RPN_stack[1]=RPN_stack[5];
	RPN_stack[0]=RPN_stack[4];
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_clx(void){
	RPN_i_savexy();
	
	RPN_stack[0]=0;
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_cla(void){
	RPN_i_savexy();
	
	RPN_stack[0]=0;
	RPN_stack[1]=0;
	RPN_stack[2]=0;
	RPN_stack[3]=0;
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}

/* -- Numbers -- */
void RPN_numeric(void){
	RPN_IOBuffer[(viewSection<<2)+cursorPos]=(char)listL1Pos+'0';
	if((cursorPos==0)&&((viewSection==2)||(viewSection==0&&RPN_IOBuffer[0]=='-')))
			cursorPos=1;
}

void RPN_add(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=RPN_stack[0]+RPN_stack[1];
	
	RPN_i_drop();
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_sub(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=RPN_stack[1]-RPN_stack[0];
	
	RPN_i_drop();
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_mul(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=RPN_stack[1]*RPN_stack[0];
	
	RPN_i_drop();
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_div(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=RPN_stack[1]/RPN_stack[0];
	
	RPN_i_drop();
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_mod(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=fmod(RPN_stack[1],RPN_stack[0]);
	
	RPN_i_drop();
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_sqrt(void){
	RPN_i_savex();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=sqrt(RPN_stack[0]);
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_pow_yx(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=pow(RPN_stack[1],RPN_stack[0]);
	
	RPN_i_drop();
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_lge(void){
	// Save x
	RPN_stack[4]=RPN_stack[0];
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=log(RPN_stack[0]);
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_pow_ex(void){
	RPN_i_savex();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=exp(RPN_stack[0]);
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_sin(void){
	RPN_i_savex();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=sin(RPN_stack[0]);
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}

// Extended logarithmic functions
void RPN_lg10(void){
	RPN_i_savex();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=log10(RPN_stack[0]);
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}
void RPN_lgxy(void){
	RPN_i_savexy();
	
	RPN_stack[0]=RPN_disp2Double(RPN_IOBuffer);
	RPN_stack[0]=log2(RPN_stack[1])/log2(RPN_stack[0]);
	
	RPN_i_drop();
	RPN_double2Disp(RPN_stack[0], RPN_IOBuffer);
}

// Extended trigonometric functions
void RPN_cos(void){
}
void RPN_tan(void){
}
void RPN_asin(void){
}
void RPN_acos(void){
}
void RPN_atan(void){
}

/* -- Settings -- */

void RPN_cfg_deg(void){
	if(FS(RPNFlags,0)){
		CF(RPNFlags,0);
		RPN_opList_conf[0].name[3]=' ';
	}
	else{
		SF(RPNFlags,0);
		RPN_opList_conf[0].name[3]='\x9E';
	}
}
void RPN_cfg_log(void){
	if(FS(RPNFlags,1)){
		CF(RPNFlags,1);
		RPN_opList_oper[7].aviliableFlag=0;
		RPN_opList_oper[9].aviliableFlag=0;
		RPN_opList_oper[10].aviliableFlag=0;
		RPN_opList_conf[1].name[3]=' ';
	}
	else{
		SF(RPNFlags,1);
		RPN_opList_oper[7].aviliableFlag=1;
		RPN_opList_oper[9].aviliableFlag=1;
		RPN_opList_oper[10].aviliableFlag=1;
		RPN_opList_conf[1].name[3]='\x9E';
	}
}
void RPN_cfg_trig(void){
	if(FS(RPNFlags,2)){
		CF(RPNFlags,2);
		RPN_opList_oper[12].aviliableFlag=0;
		RPN_opList_oper[13].aviliableFlag=0;
		RPN_opList_oper[14].aviliableFlag=0;
		RPN_opList_oper[15].aviliableFlag=0;
		RPN_opList_oper[16].aviliableFlag=0;
		RPN_opList_conf[2].name[3]=' ';
	}
	else{
		SF(RPNFlags,2);
		RPN_opList_oper[12].aviliableFlag=1;
		RPN_opList_oper[13].aviliableFlag=1;
		RPN_opList_oper[14].aviliableFlag=1;
		RPN_opList_oper[15].aviliableFlag=1;
		RPN_opList_oper[16].aviliableFlag=1;
		RPN_opList_conf[2].name[3]='\x9E';
	}
}
void RPN_cfg_integer(void){
}

/*
void RPN_cfg_dualLine(void){
}
*/
