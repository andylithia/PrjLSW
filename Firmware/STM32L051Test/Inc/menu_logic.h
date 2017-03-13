#ifndef _MENU_LOGIC_H
#define _MENU_LOGIC_H

#include "stm32l0xx.h"

/* Menu function pointer array type
*  Standard form of a menu function: void name(void*){}
*  Assignment:
*	#00. on init
*	#01. on enter
*	#02. on exit
*	#03. on key #3 (+)
*	#04. on key #4 (SEL)
*	#05. on key #5 (BACK)
*	#06. on key #6 (-)
*	#07. on accelerometer event
*	#08. loop
*	#09. reserved
*	#10. reserved
*	#11. reserved
*/

#define MFA_CALL(MENU_INSTANCE,ENTRY,ARG) (*(MENU_INSTANCE->mfa_ptr))[ENTRY](ARG);

typedef void(*mfa_t[12])(void*);

typedef struct menu* menu_p_t;

typedef struct menu{
	uint8_t id;
	uint8_t global_level;
	menu_p_t upper_level;
	mfa_t* mfa_ptr;
} menu_t;

menu_t* menu_return(menu_t* current, int delta, void* arg_exit, void* arg_enter);
menu_t* menu_enter(menu_t* current, menu_t* target, void* arg_exit, void* arg_enter);

#endif /* _MENU_LOGIC_H */
