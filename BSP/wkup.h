#ifndef _WKUP_H  //�궨�壬�����ļ�����
#define _WKUP_H

/*---------------------------ͷ�ļ�����--------------------------------------*/
#include "stm32f0xx.h"//����STM32F0ͷ�ļ�

/*---------------------------�궨������--------------------------------------*/
#define WKUP_PIN GPIO_Pin_5
#define WKUP_PORT GPIOB
#define COMWK_PIN GPIO_Pin_11
#define COMWK_PORT GPIOA

void WKUP_GPIO(void);
//void WKUP_Demo(void);
void Sleep_Bat(void);

#endif //�����ļ����ƽ���


/************************ ��̳��ַ www.zxkjmcu.com ******************************/
