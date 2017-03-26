#include "menu_l3_mastermind.h"
#include "HCMS29xx_Util.h"

#define MM_UINT16_DECODE_DATA(num, arr)	arr[0] = num & 0xF;\
										arr[1] = (num >> 4) & 0xF;\
										arr[2] = (num >> 8) & 0xF;\
										arr[3] = num >> 12;

#define MM_MAX_QUERY_COUNT 10
//#define 
//extern uint8_t testBuffer[4 * 8];
extern m_HCMS29xx_VirtualBuffer_Create(gVirtualBuffer, 8);

const unsigned char MM_Font[] = {
	0x00, 0x1F, 0x11, 0x1F, 0x00,	//0
	0x00, 0x00, 0x1F, 0x00, 0x00,	//1
	0x00, 0x19, 0x15, 0x13, 0x00,	//2
	0x00, 0x15, 0x15, 0x1F, 0x00,	//3
	0x00, 0x07, 0x04, 0x1F, 0x00,	//4
	0x00, 0x17, 0x15, 0x1D, 0x00,	//5
	0x00, 0x1F, 0x15, 0x1D, 0x00,	//6
	0x00, 0x01, 0x01, 0x1F, 0x00,	//7
	0x00, 0x1F, 0x15, 0x1F, 0x00,	//8
	0x00, 0x17, 0x15, 0x1F, 0x00,	//9

	0x00, 0x00, 0x40, 0x00, 0x00,	//10-num "."
	0x40, 0x40, 0x40, 0x40, 0x40,	//11-pos "_"

	0x20, 0x20, 0x24, 0x3E, 0x04,	//12-history
	0x10, 0x20, 0x10, 0x08, 0x04,	//13-check/OK
	0x30, 0x24, 0x2E, 0x24, 0x3C,	//14-restart
	0x22, 0x14, 0x08, 0x14, 0x22,	//15-cross/no
};

const char MM_Banner[] = {'M', 'a', 's', 't', 'e', 'r', 'M', 'i', 'n', 'd', '!', ' ', ' ', ' ', ' ', ' '};

extern int gShadeEnable;
extern menu_t *currentMenu;
extern menu_t menu_l2_apps;

#ifdef VC_TEST
typedef int RTC_HandleTypeDef;
typedef struct {
	uint8_t Hours, Minutes, Seconds, TimeFormat;
	uint32_t SubSeconds, SecondFraction, DayLightSaving, StoreOperation;
} RTC_TimeTypeDef;
typedef struct {
	uint8_t Year, Month, Day, WeekDay;
} RTC_DateTypeDef;
#define RTC_FORMAT_BIN 0
uint32_t HAL_GetTick(void);
#else
#include "stm32l0xx_hal.h"
#endif // VC_TEST

//extern RTC_HandleTypeDef hrtc;
//extern RTC_TimeTypeDef gtime;
//extern RTC_DateTypeDef gdate;

uint16_t	MM_answer;						//compressed answer
uint16_t	MM_query[MM_MAX_QUERY_COUNT];	//compressed queries
uint8_t		MM_count = 0x80;				//guess count 128 or 0 : gameover
											//any number less than count is gussed

uint8_t		MM_state = 0x00;
uint16_t	MM_counter = 0;
//uint8_t		MM_midcounter = 0;

//States:
//b00 0000 00 = welcome //MstM						ok
//b00 0001 00 = history; counter = historylevel		ok
//b00 0010 00 = correct answer is xxx				ok
//b00 0011 xx = input number digit select			ok
//b10 0001 1x = (blinking) LD yes/no  (loadgame)	ok
//b10 0000 xx = (blinking) input number				ok
//b10 0010 xx = (blinking) ? history(144)/ok/exit  (game menu)(123)  hist/ok/reset/exit		ok
//b10 1011 00 = (blinking) Win!						ok
//b10 1111 00 = (blinking) Lose						ok
//b10 1001 1x = (blinking) New ok/no  (new/exit)	ok
//b10 1000 1x = (blinking) New ok/no  (new/back)	ok

mfa_t mfa_l3_mastermind = {
	NULL,
	menu_l3_mastermind_enter,
	menu_l3_mastermind_exit,
	menu_l3_mastermind_k3,
	menu_l3_mastermind_k4,
	menu_l3_mastermind_k5,
	menu_l3_mastermind_k6,
	NULL,
	menu_l3_mastermind_refresh,
	NULL,
	NULL,
	NULL
};
menu_t menu_l3_mastermind = {
	233, //TODO Add a ID
	3,
	NULL,
	&mfa_l3_mastermind
};

