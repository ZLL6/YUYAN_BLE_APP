#include "WWDG.h"
/********************************************************
			���ڿ��Ź�ԭ��
			WWDG_CR  ��WDG_ReloadValue �� 0x40 ��ȫ�ڣ�0x40 -0x3F  ���¸�λ
			WWDG_CFR ��0x40 �� WWDG_CFR���ڼ䣬��ȫι�����������¸�λ
			��������������
*******************************************************/
//void WWDG_Config(void)
//{
//	 /* Enable WWDG clock */

//  /* WWDG clock counter = (PCLK1(10MHz)/4096)/8 = 305.18Hz (~3.28ms)  */
//  WWDG_SetPrescaler(WWDG_Prescaler_8);

//  /* Set Window value to 80; WWDG counter should be refreshed only when the counter
//    is below 80 (and greater than 64) otherwise a reset will be generated */
//  WWDG_SetWindowValue(WDG_ReloadValue);  //��������ֵ

//  /* Enable WWDG and set counter value to 127, WWDG timeout = ~683 us * 64 = 43.7 ms 
//     In this case the refresh window is: ~3.28 * (100-100)= 0ms < refresh window < ~3.28 * 36 = 118.08ms
//     */
//  WWDG_Enable(WDG_ReloadValue);   //���ڿ��Ź�����ֵ
//}
//=============================================================================================
//??????
//=============================================================================================

//=============================================================================================
//???? void IWDG_Configuration(void)
//???? 
//???? 
//???? 
//?	?   :??????????
//?	?  
//=============================================================================================
void WWDG_Config(void)
{
//??0x5555,?????????????
 IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	
 
//??????,40K/256=156HZ(6.4ms)
 IWDG_SetPrescaler(IWDG_Prescaler_256);
 
//???? 5s/6.4MS=781 .??????0xfff
	//10/6.4 =1560
	//22/6.4 =3437
//	24/6.4 = 3750
 IWDG_SetReload(0xEA6);
 
//??
 IWDG_ReloadCounter();
 
//????
 IWDG_Enable();
}
