#include "pbdata.h"
#include "stm32f0xx_pwr.h"
void WKUP_NVIC_Config(void)
{
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
/* Configure EXTI line 20 (connected to the RTC wakeup event) */
EXTI_ClearITPendingBit(EXTI_Line20);
EXTI_InitStructure.EXTI_Line = EXTI_Line20;
EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
EXTI_InitStructure.EXTI_LineCmd = ENABLE;
EXTI_Init(&EXTI_InitStructure);
/* NVIC configuration */
NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
/* Enable peirodic wakeup interrupt */
RTC_ITConfig(RTC_IT_WUT, ENABLE); //
/* Clear the wakeup Pending Bit */
RTC_ClearITPendingBit(RTC_IT_WUT);
EXTI_ClearITPendingBit(0xFFFFFFFF);            //����ⲿ�жϱ�־
}
/***************************************************************************
*   �� �� ��: WKUP
*   ����˵��: WKUP��GPIO�ܽų�ʼ��
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void WKUP_GPIO(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	    RTC_WakeUpCmd(DISABLE); //RTC���ѹ��ܹر�
      RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);//???????
	    if((BMS_config.Pack_RW_config.gStaticResumeTimer>=20)||(BMS_config.Pack_RW_config.gStaticResumeTimer == 0))
		  BMS_config.Pack_RW_config.gStaticResumeTimer=5;	
			if(BMS_config.BMS_Read.gVoltMin<3650)
			 RTC_SetWakeUpCounter( 0x9c4*20); //����ʱ��	
			else
      RTC_SetWakeUpCounter( 0x9c4*BMS_config.Pack_RW_config.gStaticResumeTimer); //����ʱ��
			
      RTC_WakeUpCmd( ENABLE); //RTC���ѹ��ܹر�
	     WKUP_NVIC_Config();
	
		//��ʼ������					
	GPIO_InitStructure.GPIO_Pin =  WKUP_PIN;  //��ʼ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(WKUP_PORT, &GPIO_InitStructure);		
 //CAN�ļ����ź�
			if(CAN_enable==1)
			{
	GPIO_InitStructure.GPIO_Pin =  COMWK_PIN;  //CAN�ļ����ź�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(COMWK_PORT, &GPIO_InitStructure);	
			}if(CAN_enable==1)
			{
			EXTI_ClearITPendingBit(EXTI_Line11);
			}
	    EXTI_ClearITPendingBit(EXTI_Line5);
		  
			EXTI_InitStructure.EXTI_Line = EXTI_Line5;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
			if(CAN_enable==1)
			{
			EXTI_ClearITPendingBit(EXTI_Line11);
			EXTI_InitStructure.EXTI_Line = EXTI_Line11;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
			 	
			}
			
			/* NVIC configuration */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
			
	    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);      //EXIT�ж�
			if(CAN_enable==1)
	    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11);      //EXIT�ж�  	
			
     EXTI_ClearITPendingBit(0xFFFFFFFF);            //����ⲿ�жϱ�־	



	

}
/********RTC wake up interrupt NVIC ********/



void Sleep_Bat(void)
{ 
//	static  u8 link;
//__disable_irq();
//	 PWR_BackupAccessCmd(DISABLE);      
	SysTick->VAL=0X00;//��ռ�����
	SysTick->CTRL=0X00;//ʹ�ܣ����������޶����������ⲿʱ��Դ
	//EXTI_ClearITPendingBit(0xFFFFFFFF);            //����ⲿ�жϱ�־
//	PWR_DeInit();
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
//	IWDG_ReloadCounter();  //�忴�Ź�
//	 IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);

	WKUP_GPIO();//���ѹܽų�ʼ���� 

	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);//��Դ���߻��ߵ͹���ģʽ	
	
 //  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);	
//	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_SLEEPONEXIT);	
//PWR_EnterSTOPMode(PWR_CR_LPDS, PWR_STOPEntry_WFI);//��Դ���߻��ߵ͹���ģʽ	
	//PWR_EnterSleepMode(PWR_SLEEPEntry_WFE);

	
	

}

/************************ ��̳��ַ www.zxkjmcu.com ******************************/
