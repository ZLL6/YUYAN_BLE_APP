#ifndef _SPI_H  //�궨�壬�����ļ�����
#define _SPI_H

/*---------------------------ͷ�ļ�����--------------------------------------*/
#include "stm32f0xx.h"//����STM32F0ͷ�ļ�

/*---------------------------��������--------------------------------------*/

void SPI1_GPIO(void);
void SPI1_Configation(void);
uint8_t SPI1_SendByte(uint8_t byte);

void SPI2_GPIO(void);
void SPI2_Configation(void);
uint8_t SPI2_SendByte(uint8_t byte);

#endif //�����ļ����ƽ���

/************************ ��̳��ַ www.zxkjmcu.com ******************************/


