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

//��Ӧ��ͬ����ID
#define CAN_12_ID     ((u16)0x02D0) 
#define CAN_14_ID     ((u16)0x02D1) 
#define CAN_6_ID      ((u16)0x02D2) 
#define CAN_13_ID     ((u16)0x02D3) 
#define CAN_28_ID     ((u16)0x02D4)
//�����ID
#define CAN_Parameter_ID     ((u16)0x02DA)  //��������
#define CAN_Handshake_ID     ((u16)0x02E4)  //��������
#define CAN_SHA1_ID          ((u16)0x02E5)  //��Կд��
#define CAN_SHA2_ID          ((u16)0x02E6)  //���Ĳ�ѯ

#define Start_of_transfer 0x80
#define End_of_transfer 0x40
#define   quCurrent(x)    (x<0x80000000?x:(~(x-1)))
extern u16 CCITT_CRC16;
 u8 CAN_R[240]; //CAN���յ�����
static u8 CAN_R_LEN_SUM; //CAN�������ݵĸ���
 u8 CAN_T[240];  //CAN���͵�����
 u8 CAN_T_LEN_SUM = 0; //CAN���͵����ݸ���
u8 CAN_RX_Falg = 0;  //���ݽ�����ɱ�־λ
volatile u8 CAN_TX_Falg = 0;  //���ݷ��ͱ�־λ
static  u16  can_ID= 0;//
extern bool Is_UsartSend;					//ʹ���������ڷ���
volatile u8 Broadcast_Falg = 0;  //���Ƹ��±�־
unsigned int APP_FLG_ADDR=0x0801FC00;//APP��־λ���λ��
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
*   �� �� ��: CAN_GPIO
*   ����˵��: CAN��GPIO�ܽų�ʼ��
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void CAN_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����һ��GPIO_InitTypeDef���͵Ľṹ��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);	//PB�˿�ʱ��ʹ��                        											 

	/* CAN1 Periph clock enable */
	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_4); //����PB8���ù���
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_4); //����PB9���ù���
	
	/* Configure CAN pin: RX */									               // PB8
	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;//����ģʽ
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;//GPIO_PuPd_UP = 0x01,
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */									               // PB9
  
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF; //����ģʽ
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;//�������
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
	
	
	CAN_InitStructure.CAN_TTCM=DISABLE;         // ʱ�䴥��ͨ�Ž�ֹ
	CAN_InitStructure.CAN_ABOM=ENABLE;	        // �����˳������ж���λ��0���˳�
	CAN_InitStructure.CAN_AWUM=DISABLE;	        // �Զ�����ģʽ������sleep
	CAN_InitStructure.CAN_NART=ENABLE;	        // �Զ����´��ͱ��ģ�֪�����ͳɹ�
	CAN_InitStructure.CAN_RFLM=DISABLE;	        // FIFOû���������±��ĸ��Ǿɱ���
	CAN_InitStructure.CAN_TXFP=DISABLE;         // ���ͱ������ȼ�ȷ������־��
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal; // ����ģʽ

	//1M
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 3;

	CAN_Init(CAN, &CAN_InitStructure);	// ��ʼ��CAN1
	
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
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE); //�ж�ʹ��
}
/***************************************************************************
*   �� �� ��:void CAN_Recv_Control(CanRxMsg RxMessage)
*   ����˵��: CAN����recv�����������
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void CAN_Recv_Control(CanRxMsg RxMessage)  //CAN����recv��������ֵ˵��
//void CAN_Recv_Control(void)  //CAN����recv��������ֵ˵��
{
	uint8_t  CAN_Pack=0; //
	uint8_t  CAN_end=0;
	uint8_t  i;
//	CanRxMsg RxMessage;
//	RxMessage =RxMessage;
	static u8 CAN_Start_of_transfer_falg=0;//�Խ��յ���־
//		CanRxMsg RxMessage;
//	CAN_Receive(CAN,CAN_FIFO0,&RxMessage);
	//CAN����
	 g_Can_falg = 1;	
	if((RxMessage.ExtId==CAN_ID0_INT)&(CAN_RX_Falg == 0))
	{

		if(((RxMessage.Data[RxMessage.DLC-1])&(Start_of_transfer)) == 0x80)	//��ʼ����
		{
			for(i=0;i<RxMessage.DLC-1;i++)
				CAN_R[i]= RxMessage.Data[i];		
			CAN_Start_of_transfer_falg=1; 									//CAN���յ�֡ͷ��¼����				
		}
		else          														//���ݷŵ���������
		{
			CAN_Pack = RxMessage.Data[RxMessage.DLC-1] & 0x1f;
			if(CAN_Start_of_transfer_falg == 1) 							//���յ�֡ͷ��¼����
			{
				for(i=0;i<RxMessage.DLC-1;i++)
					CAN_R[i+CAN_Pack*7]= RxMessage.Data[i]; 
			}

		}
		CAN_end = (RxMessage.Data[RxMessage.DLC-1])&(End_of_transfer);
		if((CAN_end==End_of_transfer)&(CAN_Start_of_transfer_falg == 1))  	//�������
		{
			CAN_RX_Falg = 1;     											//CAN�������  
			CAN_R_LEN_SUM = RxMessage.DLC+CAN_Pack*7-1;
		}
			 		 
	 }
	if((RxMessage.ExtId==CAN_BOYING_IN)|(RxMessage.ExtId ==0x0005))
		gBOying_IN = 1;
			
			
 }

/***************************************************************************
*   �� �� ��: RcvBcmuMsgHandle
*   ����˵��: RcvBcmuMsgHandle������յ������ݣ��ظ���������
*   ��    �Σ���
*   �� �� ֵ: ����׼��OK ��������
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
	if(CAN_RX_Falg == 0) //û�н��յ������˳�
		return 0;	

	CCITT_CRC16Init(&CAN_R[2],CAN_R_LEN_SUM-2); //����Ч��ֵ	
	CRC16 =	CAN_R[0]+((u16)CAN_R[1]<<8);//����Ч�鲿��		
	if(CCITT_CRC16 != CRC16) //����Ч�鲻�ɹ��˳�
	{
		CAN_RX_Falg = 0;//�������
		return 0;	
	}


	addr = CAN_R[3]+((u16)CAN_R[4]<<8);
	dataLen = CAN_R[5];
	if(addr == 0x8000)//���յ�����BOOTָ��
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
			NVIC_SystemReset();	 //��λоƬ	
		}		
		else
		{
			FLASH_Unlock();//����
			flash_Operate_St = 	FLASH_ErasePage(APP_FLG_ADDR);
			FLASH_Lock();//��������	
			if(flash_Operate_St==FLASH_COMPLETE) 
				NVIC_SystemReset();	 //��λоƬ	
		}

	}
	if(CAN_R[2]==0)  //����ָ���
	{
		if((addr >= 0x4000) && (addr <= 0x4ffe))
		{
			tAlmPrtEvtHead_R.ReadEvent_loop = (addr & 0x3fff);
			BMS_Alm_Number_Write();
			Is_UsartSend=false;					//ʹ���������ڷ���
			ReadEventRecord();
			CAN_id_add=CAN_ID1_OUT;
			CAN_TX_Falg = 1;
			CAN_RX_Falg = 0;
			return 0 ;
		}
		else if((addr >= 0x2000) && ((addr + (dataLen/2)) <= 0x20BC))   //���ݴ�����
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
		else if(addr == 0x5000) //��ֵ�洢
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

		////////////��û�ظ���λ��ǰ���������
		//CAN_RX_Falg = 0;//�������
	}
	if(CAN_R[2]==1)  //дָ���
	{
		//	dataLen=0;
		if((addr >= 0x4000) && (addr <= 0x4ffe))
		{
			ReadEvent_enable=0;//��ֹ�ٵ��ô˺���
			tAlmPrtEvtHead_R.ReadEvent_Num = 0;
			tAlmPrtEvtHead_R.ReadEventAddr_st=0;
		}
		else if((addr >= 0x2021) && ((addr + (dataLen/2)) <=0x20BC))   //���ݴ�����
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
			BMS_config_Write();//д���ò���
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
		else if(addr == 0x230A)    //�¶�ϵ��У׼
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
		else if(addr == 0x230B)//������ϼ�¼������ʧЧ
		{
			if(dataLen == 0 )
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
				GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
				GPIO_SetBits(GPIOx_W5_6,LED_W6);	 //�Էŵ����
				g_FaultAlarmState[6]&= ~(1<<3);	
				gDchg_falg_PC = 0;
				BAT_State = FreeMode;  //��������ģʽ
			}
			if(CAN_R[6] == 0x00)
			{
				gDchg_falg_PC = 1;	
				GPIO_SetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
				GPIO_ResetBits(GPIOx_W5_6,LED_W6);	//���ȵ�
				g_FaultAlarmState[6]|= 1<<3;

			}								
		}
		else if(addr == 0x2310)
		{
			if(CAN_R[6] == 0xff)
			{

				BAT_State = Sleep;//ϵͳ�ı䵽����ģʽ
				gSleep_pc = 1;
			}
		}
		else if(addr == 0x2311)
		{
			if(CAN_R[6] == 0x55)
			gBitStorageFailure =	W25QXX_Demo();

		}
		
		////////�ظ�����λ������������	
		//	CAN_RX_Falg = 0;//�������

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
			CCITT_CRC16Init(&CAN_T[2],dataLen+4); 					//����Ч��ֵ
		}
		else	
		{
			CAN_T[5]= 0X01;
			CAN_T[6]= 0X01;
			CAN_T_LEN_SUM = 7;
			CCITT_CRC16Init(&CAN_T[2],5); 							//����Ч��ֵ	

			BMS_capacity.PackNewCapacity =(u32)(BMS_config.Pack_RW_config.gNewPackCapacity)*360000;//����������	

			if(BMS_config.Pack_RW_config.gSOC!=0)  					//������������
				BMS_capacity.BatCapacity = BMS_capacity.PackNewCapacity/100* BMS_config.Pack_RW_config.gSOC;
			else
				BMS_capacity.BatCapacity = 0;	
		}				 
		CAN_T[0]=	CCITT_CRC16&0x00ff;								//CRC16��λ
		CAN_T[1]=	(CCITT_CRC16>>8)&0x00ff;						//CRC16��λ	

		CAN_RX_Falg = 0;											//�������

		if((gBitStorageFailure )&&(CAN_R[2]==1))	
		{
			CAN_T_LEN_SUM = 0;
			CAN_TX_Falg = 0; 										//������
		}	
		else{
			CAN_id_add = CAN_ID1_OUT;								//װ�����ݰ�29λID
			CAN_TX_Falg = 1; 										//������
			///�������ͺ���������µ�����
		}
	}

	return 1 ;
}
/***************************************************************************
*   �� �� ��: CAN_TX_BcmuMsgHandle
*   ����˵��: CAN_TX_BcmuMsgHandle��������
*   ��    �Σ���
*   �� �� ֵ: ���ݷ���
***************************************************************************/
u8 CAN_TX_BcmuMsgHandle(void)
	{
		static CanTxMsg TxMessage;
	  static u8 can_pack=0;
		static	u8  can_send_cnt=0;//װ������
		u8  temp_i=0;
	 	if(CAN_T_LEN_SUM<240)
		{
					 if(CAN_TX_Falg==1)
					 {
							for(temp_i=0;temp_i<7;)
								{
										TxMessage.Data[temp_i]=CAN_T[can_send_cnt++];
										temp_i++;
										if(can_send_cnt>=CAN_T_LEN_SUM)//���͵������һ�� �ֽ�
										break;					
								}		
								if(can_send_cnt>=CAN_T_LEN_SUM)
								TxMessage.Data[temp_i]=0x40 |can_pack|((can_pack%2)<<5);
								else
								{
								TxMessage.Data[temp_i]=can_pack|((can_pack%2)<<5);
								}
								
								if(can_pack==0)///��һ�����ݴ���
								{
									if(can_send_cnt>=CAN_T_LEN_SUM)
										TxMessage.Data[temp_i]=0xc0;//װ��β�ֽ�
									else
										TxMessage.Data[temp_i]=0x80;//װ��β�ֽ�
								}	
								
								TxMessage.DLC=temp_i+1;
								TxMessage.IDE=CAN_ID_EXT;//RxMessage.IDE;
//								if((!gCAN_falg)||(ReadEvent_enable==1))
//								TxMessage.ExtId=CAN_ID1_OUT;//װ�����ݰ�29λID
//								else
//								TxMessage.ExtId=CAN_ID2_OUT;//װ�����ݰ�29λID	
								 TxMessage.ExtId=  CAN_id_add;
							 CAN_Transmit(CAN , &TxMessage);	//CAN ��������,����CAN���ͺ������з���һ֡����			
							 can_pack++;
							if(can_send_cnt>=CAN_T_LEN_SUM)//���ݷ�����ɱ�־
								{
									can_pack = 0;
									can_send_cnt = 0;
									CAN_T_LEN_SUM = 0;
									CAN_TX_Falg = 0;//���ݷ�����ɱ�־ 
									////////�����UAVCAN���ͽ��������UAVCAN������///////////
									if(gCAN_falg)
									gCAN_falg = 0;
									///////////////////
								}
					 }
      }
		return 1;
  }
	/***************************************************************************
*   �� �� ��: 	CAN_Boying_Code_TX(void)
*   ����˵��: ��ӥЭ����ⷢ��
*   ��    �Σ���
*   �� �� ֵ: ��
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
							boying_ID1.gVoltSum = BMS_config.BMS_Read.gVoltSum*10;            //�ܵ�ѹ1mV   
							boying_ID1.gCurrent= BMS_config.BMS_Read.gCurrent;       //��ǰ����ֵ  
							boying_ID1.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSOC*10;//BMS_config.Pack_RW_config.gSurpulsPackCapacity*10;   //ʣ������
							boying_ID1.gTemAvg = BMS_config.BMS_Read.gTemAvg*10;          //ƽ���¶�
							//boying_ID1.gbat_state = 0x00;          //���״̬
					
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
							memcpy(&boying_ID1.g_Volt_call,&BMSVoltTemperature.g_Volt_call,sizeof(BMSVoltTemperature.g_Volt_call));//��ص�ѹ									
							boying_ID1.CRC16 = Boying_CheckCRC16((u8 *)(&boying_ID1.gVoltSum),(sizeof(boying_ID1)-2)); //����Ч��ֵ			
								can_ID = CAN_14_ID;
								memcpy(CAN_T,&boying_ID1,sizeof(boying_ID1));  //����汾��	
								CAN_T_LEN_SUM = sizeof(boying_ID1);
								CAN_TX_Falg = 1;
						}	
					else	
						{
             num =0 ;						
						boying_ID2.gBatCode  =(u8)BMS_config.CustomerCode.gBatCode[0];           //��س��� 
						boying_ID2.gBatCodeNumber = (u8)BMS_config.CustomerCode.gBatPack[0];        //����ͺ�
						boying_ID2.Product_Version = 0x01; //Ӳ���ͺ�V1.01
						boying_ID2.SoftWare_Version = 0x01;//����汾V01
						boying_ID2.gSOH = BMS_config.Pack_RW_config.gSOH;                  //��ذ�SOH
						boying_ID2.gDCHG_Dynameter = 10;       //��صķŵ籶��
						boying_ID2.gSetPackCapacity = BMS_config.Pack_RW_config.gSetPackCapacity/10;    //��ذ����������
						boying_ID2.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity/10;   //ʣ������
						boying_ID2.gMAX_Power_W = 6000;   //�������W��
						boying_ID2.gVolt_standard = 58500;           //��Ƶ�ѹ
						boying_ID2.gVoltAlm = BMS_config.OverVolt.gVoltAlm;           //����澯ֵ
						boying_ID2.gsCycleCnt = BMS_config.Pack_RW_config.gsCycleCnt;               //ѭ������	
						boying_ID2.gVoltOverNumber = Alm_Number.OverVoltNumber;       //�������
						boying_ID2.gVoltUnderNumber = Alm_Number.UnderVoltNumber;       //���Ŵ���
						boying_ID2.gHighTemNumber = Alm_Number.ChgHighTemNumber;        //���´���
						boying_ID2.gOverCurrenNumber = Alm_Number.ChgOverCurrenNumber;        //��������
						boying_ID2.gChg_IlleqaltNumber = Alm_Number.InformalChgMosNumber;	       //�Ƿ�������  		
						boying_ID2.CRC16 = Boying_CheckCRC16((u8 *)(&boying_ID2.gBatCode),(sizeof(boying_ID2)-2)); //����Ч��ֵ	
							can_ID = CAN_Parameter_ID;
							memcpy(CAN_T,&boying_ID2,sizeof(boying_ID2));  //����汾��	
							CAN_T_LEN_SUM = sizeof(boying_ID2);					
							CAN_TX_Falg = 1;		
						}
	}
	/***************************************************************************
*   �� �� ��: CAN_TX_BoyingMsgHandle
*   ����˵��: CAN_TX_BoyingMsgHandle��������
*   ��    �Σ���
*   �� �� ֵ: ���ݷ���
***************************************************************************/
u8 CAN_TX_BoyingMsgHandle(void)
	{
		static CanTxMsg TxMessage;
	  static u8 can_pack=0;
		static	u8  can_send_cnt=0;//װ������
		static  u8  can_pack_stop = 0;//��������־

    		u8  temp_i=0;	
				if(CAN_T_LEN_SUM<240)
				{
					 if(CAN_TX_Falg == 1)
					 {
							for(temp_i=0;temp_i<8;)
								{
										TxMessage.Data[temp_i]=CAN_T[can_send_cnt++];
										temp_i++;
										if(can_send_cnt>=CAN_T_LEN_SUM)//���͵������һ�� �ֽ�
										break;					
								}		

								if((can_pack*8+8) >= CAN_T_LEN_SUM)
								{
									can_pack_stop = 1;
								}
								
								TxMessage.DLC=temp_i;
								TxMessage.IDE=CAN_ID_EXT;//RxMessage.IDE;
						
								TxMessage.ExtId=3|can_pack_stop<<3|can_pack<<4|1<<10|2<<17|can_ID<<19;//װ�����ݰ�29λID			
							 CAN_Transmit(CAN , &TxMessage);	//CAN ��������,����CAN���ͺ������з���һ֡����			
							 can_pack++;
							if(can_send_cnt>=CAN_T_LEN_SUM)//���ݷ�����ɱ�־
								{
									can_pack = 0;
									can_pack_stop = 0 ;
									can_send_cnt = 0;
									CAN_T_LEN_SUM = 0;
									CAN_TX_Falg = 0;//���ݷ�����ɱ�־ 
									gBying  = 0;
								}
					 }
				 }
				return 1 ;
  }
	
	/***************************************************************************
*   �� �� ��: 	void CAN_CustomerCode_TX(void)
*   ����˵��: ���Э����ⷢ��
*   ��    �Σ���
*   �� �� ֵ: ��
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
	//��Ҫ����������
	//CAN_CustomerCode.gBatCode[0]= 0xff;              //��س��̱���
	CAN_CustomerCode.gBatCode[0]= BMS_config.CustomerCode.gBatCode[0];              //��س��̱���
	CAN_CustomerCode.gBatCode[1]= BMS_config.CustomerCode.gBatCode[1];               //��س��̱���
	//	  CAN_CustomerCode.gBatCodeNumber[0]=0x3d;        //����ͺű���
	//		CAN_CustomerCode.gBatCodeNumber[1]=0x00;        //����ͺű���
	CAN_CustomerCode.gBatCodeNumber[0]=BMS_config.CustomerCode.gBatPack[0];        //����ͺű���
	CAN_CustomerCode.gBatCodeNumber[1]=BMS_config.CustomerCode.gBatPack[1];        //����ͺű���
	////////////////////////////////////
	CAN_CustomerCode.gVoltSum = BMS_config.BMS_Read.gVoltSum*100;           //�ܵ�ѹ1mV  
	CAN_CustomerCode.gCurrent = BMS_config.BMS_Read.gCurrent*10;       //��ǰ����ֵ 
	CAN_CustomerCode.gTemAvg = BMS_config.BMS_Read.gTemAvg;          //ƽ���¶�
	CAN_CustomerCode.gSOC = BMS_config.Pack_RW_config.gSOC;                      //��ذ�SOC   
	CAN_CustomerCode.gsCycleCnt= BMS_config.Pack_RW_config.gsCycleCnt;              //ѭ������
	CAN_CustomerCode.gSOH= BMS_config.Pack_RW_config.gSOH;                 //��ذ�SOH
	memcpy(&CAN_CustomerCode.g_Volt_call,&BMSVoltTemperature.g_Volt_call,sizeof(CAN_CustomerCode.g_Volt_call));  
	CAN_CustomerCode.gSetPackCapacity = BMS_config.Pack_RW_config.gSetPackCapacity*10;    //��ذ����������
	CAN_CustomerCode.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity*10;   //ʣ������
	////////////��Ҫ������ʵ�Ĺ��ϴ���////////////////
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
	memcpy(&CAN_CustomerCode.gXUHAO,&BMS_config.CustomerCode.gBatCodeNumber,sizeof(CAN_CustomerCode.gXUHAO)); //������		
	CCITT_CRC16Init((u8 *)(&CAN_CustomerCode.gBatCode),(sizeof(CAN_CustomerCode)-2)); //����Ч��ֵ			
	CAN_CustomerCode.CRC16 = CCITT_CRC16;
	//////////////////����ȥ��///////////////////
	//if(CAN_TX_Falg == 1)
	//	return ;
	/////////////////////////////////////					
	memcpy(CAN_T,&CAN_CustomerCode,sizeof(CAN_CustomerCode));  //����汾��	

	CAN_T_LEN_SUM = sizeof(CAN_CustomerCode);
	gCAN_falg = 1;
	///////////////����ȥ��
	//	if(CAN_RX_Falg == 0)
	//////////////////////////////////		

	CAN_TX_Falg = 1;

	CAN_id_add =CAN_ID2_OUT;//װ�����ݰ�29λID	
		
		
}
/***************************************************************************
*   �� �� ��: void CAN_Task(void)
*   ����˵��: RcvBcmuMsgHandle������յ������ݣ��ظ���������
*   ��    �Σ���
*   �� �� ֵ: ��
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
	
	RcvBcmuMsgHandle();							//CAN���ݴ�����
	if((timer5>15)&&(ReadEvent_enable==1))
	{	
	//	CAN_id_add=CAN_ID1_OUT;//װ�����ݰ�29λID
		ReadEventRecord_TX();					//��ʷ���ݴ��ڷ���
	}
 
}
/************************ ��̳��ַ www.zxkjmcu.com ******************************/
