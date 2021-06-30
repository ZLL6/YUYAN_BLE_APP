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
unsigned	char 	UART_frame_flg=0; 				//收到完整帧待处理标志
unsigned 	char 	UART_head_flg=0;				//识别到有效帧头标志
unsigned 	char 	UART_index=0;					//接收数据数组脚标
unsigned 	char 	UART_Wate_byte=0x55;			//等待接收的字符，每帧首个字符是0x55；
bool 				Is_UsartSend = false;			//串口读取历史数据标志
unsigned  	int 	flame_cmd;						//帧命令
unsigned  	int 	UART_Data_addr=0;				//帧有效数据起始地址
unsigned  	int 	Devide_addr=0;					//?帧有效数据起始地址
unsigned  	char 	UART_Data_RW=0;					//帧有效数据起始地址
extern unsigned int APP_FLG_ADDR;//APP标志位存放位置
extern RTC_DateTimeTypeDef RTC_DateTimeStructure_NEW;
RTC_RESULT RTC_OK_RESULT;
extern RTC_DateTypeDef     RTC_DateTypeDef_NEW;
//extern static s32 Current_data;
//unsigned  char RE_from_seir[250];					//?????????????;

unsigned  char USART_SEND_data[224];				/// UART有效数据帧缓存

unsigned  int UART_CRC_RE;							//收到的校验码
unsigned  int UART_CRC_CA;							//根据收到的数据计算出来的校验码
unsigned  char USART_RV_Data_long=0;				//帧有效数据长度；
volatile unsigned  char UART_sending_flg;
unsigned  char USART_SEND_LONG;
unsigned  char  USART_send_cnt;

static unsigned char ReadUARTData;					//读取串口数据寄存器


unsigned  char Usart_Frame_Buffer[224];				//UART有效数据帧缓存
unsigned char  UsartReturn_Buffer[224];  			//串口转接CAN 口数据缓存
	
 //================================================================
