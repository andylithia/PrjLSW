#ifndef __HCMS29XX_UTIL_H
#define __HCMS29XX_UTIL_H

#include "stm32l0xx.h"

uint8_t* HCMS29xx_fetchChar(uint8_t ASCII, uint8_t* outBuffer);
uint8_t* HCMS29xx_stringBuilder(char* string, int len, uint8_t* outBuffer);

// Vitrual Buffer extension
#define m_HCMS29xx_VirtualBuffer_Create(NAME,LENGTH) \
				uint8_t NAME[LENGTH*9]
uint8_t* HCMS29xx_VirtualBuffer_2Physical(uint8_t* vBuffer, uint8_t* pBuffer, uint8_t blank, uint8_t shadeLevel);
uint8_t* HCMS29xx_VirtualBuffer_LeftShift(uint8_t* vBuffer, uint8_t count);
uint8_t* HCMS29xx_VirtualBuffer_StringBuilder(char* string, int len, uint8_t* vBuffer);
uint8_t* HCMS29xx_VirtualBuffer_SetShadePattern(uint8_t* vBuffer,uint8_t bufferLen, int pattern);

#endif /* __HCMS29XX_UTIL_H */
