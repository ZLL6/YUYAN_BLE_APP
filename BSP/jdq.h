#ifndef _JDQ_H  //宏定义，定义文件名称
#define _JDQ_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm32f0xx.h"//引用STM32F0头文件
#include "pbdata.h"
#include "SYSTypeDef.h"
/*---------------------------宏定义声明--------------------------------------*/

#define  HOLD  GPIO_Pin_1
#define  GPIOx_HOLD   GPIOA

#define  LED_W1  GPIO_Pin_10 
#define  LED_W2  GPIO_Pin_2 
#define  LED_W3  GPIO_Pin_1
#define  LED_W4  GPIO_Pin_0
#define  GPIOx_W1_4   GPIOB
#define  LED_W5  GPIO_Pin_3
#define  LED_W6  GPIO_Pin_2
#define  GPIOx_W5_6   GPIOA
#define  LED_W7  GPIO_Pin_1
#define  GPIOx_W7   GPIOF
#define  btdate  GPIO_Pin_11 

#define  LNVCC_EN GPIO_Pin_4
#define  GPIOx_LNVCC_EN GPIOB

#define  CAN_EN GPIO_Pin_0
#define  GPIOx_CAN_EN GPIOF

#define  POW_SAVE  GPIO_Pin_12
#define  GPIOx_POW_SAVE  GPIOA

#define  W25Q128_EN  GPIO_Pin_13
#define  GPIOx_W25Q128_EN  GPIOC

#define  W25Q128_WP  GPIO_Pin_1
#define  GPIOx_W25Q128_WP  GPIOA

#define  SELF_DISCHAR GPIO_Pin_12 
#define  GPIOx_SELF_DISCHAR GPIOB

#define  ALERT GPIO_Pin_3 
#define  GPIOx_ALERT GPIOB

#define LED_W1_H GPIO_ResetBits(GPIOB,LED_W1)
#define LED_W1_l GPIO_SetBits(GPIOB,LED_W1)
#define LED_W2_H GPIO_ResetBits(GPIOB,LED_W2)
#define LED_W2_l GPIO_SetBits(GPIOB,LED_W2)
#define LED_W3_H GPIO_ResetBits(GPIOB,LED_W3)
#define LED_W3_l GPIO_SetBits(GPIOB,LED_W3)
#define LED_W4_H GPIO_ResetBits(GPIOB,LED_W4)
#define LED_W4_l GPIO_SetBits(GPIOB,LED_W4)
#define LED_W5_H GPIO_ResetBits(GPIOA,LED_W5)
#define LED_W5_l GPIO_SetBits(GPIOA,LED_W5)
#define LED_W6_H GPIO_ResetBits(GPIOA,LED_W6)
#define LED_W6_l GPIO_SetBits(GPIOA,LED_W6)
#define LED_W7_H GPIO_ResetBits(GPIOF,LED_W7)
#define LED_W7_l GPIO_SetBits(GPIOF,LED_W7)


#define   LED_1_4_OFF  LED_W1_l;LED_W2_l;LED_W3_l;LED_W4_l
#define   LED_1_4_ON  LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_H

#define   Green_ON     LED_W7_l;LED_W6_H;LED_W5_l
#define   Yellow_ON    LED_W7_l;LED_W6_H;LED_W5_H
#define   Red_ON       LED_W7_l;LED_W6_l;LED_W5_H
#define   Magenta_ON   LED_W7_H;LED_W6_l;LED_W5_H
#define   Cyan_ON      LED_W7_H;LED_W6_H;LED_W5_l
#define   Blue_ON      LED_W7_H;LED_W6_l;LED_W5_l
#define   LED_ON      LED_W7_H;LED_W6_H;LED_W5_H
#define  LED_OFF       LED_W7_l;LED_W6_l;LED_W5_l

#define Buzzer_OFF  GPIO_SetBits(GPIOA,Buzzer)
#define Buzzer_ON GPIO_ResetBits(GPIOA,Buzzer)
#define EE_EN_ON   GPIO_ResetBits(GPIOC,EE_EN)
#define EE_EN_OFF   GPIO_SetBits(GPIOC,EE_EN)
//#define    ALERT_STATE    GPIO_ReadInputDataBit(GPIOB,ALERT)  //读取按键
#define WK_UP       GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)  //读取按键
extern u8 key_ack_status;
extern volatile  u8 key_finish_flg;
extern u8 key_down_FLAG;
typedef enum
{
  KEY_Ready = 0,//允许按下
  KEY_Down,
  KEY_Lock,
  KEY_Ulock     
} key_event;//按键事件
#pragma pack (push)
#pragma pack(1) 
typedef struct
{
u8  Key_Mode;//工作模式长按，断按
key_event  Key_state; //按键状态
u32 Key_Down_Timer;//按键事件
u16 Key_Timer_Over;//按下超时时间
}Key_TypeDef;
#pragma pack(pop)
extern Key_TypeDef Key_set;
/*---------------------------函数声明--------------------------------------*/
void LED_GPIO(void);  //LED初始化函数
void GPIO_ENABLE( FunctionalState x);
Key_TypeDef KEY_Scan(void);
void LED_GPIO_TASK(void);
void key_event_hander(void);
uint32_t HAL_GetTick(void);

#endif //定义文件名称结束


/************************ 论坛地址 www.zxkjmcu.com ******************************/