extern unsigned char    UsartCAN_frame_flg; 		//收到完整帧待处理标志 
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
//CAN缓存发送到串口
//==============================================================
/*void CANToUsart_Process(void)
{
			if(UartSendWait==1)    
			{
				USART_Send_string(CanReturn_Buffer,CAN_Send_long);//数据缓存，发送长度
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
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);   //初始化UART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	USART_InitStructure.USART_BaudRate = BaudRate;   //串口参数初始化
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	USART_ClearFlag(USART1,0XFFFFFFFF);
	USART_Init(USART1, &USART_InitStructure); //配置串口中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);


	USART_OverrunDetectionConfig(USART1,USART_OVRDetection_Disable); //溢出检测关闭
	USART_ClearFlag(USART1,0XFFFFFFFF);

	USART_Cmd(USART1, ENABLE);   //开启串口使能

	UART_index=0;//?????0
	UART_Wate_byte =0x55;//??????????		
	UART_frame_flg=0;
	UART_head_flg=0;
}
	
//========================================================================================
 //把串口接收到的完整帧加载到CAN发送中间缓存	
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
	if(UART_frame_flg==1)  //串口缓存有帧接收完成标志，程序处理
	{
					
		UART_CCITT_CRC16Init( (uint8_t const *)Usart_Frame_Buffer, (4+USART_RV_Data_long) );								
		UART_CRC_CA= Uart_CCITT_CRC16;				
		//根据收到的数据自己计算校验码UART_CRC_CA
		if(UART_CRC_RE==UART_CRC_CA)//校验码正确，置帧接收完成标志，通知程序处理一帧数据,处理完数据后程序要让flame_flg=0;UART_Wate_byte=0x55;//head_flg=0//index = 0;//脚标归0
		{
			g_Can_falg = 1;	
			temp_cmd = Usart_Frame_Buffer[0]; 	 
			temp_addr = Usart_Frame_Buffer[2];
			temp_addr = (temp_addr<<8)+Usart_Frame_Buffer[1];
			temp_Data_long=Usart_Frame_Buffer[3];
			/*校验正确*/
			/*根据不同功能码对flash进行操作*/
			/*USART_Send_status(USART_SUCCESS)OR USART_Send_status(USART_Operate_ERR);*/
			//返回操作成功或者失败帧
			if(temp_addr==0x8000)//上位进请求进入bootloader
			{
			
				FLASH_Status flash_Operate_St;
				BMS_config_Write();//delay_ms(10);
				BMS_Calibration_Write();//delay_ms(10);
				BMS_Alm_Number_Write();
				delay_ms(10);
				FLASH_Unlock();//解锁
				flash_Operate_St = 	FLASH_ErasePage(APP_FLG_ADDR);
				FLASH_Lock();//重新上锁	
				if(flash_Operate_St==FLASH_COMPLETE)  
				{
					USART_Send_status(USART_SUCCESS);//enter bootloader success
					delay_ms(10);
					NVIC_SystemReset();	 //复位芯片	
				}		
				else
				{
					USART_Send_status(USART_Operate_ERR);
				//						FLASH_Unlock();//解锁
				//				    flash_Operate_St = 	FLASH_ErasePage(APP_FLG_ADDR);
				//					  FLASH_Lock();//重新上锁	
				//						if(flash_Operate_St==FLASH_COMPLETE) 
				//						NVIC_SystemReset();	 //复位芯片	

				}
				//USART_Send_status(USART_SUCCESS);//enter bootloader success
			}
			if(temp_cmd==0)  //读的指令处理
			{
				if((temp_addr >= 0x4000) && (temp_addr <= 0x4ffe))
				{					
					tAlmPrtEvtHead_R.ReadEvent_loop = (temp_addr&0x3fff);
					BMS_Alm_Number_Write();
					Is_UsartSend=true;					//使能蓝牙串口发送
					ReadEventRecord();					
				}
				else if((temp_addr >= 0x2000) && ((temp_addr + (temp_Data_long/2)) <= 0x20BC))   //数据处理函数
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
					UsartReturn_Buffer[temp_i++]=temp_Data_long;//数据长度固定4
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
					UsartReturn_Buffer[temp_i++]=temp_Data_long;//数据长度固定4
					for(i = 0;i < (temp_Data_long);i++)
					{
						UsartReturn_Buffer[temp_i++] =TX[i+lin];
					}	
					UART_CCITT_CRC16Init(&UsartReturn_Buffer[6],(temp_Data_long+4)); 
					UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
					UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
					USART_Send_string(UsartReturn_Buffer,(temp_Data_long+12));
				}
				else if(temp_addr == 0x5000) //极值存储
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
					UsartReturn_Buffer[temp_i++]=temp_Data_long;//数据长度固定4
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
				   USART_Send_status(USART_Operate_ERR);//错误的功能码
				}	
			}
			if(temp_cmd==1)  //写指令处理	
			{
				if((temp_addr >= 0x4000) && (temp_addr <= 0x4ffe))
				  {
							ReadEvent_enable=0;//禁止再调用此函数
							tAlmPrtEvtHead_R.ReadEvent_Num = 0;
							tAlmPrtEvtHead_R.ReadEventAddr_st=0;
					}
				else if((temp_addr >= 0x2021) && ((temp_addr + (temp_Data_long/2)) <=0x20BC))   //数据处理函数
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
								BMS_config_Write();//写配置参数
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
						else if(temp_addr == 0x230A)    //温度系数校准
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
						else if(temp_addr == 0x230B)//清除故障记录与永久失效
						{
						 	if(temp_Data_long == 0 )
							{
								gBit_BatHighTemFailure = 0;            //电池高温失效
					      gBit_BatlowTemFailure	= 0;								//电池低温失效 
                gBit_BatOverVoltFailure = 0;							//电池电池过压导致永久失效
                gBit_BatUnderVoltFailure	= 0;						//电池电池欠压导致永久失效
                gBit_BatVoltDiffMaxFailure	= 0;						//电池压差过大导致失效
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
						   GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
		           GPIO_SetBits(GPIOx_W5_6,LED_W6);	 //自放电完成
			         g_FaultAlarmState[6]&= ~(1<<3);	
							 gDchg_falg_PC = 0;
							 BAT_State = FreeMode;  //调到静置模式
							}
							if(Usart_Frame_Buffer[4] == 0x00)
							{
								gDchg_falg_PC = 1;	
								GPIO_SetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
			          GPIO_ResetBits(GPIOx_W5_6,LED_W6);	//开橙灯
			          g_FaultAlarmState[6]|= 1<<3;
								
							}								
						}
						else if(temp_addr == 0x2310)
						{
									if(Usart_Frame_Buffer[4] == 0xff)
									{

									 BAT_State = Sleep;//系统改变到休眠模式
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
				UsartReturn_Buffer[temp_i++]=1;//数据长度固定1
				UsartReturn_Buffer[temp_i++]=1;//
				UART_CCITT_CRC16Init(&UsartReturn_Buffer[6],5); 
				UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
				UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
				if(~gBitStorageFailure )	
				{
				 USART_Send_string(UsartReturn_Buffer,(13));
				}
				 BMS_capacity.PackNewCapacity =(u32)(BMS_config.Pack_RW_config.gNewPackCapacity)*360000;//算满充容量	
			   if(BMS_config.Pack_RW_config.gSOC!=0)  //计算现有容量
				 BMS_capacity.BatCapacity = BMS_capacity.PackNewCapacity/100* BMS_config.Pack_RW_config.gSOC;
				 else
				 BMS_capacity.BatCapacity = 0;	
			 
			} 						
		//}
	
		/*CANSendBuffer_long = 4+USART_RV_Data_long+2;
		UsartReturn_Buffer[0]=Usart_Frame_Buffer[4+USART_RV_Data_long];	
		UsartReturn_Buffer[1]=Usart_Frame_Buffer[4+USART_RV_Data_long+1];	

		for(i=0;i<CANSendBuffer_long-2;i++)//把串口接收到的数据放入CAN发送中间缓存
		{	
			UsartReturn_Buffer[i+2]=Usart_Frame_Buffer[i];													

		}			
		UsartCAN_frame_flg =1; //通知CAN可以发送数据
		UART_index=0;//?????0
		UART_Wate_byte =0x55;//??????????		
		UART_head_flg=0;		
		UART_frame_flg = 0; // 重新打开接收数据
			  */
		}
		else /*校验失败*/
		{ 
			/*返回校验错误帧*/
			USART_Send_status(USART_CRC_ERR);
		}
		UART_index=0;//?????0
		UART_Wate_byte =0x55;//??????????		
		UART_frame_flg=0;
		UART_head_flg=0;
	}
}
/*串口收到命令后的状态答复函数*/
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
	UsartReturn_Buffer[temp_i++]=1;//数据长度固定1
	UsartReturn_Buffer[temp_i++]=status;
	UART_CCITT_CRC16Init(&UsartReturn_Buffer[6], 5); 
	UsartReturn_Buffer[temp_i++]=(char)Uart_CCITT_CRC16;
	UsartReturn_Buffer[temp_i++]=(char)(Uart_CCITT_CRC16>>8);
	USART_Send_string(UsartReturn_Buffer,13);
 
}

