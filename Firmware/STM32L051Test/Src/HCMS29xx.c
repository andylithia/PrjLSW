#include "HCMS29xx.h"

void _delay(uint16_t ticks){
	uint16_t i=0;
	while(ticks--){
		i=10;
		while(i--)__nop();
	}
}

#define _m_HCMS29xx_Tick() \
	do{\
		_m_SetHigh(HCMS_CLK);\
		_m_SetLow(HCMS_CLK);\
	}while(0)
	
void HCMS29xx_Reset(void){
	_m_SetHigh(HCMS_RST);
	_m_SetLow(HCMS_RST);
	_delay(10);
	_m_SetHigh(HCMS_RST);
}

void HCMS29xx_Init(void){
	
	_m_SetHigh(HCMS_CE);
	_m_SetLow(HCMS_RS);
	_m_SetLow(HCMS_CLK);
	// _m_SetLow(HPBLK);

	HCMS29xx_Reset();
}


void HCMS29xx_SendSingle(uint8_t data){
	_m_SetLow(HCMS_CE);
	uint8_t i;
	// 8-bit send
    for(i=0; i<8; i++){  
	
      if(data & BIT(7))
		_m_SetHigh(HCMS_DIN);
      else
		_m_SetLow(HCMS_DIN);
      // Shift to prepare for the next send
      data <<= 1;
      // CLK Tick
	  _m_SetHigh(HCMS_CLK);
      _m_SetLow(HCMS_CLK);
    }
	_m_SetHigh(HCMS_CE);
}

void HCMS29xx_SendCWR(uint8_t ctrl){
	_m_SetHigh(HCMS_RS);
	HCMS29xx_SendSingle(ctrl);
	_m_SetLow(HCMS_RS);
}

void HCMS29xx_SendMultiple(uint8_t* buffer, unsigned int count){
	_m_SetLow(HCMS_CE);

	uint8_t b;
	for(b=0;b<count;b++){
		uint8_t tmp = buffer[b];
		uint8_t i;
		// 8-bit shift out
		for(i=0; i<8; i++){  
		  if(tmp & BIT(7))
			_m_SetHigh(HCMS_DIN);
		  else
			_m_SetLow(HCMS_DIN);
		  // Shift to prepare for the next send
		  tmp <<= 1;
		  
		  // CLK Tick
		  _m_SetHigh(HCMS_CLK);
		  _m_SetLow(HCMS_CLK);
		}
	}
	
	_m_SetHigh(HCMS_CE);
}
