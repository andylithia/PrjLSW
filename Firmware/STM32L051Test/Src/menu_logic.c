#include "menu_logic.h"
extern uint8_t MFA_allowInterrupt;
/*
menu_t* menu_return(menu_t* current, int delta, void* arg_exit, void* arg_enter){
	if((*(current->mfa_ptr))[2]!=NULL)
		(*(current->mfa_ptr))[2](arg_exit);			// Call 'on exit' event
	menu_t* temp_pointer;
	for(int i=0; i<delta; i++)
		temp_pointer=temp_pointer->upper_level;
	if((*(temp_pointer->mfa_ptr))[1]!=NULL)
		(*(temp_pointer->mfa_ptr))[1](arg_enter);	// Call 'on enter' event
	return temp_pointer;
}
*/

menu_t* menu_enter(menu_t* current, menu_t* target, void* arg_exit, void* arg_enter){
	MFA_allowInterrupt=0;
	if((*(current->mfa_ptr))[2]!=NULL)
		(*(current->mfa_ptr))[2](arg_exit); 		// Call 'on exit' event
	if((*(target->mfa_ptr))[1]!=NULL)
		(*(target->mfa_ptr))[1](arg_enter);			// Call 'on enter' event
	MFA_allowInterrupt=1;
	return target;
}