////////////////////////////////
/*串口发送字符串*/
///////////////////////////////////////////////////////
void USART_Send_string(uint8_t *addr,uint8_t USART_RV_Data_long)
{
	uint8_t tem_i;
	while(UART_sending_flg);
	USART_SEND_LONG=USART_RV_Data_long;
	for(tem_i=0;tem_i<USART_SEND_LONG;tem_i++)//把需要发送的数据装载到发送缓存
	{
		USART_SEND_data[tem_i] = addr[tem_i];
	}
	USART_ClearFlag(USART1, USART_IT_TC);
	USART_SendData(USART1,USART_SEND_data[USART_send_cnt++]);//发送第一个字符
	UART_sending_flg =1;
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);//使能发送完成中断，后续的字节发送在中断中完成
}	

//=====================================================================================
void USART1_IRQHandler(void)//串口中断函数
{
	
	if(UART_sending_flg)  //UART正在发送数据中
	{ 
		if(USART_GetITStatus(USART1,USART_IT_TC) != RESET)//发送完成中断
		{
			USART_ClearFlag(USART1, USART_IT_TC);//清中断标志位
			if(USART_send_cnt==USART_SEND_LONG)//数据发送完成
			{
				UART_sending_flg=0;
				USART_send_cnt=0;
				USART_ITConfig(USART1, USART_IT_TC, DISABLE);//关闭串口发送完成中断
			}
			else//还有数据没发完
			{
				USART_SendData(USART1,USART_SEND_data[USART_send_cnt++]);//继续装入下一数据
			}
		}
	}
//===============读取数据================================================================================	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)//字符接收完成中断
	{
		ReadUARTData =USART_ReceiveData(USART1);	//缓冲器3收串口接收缓存内获得每一个字符
						
		if(UART_frame_flg==1)//已经有接收完成的帧未处理
		{
			UART_Wate_byte =0x55;//??????????		
		}
		else
		{

			if(UART_index>224)  //  最大允许224个字节， 串口接收数据长度过负载，重新清0
			{
				UART_index=0;//数组脚标归0
				UART_Wate_byte =0x55;//重新等待帧头首个字符
				UART_frame_flg=0;
				UART_head_flg=0;
			} 
			if(UART_head_flg==0)//尚未捕捉到完整帧头
			{
				if(UART_Wate_byte !=ReadUARTData)//非等待的帧头字符
				{ 
					UART_index=0;//数组脚标归0
					UART_Wate_byte =0x55;//重新等待帧头首个字符
					if(ReadUARTData==0x55)//非等待字符，但是帧开始字符0x55
					{
						Usart_Frame_Buffer[UART_index++]=ReadUARTData;
						UART_Wate_byte = 0xAA;						
					}				 
				}
				else  //是等待的帧头字符
				{
					Usart_Frame_Buffer[UART_index++]=ReadUARTData;
					switch(UART_index)//根据当前脚标位置确定下一个等待接收的字符是什么
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
					}//收到完整的帧头，6个字符
				}
			}
			else  //已经收到有完整的帧头 UART_head_flg==1
			{

				Usart_Frame_Buffer[UART_index++]=ReadUARTData;//直接把数据放入帧缓存
				if(UART_index==4)// ?收到字节数等于11，开始计算设备地址，数据起始地址，数据长度，帧命令
				{						 								 
					UART_Data_addr = Usart_Frame_Buffer[2U];         
					UART_Data_addr = (UART_Data_addr<<8)+Usart_Frame_Buffer[1U];
						 
					UART_Data_RW= Usart_Frame_Buffer[0U];          // igh byte
					if(UART_Data_RW==0x01)  //控制数据方向，0代表读，1代表写入；转接板传给上位机时该字节为1
					{ 
						USART_RV_Data_long = Usart_Frame_Buffer[3U];		
					}
					else
					{
						USART_RV_Data_long=0;
					}
				}
									
				if(UART_index==(4+USART_RV_Data_long+2))   //收完所有数据和2个字节的CRC校验码
				{
					UART_CRC_RE = Usart_Frame_Buffer[USART_RV_Data_long+5U];         
					UART_CRC_RE= (UART_CRC_RE<<8)+Usart_Frame_Buffer[USART_RV_Data_long+4U];         
					UART_frame_flg=1;
								  
					//UART_CCITT_CRC16Init( (uint8_t const *)Usart_Frame_Buffer, (4+USART_RV_Data_long) );								
					//UART_CRC_CA= Uart_CCITT_CRC16;				
					//								
													
					//根据收到的数据自己计算校验码UART_CRC_CA
					////if(UART_CRC_RE==UART_CRC_CA)//校验码正确，置帧接收完成标志，通知程序处理一帧数据,处理完数据后程序要让flame_flg=0;UART_Wate_byte=0x55;//head_flg=0//index = 0;//脚标归0
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
