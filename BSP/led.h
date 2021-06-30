#ifndef _LED_H  //�궨�壬�����ļ�����
#define _LED_H

/*---------------------------ͷ�ļ�����--------------------------------------*/
#include "stm32f0xx.h"//����STM32F0ͷ�ļ�

/*---------------------------�궨������--------------------------------------*/
#define LED_1_PIN GPIO_Pin_6 //����GPIO_Pin_6����ΪLED_1_PIN���൱������������
#define LED_B_PIN GPIO_Pin_7 //����GPIO_Pin_7����ΪLED_B_PIN���൱������������
#define LED_G_PIN GPIO_Pin_8 //����GPIO_Pin_8����ΪLED_G_PIN���൱������������
#define LED_R_PIN GPIO_Pin_9 //����GPIO_Pin_9����ΪLED_R_PIN���൱������������

#define LED_PORT GPIOC  //����GPIOC�˿�ΪLED_PORT���൱������������

//������������ܽſ���LED,��ôֻ����������Ӧ�Ķ˿������ű�ż���

#define LED_1_L GPIO_ResetBits(LED_PORT,LED_1_PIN);    //����LED_1_L,����LED_1_L���PC6��������͵�ƽ
#define LED_1_H GPIO_SetBits(LED_PORT,LED_1_PIN);      //����LED_1_H,����LED_1_H���PC6��������͸�ƽ
#define LED_1_T GPIO_WriteBit(LED_PORT,LED_1_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_1_PIN)));//����LED_1_T,����LED_1_T���PC6���������ƽ��ת

#define LED_B_L GPIO_ResetBits(LED_PORT,LED_B_PIN);    //����LED_B_L,����LED_B_L���PC7��������͵�ƽ
#define LED_B_H GPIO_SetBits(LED_PORT,LED_B_PIN);      //����LED_B_H,����LED_B_H���PC7��������͸�ƽ
#define LED_B_T GPIO_WriteBit(LED_PORT,LED_B_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_B_PIN)));//����LED_1_T,����LED_1_T���PC7���������ƽ��ת

#define LED_G_L GPIO_ResetBits(LED_PORT,LED_G_PIN);    //����LED_G_L,����LED_G_L���PC8��������͵�ƽ
#define LED_G_H GPIO_SetBits(LED_PORT,LED_G_PIN);      //����LED_G_H,����LED_G_H���PC8��������͸�ƽ
#define LED_G_T GPIO_WriteBit(LED_PORT,LED_G_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_G_PIN)));//����LED_1_T,����LED_1_T���PC8���������ƽ��ת

#define LED_R_L GPIO_ResetBits(LED_PORT,LED_R_PIN);    //����LED_R_L,����LED_R_L���PC9��������͵�ƽ
#define LED_R_H GPIO_SetBits(LED_PORT,LED_R_PIN);      //����LED_R_H,����LED_R_H���PC9��������͸�ƽ
#define LED_R_T GPIO_WriteBit(LED_PORT,LED_R_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_R_PIN)));//����LED_1_T,����LED_1_T���PC9���������ƽ��ת

/*---------------------------��������--------------------------------------*/
void LED_GPIO(void);  //LED��ʼ������
void LED_Demo(void);

#endif //�����ļ����ƽ���


/************************ ��̳��ַ www.zxkjmcu.com ******************************/
