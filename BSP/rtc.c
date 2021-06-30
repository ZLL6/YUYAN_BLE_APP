#include "pbdata.h"

/* ���� ----------------------------------------------------------------------*/
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

#define RTC_CLOCK_SOURCE_LSI                     //ʹ��LSIʱ��
#define RTC_FLAG_BKP              0xA55A         //��־

/************************************************
�������� �� RTC_Configuration
��    �� �� RTC����
��    �� �� ��
�� �� ֵ �� ��
��    �� �� MCD Application Team
*************************************************/
void RTC_Configuration(void)
{
/* 1��ʹ��LSIʱ�� */
#if defined (RTC_CLOCK_SOURCE_LSI)

  RCC_LSICmd(ENABLE);                            //ʹ��LSIʱ��

  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {                                              //�ȴ�LSI����
  }

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);        //ѡ��LSIʱ��

  SynchPrediv = 0x18F;
  AsynchPrediv = 0x63;

/* 2��ʹ��LSEʱ�� */
#elif defined (RTC_CLOCK_SOURCE_LSE)

  RCC_LSEConfig(RCC_LSE_ON);                     //ʹ��LSIʱ��

  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {                                              //�ȴ�LSI����
  }

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);        //ѡ��LSIʱ��
  
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#else
  #error Please select the RTC Clock source inside the main.c file
#endif

  RCC_RTCCLKCmd(ENABLE);                         //ʹ��RTC
  RTC_WaitForSynchro();                          //�ȴ�ͬ��
}

/************************************************
�������� �� RTC_Initializes
��    �� �� RTC��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void RTC_Initializes(void)
{
  RTC_InitTypeDef RTC_InitStructure;
  RTC_DateTimeTypeDef RTC_DateTimeStructure;

  PWR_BackupAccessCmd(ENABLE);                   //�������RTC

  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != RTC_FLAG_BKP)
  {
    RTC_Configuration();                         //����RTC

    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    if(RTC_Init(&RTC_InitStructure) == ERROR)
    {
      while(1);          //��ʼ��ʧ��
    }

    RTC_DateTimeStructure.Hour = 23;
    RTC_DateTimeStructure.Minute = 59;
    RTC_DateTimeStructure.Second = 59;
    RTC_SetDateTime(RTC_DateTimeStructure);
    RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_FLAG_BKP);
  }
  else
  {
 #ifdef RTC_CLOCK_SOURCE_LSI
    RCC_LSICmd(ENABLE);
 #endif

    RTC_WaitForSynchro();  //�ȴ�RTC��RTC_APBʱ��ͬ��
  }
	
}

/************************************************
�������� �� RTC_DateRegulate
��    �� �� RTC���ڽ���
��    �� �� RTC_DateTimeStructure -- RTC�ṹ��
�� �� ֵ �� RTC_OK ----------------- �ɹ�
            RTC_ERR ---------------- ����
��    �� �� Huang Fugui
*************************************************/
RTC_RESULT RTC_DateRegulate(RTC_DateTimeTypeDef RTC_DateTimeStructure)
{
  RTC_DateTypeDef RTC_DateStructure;

  if(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure) == SUCCESS)
  {
    return RTC_OK;
  }
  else
  {
    return RTC_ERR;
  }
}

/************************************************
�������� �� RTC_TimeRegulate
��    �� �� RTCʱ�����
��    �� �� RTC_DateTimeStructure -- RTC�ṹ��
�� �� ֵ �� RTC_OK ----------------- �ɹ�
            RTC_ERR ---------------- ����
��    �� �� Huang Fugui
*************************************************/
RTC_RESULT RTC_TimeRegulate(RTC_DateTimeTypeDef RTC_DateTimeStructure)
{
  RTC_TimeTypeDef RTC_TimeStructure;

  RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours = RTC_DateTimeStructure.Hour;
  RTC_TimeStructure.RTC_Minutes = RTC_DateTimeStructure.Minute;
  RTC_TimeStructure.RTC_Seconds = RTC_DateTimeStructure.Second;

  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == SUCCESS)
  {
    return RTC_OK;
  }
  else
  {
    return RTC_ERR;
  }
}

/************************************************
�������� �� RTC_SetDateTime
��    �� �� RTC����ʱ������
��    �� �� RTC_DateTimeStructure -- RTC�ṹ��
�� �� ֵ �� RTC_OK ----------------- �ɹ�
            RTC_ERR ---------------- ����
��    �� �� Huang Fugui
*************************************************/
RTC_RESULT RTC_SetDateTime(RTC_DateTimeTypeDef RTC_DateTimeStructure)
{
  if(RTC_ERR == RTC_DateRegulate(RTC_DateTimeStructure))
  {
    return RTC_ERR;
  }

  if(RTC_ERR == RTC_TimeRegulate(RTC_DateTimeStructure))
  {
    return RTC_ERR;
  }

  return RTC_OK;
}

/************************************************
�������� �� RTC_GetDateTime
��    �� �� ��ȡRTC����ʱ�䣨�����������ڣ�
��    �� �� RTC_DateTimeStructure -- RTC�ṹ��
�� �� ֵ �� ��
��    �� �� Huang Fugui
*************************************************/
void RTC_GetDateTime(RTC_DateTimeTypeDef *RTC_DateTimeStructure)
{
  RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef     RTC_DateTypeDef_N0W;

  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  RTC_DateTimeStructure->Hour = RTC_TimeStructure.RTC_Hours;
  RTC_DateTimeStructure->Minute = RTC_TimeStructure.RTC_Minutes;
  RTC_DateTimeStructure->Second = RTC_TimeStructure.RTC_Seconds;
	
	RTC_GetDate(RTC_Format_BIN, &RTC_DateTypeDef_N0W);
	RTC_DateTimeStructure->Year = RTC_DateTypeDef_N0W.RTC_Year;
	RTC_DateTimeStructure->Month = RTC_DateTypeDef_N0W.RTC_Month;
	RTC_DateTimeStructure->Date = RTC_DateTypeDef_N0W.RTC_Date;	
}

//void RTC_Demo(void)
//{
//	RTC_DateTimeTypeDef RTC_DateTimeStructure;

//	RTC_GetDateTime(&RTC_DateTimeStructure);   //��ȡ��ǰʱ��		
////	printf("ʱ�䣺%0.2d:%0.2d:%0.2d\r\n",
////	RTC_DateTimeStructure.Hour,
////	RTC_DateTimeStructure.Minute,
////	RTC_DateTimeStructure.Second);
////	delay_ms(1000);
//}

/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/
