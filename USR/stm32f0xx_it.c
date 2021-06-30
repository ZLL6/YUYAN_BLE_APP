/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "pbdata.h"
//extern uint8_t rs232_Buf_ry[500];
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	
}

void EXTI0_1_IRQHandler(void)
{
 
}

void EXTI2_3_IRQHandler(void)
{
 
}
void EXTI4_15_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5);
	} 
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line11);
	} 

}

//void USART1_IRQHandler(void)
//{

//}

void USART2_IRQHandler(void)
{

}

void USART3_4_IRQHandler(void)
{

}
void TIM1_IRQHandler(void)
{
//	static u8 num=0;

}
void TIM2_IRQHandler(void)
{
//	static u8 num=0;
 TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//先清空中断标志位，以备下次使用
//	num++;
//	   if(num%2)
//GPIO_ResetBits(GPIOx_W1_4,LED_W1);
//		 else
//			 GPIO_SetBits(GPIOx_W1_4,LED_W1);	
	socIntegralRoutine();
}

void TIM3_IRQHandler(void)
{	
	//	static u8 num=0;
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//先清空中断标志位，以备下次使用
	timer1++;	
	timer2++;	
	timer3++;	
	timer4++;	
	timer5++;	
	timer6++;	
	if(BAT_State != Sleep)
	{
		if(gBying)
			CAN_TX_BoyingMsgHandle();
		else
			CAN_TX_BcmuMsgHandle();
	}
//		num++;
//	   if(num%2)
//GPIO_ResetBits(GPIOx_W1_4,LED_W1);
//		 else
//			 GPIO_SetBits(GPIOx_W1_4,LED_W1);	
}
//void TIM7_IRQHandler(void)
//{
// TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//先清空中断标志位，以备下次使用
//}
void ADC1_COMP_IRQHandler(void)
{

}

void CEC_CAN_IRQHandler(void)
{
	CanRxMsg Message;
	CAN_Receive(CAN,CAN_FIFO0,&Message);
	CAN_Recv_Control(Message);
	//CAN_Recv_Control();
}
void RTC_IRQHandler(void)
{
		if (RTC_GetITStatus(RTC_IT_WUT) != RESET)
		{
		/* Clear RTC wake up interrupt pending bit */
		RTC_ClearITPendingBit(RTC_IT_WUT);
		/* Clear EXTI line20 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line20);
		}	
}
//低压掉电中断处理函数
void  PVD_IRQHandler(void)
{
   if(PWR_GetFlagStatus(PWR_FLAG_PVDO))
   {
//				BMS_config_Write(); //delay_ms(10);
//        BMS_Calibration_Write(); //delay_ms(10);
//        BMS_Alm_Number_Write(); 
//		    GPIO_ENABLE(DISABLE);	
//        BQ769X0_BalanceCtrl(0x00000000);
//        BQ769X0_Sleep();
		    BAT_State = Power_Dowm;	
   }
   EXTI_ClearITPendingBit(EXTI_Line16);
}
/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
