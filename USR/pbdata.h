/************************ 自定义公共头文件 **************************************/

#ifndef _PBDATA_H//宏定义，定义文件名称
#define _PBDATA_H
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stdio.h"
#include "string.h"

#include "jdq.h"
#include "can.h"
//#include "rs232.h"
#include "spi.h"
#include "w25qxx.h"
#include "SYSTypeDef.h"
#include "SimulateI2C.h"
#include "BQ769x0.h"
#include "rtc.h"
#include "wkup.h"
#include "WWDG.h"
#include "soc.h"
#include "flash.h"
#include "EventRecord.h"
//#include "stm32f0xx_tim.h"
//#include "stm32f0xx_spi.h"
//#include "stm32f0xx_gpio.h"
#include "data.h"

void delay_us(uint32_t nCount); //微秒延时程序
void delay_ms(uint16_t nCount); //毫秒延时程序
void Delay10ms( void );

#endif //定义文件名称结束


/************************ 论坛地址 www.zxkjmcu.com ******************************/