uint8_t mm_checkValid(uint16_t number) {
	uint8_t numArr[4];
	MM_UINT16_DECODE_DATA(number, numArr);

	return (numArr[0] ^ numArr[1]) && (numArr[0] ^ numArr[2]) && (numArr[0] ^ numArr[3]) && \
		(numArr[1] ^ numArr[2]) && (numArr[1] ^ numArr[3]) && \
		(numArr[2] ^ numArr[3]);
}
uint8_t mm_getRet(uint16_t number) {
	uint8_t ret = 0;//ret:low4bit=number high4bit=position
	uint8_t numArr[4], ansArr[4];
	MM_UINT16_DECODE_DATA(number, numArr);
	MM_UINT16_DECODE_DATA(MM_answer, ansArr);
	for (uint8_t i = 0; i < 4; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			if (!(numArr[i] ^ ansArr[j])) {// ==
				if (i^j) {// i != j -> num
					ret += 0x01;
				} else {
					ret += 0x10;
				}
			}
		}
	}
	return ret;
}

void menu_l3_mastermind_enter(void* arg) {
	MM_state = 0x00;
	MM_counter = 0xFFFF;
	gShadeEnable = 0;
	//show lbl
	HCMS29xx_VirtualBuffer_StringBuilder((char *)MM_Banner + 12, 4, gVirtualBuffer);//TODO Dangerous!
}
void menu_l3_mastermind_exit(void* arg) {
	gShadeEnable = 0;
}

void menu_l3_mastermind_k3(void* arg) {//2 -
	mm_keyProcesser(2);
}
void menu_l3_mastermind_k4(void* arg) {//1 ok
	mm_keyProcesser(1);
}
void menu_l3_mastermind_k5(void* arg) {//0 back
	mm_keyProcesser(0);
}
void menu_l3_mastermind_k6(void* arg) {//3 +
	mm_keyProcesser(3);
}

void menu_l3_mastermind_refresh(void* arg) {
	if (MM_state & 0x80) {//flash(blink)
		MM_counter = (MM_counter + 1) & 0x0FFF;
		if (!(MM_counter & 0x00FF)) {//b 00000000 xxxxxxxx reverse
			if (MM_counter & 0x0100) {//dim
				if (~MM_state & 0x2C) {//others
					HCMS29xx_VirtualBuffer_StringBuilder(" ", 1, gVirtualBuffer + 9 * (MM_state & 0x03));
				} else {//win/lose
					HCMS29xx_VirtualBuffer_StringBuilder("    ", 4, gVirtualBuffer);
				}
			} else {//light
				uint8_t charindex, charpos = MM_state & 0x03;
				switch (MM_state & 0x3C) {
				case 0x00://in number
					
					charindex = (MM_query[MM_count] >> (charpos << 2)) & 0xF;
					break;
				case 0x04: case 0x24: case 0x20://? ok/no
					
					charindex = (charpos << 1) + 9;
					break;
				case 0x08://new:hist/ok/reset/exit

					charindex = 12 + charpos;
					break;
				case 0x2C://win
					HCMS29xx_VirtualBuffer_StringBuilder("WIN!", 4, gVirtualBuffer);
					return;
				case 0x3C://lose
					HCMS29xx_VirtualBuffer_StringBuilder("Lose", 4, gVirtualBuffer);
					//return;
				default:;
				}
				//write vbuffer
				mm_writeBuffer(charpos, charindex);
			}
		}

	} else if (!MM_state) {//00 0000 00 - welcome
		MM_counter = (MM_counter + 1) & 0x1FFF;

		if (!(MM_counter & 0x003F)) {
			if (!(MM_counter & 0x07FF)) {
				HCMS29xx_VirtualBuffer_StringBuilder((char *)MM_Banner + (MM_counter >> 9), 4, gVirtualBuffer + 36);//TODO Dangerous!
			}

			HCMS29xx_VirtualBuffer_LeftShift(gVirtualBuffer, 8);
		}
	}
}

