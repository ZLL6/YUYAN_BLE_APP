/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _RTC_H
#define _RTC_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f0xx.h"


/* 类型定义 ------------------------------------------------------------------*/ 
#pragma pack (push)
#pragma pack(1) 
typedef struct
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
	//uint8_t Week;
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
}RTC_DateTimeTypeDef;
#pragma pack(pop)
typedef enum{
  RTC_OK = 0x00,                  //正确
  RTC_ERR,                        //错误
  RTC_TIMEOUT,                    //超时
  RTC_EXIT,                       //退出
  RTC_OVERFLOW,                   //溢出
}RTC_RESULT;

/* 宏定义 --------------------------------------------------------------------*/
#define SEC_DAY                   (24*3600)                //一天秒数
#define SEC_YEAR                  (365*SEC_DAY)            //一年秒数(平年)

/* 函数申明 ------------------------------------------------------------------*/
void RTC_Configuration(void);
void RTC_Initializes(void);
RTC_RESULT RTC_DateRegulate(RTC_DateTimeTypeDef RTC_DateTimeStructure);
RTC_RESULT RTC_TimeRegulate(RTC_DateTimeTypeDef RTC_DateTimeStructure);
RTC_RESULT RTC_SetDateTime(RTC_DateTimeTypeDef RTC_DateTimeStructure);
void RTC_GetDateTime(RTC_DateTimeTypeDef *RTC_DateTimeStructure);
//void RTC_Demo(void);

#endif /* _RTC_H */

/************************ 论坛地址 www.zxkjmcu.com ******************************/
