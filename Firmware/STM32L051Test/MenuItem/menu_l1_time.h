#ifndef _MENU_L1_TIME_H
#define _MENU_L1_TIME_H

#include "menu_logic.h"

/* Menu function pointer array type
*  Standard form of a menu function: void name(void*){}
*  Assignment:
*	#00. on parent menu init
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

void menu_l1_time_enter(void* arg);
void menu_l1_time_exit(void* arg);
void menu_l1_time_k3(void* arg);
void menu_l1_time_k4(void* arg);
void menu_l1_time_k5(void* arg);
void menu_l1_time_k6(void* arg);
// void menu_l1_time_accel(void* arg);
void menu_l1_time_loop(void* arg);

#endif /* _MENU_L1_TIME_H */