void mm_keyProcesser(uint8_t key) {
	//0123:back Ok - +
	switch (MM_state & 0xFC) {
	case 0x00://welc
		if (!key) {//back - quit
			currentMenu = menu_enter(currentMenu, &menu_l2_apps, NULL, NULL);
			return;
		}
		//check saved game
		//NEW: treat 0 as not-saved
		if (MM_count & 0x7F) {//saved
			MM_state = 0x86;
			MM_counter = 0;
			gShadeEnable = 1;
			HCMS29xx_VirtualBuffer_StringBuilder("L?", 2, gVirtualBuffer);
			mm_writeBuffer(2, 13); mm_writeBuffer(3, 15);
			HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x7);//hl selection
		} else {//not saved
			mm_newGame();
		}

		return;
	case 0x84://load game?
		if (!key) {//back - quit
			currentMenu = menu_enter(currentMenu, &menu_l2_apps, NULL, NULL);
		} else if (key & 0x2) {//selc
			mm_itemselector(key, 1);
		} else {
			if (MM_state & 0x01) {//no load:new
				mm_newGame();
			} else {//history
				MM_state = 0x04;
				MM_counter = MM_count;
				gShadeEnable = 0;
				mm_keyProcesser(3);
			}
		}

		return;
	case 0x0C://select input number digit
		if (!key) {//back - menu
			MM_state = 0x88;
			MM_counter = 0;
			HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x1);//except 1 gray
			mm_writeBuffer(0, 12); mm_writeBuffer(1, 13); mm_writeBuffer(2, 14); mm_writeBuffer(3, 15);
		} else if (key & 0x2) {//selc
			mm_itemselector(key, 3);
		} else {//ok - changenum
				//save old digit
			MM_counter = (((MM_query[MM_count] >> ((MM_state & 0x03) << 2)) & 0xF) << 12);// | (MM_counter & 0x0FFF);
			//jmp
			MM_state = (MM_state & 0x03) | 0x80;
		}
		return;
	case 0x80://(blinking)input number
		if (key & 0x2) {//rebuilded: add/sub 1
			
			uint8_t movebits = (MM_state & 0x03) << 2;
			uint16_t num = MM_query[MM_count] & (0xF << movebits);//get the num
			MM_query[MM_count] ^= num;//delete old num
			num = (((num >> movebits) + ((key ^ 3) ? 1 : 9)) % 10) << movebits;//change the num
			MM_query[MM_count] ^= num;//restore num

			MM_counter = 0xFFFF;
			menu_l3_mastermind_refresh(NULL);//force refresh
		} else {
			if (!key) {//back - cancel
					   //restore old digit
				MM_query[MM_count] = (MM_query[MM_count] & (~(uint16_t)(0xF << ((MM_state & 0x03) << 2)))) | ((MM_counter >> 12) << ((MM_state & 0x03) << 2));
			}
			MM_counter = 0xFFFF;
			menu_l3_mastermind_refresh(NULL);//stop blinking chars
											 //jmp
			MM_state = (MM_state & 0x03) | 0x0C;
		}
		return;
	case 0x88://(blinking) hist/ok/reset/exit (game menu)
		if (!key) {//back - selectdigit
			MM_state = 0x0C;
			MM_counter = 0;
			mm_showNumbers(MM_query[MM_count], 0);
			HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x1);//except 1 gray
		} else if (key & 0x2) {
			mm_itemselector(key, 3);
		} else {
			switch (MM_state) {
			case 0x89://ok - guess!

				if (!mm_checkValid(MM_query[MM_count])) {
					mm_keyProcesser(0);
					return;
				}

				MM_count++;
				MM_query[MM_count] = 0;
			case 0x88://hist
				if (!MM_count) return; //no hist
				MM_state = 0x04;
				MM_counter = MM_count;
				gShadeEnable = 0;
				mm_keyProcesser(2);//force display
				//menu_l3_mastermind_k3(NULL);
				return;
			case 0x8A://reset
				MM_state = 0xA2;
				MM_counter = 0;
				HCMS29xx_VirtualBuffer_StringBuilder("N?", 2, gVirtualBuffer);
				mm_writeBuffer(2, 13); mm_writeBuffer(3, 15);
				HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x7);//hl selection
				//mm_newGame();
				return;
			case 0x8B://exit
				currentMenu = menu_enter(currentMenu, &menu_l2_apps, NULL, NULL);
				//return;
			default:;
			}
		}
		return;
	case 0x04://history
		if (key & 0x2) {//change hist
			MM_counter += (key ^ 3) ? 1 : -1;
			if (MM_counter > 0x7FFF) MM_counter = 0;
			if (MM_counter >= MM_count) MM_counter = MM_count - 1;
			//display
			mm_showNumbers(MM_query[MM_counter], mm_getRet(MM_query[MM_counter]));
		} else {//resume
			//chk win/lost
			if (!(mm_getRet(MM_query[MM_count - 1]) ^ 0x40)) {//win
				MM_state = 0xAC;
				MM_counter = 0xFFFF;
				MM_count = 0x80;
				//gShadeEnable = 0;
				//HCMS29xx_VirtualBuffer_StringBuilder("Win!", 4, gVirtualBuffer);
			} else if (MM_count >= MM_MAX_QUERY_COUNT) {//lose
				MM_state = 0xBC;
				MM_counter = 0xFFFF;
				MM_count = 0x80;
				//gShadeEnable = 0;
				//HCMS29xx_VirtualBuffer_StringBuilder("Lose", 4, gVirtualBuffer);
			} else {//continue
				MM_state = 0x0C; 
				gShadeEnable = 1;
				mm_showNumbers(MM_query[MM_count], 0);
				HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x1);//except 1 gray
				//MM_counter = 0;
				
			}
		}
		return;
	case 0xBC://Lose
		//goto showfailanswer
		MM_state = 0x08;
		mm_showNumbers(MM_answer, 0);
		
		return;
	case 0xAC: case 0x08://Win/Lost+answer
		//goto querynew
		MM_state = 0xA6;
		MM_counter = 0;
		gShadeEnable = 1;
		HCMS29xx_VirtualBuffer_StringBuilder("N?", 2, gVirtualBuffer);
		mm_writeBuffer(2, 13); mm_writeBuffer(3, 15);
		HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x7);
		
		return;
	case 0xA4://(blinking) new?yn
		if (key & 0x2) {
			mm_itemselector(key, 1);
		} else if ((MM_state ^ 0xA6) || !key) {//selected = exit or keyback
			currentMenu = menu_enter(currentMenu, &menu_l2_apps, NULL, NULL);
		} else {//ok - newgame
			mm_newGame();
		}
		return;
	case 0xA0://(blinking) new?yn(back)
		if (key & 0x2) {
			mm_itemselector(key, 1);
		} else if ((MM_state ^ 0xA2) || !key) {//selected = exit or keyback
			//back to game
			MM_state = 0x0C; 
			mm_showNumbers(MM_query[MM_count], 0);
			HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x1);//except 1 gray
			//MM_counter = 0;
		} else {//ok - newgame
			mm_newGame();
		}
		//return;
	default:;
	}
}

