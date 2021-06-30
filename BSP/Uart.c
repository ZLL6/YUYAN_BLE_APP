//unsigned char
#include "pbdata.h"
#include "data.h"
#include "core_cmFunc.h"
#include "stdio.h"
#include "stdlib.h"
#include "Uart.h"
//#include "Can.h"
#include <string.h>
//#include "stm32f0xx_can.h"
#include "Flash.h"
#include "stdbool.h"
#define   quCurrent(x)    (x<0x80000000?x:(~(x-1)))
// USART1
unsigned	char 	UART_frame_flg=0; 				//�յ�����֡�������־
unsigned 	char 	UART_head_flg=0;				//ʶ����Ч֡ͷ��־
unsigned 	char 	UART_index=0;					//������������ű�
unsigned 	char 	UART_Wate_byte=0x55;			//�ȴ����յ��ַ���ÿ֡�׸��ַ���0x55��
bool 				Is_UsartSend = false;			//���ڶ�ȡ��ʷ���ݱ�־
unsigned  	int 	flame_cmd;						//֡����
unsigned  	int 	UART_Data_addr=0;				//֡��Ч������ʼ��ַ
unsigned  	int 	Devide_addr=0;					//?֡��Ч������ʼ��ַ
unsigned  	char 	UART_Data_RW=0;					//֡��Ч������ʼ��ַ
extern unsigned int APP_FLG_ADDR;//APP��־λ���λ��
extern RTC_DateTimeTypeDef RTC_DateTimeStructure_NEW;
RTC_RESULT RTC_OK_RESULT;
extern RTC_DateTypeDef     RTC_DateTypeDef_NEW;
//extern static s32 Current_data;
//unsigned  char RE_from_seir[250];					//?????????????;

unsigned  char USART_SEND_data[224];				/// UART��Ч����֡����

unsigned  int UART_CRC_RE;							//�յ���У����
unsigned  int UART_CRC_CA;							//�����յ������ݼ��������У����
unsigned  char USART_RV_Data_long=0;				//֡��Ч���ݳ��ȣ�
volatile unsigned  char UART_sending_flg;
unsigned  char USART_SEND_LONG;
unsigned  char  USART_send_cnt;

static unsigned char ReadUARTData;					//��ȡ�������ݼĴ���


unsigned  char Usart_Frame_Buffer[224];				//UART��Ч����֡����
unsigned char  UsartReturn_Buffer[224];  			//����ת��CAN �����ݻ���
	
 //================================================================
extern unsigned char    UsartCAN_frame_flg; 		//�յ�����֡�������־ 
extern unsigned  char   UartSendWait;	
extern unsigned  char	CanReturn_Buffer[224];
extern unsigned  char   CAN_Send_long;
//extern uint32_t FLASH_WR_ADDR;
unsigned int  FLASH_WR_ADDR;
unsigned char FLASH_Program_finish_flg=0;
unsigned char FLASH_Erase_finish_flg=0;


/////////////////???????????
//int i;
//int len= strlen(RE_from_seir);??????????

//======================================================

/*union
{
	unsigned  char tmpBytes[2U];
	unsigned   int tmpWord;
}un_ByteWord;
*/

//static unsigned char fg_Usart1Busying;
//static unsigned  char Usart1BusyTime;



//=CRC ??==================================================================
#define CRC_CCITT_INIT 0xFFFF
#define CRC_CCITT_POLY 0x1021U
uint16_t Uart_CCITT_CRC16;
void CCITT_CRCStep(uint8_t byte)
{
	uint32_t j;
	Uart_CCITT_CRC16 ^= ((uint16_t)byte << 8); 
	for ( j = 0; j < 8; j++ )
	{
		Uart_CCITT_CRC16 = (Uart_CCITT_CRC16 & 0x8000U)?((Uart_CCITT_CRC16 << 1) ^ CRC_CCITT_POLY):(Uart_CCITT_CRC16 << 1);
	}
}
void CCITT_CRC_ARRAY(uint8_t const * bytes, uint16_t len)
{
	while (len--) CCITT_CRCStep(*bytes++);
}

