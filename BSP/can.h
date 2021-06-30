#ifndef _CAN_H  //�궨�壬�����ļ�����
#define _CAN_H

/*---------------------------ͷ�ļ�����--------------------------------------*/
#include "stm32f0xx.h"//����STM32F0ͷ�ļ�
#include "data.h"
#define CAN_RX_PIN                 GPIO_Pin_8
#define CAN_TX_PIN                 GPIO_Pin_9
#define CAN_GPIO_PORT              GPIOB
#define CAN_AF_PORT                GPIO_AF_0
#define CAN_RX_SOURCE              GPIO_PinSource8
#define CAN_TX_SOURCE              GPIO_PinSource9

extern u8 CAN_T[240];  //CAN���͵�����
extern u8 CAN_T_LEN_SUM ; //CAN���͵����ݸ���
extern volatile u8 CAN_TX_Falg;  //���ݷ��ͱ�־λ
extern u32 CAN_id_add;  
void CAN_GPIO(void);
void CAN_Configation(void);
void CAN_Recv_Control(CanRxMsg RxMessage);
//void CAN_Recv_Control(void);
void CAN_Task(void);
/***************************************************************************
*   �� �� ��: CAN_TX_BcmuMsgHandle
*   ����˵��: CAN_TX_BcmuMsgHandle��������
*   ��    �Σ���
*   �� �� ֵ: ���ݷ���
***************************************************************************/
u8 CAN_TX_BcmuMsgHandle(void);

u8 CAN_TX_BoyingMsgHandle(void);

#endif //�����ļ����ƽ���


/************************ ��̳��ַ www.zxkjmcu.com ******************************/
