#ifndef _HCMS29XX_H
#define _HCMS29XX_H

#include "stm32l0xx.h"

#define BIT(X)	(0x1<<X)

#define _m_SetLow(x) ( x##_GPIO_Port)->BRR = (uint32_t)( x##_Pin)
#define _m_SetHigh(x) ( x##_GPIO_Port)->BSRR = (uint32_t)( x##_Pin)

void HCMS29xx_Init(void);
void HCMS29xx_Reset(void);
void HCMS29xx_SendSingle(uint8_t data);
void HCMS29xx_SendCWR(uint8_t ctrl);

void HCMS29xx_SendMultiple(uint8_t* buffer, unsigned int count);

#endif /* _HCMS29XX_H */