void UART_CCITT_CRC16Init(uint8_t const * bytes, uint16_t len) 
{
	Uart_CCITT_CRC16 = CRC_CCITT_INIT; 
	CCITT_CRC_ARRAY(bytes,len);
}

//==============================================================
//CAN���淢�͵�����
//==============================================================
/*void CANToUsart_Process(void)
{
			if(UartSendWait==1)    
			{
				USART_Send_string(CanReturn_Buffer,CAN_Send_long);//���ݻ��棬���ͳ���
				UartSendWait=0;
			}

}
*/
//===================================================================
void Uart_Init(uint32_t BaudRate)  
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1); 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);   //��ʼ��UART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	USART_InitStructure.USART_BaudRate = BaudRate;   //���ڲ�����ʼ��
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	USART_ClearFlag(USART1,0XFFFFFFFF);
	USART_Init(USART1, &USART_InitStructure); //���ô����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);


	USART_OverrunDetectionConfig(USART1,USART_OVRDetection_Disable); //������ر�
	USART_ClearFlag(USART1,0XFFFFFFFF);

	USART_Cmd(USART1, ENABLE);   //��������ʹ��

	UART_index=0;//?????0
	UART_Wate_byte =0x55;//??????????		
	UART_frame_flg=0;
	UART_head_flg=0;
}
	
//========================================================================================
 //�Ѵ��ڽ��յ�������֡���ص�CAN�����м仺��	