void mm_writeBuffer(uint8_t charpos, uint8_t charindex) {
	for (uint8_t i = 0; i < 5; i++) {
		gVirtualBuffer[charpos * 9 + i] = MM_Font[charindex * 5 + i];
	}
}

void mm_writeBuffer_noErase(uint8_t charpos, uint8_t charindex) {
	for (uint8_t i = 0; i < 5; i++) {
		gVirtualBuffer[charpos * 9 + i] |= MM_Font[charindex * 5 + i];
	}
}

void mm_itemselector(uint8_t key, uint8_t range) {
	//range 1/3 only
	MM_counter = 0xFFFF;
	menu_l3_mastermind_refresh(NULL);//stop blinking chars
									 //k=2(-) 3(+)
	MM_state = (MM_state & (0xFF ^ range)) | ((MM_state + ((key ^ 3) ? 1 : -1)) & range);
	HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, (1 << (MM_state & 0x03)) | ((range ^ 3) ? 0x03 : 0x00));
}

void mm_newGame(void) {
	//newgame, refresh screen, init
	MM_state = 0x7F;//disable interrupt
	MM_count = 0;
	gShadeEnable = 1;

	uint32_t seed = HAL_GetTick();//generate seed
	do {
		MM_answer = 0;
		for (uint8_t i = 0; i < 4; i++) {
			do {
				seed = (uint32_t)(907633515U + seed * 196314165U);
			} while ((seed >> 28) >= 10);
			MM_answer |= (seed >> 28) << (4 * i);
		}
	} while (!mm_checkValid(MM_answer));
	MM_query[0] = 0;
	//mm_showNumbers(MM_answer, 0);
	mm_showNumbers(0, 0);
	HCMS29xx_VirtualBuffer_SetShadePattern(gVirtualBuffer, 4, 0x1);//except 1 gray
	//MM_counter = 0;
	MM_state = 0x0C;
}

void mm_showNumbers(uint16_t number, uint8_t ret) {
	mm_writeBuffer(0, number & 0xF);
	mm_writeBuffer(1, (number >> 4) & 0xF);
	mm_writeBuffer(2, (number >> 8) & 0xF);
	mm_writeBuffer(3, number >> 12);
	uint8_t i;
	for (i = 0; i < (ret & 0x0F); i++) {
		mm_writeBuffer_noErase(i, 10);
	}
	for (uint8_t j = 0; j < (ret >> 4); j++) {
		mm_writeBuffer_noErase(i + j, 11);
	}
}
