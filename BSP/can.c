#include "pbdata.h"
#include "data.h"
#include "core_cmFunc.h"
#include "stdio.h"
#include "uart.h"
#include "stdbool.h"
#include "stdlib.h"
//CANID

#define CAN_ID0_INT   					((u32)0x0E416E4)
#define CAN_ID1_OUT   					((u32)0x016E416)  
//#define CAN_ID2_OUT   					((u32)0x010922C)  
#define CAN_ID2_OUT   					((u32)0x1109216) 

#define CAN_BOYING_IN   					((u32)0x1724040B) 

//对应不同串数ID
#define CAN_12_ID     ((u16)0x02D0) 
#define CAN_14_ID     ((u16)0x02D1) 
#define CAN_6_ID      ((u16)0x02D2) 
#define CAN_13_ID     ((u16)0x02D3) 
#define CAN_28_ID     ((u16)0x02D4)
//命令的ID
#define CAN_Parameter_ID     ((u16)0x02DA)  //参数命令
#define CAN_Handshake_ID     ((u16)0x02E4)  //握手命令
#define CAN_SHA1_ID          ((u16)0x02E5)  //秘钥写入
#define CAN_SHA2_ID          ((u16)0x02E6)  //报文查询

#define Start_of_transfer 0x80
#define End_of_transfer 0x40
#define   quCurrent(x)    (x<0x80000000?x:(~(x-1)))
extern u16 CCITT_CRC16;
 u8 CAN_R[240]; //CAN接收的数据
static u8 CAN_R_LEN_SUM; //CAN接收数据的个数
 u8 CAN_T[240];  //CAN发送的数据
 u8 CAN_T_LEN_SUM = 0; //CAN发送的数据个数
u8 CAN_RX_Falg = 0;  //数据接收完成标志位
volatile u8 CAN_TX_Falg = 0;  //数据发送标志位
static  u16  can_ID= 0;//
extern bool Is_UsartSend;					//使能蓝牙串口发送
volatile u8 Broadcast_Falg = 0;  //名称更新标志
unsigned int APP_FLG_ADDR=0x0801FC00;//APP标志位存放位置
//static unsigned short FLASH_APP_FLG;
 u32 CAN_id_add=0; 
R_CAN PC_CAN_Data_R;
T_CAN PC_CAN_Data_T;
CAN_CustomerCode_typedef CAN_CustomerCode;
CAN_boying_ID1_typedef  boying_ID1;
CAN_boying_ID2_typedef  boying_ID2;
RTC_DateTimeTypeDef RTC_DateTimeStructure_NEW;
RTC_DateTypeDef     RTC_DateTypeDef_NEW;