//========================================================================================	
void USART_FRAME_PROCESS(void)	
{
	unsigned char temp_cmd = 2;
	unsigned short temp_addr;  
	unsigned char temp_Data_long;
	FLASH_Status flash_Operate_St;
	unsigned char flash_Operate_cnt=0;
	unsigned char temp_i;
	unsigned int temp_app_addr;
	unsigned int temp_data;
	unsigned char DDword_num;
	static s32 Current_data = 0 ;
	//////////////////////////////
	u8 TX[400];
	u8 BUWEI[34];
	u16 temp1 = 0;
	u16 temp2 = 0;
	u16 i = 0;
	static u16 lin=0;
	static	s32 Current1 = 0 ;
	static	s32 Current2 = 0 ;
	static s16 Volt = 0 ;
	/////////////////////////////////
	if(UART_frame_flg==1)  //���ڻ�����֡������ɱ�־��������
	{
					
		UART_CCITT_CRC16Init( (uint8_t const *)Usart_Frame_Buffer, (4+USART_RV_Data_long) );								
		UART_CRC_CA= Uart_CCITT_CRC16;				
		//�����յ��������Լ�����У����UART_CRC_CA
		if(UART_CRC_RE==UART_CRC_CA)//У������ȷ����֡������ɱ�־��֪ͨ������һ֡����,���������ݺ����Ҫ��flame_flg=0;UART_Wate_byte=0x55;//head_flg=0//index = 0;//�ű��0
		{
			g_Can_falg = 1;	
			temp_cmd = Usart_Frame_Buffer[0]; 	 
			temp_addr = Usart_Frame_Buffer[2];
			temp_addr = (temp_addr<<8)+Usart_Frame_Buffer[1];
			temp_Data_long=Usart_Frame_Buffer[3];
			/*У����ȷ*/
			/*���ݲ�ͬ�������flash���в���*/
			/*USART_Send_status(USART_SUCCESS)OR USART_Send_status(USART_Operate_ERR);*/
			//���ز����ɹ�����ʧ��֡
			if(temp_addr==0x8000)//��λ���������bootloader
			{
			
				FLASH_Status flash_Operate_St;
				BMS_config_Write();//delay_ms(10);
				BMS_Calibration_Write();//delay_ms(10);
				BMS_Alm_Number_Write();
				delay_ms(10);
				FLASH_Unlock();//����
				flash_Operate_St = 	FLASH_ErasePage(APP_FLG_ADDR);
				FLASH_Lock();//��������	
				if(flash_Operate_St==FLASH_COMPLETE)  
				{
					USART_Send_status(USART_SUCCESS);//enter bootloader success
					delay_ms(10);
					NVIC_SystemReset();	 //��λоƬ	
				}		
				else
				{
					USART_Send_status(USART_Operate_ERR);
				//						FLASH_Unlock();//����
				//				    flash_Operate_St = 	FLASH_ErasePage(APP_FLG_ADDR);
				//					  FLASH_Lock();//��������	
				//						if(flash_Operate_St==FLASH_COMPLETE) 
				//						NVIC_SystemReset();	 //��λоƬ	

				}
				//USART_Send_status(USART_SUCCESS);//enter bootloader success
			}
			if(temp_cmd==0)  //����ָ���
			{
				if((temp_addr >= 0x4000) && (temp_addr <= 0x4ffe))
				{					
					tAlmPrtEvtHead_R.ReadEvent_loop = (temp_addr&0x3fff);
					BMS_Alm_Number_Write();
					Is_UsartSend=true;					//ʹ���������ڷ���
					ReadEventRecord();					
				}
				else if((temp_addr >= 0x2000) && ((temp_addr + (temp_Data_long/2)) <= 0x20BC))   //���ݴ�����
				{

					temp1 =	sizeof(BMS_config);	
					temp2 =	sizeof(Alm_Number);	
					memcpy(TX,&BMS_config,temp1);  
					memcpy(BUWEI,&Alm_Number,temp2);  

					for(i = 0;i < (temp2);i++)
					{
						TX[temp1+i-2] =BUWEI[i];
					}							
					lin = (temp_addr-0x2000)*2;	
					//							timer6 =0;
					//							 while(CAN_TX_Falg)
					//							 {
					//								 if(timer6>35)
					//								 break;
					//							 }
					//char temp_i=0;
					UsartReturn_Buffer[temp_i++]=0x55;
					UsartReturn_Buffer[temp_i++]=0xAA;	
					UsartReturn_Buffer[temp_i++]=0X00;
					UsartReturn_Buffer[temp_i++]=0X00;
					UsartReturn_Buffer[temp_i++]=0XAA;
					UsartReturn_Buffer[temp_i++]=0X55;
					UsartReturn_Buffer[temp_i++]=1;
					UsartReturn_Buffer[temp_i++]=(char)temp_addr;
					UsartReturn_Buffer[temp_i++]=(char)(temp_addr>>8);
					UsartReturn_Buffer[temp_i++]=temp_Data_long;//���ݳ��ȹ̶�4
					for(i = 0;i < (temp_Data_long);i++)
					{
						UsartReturn_Buffer[temp_i++] =TX[i+lin];
					}	
					UART_CCITT_CRC16Init(&UsartReturn_Buffer[6],(temp_Data_long+4)); 
					UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
					UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
					USART_Send_string(UsartReturn_Buffer,(temp_Data_long+12));
				}
				else if((temp_addr >= 0x2100) && ((temp_addr + (temp_Data_long/2)) <=0x2170)) 
				{
					memcpy(TX,&BMSVoltTemperature,sizeof(BMSVoltTemperature)); 
					lin = (temp_addr-0x2100)*2;
					//char temp_i=0;
					UsartReturn_Buffer[temp_i++]=0x55;
					UsartReturn_Buffer[temp_i++]=0xAA;	
					UsartReturn_Buffer[temp_i++]=0X00;
					UsartReturn_Buffer[temp_i++]=0X00;
					UsartReturn_Buffer[temp_i++]=0XAA;
					UsartReturn_Buffer[temp_i++]=0X55;
					UsartReturn_Buffer[temp_i++]=1;
					UsartReturn_Buffer[temp_i++]=(char)temp_addr;
					UsartReturn_Buffer[temp_i++]=(char)(temp_addr>>8);
					UsartReturn_Buffer[temp_i++]=temp_Data_long;//���ݳ��ȹ̶�4
					for(i = 0;i < (temp_Data_long);i++)
					{
						UsartReturn_Buffer[temp_i++] =TX[i+lin];
					}	
					UART_CCITT_CRC16Init(&UsartReturn_Buffer[6],(temp_Data_long+4)); 
					UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
					UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
					USART_Send_string(UsartReturn_Buffer,(temp_Data_long+12));
				}
				else if(temp_addr == 0x5000) //��ֵ�洢
				{

					memcpy(TX,&Extremum,sizeof(Extremum));
					//char temp_i=0;
					UsartReturn_Buffer[temp_i++]=0x55;
					UsartReturn_Buffer[temp_i++]=0xAA;	
					UsartReturn_Buffer[temp_i++]=0X00;
					UsartReturn_Buffer[temp_i++]=0X00;
					UsartReturn_Buffer[temp_i++]=0XAA;
					UsartReturn_Buffer[temp_i++]=0X55;
					UsartReturn_Buffer[temp_i++]=1;
					UsartReturn_Buffer[temp_i++]=(char)temp_addr;
					UsartReturn_Buffer[temp_i++]=(char)(temp_addr>>8);
					UsartReturn_Buffer[temp_i++]=temp_Data_long;//���ݳ��ȹ̶�4
					for(i = 0;i < (temp_Data_long);i++)
					{
						UsartReturn_Buffer[temp_i++] =TX[i];
					}	
					UART_CCITT_CRC16Init(&UsartReturn_Buffer[6],(temp_Data_long+4)); 
					UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
					UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
					USART_Send_string(UsartReturn_Buffer,(temp_Data_long+12));				
				}
				else
				{
				   USART_Send_status(USART_Operate_ERR);//����Ĺ�����
				}	
			}
			if(temp_cmd==1)  //дָ���	
			{
				if((temp_addr >= 0x4000) && (temp_addr <= 0x4ffe))
				  {
							ReadEvent_enable=0;//��ֹ�ٵ��ô˺���
							tAlmPrtEvtHead_R.ReadEvent_Num = 0;
							tAlmPrtEvtHead_R.ReadEventAddr_st=0;
					}
				else if((temp_addr >= 0x2021) && ((temp_addr + (temp_Data_long/2)) <=0x20BC))   //���ݴ�����
						{
				        memcpy(TX,&BMS_config,sizeof(BMS_config));  
				        lin = (temp_addr-0x2000)*2;
							 
								for(i = 0;i < (temp_Data_long);i++)
								{
									TX[i+lin] =Usart_Frame_Buffer[i+4];
								}	
								memcpy(&BMS_config,TX,sizeof(BMS_config)); 
								BMS_config.Pack_RW_config.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSOC*BMS_config.Pack_RW_config.gNewPackCapacity/100;
								BMS_capacity.BatCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity*360000;  					
							  BMS_config.Pack_RW_config.gNewPackCapacity	= BMS_config.Pack_RW_config.gSOH * BMS_config.Pack_RW_config.gSetPackCapacity/100;				
								BMS_config.Pack_RW_config.gsCycleCnt =	BMS_config.Pack_RW_config.gCHGPackCapacity*1000/( BMS_config.Pack_RW_config.gSetPackCapacity*8);								
								BMS_config_Write();//д���ò���
								delay_ms(20);
								BQ769X0_ConfigPara(0);
			
						}	
				 else if(temp_addr == 0x2303)
						{
              tAlmPrtEvtHead_W.WriteEventAddr  = 0;
							tEvtDataW.index = 0 ;
							tAlmPrtEvtHead_W.EventNum = 0;
	            tAlmPrtEvtHead_R.WriteEventAddr  = 0;
							tAlmPrtEvtHead_R.EventNum = 0;						
              BMS_Alm_Number_Write();
						}	
         else if(temp_addr == 0x2304)
						{
							Current_data =Usart_Frame_Buffer[4]+(u16)(Usart_Frame_Buffer[5]<<8);
							if(Current_f<0)
             	return ;
							Calibration.gChgCurrent_double  = (double)Current_data*10.0f/Current_f;
							BMS_Calibration_Write();
						}	
				 else if(temp_addr == 0x2305)
						{
							Current_data =(s32)(Usart_Frame_Buffer[4]+(s16)(Usart_Frame_Buffer[5]<<8));
							Current1 = quCurrent((u32)Current_data) ;
							Current2  = quCurrent((u32)Current_f);
							if(Current_f>0)
             	return;
							Calibration.gDchgCurrent_double  = (double)Current1*10.0/Current2;
							BMS_Calibration_Write();
						}	
				 else if(temp_addr == 0x2306)
						{
							Volt =Usart_Frame_Buffer[4]+(u16)(Usart_Frame_Buffer[5]<<8);
									if((Volt>=3700)&&(Volt<4500))
									{
											if(Usart_Frame_Buffer[6]==0)
											{
												for(i = 0;i < BMS_config.Pack_RW_config.gBatNumber;i++)
												Calibration.g_Volt_double_max[i] = (double)Volt / Volt_call[i];						
											}
											else{
												Calibration.g_Volt_double_max[Usart_Frame_Buffer[6]-1] = (double)Volt / Volt_call[Usart_Frame_Buffer[6]-1];
											}
											BMS_Calibration_Write();
									}
						}	
         else if(temp_addr == 0x2308)
						{
							Volt =Usart_Frame_Buffer[4]+(u16)(Usart_Frame_Buffer[5]<<8);
									if((Volt>2500)&&(Volt<=3700))
									{
												if(Usart_Frame_Buffer[6]==0)
												{
													for(i = 0;i < BMS_config.Pack_RW_config.gBatNumber;i++)
													Calibration.g_Volt_double_min[i] = (double)Volt / Volt_call[i];						
												}
												else{
													Calibration.g_Volt_double_min[Usart_Frame_Buffer[6]-1] = (double)Volt / Volt_call[Usart_Frame_Buffer[6]-1];
												}
													BMS_Calibration_Write();
									}
						}
						else if(temp_addr == 0x230A)    //�¶�ϵ��У׼
						{
						      if ((Usart_Frame_Buffer[5]<0x80)&&((u8)Temperature_call [Usart_Frame_Buffer[5]] < 0x80))
						        {
											if(Usart_Frame_Buffer[4]==0)
											{
												for(i = 0;i < BMS_config.Pack_RW_config.gTemNumber;i++)
												Calibration.gTem_double[Usart_Frame_Buffer[i]]= (double)Usart_Frame_Buffer[5] /Temperature_call[Usart_Frame_Buffer[i]]	;						
											}
										else{
												Calibration.gTem_double[Usart_Frame_Buffer[4]-1]= (double)Usart_Frame_Buffer[5] /Temperature_call[Usart_Frame_Buffer[4]-1]	;	
											}		
										}			
                   BMS_Calibration_Write();
						}
						else if(temp_addr == 0x230B)//������ϼ�¼������ʧЧ
						{
						 	if(temp_Data_long == 0 )
							{
								gBit_BatHighTemFailure = 0;            //��ظ���ʧЧ
					      gBit_BatlowTemFailure	= 0;								//��ص���ʧЧ 
                gBit_BatOverVoltFailure = 0;							//��ص�ع�ѹ��������ʧЧ
                gBit_BatUnderVoltFailure	= 0;						//��ص��Ƿѹ��������ʧЧ
                gBit_BatVoltDiffMaxFailure	= 0;						//���ѹ�������ʧЧ
								Alm_Number.Failure = 0x00;
								memset(&Alm_Number,0,sizeof(Alm_Number));
								BMS_Alm_Number_Write(); 
							}
							
						}
			     else if(temp_addr == 0x230D)
						{
						  gSOC_Balance_Pc = 1;							
              gBalance_pc= Usart_Frame_Buffer[4]|(Usart_Frame_Buffer[5]<<8);
							if(gBalance_pc == 0)
              gSOC_Balance_Pc = 0;	
  						BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	
						}
					else if(temp_addr == 0x2300)
						{
								RTC_DateTimeStructure_NEW.Second = 	Usart_Frame_Buffer[9];			
                RTC_DateTimeStructure_NEW.Minute =  Usart_Frame_Buffer[8];
                RTC_DateTimeStructure_NEW.Hour   =  Usart_Frame_Buffer[7];
                RTC_DateTimeStructure_NEW.Date   =  Usart_Frame_Buffer[6];
                RTC_DateTimeStructure_NEW.Month  =  Usart_Frame_Buffer[5];
                RTC_DateTimeStructure_NEW.Year   = 	Usart_Frame_Buffer[4];	
      			  	RTC_OK_RESULT =		RTC_SetDateTime(RTC_DateTimeStructure_NEW);	
							if(RTC_OK ==RTC_OK_RESULT)
							{
							   RTC_DateTypeDef_NEW.RTC_Year = RTC_DateTimeStructure_NEW.Year;
							   RTC_DateTypeDef_NEW.RTC_Month = RTC_DateTimeStructure_NEW.Month;
							   RTC_DateTypeDef_NEW.RTC_Date =  RTC_DateTimeStructure_NEW.Date;							
							   RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeDef_NEW);
							}
	              
						}
						else if(temp_addr == 0x230C)
						{
							if(Usart_Frame_Buffer[4] == 0xff)
							{
						   GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
		           GPIO_SetBits(GPIOx_W5_6,LED_W6);	 //�Էŵ����
			         g_FaultAlarmState[6]&= ~(1<<3);	
							 gDchg_falg_PC = 0;
							 BAT_State = FreeMode;  //��������ģʽ
							}
							if(Usart_Frame_Buffer[4] == 0x00)
							{
								gDchg_falg_PC = 1;	
								GPIO_SetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
			          GPIO_ResetBits(GPIOx_W5_6,LED_W6);	//���ȵ�
			          g_FaultAlarmState[6]|= 1<<3;
								
							}								
						}
						else if(temp_addr == 0x2310)
						{
									if(Usart_Frame_Buffer[4] == 0xff)
									{

									 BAT_State = Sleep;//ϵͳ�ı䵽����ģʽ
										gSleep_pc = 1;
									}
						}
						else if(temp_addr == 0x2311)
						{
									if(Usart_Frame_Buffer[4] == 0x55)
									gBitStorageFailure =	W25QXX_Demo();
							
						}
						else
						{
							;
							
						} 
//         if((CAN_R[2]==0)|(CAN_R[2]==1))	
//			{
////				if(CAN_TX_Falg == 1)
////				return 0;	
//				timer6 =0;
//					 while(CAN_TX_Falg)
//					 {
//						 if(timer6>35)
//						 break;
//					 }
//				CAN_T[2]=  0x01;
//				CAN_T[3]= CAN_R[3];
//				CAN_T[4]= CAN_R[4];
//				CAN_T[5]= dataLen;
				temp_i=0;
			  UsartReturn_Buffer[temp_i++]=0x55;
				UsartReturn_Buffer[temp_i++]=0xAA;	
				UsartReturn_Buffer[temp_i++]=0X00;
				UsartReturn_Buffer[temp_i++]=0X00;
				UsartReturn_Buffer[temp_i++]=0XAA;
				UsartReturn_Buffer[temp_i++]=0X55;
				UsartReturn_Buffer[temp_i++]=1;
				UsartReturn_Buffer[temp_i++]=(char)temp_addr;
				UsartReturn_Buffer[temp_i++]=(char)(temp_addr>>8);
				UsartReturn_Buffer[temp_i++]=1;//���ݳ��ȹ̶�1
				UsartReturn_Buffer[temp_i++]=1;//
				UART_CCITT_CRC16Init(&UsartReturn_Buffer[6],5); 
				UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
				UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
				if(~gBitStorageFailure )	
				{
				 USART_Send_string(UsartReturn_Buffer,(13));
				}
				 BMS_capacity.PackNewCapacity =(u32)(BMS_config.Pack_RW_config.gNewPackCapacity)*360000;//����������	
			   if(BMS_config.Pack_RW_config.gSOC!=0)  //������������
				 BMS_capacity.BatCapacity = BMS_capacity.PackNewCapacity/100* BMS_config.Pack_RW_config.gSOC;
				 else
				 BMS_capacity.BatCapacity = 0;	
			 
			} 						
		//}
	
		/*CANSendBuffer_long = 4+USART_RV_Data_long+2;
		UsartReturn_Buffer[0]=Usart_Frame_Buffer[4+USART_RV_Data_long];	
		UsartReturn_Buffer[1]=Usart_Frame_Buffer[4+USART_RV_Data_long+1];	

		for(i=0;i<CANSendBuffer_long-2;i++)//�Ѵ��ڽ��յ������ݷ���CAN�����м仺��
		{	
			UsartReturn_Buffer[i+2]=Usart_Frame_Buffer[i];													

		}			
		UsartCAN_frame_flg =1; //֪ͨCAN���Է�������
		UART_index=0;//?????0
		UART_Wate_byte =0x55;//??????????		
		UART_head_flg=0;		
		UART_frame_flg = 0; // ���´򿪽�������
			  */
		}
		else /*У��ʧ��*/
		{ 
			/*����У�����֡*/
			USART_Send_status(USART_CRC_ERR);
		}
		UART_index=0;//?????0
		UART_Wate_byte =0x55;//??????????		
		UART_frame_flg=0;
		UART_head_flg=0;
	}
}
/*�����յ�������״̬�𸴺���*/
void USART_Send_status(uint8_t status)
{
	char temp_i=0;
	UsartReturn_Buffer[temp_i++]=0x55;
	UsartReturn_Buffer[temp_i++]=0xAA;	
	UsartReturn_Buffer[temp_i++]=0X00;
	UsartReturn_Buffer[temp_i++]=0X00;
	UsartReturn_Buffer[temp_i++]=0XAA;
	UsartReturn_Buffer[temp_i++]=0X55;
	UsartReturn_Buffer[temp_i++]=UART_Data_RW;
	UsartReturn_Buffer[temp_i++]=(char)UART_Data_addr;
	UsartReturn_Buffer[temp_i++]=(char)(UART_Data_addr>>8);
	UsartReturn_Buffer[temp_i++]=1;//���ݳ��ȹ̶�1
	UsartReturn_Buffer[temp_i++]=status;
	UART_CCITT_CRC16Init(&UsartReturn_Buffer[6], 5); 
	UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
	UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
	USART_Send_string(UsartReturn_Buffer,13);
 
}

