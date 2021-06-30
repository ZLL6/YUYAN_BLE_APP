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
EXTI_ClearITPendingBit(0xFFFFFFFF);            //清除外部中断标志
}
/***************************************************************************
*   函 数 名: WKUP
*   功能说明: WKUP的GPIO管脚初始化
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void WKUP_GPIO(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	    RTC_WakeUpCmd(DISABLE); //RTC唤醒功能关闭
      RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);//???????
	    if((BMS_config.Pack_RW_config.gStaticResumeTimer>=20)||(BMS_config.Pack_RW_config.gStaticResumeTimer == 0))
		  BMS_config.Pack_RW_config.gStaticResumeTimer=5;	
			if(BMS_config.BMS_Read.gVoltMin<3650)
			 RTC_SetWakeUpCounter( 0x9c4*20); //唤醒时间	
			else
      RTC_SetWakeUpCounter( 0x9c4*BMS_config.Pack_RW_config.gStaticResumeTimer); //唤醒时间
			
      RTC_WakeUpCmd( ENABLE); //RTC唤醒功能关闭
	     WKUP_NVIC_Config();
	
		//初始化按键					
	GPIO_InitStructure.GPIO_Pin =  WKUP_PIN;  //初始化按键
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(WKUP_PORT, &GPIO_InitStructure);		
 //CAN的激活信号
			if(CAN_enable==1)
			{
	GPIO_InitStructure.GPIO_Pin =  COMWK_PIN;  //CAN的激活信号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*设置引脚模式为通用推挽输出*/
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
			
	    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);      //EXIT中断
			if(CAN_enable==1)
	    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource11);      //EXIT中断  	
			
     EXTI_ClearITPendingBit(0xFFFFFFFF);            //清除外部中断标志	



	

}
/********RTC wake up interrupt NVIC ********/



void Sleep_Bat(void)
{ 
//	static  u8 link;
//__disable_irq();
//	 PWR_BackupAccessCmd(DISABLE);      
	SysTick->VAL=0X00;//清空计数器
	SysTick->CTRL=0X00;//使能，减到零是无动作，采用外部时钟源
	//EXTI_ClearITPendingBit(0xFFFFFFFF);            //清除外部中断标志
//	PWR_DeInit();
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
//	IWDG_ReloadCounter();  //清看门狗
//	 IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);

	WKUP_GPIO();//唤醒管脚初始化函 

	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);//电源休眠或者低功耗模式	
	
 //  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);	
//	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_SLEEPONEXIT);	
//PWR_EnterSTOPMode(PWR_CR_LPDS, PWR_STOPEntry_WFI);//电源休眠或者低功耗模式	
	//PWR_EnterSleepMode(PWR_SLEEPEntry_WFE);

	
	

}

/************************ 论坛地址 www.zxkjmcu.com ******************************/