/***************************************************************************
*   函 数 名: CAN_GPIO
*   功能说明: CAN的GPIO管脚初始化
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void CAN_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义一个GPIO_InitTypeDef类型的结构体
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);	//PB端口时钟使能                        											 

	/* CAN1 Periph clock enable */
	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_4); //定义PB8复用功能
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_4); //定义PB9复用功能
	
	/* Configure CAN pin: RX */									               // PB8
	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;//复用模式
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;//GPIO_PuPd_UP = 0x01,
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */									               // PB9
  
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF; //复用模式
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL; //GPIO_PuPd_NOPULL = 0x00
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void CAN_Configation(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	CAN_GPIO();
	
	CAN_DeInit(CAN);
	CAN_StructInit(&CAN_InitStructure);
	
	
	CAN_InitStructure.CAN_TTCM=DISABLE;         // 时间触发通信禁止
	CAN_InitStructure.CAN_ABOM=ENABLE;	        // 离线退出是在中断置位清0后退出
	CAN_InitStructure.CAN_AWUM=DISABLE;	        // 自动唤醒模式：清零sleep
	CAN_InitStructure.CAN_NART=ENABLE;	        // 自动重新传送报文，知道发送成功
	CAN_InitStructure.CAN_RFLM=DISABLE;	        // FIFO没有锁定，新报文覆盖旧报文
	CAN_InitStructure.CAN_TXFP=DISABLE;         // 发送报文优先级确定：标志符
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal; // 正常模式

	//1M
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 3;

	CAN_Init(CAN, &CAN_InitStructure);	// 初始化CAN1
	
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE); //中断使能
}
/***************************************************************************
*   函 数 名:void CAN_Recv_Control(CanRxMsg RxMessage)
*   功能说明: CAN控制recv函数打包数据
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void CAN_Recv_Control(CanRxMsg RxMessage)  //CAN控制recv函数返回值说明
//void CAN_Recv_Control(void)  //CAN控制recv函数返回值说明
{
	uint8_t  CAN_Pack=0; //
	uint8_t  CAN_end=0;
	uint8_t  i;
//	CanRxMsg RxMessage;
//	RxMessage =RxMessage;
	static u8 CAN_Start_of_transfer_falg=0;//以接收到标志
//		CanRxMsg RxMessage;
//	CAN_Receive(CAN,CAN_FIFO0,&RxMessage);
	//CAN接收
	 g_Can_falg = 1;	
	if((RxMessage.ExtId==CAN_ID0_INT)&(CAN_RX_Falg == 0))
	{

		if(((RxMessage.Data[RxMessage.DLC-1])&(Start_of_transfer)) == 0x80)	//开始接收
		{
			for(i=0;i<RxMessage.DLC-1;i++)
				CAN_R[i]= RxMessage.Data[i];		
			CAN_Start_of_transfer_falg=1; 									//CAN接收到帧头记录数据				
		}
		else          														//数据放到接收数据
		{
			CAN_Pack = RxMessage.Data[RxMessage.DLC-1] & 0x1f;
			if(CAN_Start_of_transfer_falg == 1) 							//接收到帧头记录数据
			{
				for(i=0;i<RxMessage.DLC-1;i++)
					CAN_R[i+CAN_Pack*7]= RxMessage.Data[i]; 
			}

		}
		CAN_end = (RxMessage.Data[RxMessage.DLC-1])&(End_of_transfer);
		if((CAN_end==End_of_transfer)&(CAN_Start_of_transfer_falg == 1))  	//接收完成
		{
			CAN_RX_Falg = 1;     											//CAN接收完成  
			CAN_R_LEN_SUM = RxMessage.DLC+CAN_Pack*7-1;
		}
			 		 
	 }
	if((RxMessage.ExtId==CAN_BOYING_IN)|(RxMessage.ExtId ==0x0005))
		gBOying_IN = 1;
			
			
 }

/***************************************************************************
*   函 数 名: RcvBcmuMsgHandle
*   功能说明: RcvBcmuMsgHandle处理接收到的数据，回复发送数据
*   形    参：无
*   返 回 值: 数据准备OK 启动发送
***************************************************************************/
u8 RcvBcmuMsgHandle()
{
	u16  addr = 0;
	u8  dataLen = 0;
	u8 i = 0;
	static u16 lin=0;
	uint16_t CRC16 = 0;
	static s32 Current_data = 0 ;
	static	s32 Current1 = 0 ;
	static	s32 Current2 = 0 ;
	static s16 Volt = 0 ;
	RTC_RESULT RTC_OK_RESULT;
	u8 TX[400];
	u8 BUWEI[34];
	u16 temp1 = 0;
	u16 temp2 = 0;
	if(CAN_RX_Falg == 0) //没有接收到数据退出
		return 0;	

	CCITT_CRC16Init(&CAN_R[2],CAN_R_LEN_SUM-2); //计算效验值	
	CRC16 =	CAN_R[0]+((u16)CAN_R[1]<<8);//读的效验部分		
	if(CCITT_CRC16 != CRC16) //数据效验不成功退出
	{
		CAN_RX_Falg = 0;//允许接受
		return 0;	
	}


	addr = CAN_R[3]+((u16)CAN_R[4]<<8);
	dataLen = CAN_R[5];
	if(addr == 0x8000)//接收到进入BOOT指令
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
			NVIC_SystemReset();	 //复位芯片	
		}		
		else
		{
			FLASH_Unlock();//解锁
			flash_Operate_St = 	FLASH_ErasePage(APP_FLG_ADDR);
			FLASH_Lock();//重新上锁	
			if(flash_Operate_St==FLASH_COMPLETE) 
				NVIC_SystemReset();	 //复位芯片	
		}

	}
	if(CAN_R[2]==0)  //读的指令处理
	{
		if((addr >= 0x4000) && (addr <= 0x4ffe))
		{
			tAlmPrtEvtHead_R.ReadEvent_loop = (addr & 0x3fff);
			BMS_Alm_Number_Write();
			Is_UsartSend=false;					//使能蓝牙串口发送
			ReadEventRecord();
			CAN_id_add=CAN_ID1_OUT;
			CAN_TX_Falg = 1;
			CAN_RX_Falg = 0;
			return 0 ;
		}
		else if((addr >= 0x2000) && ((addr + (dataLen/2)) <= 0x20BC))   //数据处理函数
		{

			temp1 =	sizeof(BMS_config);	
			temp2 =	sizeof(Alm_Number);	
			memcpy(TX,&BMS_config,temp1);  
			memcpy(BUWEI,&Alm_Number,temp2);  

			for(i = 0;i < (temp2);i++)
			{
				TX[temp1+i-2] =BUWEI[i];
			}							
			lin = (addr-0x2000)*2;	
			timer6 =0;
			while(CAN_TX_Falg)
			{
				if(timer6>35)
					break;
			}
			for(i = 0;i < (dataLen);i++)
			{
				CAN_T[i+6] =TX[i+lin];
			}	
		}

		else if((addr >= 0x2100) && ((addr + (dataLen/2)) <=0x2170)) 
		{
			memcpy(TX,&BMSVoltTemperature,sizeof(BMSVoltTemperature)); 
			lin = (addr-0x2100)*2;

			timer6 =0;
			while(CAN_TX_Falg)
			{
				if(timer6>35)
					break;
			}
			for(i = 0;i < (dataLen);i++)
			{
				CAN_T[i+6] =TX[i+lin];
			}	


		}
		else if(addr == 0x5000) //极值存储
		{

			memcpy(TX,&Extremum,sizeof(Extremum));
			timer6 =0;
			while(CAN_TX_Falg)
			{
				if(timer6>35)
					break;
			}

			for(i = 0;i < (dataLen);i++)
			{
				CAN_T[i+6] =TX[i];
			}						
		}
		else
		{
			CAN_R[2]= 0xff;
		}

		////////////在没回复上位机前不允许接收
		//CAN_RX_Falg = 0;//允许接受
	}
	if(CAN_R[2]==1)  //写指令处理
	{
		//	dataLen=0;
		if((addr >= 0x4000) && (addr <= 0x4ffe))
		{
			ReadEvent_enable=0;//禁止再调用此函数
			tAlmPrtEvtHead_R.ReadEvent_Num = 0;
			tAlmPrtEvtHead_R.ReadEventAddr_st=0;
		}
		else if((addr >= 0x2021) && ((addr + (dataLen/2)) <=0x20BC))   //数据处理函数
		{
			memcpy(TX,&BMS_config,sizeof(BMS_config));  
			lin = (addr-0x2000)*2;

			for(i = 0;i < (dataLen);i++)
			{
				TX[i+lin] =CAN_R[i+6];
			}	
			memcpy(&BMS_config,TX,sizeof(BMS_config)); 
			BMS_config.Pack_RW_config.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSOC*BMS_config.Pack_RW_config.gNewPackCapacity/100;
			BMS_capacity.BatCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity*360000;  					
			BMS_config.Pack_RW_config.gNewPackCapacity	= BMS_config.Pack_RW_config.gSOH * BMS_config.Pack_RW_config.gSetPackCapacity/100;				
			BMS_config.Pack_RW_config.gsCycleCnt =	BMS_config.Pack_RW_config.gCHGPackCapacity*1000/( BMS_config.Pack_RW_config.gSetPackCapacity*8);								
			BMS_config_Write();//写配置参数
			delay_ms(20);
			BQ769X0_ConfigPara(0);
			Broadcast_Falg=1;
		}
		else if(addr == 0x2303)
		{
			tAlmPrtEvtHead_W.WriteEventAddr  = 0;
			tEvtDataW.index = 0 ;
			tAlmPrtEvtHead_W.EventNum = 0;
			tAlmPrtEvtHead_R.WriteEventAddr  = 0;
			tAlmPrtEvtHead_R.EventNum = 0;						
			BMS_Alm_Number_Write();
		}
		else if(addr == 0x2304)
		{
			Current_data =CAN_R[i+6]+(u16)(CAN_R[i+7]<<8);
			if(Current_f<0)
				return 0;
			Calibration.gChgCurrent_double  = (double)Current_data*10.0f/Current_f;
			BMS_Calibration_Write();
		}
		else if(addr == 0x2305)
		{
			Current_data =(s32)(CAN_R[i+6]+(s16)(CAN_R[i+7]<<8));
			Current1 = quCurrent((u32)Current_data) ;
			Current2  = quCurrent((u32)Current_f);
			if(Current_f>0)
				return 0;
			Calibration.gDchgCurrent_double  = (double)Current1*10.0/Current2;
			BMS_Calibration_Write();
		}
		else if(addr == 0x2306)
		{
			Volt =CAN_R[i+6]+(u16)(CAN_R[i+7]<<8);
			if((Volt>=3700)&&(Volt<4500))
			{
				if(CAN_R[i+8]==0)
				{
					for(i = 0;i < BMS_config.Pack_RW_config.gBatNumber;i++)
					Calibration.g_Volt_double_max[i] = (double)Volt / Volt_call[i];						
				}
				else{
					Calibration.g_Volt_double_max[CAN_R[i+8]-1] = (double)Volt / Volt_call[CAN_R[i+8]-1];
				}
				BMS_Calibration_Write();
			}
		}
		else if(addr == 0x2308)
		{
			Volt =CAN_R[i+6]+(u16)(CAN_R[i+7]<<8);
			if((Volt>2500)&&(Volt<=3700))
			{
				if(CAN_R[i+8]==0)
				{
					for(i = 0;i < BMS_config.Pack_RW_config.gBatNumber;i++)
					Calibration.g_Volt_double_min[i] = (double)Volt / Volt_call[i];						
				}
				else{
					Calibration.g_Volt_double_min[CAN_R[i+8]-1] = (double)Volt / Volt_call[CAN_R[i+8]-1];
				}
				BMS_Calibration_Write();
			}
		}
		else if(addr == 0x230A)    //温度系数校准
		{
			if ((CAN_R[7]<0x80)&&((u8)Temperature_call [CAN_R[i+7]] < 0x80))
			{
				if(CAN_R[i+6]==0)
				{
					for(i = 0;i < BMS_config.Pack_RW_config.gTemNumber;i++)
					Calibration.gTem_double[CAN_R[i+6]]= (double)CAN_R[7] /Temperature_call[CAN_R[i+6]]	;						
				}
				else{
					Calibration.gTem_double[CAN_R[i+6]-1]= (double)CAN_R[7] /Temperature_call[CAN_R[i+6]-1]	;	
				}		
			}			
			BMS_Calibration_Write();
		}
		else if(addr == 0x230B)//清除故障记录与永久失效
		{
			if(dataLen == 0 )
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
		else if(addr == 0x230D)
		{
			gSOC_Balance_Pc = 1;							
			gBalance_pc= CAN_R[6]|(CAN_R[7]<<8);
			if(gBalance_pc == 0)
				gSOC_Balance_Pc = 0;	
			BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	
		}
		else if(addr == 0x2300)
		{
			RTC_DateTimeStructure_NEW.Second = 	CAN_R[11];			
			RTC_DateTimeStructure_NEW.Minute =  CAN_R[10];
			RTC_DateTimeStructure_NEW.Hour   =  CAN_R[9];
			RTC_DateTimeStructure_NEW.Date   =  CAN_R[8];
			RTC_DateTimeStructure_NEW.Month  =  CAN_R[7];
			RTC_DateTimeStructure_NEW.Year   = 	CAN_R[6];	
			RTC_OK_RESULT =		RTC_SetDateTime(RTC_DateTimeStructure_NEW);	
			if(RTC_OK ==RTC_OK_RESULT)
			{
				RTC_DateTypeDef_NEW.RTC_Year = RTC_DateTimeStructure_NEW.Year;
				RTC_DateTypeDef_NEW.RTC_Month = RTC_DateTimeStructure_NEW.Month;
				RTC_DateTypeDef_NEW.RTC_Date =  RTC_DateTimeStructure_NEW.Date;							
				RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeDef_NEW);
			}

		}
		else if(addr == 0x230C)
		{
			if(CAN_R[6] == 0xff)
			{
				GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
				GPIO_SetBits(GPIOx_W5_6,LED_W6);	 //自放电完成
				g_FaultAlarmState[6]&= ~(1<<3);	
				gDchg_falg_PC = 0;
				BAT_State = FreeMode;  //调到静置模式
			}
			if(CAN_R[6] == 0x00)
			{
				gDchg_falg_PC = 1;	
				GPIO_SetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
				GPIO_ResetBits(GPIOx_W5_6,LED_W6);	//开橙灯
				g_FaultAlarmState[6]|= 1<<3;

			}								
		}
		else if(addr == 0x2310)
		{
			if(CAN_R[6] == 0xff)
			{

				BAT_State = Sleep;//系统改变到休眠模式
				gSleep_pc = 1;
			}
		}
		else if(addr == 0x2311)
		{
			if(CAN_R[6] == 0x55)
			gBitStorageFailure =	W25QXX_Demo();

		}
		
		////////回复给上位机后才允许接收	
		//	CAN_RX_Falg = 0;//允许接受

	}

	if((CAN_R[2]==0)||(CAN_R[2]==1))	
	{
		//				if(CAN_TX_Falg == 1)
		//				return 0;	
		timer6 =0;
		while(CAN_TX_Falg)
		{
			if(timer6>35)
				break;
		}
		CAN_T[2]=  0x01;
		CAN_T[3]= CAN_R[3];
		CAN_T[4]= CAN_R[4];
		CAN_T[5]= dataLen;
		if(CAN_R[2]==0)
		{
			CAN_T_LEN_SUM = dataLen+6;
			CCITT_CRC16Init(&CAN_T[2],dataLen+4); 					//计算效验值
		}
		else	
		{
			CAN_T[5]= 0X01;
			CAN_T[6]= 0X01;
			CAN_T_LEN_SUM = 7;
			CCITT_CRC16Init(&CAN_T[2],5); 							//计算效验值	

			BMS_capacity.PackNewCapacity =(u32)(BMS_config.Pack_RW_config.gNewPackCapacity)*360000;//算满充容量	

			if(BMS_config.Pack_RW_config.gSOC!=0)  					//计算现有容量
				BMS_capacity.BatCapacity = BMS_capacity.PackNewCapacity/100* BMS_config.Pack_RW_config.gSOC;
			else
				BMS_capacity.BatCapacity = 0;	
		}				 
		CAN_T[0]=	CCITT_CRC16&0x00ff;								//CRC16低位
		CAN_T[1]=	(CCITT_CRC16>>8)&0x00ff;						//CRC16低位	

		CAN_RX_Falg = 0;											//允许接受

		if((gBitStorageFailure )&&(CAN_R[2]==1))	
		{
			CAN_T_LEN_SUM = 0;
			CAN_TX_Falg = 0; 										//不发送
		}	
		else{
			CAN_id_add = CAN_ID1_OUT;								//装入数据包29位ID
			CAN_TX_Falg = 1; 										//允许发送
			///启动发送后允许接收新的数据
		}
	}

	return 1 ;
}
/***************************************************************************
*   函 数 名: CAN_TX_BcmuMsgHandle
*   功能说明: CAN_TX_BcmuMsgHandle发送数据
*   形    参：无
*   返 回 值: 数据发送
***************************************************************************/
u8 CAN_TX_BcmuMsgHandle(void)
	{
		static CanTxMsg TxMessage;
	  static u8 can_pack=0;
		static	u8  can_send_cnt=0;//装在数据
		u8  temp_i=0;
	 	if(CAN_T_LEN_SUM<240)
		{
					 if(CAN_TX_Falg==1)
					 {
							for(temp_i=0;temp_i<7;)
								{
										TxMessage.Data[temp_i]=CAN_T[can_send_cnt++];
										temp_i++;
										if(can_send_cnt>=CAN_T_LEN_SUM)//发送到了最后一个 字节
										break;					
								}		
								if(can_send_cnt>=CAN_T_LEN_SUM)
								TxMessage.Data[temp_i]=0x40 |can_pack|((can_pack%2)<<5);
								else
								{
								TxMessage.Data[temp_i]=can_pack|((can_pack%2)<<5);
								}
								
								if(can_pack==0)///第一包数据处理
								{
									if(can_send_cnt>=CAN_T_LEN_SUM)
										TxMessage.Data[temp_i]=0xc0;//装载尾字节
									else
										TxMessage.Data[temp_i]=0x80;//装载尾字节
								}	
								
								TxMessage.DLC=temp_i+1;
								TxMessage.IDE=CAN_ID_EXT;//RxMessage.IDE;
//								if((!gCAN_falg)||(ReadEvent_enable==1))
//								TxMessage.ExtId=CAN_ID1_OUT;//装入数据包29位ID
//								else
//								TxMessage.ExtId=CAN_ID2_OUT;//装入数据包29位ID	
								 TxMessage.ExtId=  CAN_id_add;
							 CAN_Transmit(CAN , &TxMessage);	//CAN 发送数据,掉用CAN发送函数进行发送一帧数据			
							 can_pack++;
							if(can_send_cnt>=CAN_T_LEN_SUM)//数据发送完成标志
								{
									can_pack = 0;
									can_send_cnt = 0;
									CAN_T_LEN_SUM = 0;
									CAN_TX_Falg = 0;//数据发送完成标志 
									////////如果是UAVCAN发送结束，清除UAVCAN发送置///////////
									if(gCAN_falg)
									gCAN_falg = 0;
									///////////////////
								}
					 }
      }
		return 1;
  }
	/***************************************************************************
*   函 数 名: 	CAN_Boying_Code_TX(void)
*   功能说明: 博鹰协议对外发送
*   形    参：无
*   返 回 值: 无
***************************************************************************/
	void CAN_Boying_Code_TX(void)
	{
	  static 	uint32_t tickstart_CAN = 0;
    static 	uint32_t ticksotp_CAN = 0;
		static  u8 num = 0 ;
		u8 soh_falg=0;
	     if(tickstart_CAN == 0)
			 {
				 tickstart_CAN = HAL_GetTick();
			 }
	
			ticksotp_CAN = HAL_GetTick();
				if(ticksotp_CAN<tickstart_CAN)//
				{
				ticksotp_CAN=0xffffffff-tickstart_CAN+ticksotp_CAN;
				}
				else
				ticksotp_CAN=ticksotp_CAN-tickstart_CAN;			  
				
				if((ticksotp_CAN>249)&&(CAN_TX_Falg == 0))
				{
					tickstart_CAN = 0;
					ticksotp_CAN = 0;
				}
				else
				{
					return;
				}	
//								if(CAN_TX_Falg == 1)
//								return ;	
						timer6 =0;
						 while(CAN_TX_Falg)
						 {
							 if(timer6>35)
							 break;
						 }
				gBying  = 1;
		    num++;
				if(num <= 3)
					{				
							boying_ID1.gVoltSum = BMS_config.BMS_Read.gVoltSum*10;            //总电压1mV   
							boying_ID1.gCurrent= BMS_config.BMS_Read.gCurrent;       //当前电流值  
							boying_ID1.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSOC*10;//BMS_config.Pack_RW_config.gSurpulsPackCapacity*10;   //剩余容量
							boying_ID1.gTemAvg = BMS_config.BMS_Read.gTemAvg*10;          //平均温度
							//boying_ID1.gbat_state = 0x00;          //电池状态
					
						if(BMS_config.Pack_RW_config.gSOH<80)
						soh_falg=1;
						else
								soh_falg=0;
					  	boying_ID1.gbat_state = ((gBitChgHighTemAlm|gBitChgHighTemAlmPrt|gBitDchgHighTemAlm|gBitDchgHighTemAlmPrt)<<1)|\
						                          ((gBitVoltDiff1MaxAlm)|(gBitVoltDiff1MaxAlmPrt)|(gBitVoltDiff2MaxAlm)|(gBitVoltDiff2MaxAlmPrt)<<2)|\
                                   		(((gBitOverVoltAlm|gBitOverVoltAlmPrt)|(gBitUnderVoltAlm|gBitUnderVoltAlmPrt))<<3)|\
						                          ((gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt<<4))|\
						                          (soh_falg<<5)|\
						                          (gBit_Chg_Error<<6)|\
						                          ((gBitOverVoltAlm|gBitOverVoltAlmPrt)<<8)|\
						                          ((gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt<<9))|\
						                          ((gBitChgHighTemAlm|gBitChgHighTemAlmPrt)<<10)|\
						                          ((gBitUnderVoltAlm|gBitUnderVoltAlmPrt)<<11);											
							memcpy(&boying_ID1.g_Volt_call,&BMSVoltTemperature.g_Volt_call,sizeof(BMSVoltTemperature.g_Volt_call));//电池电压									
							boying_ID1.CRC16 = Boying_CheckCRC16((u8 *)(&boying_ID1.gVoltSum),(sizeof(boying_ID1)-2)); //计算效验值			
								can_ID = CAN_14_ID;
								memcpy(CAN_T,&boying_ID1,sizeof(boying_ID1));  //软件版本号	
								CAN_T_LEN_SUM = sizeof(boying_ID1);
								CAN_TX_Falg = 1;
						}	
					else	
						{
             num =0 ;						
						boying_ID2.gBatCode  =(u8)BMS_config.CustomerCode.gBatCode[0];           //电池厂商 
						boying_ID2.gBatCodeNumber = (u8)BMS_config.CustomerCode.gBatPack[0];        //电池型号
						boying_ID2.Product_Version = 0x01; //硬件型号V1.01
						boying_ID2.SoftWare_Version = 0x01;//软件版本V01
						boying_ID2.gSOH = BMS_config.Pack_RW_config.gSOH;                  //电池包SOH
						boying_ID2.gDCHG_Dynameter = 10;       //电池的放电倍率
						boying_ID2.gSetPackCapacity = BMS_config.Pack_RW_config.gSetPackCapacity/10;    //电池包的设计容量
						boying_ID2.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity/10;   //剩余容量
						boying_ID2.gMAX_Power_W = 6000;   //最大的输出W数
						boying_ID2.gVolt_standard = 58500;           //标称电压
						boying_ID2.gVoltAlm = BMS_config.OverVolt.gVoltAlm;           //单体告警值
						boying_ID2.gsCycleCnt = BMS_config.Pack_RW_config.gsCycleCnt;               //循环次数	
						boying_ID2.gVoltOverNumber = Alm_Number.OverVoltNumber;       //过充次数
						boying_ID2.gVoltUnderNumber = Alm_Number.UnderVoltNumber;       //过放次数
						boying_ID2.gHighTemNumber = Alm_Number.ChgHighTemNumber;        //超温次数
						boying_ID2.gOverCurrenNumber = Alm_Number.ChgOverCurrenNumber;        //过流次数
						boying_ID2.gChg_IlleqaltNumber = Alm_Number.InformalChgMosNumber;	       //非法充电次数  		
						boying_ID2.CRC16 = Boying_CheckCRC16((u8 *)(&boying_ID2.gBatCode),(sizeof(boying_ID2)-2)); //计算效验值	
							can_ID = CAN_Parameter_ID;
							memcpy(CAN_T,&boying_ID2,sizeof(boying_ID2));  //软件版本号	
							CAN_T_LEN_SUM = sizeof(boying_ID2);					
							CAN_TX_Falg = 1;		
						}
	}
	/***************************************************************************
*   函 数 名: CAN_TX_BoyingMsgHandle
*   功能说明: CAN_TX_BoyingMsgHandle发送数据
*   形    参：无
*   返 回 值: 数据发送
***************************************************************************/
u8 CAN_TX_BoyingMsgHandle(void)
	{
		static CanTxMsg TxMessage;
	  static u8 can_pack=0;
		static	u8  can_send_cnt=0;//装在数据
		static  u8  can_pack_stop = 0;//包结束标志

    		u8  temp_i=0;	
				if(CAN_T_LEN_SUM<240)
				{
					 if(CAN_TX_Falg == 1)
					 {
							for(temp_i=0;temp_i<8;)
								{
										TxMessage.Data[temp_i]=CAN_T[can_send_cnt++];
										temp_i++;
										if(can_send_cnt>=CAN_T_LEN_SUM)//发送到了最后一个 字节
										break;					
								}		

								if((can_pack*8+8) >= CAN_T_LEN_SUM)
								{
									can_pack_stop = 1;
								}
								
								TxMessage.DLC=temp_i;
								TxMessage.IDE=CAN_ID_EXT;//RxMessage.IDE;
						
								TxMessage.ExtId=3|can_pack_stop<<3|can_pack<<4|1<<10|2<<17|can_ID<<19;//装入数据包29位ID			
							 CAN_Transmit(CAN , &TxMessage);	//CAN 发送数据,掉用CAN发送函数进行发送一帧数据			
							 can_pack++;
							if(can_send_cnt>=CAN_T_LEN_SUM)//数据发送完成标志
								{
									can_pack = 0;
									can_pack_stop = 0 ;
									can_send_cnt = 0;
									CAN_T_LEN_SUM = 0;
									CAN_TX_Falg = 0;//数据发送完成标志 
									gBying  = 0;
								}
					 }
				 }
				return 1 ;
  }
	
	/***************************************************************************
*   函 数 名: 	void CAN_CustomerCode_TX(void)
*   功能说明: 充电协议对外发送
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void CAN_CustomerCode_TX(void)   
{
	static 	uint32_t tickstart_CAN = 0;
	static 	uint32_t ticksotp_CAN = 0;
	if(tickstart_CAN == 0)
	{
		tickstart_CAN = HAL_GetTick();
	}

	ticksotp_CAN = HAL_GetTick();
	if(ticksotp_CAN<tickstart_CAN)//
	{
		ticksotp_CAN=0xffffffff-tickstart_CAN+ticksotp_CAN;
	}
	else
		ticksotp_CAN=ticksotp_CAN-tickstart_CAN;

	if((ticksotp_CAN>250)&&(CAN_TX_Falg == 0))
	{
		tickstart_CAN = 0;
		ticksotp_CAN = 0;
	}
	else
	{
		return;
	}	
	//需要做参数关联
	//CAN_CustomerCode.gBatCode[0]= 0xff;              //电池厂商编码
	CAN_CustomerCode.gBatCode[0]= BMS_config.CustomerCode.gBatCode[0];              //电池厂商编码
	CAN_CustomerCode.gBatCode[1]= BMS_config.CustomerCode.gBatCode[1];               //电池厂商编码
	//	  CAN_CustomerCode.gBatCodeNumber[0]=0x3d;        //电池型号编码
	//		CAN_CustomerCode.gBatCodeNumber[1]=0x00;        //电池型号编码
	CAN_CustomerCode.gBatCodeNumber[0]=BMS_config.CustomerCode.gBatPack[0];        //电池型号编码
	CAN_CustomerCode.gBatCodeNumber[1]=BMS_config.CustomerCode.gBatPack[1];        //电池型号编码
	////////////////////////////////////
	CAN_CustomerCode.gVoltSum = BMS_config.BMS_Read.gVoltSum*100;           //总电压1mV  
	CAN_CustomerCode.gCurrent = BMS_config.BMS_Read.gCurrent*10;       //当前电流值 
	CAN_CustomerCode.gTemAvg = BMS_config.BMS_Read.gTemAvg;          //平均温度
	CAN_CustomerCode.gSOC = BMS_config.Pack_RW_config.gSOC;                      //电池包SOC   
	CAN_CustomerCode.gsCycleCnt= BMS_config.Pack_RW_config.gsCycleCnt;              //循环次数
	CAN_CustomerCode.gSOH= BMS_config.Pack_RW_config.gSOH;                 //电池包SOH
	memcpy(&CAN_CustomerCode.g_Volt_call,&BMSVoltTemperature.g_Volt_call,sizeof(CAN_CustomerCode.g_Volt_call));  
	CAN_CustomerCode.gSetPackCapacity = BMS_config.Pack_RW_config.gSetPackCapacity*10;    //电池包的设计容量
	CAN_CustomerCode.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity*10;   //剩余容量
	////////////需要填入真实的故障代码////////////////
	CAN_CustomerCode.gAlarm[0] = (gBitChgLowTemAlm|gBitChgLowTemAlmPrt|gBitDchgLowTemAlm|gBitDchgLowTemAlmPrt)|\
	((gBitChgHighTemAlm|gBitChgHighTemAlmPrt|gBitDchgHighTemAlm|gBitDchgHighTemAlmPrt)<<1)|\
	((gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt<<2))|\
	((gBitDchgOverCurrentAlm|gBitDchgOverCurrentAlmPrt)<<3)|\
	((gBitVoltDiff1MaxAlm)|(gBitVoltDiff1MaxAlmPrt)|(gBitVoltDiff2MaxAlm)|(gBitVoltDiff2MaxAlmPrt)<<6)|\
	((gBitOverVoltAlm|gBitOverVoltAlmPrt)<<7)|\
	((gBitUnderVoltAlm|gBitUnderVoltAlmPrt)<<8)|\
	(gBitChgShortCircuit<<9)|(gBitDchgShortCircuit<<10)|(gBitSOClow<<11)|(gBit_Chg_Error<<12);
	CAN_CustomerCode.gAlarm[1] = 0;

	/////////////////////////		
	//	memset(&CAN_CustomerCode.gXUHAO,0X55,sizeof(CAN_CustomerCode.gXUHAO));		
	memcpy(&CAN_CustomerCode.gXUHAO,&BMS_config.CustomerCode.gBatCodeNumber,sizeof(CAN_CustomerCode.gXUHAO)); //电池序号		
	CCITT_CRC16Init((u8 *)(&CAN_CustomerCode.gBatCode),(sizeof(CAN_CustomerCode)-2)); //计算效验值			
	CAN_CustomerCode.CRC16 = CCITT_CRC16;
	//////////////////可以去掉///////////////////
	//if(CAN_TX_Falg == 1)
	//	return ;
	/////////////////////////////////////					
	memcpy(CAN_T,&CAN_CustomerCode,sizeof(CAN_CustomerCode));  //软件版本号	

	CAN_T_LEN_SUM = sizeof(CAN_CustomerCode);
	gCAN_falg = 1;
	///////////////可以去掉
	//	if(CAN_RX_Falg == 0)
	//////////////////////////////////		

	CAN_TX_Falg = 1;

	CAN_id_add =CAN_ID2_OUT;//装入数据包29位ID	
		
		
}
/***************************************************************************
*   函 数 名: void CAN_Task(void)
*   功能说明: RcvBcmuMsgHandle处理接收到的数据，回复发送数据
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void CAN_Task(void)
{
//	char *hadd;
	
	u8 i = 0;
	static	u8 falg_uav_can = 0;
	static	u8 falg_boying_can = 0;
	const char *p = "UAVCAN";
		
	const char  *q ="BOYING";	
	
	char lene[10];
	//memcpy(&BMS_config.BMS_Read.RTC_DateTimeStructure,&RTC_DateTimeStructure_NOW,sizeof(RTC_DateTimeStructure_NOW));
	BMS_config.BMS_Read.RTC_DateTimeStructure =	RTC_DateTimeStructure_NOW;	
	//lene = &link[0];
	for(i=0;i<5;i++)
	{
		lene[i*2]= (u8)BMS_config.CustomerCode.gAircraftAgreement[i];
		lene[i*2+1]= (u8)(BMS_config.CustomerCode.gAircraftAgreement[i]>>8);
	}
	
	falg_uav_can = memcmp(p,lene,6);
	falg_boying_can = memcmp(q,lene,6);
	
	if(falg_uav_can == 0)
		CAN_CustomerCode_TX();	
	if(falg_boying_can == 0)
		CAN_Boying_Code_TX();
	
	RcvBcmuMsgHandle();							//CAN数据处理发送
	if((timer5>15)&&(ReadEvent_enable==1))
	{	
	//	CAN_id_add=CAN_ID1_OUT;//装入数据包29位ID
		ReadEventRecord_TX();					//历史数据串口发送
	}
 
}
/************************ 论坛地址 www.zxkjmcu.com ******************************/