////////////////////////////////
/*���ڷ����ַ���*/
///////////////////////////////////////////////////////
void USART_Send_string(uint8_t *addr,uint8_t USART_RV_Data_long)
{
	uint8_t tem_i;
	while(UART_sending_flg);
	USART_SEND_LONG=USART_RV_Data_long;
	for(tem_i=0;tem_i<USART_SEND_LONG;tem_i++)//����Ҫ���͵�����װ�ص����ͻ���
	{
		USART_SEND_data[tem_i] = addr[tem_i];
	}
	USART_ClearFlag(USART1, USART_IT_TC);
	USART_SendData(USART1,USART_SEND_data[USART_send_cnt++]);//���͵�һ���ַ�
	UART_sending_flg =1;
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);//ʹ�ܷ�������жϣ��������ֽڷ������ж������
}	

//=====================================================================================
void USART1_IRQHandler(void)//�����жϺ���
{
	
	if(UART_sending_flg)  //UART���ڷ���������
	{ 
		if(USART_GetITStatus(USART1,USART_IT_TC) != RESET)//��������ж�
		{
			USART_ClearFlag(USART1, USART_IT_TC);//���жϱ�־λ
			if(USART_send_cnt==USART_SEND_LONG)//���ݷ������
			{
				UART_sending_flg=0;
				USART_send_cnt=0;
				USART_ITConfig(USART1, USART_IT_TC, DISABLE);//�رմ��ڷ�������ж�
			}
			else//��������û����
			{
				USART_SendData(USART1,USART_SEND_data[USART_send_cnt++]);//����װ����һ����
			}
		}
	}
//===============��ȡ����================================================================================	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)//�ַ���������ж�
	{
		ReadUARTData =USART_ReceiveData(USART1);	//������3�մ��ڽ��ջ����ڻ��ÿһ���ַ�
						
		if(UART_frame_flg==1)//�Ѿ��н�����ɵ�֡δ����
		{
			UART_Wate_byte =0x55;//??????????		
		}
		else
		{

			if(UART_index>224)  //  �������224���ֽڣ� ���ڽ������ݳ��ȹ����أ�������0
			{
				UART_index=0;//����ű��0
				UART_Wate_byte =0x55;//���µȴ�֡ͷ�׸��ַ�
				UART_frame_flg=0;
				UART_head_flg=0;
			} 
			if(UART_head_flg==0)//��δ��׽������֡ͷ
			{
				if(UART_Wate_byte !=ReadUARTData)//�ǵȴ���֡ͷ�ַ�
				{ 
					UART_index=0;//����ű��0
					UART_Wate_byte =0x55;//���µȴ�֡ͷ�׸��ַ�
					if(ReadUARTData==0x55)//�ǵȴ��ַ�������֡��ʼ�ַ�0x55
					{
						Usart_Frame_Buffer[UART_index++]=ReadUARTData;
						UART_Wate_byte = 0xAA;						
					}				 
				}
				else  //�ǵȴ���֡ͷ�ַ�
				{
					Usart_Frame_Buffer[UART_index++]=ReadUARTData;
					switch(UART_index)//���ݵ�ǰ�ű�λ��ȷ����һ���ȴ����յ��ַ���ʲô
					{
						 case 1:UART_Wate_byte = 0xAA;break;
						 case 2:UART_Wate_byte = 0x00;break;
						 case 3:UART_Wate_byte = 0x00;break;
						 case 4:UART_Wate_byte = 0xAA;break;
						 case 5:UART_Wate_byte = 0x55;break;
						 case 6:UART_Wate_byte = 0x55;break;
						 default: UART_Wate_byte =0x55; UART_index=0;break;          
					}
					if(UART_index==6)
					{
						UART_head_flg=1;
						UART_index=0;
						//fg_Usart1Busying=1;			
						//Usart1BusyTime=0;											
					}//�յ�������֡ͷ��6���ַ�
				}
			}
			else  //�Ѿ��յ���������֡ͷ UART_head_flg==1
			{

				Usart_Frame_Buffer[UART_index++]=ReadUARTData;//ֱ�Ӱ����ݷ���֡����
				if(UART_index==4)// ?�յ��ֽ�������11����ʼ�����豸��ַ��������ʼ��ַ�����ݳ��ȣ�֡����
				{						 								 
					UART_Data_addr = Usart_Frame_Buffer[2U];         
					UART_Data_addr = (UART_Data_addr<<8)+Usart_Frame_Buffer[1U];
						 
					UART_Data_RW= Usart_Frame_Buffer[0U];          // igh byte
					if(UART_Data_RW==0x01)  //�������ݷ���0�������1����д�룻ת�Ӱ崫����λ��ʱ���ֽ�Ϊ1
					{ 
						USART_RV_Data_long = Usart_Frame_Buffer[3U];		
					}
					else
					{
						USART_RV_Data_long=0;
					}
				}
									
				if(UART_index==(4+USART_RV_Data_long+2))   //�����������ݺ�2���ֽڵ�CRCУ����
				{
					UART_CRC_RE = Usart_Frame_Buffer[USART_RV_Data_long+5U];         
					UART_CRC_RE= (UART_CRC_RE<<8)+Usart_Frame_Buffer[USART_RV_Data_long+4U];         
					UART_frame_flg=1;
								  
					//UART_CCITT_CRC16Init( (uint8_t const *)Usart_Frame_Buffer, (4+USART_RV_Data_long) );								
					//UART_CRC_CA= Uart_CCITT_CRC16;				
					//								
													
					//�����յ��������Լ�����У����UART_CRC_CA
					////if(UART_CRC_RE==UART_CRC_CA)//У������ȷ����֡������ɱ�־��֪ͨ������һ֡����,���������ݺ����Ҫ��flame_flg=0;UART_Wate_byte=0x55;//head_flg=0//index = 0;//�ű��0
					//{
					//	UART_frame_flg=1;

					//}
					//else
					//{ 
					//	UART_index=0;//?????0
					//	UART_Wate_byte =0x55;//??????????		
					//	UART_frame_flg=0;
					//	UART_head_flg=0;
					//}
				}
			}
		}
	}
}
