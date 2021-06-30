#ifndef _CAN_H  //宏定义，定义文件名称
#define _CAN_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm32f0xx.h"//引用STM32F0头文件
#include "data.h"
#define CAN_RX_PIN                 GPIO_Pin_8
#define CAN_TX_PIN                 GPIO_Pin_9
#define CAN_GPIO_PORT              GPIOB
#define CAN_AF_PORT                GPIO_AF_0
#define CAN_RX_SOURCE              GPIO_PinSource8
#define CAN_TX_SOURCE              GPIO_PinSource9

extern u8 CAN_T[240];  //CAN发送的数据
extern u8 CAN_T_LEN_SUM ; //CAN发送的数据个数
extern volatile u8 CAN_TX_Falg;  //数据发送标志位
extern u32 CAN_id_add;  
void CAN_GPIO(void);
void CAN_Configation(void);
void CAN_Recv_Control(CanRxMsg RxMessage);
//void CAN_Recv_Control(void);
void CAN_Task(void);
/***************************************************************************
*   函 数 名: CAN_TX_BcmuMsgHandle
*   功能说明: CAN_TX_BcmuMsgHandle发送数据
*   形    参：无
*   返 回 值: 数据发送
***************************************************************************/
u8 CAN_TX_BcmuMsgHandle(void);

u8 CAN_TX_BoyingMsgHandle(void);

#endif //定义文件名称结束


/************************ 论坛地址 www.zxkjmcu.com ******************************/
