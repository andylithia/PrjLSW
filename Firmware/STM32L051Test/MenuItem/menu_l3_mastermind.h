#pragma once

#ifndef _MENU_L3_MASTERMIND_H
#define _MENU_L3_MASTERMIND_H

#ifdef VC_TEST
#include <stdint.h>
#endif // VC_TEST

#include "menu_logic.h"

//void decodeData(uint16_t compressed, uint8_t out);

uint8_t mm_checkValid(uint16_t number);
uint8_t mm_getRet(uint16_t number);
void mm_keyProcesser(uint8_t key);
void mm_writeBuffer(uint8_t charpos, uint8_t charindex);
void mm_writeBuffer_noErase(uint8_t charpos, uint8_t charindex);
void mm_itemselector(uint8_t key, uint8_t range);
void mm_newGame(void);
void mm_showNumbers(uint16_t number, uint8_t ret);

void menu_l3_mastermind_enter(void* arg);
void menu_l3_mastermind_exit(void* arg);
void menu_l3_mastermind_k3(void* arg);
void menu_l3_mastermind_k4(void* arg);
void menu_l3_mastermind_k5(void* arg);
void menu_l3_mastermind_k6(void* arg);
void menu_l3_mastermind_refresh(void* arg);

#endif
