#ifndef _MENU_L3_RPN_H
#define _MENU_L3_RPN_H

#include "menu_logic.h"
#include "math.h"

typedef void(*fp_v_t)(void);

typedef struct RPN_operator{
	uint8_t aviliableFlag;
	char*	name;			// limited to 4 char
	fp_v_t	instance;
} RPN_operator_t;

typedef struct RPN_operatorList{
	char*			name;	// limited to 4 char
	uint8_t			operatorListLength;
	RPN_operator_t*	operatorList;
} RPN_operatorList_t;

// {"Ctrl","Nums ","Oper","Conf"}
/*
*	Control:
*		¡ûCUR
*		DUP
*		x<>y
*		R¡ý
*		¡À
*		R¡ü
*		UNDO
*		CLX
*		CLA
*
*	Numbers :
*		0
*		1
*		2
*		3
*		4
*		5
*		6
*		7
*		8
*		9
*
*	Operator:
*		+
*		-
*		*
*		/
*		%
*		SQRT
*		x^y
*		----
*		LG10
*		LGe
*		LGxy
*		e^x
*		----
*		SIN
*		COS
*		TAN
*		ASIN
*		ACOS
*		ATAN
*		----
*		
*	Settings:
*		Deg[]		Enable DEG mode?
*		Log[]		Enable all Log functions?
*		Tri[]		Enable all Trig functions?
*		Int[]		Enable Integer mode?
*		DLi[]		Enable Dual Line feature?
*		
*
*	Key:
*		
*	Stack View:
*		+:		VIEW
*		-:		UNDO
*		SEL:	Enter op list
*		BACK:	Return to app menu
*
*	Op Menu:
*		+:		Next Item
*		-:		Prev Item
*		SEL:	Select
*		BACK:	Return to Stack View
*/
void menu_l3_RPN_enter(void* arg);
void menu_l3_RPN_exit(void* arg);
void menu_l3_RPN_k3(void* arg);
void menu_l3_RPN_k4(void* arg);
void menu_l3_RPN_k5(void* arg);
void menu_l3_RPN_k6(void* arg);
// void menu_l1_time_accel(void* arg);
void menu_l3_RPN_loop(void* arg);

char* RPN_double2Disp(double input, char outBuffer[3*4+1]);
double RPN_disp2Double(char* charBuffer);

void RPN_cancel(void);
void RPN_dup(void);
void RPN_swap_xy(void);
void RPN_rd(void);
void RPN_chs(void);
void RPN_drop(void);
void RPN_ru(void);
void RPN_undo(void);
void RPN_clx(void);
void RPN_cla(void);

void RPN_numeric(void);

void RPN_add(void);
void RPN_sub(void);
void RPN_mul(void);
void RPN_div(void);
void RPN_mod(void);
void RPN_sqrt(void);
void RPN_pow_yx(void);
void RPN_lge(void);
void RPN_pow_ex(void);
void RPN_sin(void);

// Extended logarithmic functions
void RPN_lg10(void);
void RPN_lgxy(void);

// Extended trigonometric functions
void RPN_cos(void);
void RPN_tan(void);
void RPN_asin(void);
void RPN_acos(void);
void RPN_atan(void);

void RPN_cfg_deg(void);
void RPN_cfg_log(void);
void RPN_cfg_trig(void);
void RPN_cfg_integer(void);
void RPN_cfg_dualLine(void);

#endif /* _MENU_L3_RPN_H */
