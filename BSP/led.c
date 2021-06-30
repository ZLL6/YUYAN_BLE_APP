#include "pbdata.h"
/*********************************************************************************
*   �� �� ��: LED_Init
*   ����˵��: LED��GPIO�ܽų�ʼ��
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************/
void LED_GPIO(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*ѡ��Ҫ���Ƶ�GPIOB����*/															   
	GPIO_InitStructure.GPIO_Pin = LED_1_PIN|LED_B_PIN|LED_G_PIN|LED_R_PIN;	

	/*����Ҫ���Ƶ�GPIOB����Ϊ���ģʽ*/	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;

	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 

	/*��������ģʽΪ����*/
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;  

	/*���ÿ⺯������ʼ��GPIOB*/
	GPIO_Init(LED_PORT, &GPIO_InitStructure);	
	
	LED_1_H;
	LED_B_H;//����Ϩ��
	LED_G_H;//����Ϩ��
	LED_R_H;//����Ϩ��
}

/*********************************************************************************
*   �� �� ��: LED_Demo2
*   ����˵��: ��˸���̣����ʱ��Ϊ0.5�롣
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************/
void LED_Demo(void)
{
	LED_1_L;
	delay_ms(500);
	LED_1_H;
	delay_ms(500);
	
	LED_R_L;
	delay_ms(500);
	LED_R_H;
	delay_ms(500);
	
	LED_G_L;
	delay_ms(500);
	LED_G_H;
	delay_ms(500);
	
	LED_B_L;
	delay_ms(500);
	LED_B_H;
	delay_ms(500);
}

/************************ ��̳��ַ www.zxkjmcu.com ******************************/
