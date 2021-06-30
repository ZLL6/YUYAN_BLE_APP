//=============================================================================
//				           �����и����յ�����޹�˾
//                         ���Ӳ�Ʒ��			     
//-----------------------------------------------------------------------------
//�ļ�����: Main.c
//�ļ���ʶ:
//ժҪ    : 
//-----------------------------------------------------------------------------
//��ǰ�汾: V1.0
//����    : dangxinkai
//�������: 2021.02.25
//=============================================================================
		#include "pbdata.h"
		#include "data.h"
		#include "soc.h"
		#include "Uart.h"
		#include "stdbool.h"
		#define DUBAG 1
		extern R_CAN PC_CAN_Data_R;//can�������ݻ�����
		extern volatile u8 Broadcast_Falg;  //���Ƹ��±�־
		//gBatCodeNumber
		//unsigned char Broadcast_name[]="gerpow";
    unsigned char BT_RESET[]="TTM:RST-SYS";
		unsigned char broadcast_cmd[]="TTM:REN-";
		unsigned char broadcast_name[16]="123456789";
		u16 broadcast_name1[8];
		unsigned char cnt=0;
		//BMS_config.CustomerCode.gBatCodeNumber[8]
		u8 SEELP_falg = 0;//ϵͳ����ָʾ��־
		u16 SEELP_cnt = 0;//ϵͳ����ָʾ��־
		BAT_STATE_enum BAT_State = ReadyMode;//ϵͳ����ģʽ
		BAT_STATE_enum BAT_State_old = ReadyMode;//ϵͳ����ģʽ����
		RTC_DateTimeTypeDef RTC_DateTimeStructure_NOW;	//RTCʵʱʱ�����ݻ���
///////////////////////////////////////////////////
		void BSP_Configuration(void);//Ӳ����ʼ����������
		void RCC_Configuration(void);//ʱ�ӳ�ʼ����������
		void NVIC_Configuration(void);//�жϳ�ʼ����������
		void TIM3_Configuration(void);
		void TIM2_Configuration(void);
		void Dchg_Chg_Mode(void); //���ģʽ
		void Alternate_Mode(void);   //ģʽ�л� 
		void Free_Mode(void);   //����ģʽ
		void Balance_Mode(void);   //����ģʽ
		void Self_Dchg_Mode(void);   //�Էŵ�ģʽ
		void Sleep_Mode(void);   //����ģʽ
		void Power_Dowm_Mode(void);   //����ģʽ
		void open_vvvvv(void);    //��·��ѹ���
		void Bat_State(void);
		void PVD_Init(void);  //��ѹ�����жϿ���
void GPIO_LED_dely(void)
{
	u8 i = 0;
	for(i = 0; i < 10; i++)
	{
		LED_1_4_ON;LED_ON;
		delay_ms(200);
		LED_1_4_OFF;LED_OFF;
		delay_ms(200);
	}
	

}
int main(void)
{
	memcpy((uint32_t*)0x20000000,(uint32_t*)0x08004000,48*4);//�ض����ж���������Ҫ����BOOTLOADER����
	SYSCFG->CFGR1|=0X03;//���ô�SRAM��������Ҫ����BOOTLOADER
	BSP_Configuration();   //����Ӳ����ʼ������
	RTC_Initializes();     //RTC��ʼ��
	GPIO_ENABLE(ENABLE);//IO�ڳ�ʼ����ʹ��
	//					delay_ms(1000);
	//					delay_ms(1000);//��������ʱ����Ҫ�ȴ�ǰ�˲ɼ�оƬ׼������ 
	GPIO_LED_dely();
	BQ769X0Init();         //BQ76940��ʼ�� 
	Board_int(); 
	//��ʼ��Ĭ�ϲ���
	Uart_Init(115200);
	CAN_enable= 1;
	Broadcast_Falg=1;
	//				  for(cnt=0 ; cnt<8; cnt++)//ʹ��PACK��������Ϊ�����㲥����
	//							{
	//			         broadcast_name[2*cnt]=(unsigned char)BMS_config.CustomerCode.gBatCodeNumber[cnt];
	//	             broadcast_name[2*cnt+1]=(unsigned char)(BMS_config.CustomerCode.gBatCodeNumber[cnt]>>8); 
	//							}
	while(1)
	{
		if(Broadcast_Falg==1)//�в����궨�����������㲥����
		{
			Broadcast_Falg=0;
			delay_ms(100);
			memcpy(&broadcast_name1,&BMS_config.CustomerCode.gBatCodeNumber,16);	
			for(cnt=0 ; cnt<8; cnt++)//ʹ��PACK��������Ϊ�����㲥����
			{
				broadcast_name[2*cnt]=broadcast_name1[cnt];
				broadcast_name[2*cnt+1]=broadcast_name1[cnt]>>8;
			}
			USART_Send_string(broadcast_cmd,8);
			delay_ms(1);
			USART_Send_string(broadcast_name,16);
			delay_ms(100);
			USART_Send_string(broadcast_cmd,8);
			delay_ms(1);
			USART_Send_string(broadcast_name,16);
			delay_ms(100);
			USART_Send_string(BT_RESET,11);
			delay_ms(100);
			USART_Send_string(BT_RESET,11);

		}
		RTC_GetDateTime(&RTC_DateTimeStructure_NOW);   //��ȡ��ǰʱ��	
		Bat_State();   //��ѭ������           //
		WriteEventEeprom();       //�洢��ʷ����
#if(DUBAG == 1)
		IWDG_ReloadCounter();  //�忴�Ź�
#endif
		USART_FRAME_PROCESS();	
	}
}
//=============================================================================
//��������: void BSP_Configuration(void)//Ӳ����ʼ������
//�������: 
//�������: ��
//����ֵ  : ��
//��������: ��ʼ��Ӳ����Դ
//ע������:
//=============================================================================		
		void BSP_Configuration(void)//Ӳ����ʼ������
		{	  
			RCC_Configuration();	//��ʼ��RCC	
			NVIC_Configuration();//�����ж����ȼ���ʼ������
			#if(DUBAG == 1)
			WWDG_Config();       //��ʼ�����Ź�
			#endif
			CAN_GPIO();          //��ʼ�� CAN GPIO 
			CAN_Configation();   //��ʼ��CAN��
			LED_GPIO();	         //��ʼ�� CAN GPIO 
			TIM3_Configuration();  //��ʼ�� ��ʱ��3
			TIM2_Configuration();   //��ʼ�� ��ʱ��2
			SPI1_GPIO();//����SPI��׼ͨѶ��ʽ��ʼ������
			SPI1_Configation();//����SPIͨѶ��ʽ���ú���
			W25QXX_GPIO();//����25Qxx���ƹܽų�ʼ������
			PVD_Init();	    //��ʼ����Ƭ���͵�ѹ��⹦��
		}
//=============================================================================
//��������: void RCC_Configuration(void)//ʱ�ӳ�ʼ������
//�������: ��
//�������: ��
//����ֵ     :
//��������: ��ʼ��ϵͳʱ��
//ע������:
//=============================================================================
	void RCC_Configuration(void)//ʱ�ӳ�ʼ������
			{  
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
				//RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//����SYSCFG����ʹ��
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//����PWR����ʹ��
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ��TIM1ʱ��
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ��TIM2ʱ��
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//����SPIʱ��ʹ��
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);//CAN�˿�ʱ��ʹ��
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
			}
//=============================================================================
//��������: void PVD_Init(void)
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: ��ʼ����Ƭ���͵�ѹ���
//ע������:
//=============================================================================			
		void PVD_Init(void)
			{
				 PWR_PVDLevelConfig(PWR_PVDLevel_2); //�͵�ѹ��λ����
				 PWR_PVDCmd(ENABLE);
			}
			
//=============================================================================
//��������: void TIM3_Configuration(void)
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: ����1MS�Ķ�ʱ
//ע������:
//=============================================================================						
void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//����һ��TIM_InitTypeDef���͵Ľṹ��
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//���жϱ�־���Ա��´��жϵ���ʹ��
	TIM_TimeBaseStructure.TIM_Period =3;//1���ӻ�2000������ 20ms�ж�һ�� 40  10ms�ж�һ�� 20  1ms�ж�һ�� 2
	TIM_TimeBaseStructure.TIM_Prescaler =11999; //24000��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision =0; //TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//��ʼ����ʱ��
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//ʹ������ж�
	TIM_Cmd(TIM3,ENABLE);//��ʱ��ʹ��
}
//=============================================================================
//��������: void TIM2_Configuration(void)
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: ����10MS�Ķ�ʱ
//ע������:
//=============================================================================		
void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//����һ��TIM_InitTypeDef���͵Ľṹ��
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//���жϱ�־���Ա��´��жϵ���ʹ��
	TIM_TimeBaseStructure.TIM_Period =20;//1���ӻ�2000������ 20ms�ж�һ�� 40  10ms�ж�һ��  20
	TIM_TimeBaseStructure.TIM_Prescaler =23999; //24000��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1; //TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//��ʼ����ʱ��
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//ʹ������ж�
	TIM_Cmd(TIM2,ENABLE);//��ʱ��ʹ��
}
//=============================================================================
//��������: static void NVIC_Configuration(void)
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: �����жϵ����ȼ�
//ע������:
//=============================================================================		
static void NVIC_Configuration(void)//�ж����ȼ���ʼ������
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	/////////////////////////
	EXTI_DeInit();
	EXTI_StructInit(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line16; //PVD
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;   //
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//CAN
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//TIM1
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//TIM2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// wkup  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//	//RTC
	//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	//	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(0xFFFFFFFF);            //����ⲿ�жϱ�־
}
//=============================================================================
//��������: sleep_soc_up(void)
//�������: ��
//�������: ��
//����ֵ  : ����0 ����SOC���¼�ʱ����;����1 SOC���³ɹ� ����2 SOC����ʱ�䲻��
//��������: ��������ʱ����SOC����SOC������ֵ��OCV�ȶ�ֵ�������3%��ʱ�򣬸���SOC
//ע������: 
//=============================================================================		
			u8 sleep_soc_up(void)
			{
				static u8 dealy_Hour_staty=0;
				static u8 dealy_Hour_stop=0;
				static u8 dealy_Hour_UP=0;
				static u8 Up_falg=0;
				u16 soc=0;
				if(gSOC_OCVupdata_falsh==0)//ֻҪ�г�ŵ��˳���·��ѹ���
				{
				 Up_falg = 0;
				 dealy_Hour_staty = 0;
				 dealy_Hour_stop = 0;	
				}
				
				if(Up_falg == 0)//��δ��ʱ��ʼ
				{
					dealy_Hour_staty=RTC_DateTimeStructure_NOW.Hour;
					Up_falg = 1;//������ʱ
					return 0;
				}
					else
					{
							if(RTC_DateTimeStructure_NOW.Hour >= dealy_Hour_staty)
							dealy_Hour_stop=RTC_DateTimeStructure_NOW.Hour-dealy_Hour_staty;
							else
							dealy_Hour_stop=RTC_DateTimeStructure_NOW.Hour+24-dealy_Hour_staty;	
							dealy_Hour_UP= dealy_Hour_UP+	dealy_Hour_stop;
							dealy_Hour_stop = 0;
							dealy_Hour_staty =  RTC_DateTimeStructure_NOW.Hour;
						  if(dealy_Hour_UP>24)//��ֹʱ�����24Сʱ
							{
									if(BMS_capacity.FCC_UPdatat) //FCC�����Ѿ�����
									{
											if(BMS_capacity.charge>36000000) //����Ѿ����й���磬��û������
											{
												BMS_capacity.FCC_UPdatat = 0;//ֹͣ����FCC����	
												BMS_capacity.charge = 0; 
											}
										
									}
									/////////////////�ͷű�־λ���ȴ���һ�θ���
									Up_falg = 0;
									dealy_Hour_staty = 0;
									dealy_Hour_stop = 0;	
									dealy_Hour_UP=  0;
									////////////////////////////
									if(BMS_config.Pack_RW_config.gSOC>95)//�㷨SOC����95%
									soc =openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMax);//��������ѹ���㿪·��ѹSOC����
									else
									soc =openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMin);//������С��ѹ���㿪·��ѹSOC����
									soc=soc/10;//ת��1%�ֱ���
									/////////////////////////////////////
									if(BMS_config.Pack_RW_config.gSOC>soc)
									soc =BMS_config.Pack_RW_config.gSOC-soc;
									else
									soc =soc-BMS_config.Pack_RW_config.gSOC;
									//////////////////////////////////////////
									if(soc>3)//��·��ѹ�����SOC������㷨SOCƫ�����3%��ʹ��OCV��Ӧ��SOC�����滻
									{
									 BMS_capacity.SOC = openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMin);//��·��ѹSOC����
									 BMS_config.Pack_RW_config.gSOC = (u8)(BMS_capacity.SOC/10);        //��ذ�SOC ����
									 BMS_capacity.BatCapacity =  BMS_capacity.PackNewCapacity/100*BMS_config.Pack_RW_config.gSOC; //��ذ�������	
									 BMS_config.Pack_RW_config.gSurpulsPackCapacity = (u16)(BMS_capacity.BatCapacity/100/3600);  //ʣ������ 0.1AH	
									}
									return 1;
								}
								
								return 2;
					}
						
			}
//=============================================================================
//��������: void gSOCupdata_falsh_Write()
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: �������󣬵����±�־������󣬸��µ�ص���������������������ʣ������ SOH,SOC������
//ע������: 
//=============================================================================		
		void gSOCupdata_falsh_Write()
		{
					if(gSOCupdata_falsh)//gSOCupdata_falshΪ1���������һ��FLASH���ݣ�������������
					{
						 gSOCupdata_falsh = 0;
						 BMS_config.Pack_RW_config.gNewPackCapacity =  BMS_capacity.PackNewCapacity/360000;//�������µ������
						 BMS_config.Pack_RW_config.gSurpulsPackCapacity  = BMS_config.Pack_RW_config.gNewPackCapacity; //������������ʣ������         
						 BMS_config.Pack_RW_config.gSOH = BMS_config.Pack_RW_config.gNewPackCapacity*100/BMS_config.Pack_RW_config.gSetPackCapacity;
						 BMS_config.Pack_RW_config.gSOC = 100;
						 BMS_config_Write();//д���ò���
					}		
		}	
	//=============================================================================
//��������: void Chg_Mode(void) 
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: �����ŵ�ʱ��		�ۻ��������  дFALSH  û�е���ʱ��������ģʽ
//ע������: 
//=============================================================================		
		void Chg_Mode(void) //���ģʽ 
		{
			if(BAT_STATE_Curren == 0)
			{
			
				if((BAT_State_old == Sleep)&&(gSOCupdata_falsh))
				{
				  CAN_enable = 0;
						delay_ms(35);
					BAT_State = Sleep;  //��������ģʽ
				}
				else
				{
					 BAT_State = FreeMode;  //��������ģʽ
				}
					
		 }
		}
//=============================================================================
//��������: void Dchg_Chg_Mode(void) 
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: �����ŵ�ʱ��		�ۻ��������  дFALSH  û�е���ʱ��������ģʽ
//ע������: 
//=============================================================================		
		void Dchg_Chg_Mode(void) //���ģʽ 
		{
				GPIO_SetBits(GPIOB, LNVCC_EN);   //��5v��Դ
				socCalculateTime(); //�����ŵ�ʱ��
			 if(BAT_STATE_Curren==1)//����ۻ���������
				{										
					if(BMS_capacity.Bat_charge>36000000)
					{
						BMS_capacity.Bat_charge =	BMS_capacity.Bat_charge-36000000;
						BMS_config.Pack_RW_config.gCHGPackCapacity+=1;	
						BMS_config.Pack_RW_config.gsCycleCnt =	BMS_config.Pack_RW_config.gCHGPackCapacity*1000/( BMS_config.Pack_RW_config.gSetPackCapacity*8);
					}
				}		
       Chg_Mode();				
			 gSOCupdata_falsh_Write();	//��������������д��FALSH��
				//���������ж�
//			 if(BAT_STATE_Curren==0) //ֻҪû�е���������
//			 BAT_State = FreeMode;  //��������ģʽ
		}
//=============================================================================
//��������: void Alternate_Mode(void) 
//�������: ��
//�������: ��
//����ֵ  : ��
//��������: û�е����� ��ʼ��¼�������Էŵ���ʼ�ĸ���ʱ��    �ھ���ʱ�����Էŵ� 
//ע������: 
//=============================================================================		
void Alternate_Mode(void)   //ģʽ�л� 
{ 
	u8 i=0;
	u32 ChannelBuf = 0; 
	////////////////// ���⿪��ʱ�����
	static bool is_balance = false;
	static u8 time_start=0;
	static u8 time_differ=0;
	static u16 dealy_gBatBalance_Event_Hour = 0;
	////////////////�Էŵ�ʱ�����
	static u8 Falg_gSelf_Hour = 0;
	static u8 dealy_gSelf_Hour_staty=0;
	static u8 dealy_gSelf_Hour_stop=0;	
	static u16 dealy_gSelf_Event_Hour = 0;
	//////////////////////////////////////////////
	if(BAT_STATE_Curren == 0)  //����ʱ�������������Էŵ�
	{
		if(!is_balance)	//�������⾲ֹʱ�����
		{
			time_start = RTC_DateTimeStructure_NOW.Hour;
			is_balance = true;
			//dealy_Hour_staty = RTC_DateTimeStructure_NOW.Minute;
			//Falg_Hour = 1;

		}
		else
		{
			//////////////////////////////////////�������/////////////////////////////////////////////////////////////////////////												
			if(RTC_DateTimeStructure_NOW.Hour >= time_start)
				time_differ=RTC_DateTimeStructure_NOW.Hour-time_start;//��ǰʱ����ϴν�����ʱ�䣬���ʱ����
			else
				time_differ=RTC_DateTimeStructure_NOW.Hour+24-time_start;	 
			dealy_gBatBalance_Event_Hour = dealy_gBatBalance_Event_Hour + time_differ;    //����ʱ���ֹʱ��Խ��
			time_differ = 0 ;
			time_start =  RTC_DateTimeStructure_NOW.Hour;
			////////////////////////////////////////							
			if(BMS_config.Pack_RW_config.gBatBalanceStateTime < 1 ) //��������ʱ��궨С��1Сʱ������1Сʱ
				BMS_config.Pack_RW_config.gBatBalanceStateTime = 1;


			//if(RTC_DateTimeStructure_NOW.Minute >= dealy_Hour_staty)
			//dealy_Hour_stop=RTC_DateTimeStructure_NOW.Minute-dealy_Hour_staty;//��ǰʱ����ϴν�����ʱ�䣬���ʱ����
			//else
			//dealy_Hour_stop=RTC_DateTimeStructure_NOW.Minute+60-dealy_Hour_staty;	 
			//dealy_gBatBalance_Event_Hour = dealy_gBatBalance_Event_Hour + dealy_Hour_stop;    //����ʱ���ֹʱ��Խ��
			//dealy_Hour_stop = 0 ;
			//dealy_Hour_staty =  RTC_DateTimeStructure_NOW.Minute;
			//								
			//////////////////////////////////////////							
			//if(BMS_config.Pack_RW_config.gBatBalanceStateTime < 1 ) //��������ʱ��궨С��1Сʱ������1Сʱ
			//BMS_config.Pack_RW_config.gBatBalanceStateTime = 1;


			///////////////////////////////////////////////////////////									
			if((dealy_gBatBalance_Event_Hour>BMS_config.Pack_RW_config.gBatBalanceStateTime)&&(gBalance_UP == 0)) //��ֹʱ����ھ��⿪���궨ʱ�䣬�Ҿ�ֹ����û��������
			{
				if(BMS_config.Pack_RW_config.gBatBalanceVoltStart < BMS_config.BMS_Read.gVoltMin) //��С��ѹ���ھ��⿪����ѹ�����궨�����ѹ							
				{
					for(i = 0; i < gCellNum; i++)
					{
						if((((BMSVoltTemperature.g_Volt_call[i] - BMS_config.BMS_Read.gVoltMin) >= BMS_config.Pack_RW_config.gBatBalanceDiffVoltState) && (BMSVoltTemperature.g_Volt_call[i] >= BMS_config.Pack_RW_config.gBatBalanceVoltStart)))//�ýڵ�ѹ�ﵽ���⿪������
						{
							ChannelBuf |= ((u32)1 << i); //�þ���ͨ��������־
						}
						else
						{
							ChannelBuf &= ~((u32)1 << i); //�������ͨ��
						}
					}

					//for(i = 0; i < gCellNum; i++)
					//{
					//		if(((BMSVoltTemperature.g_Volt_call[i] - BMS_config.BMS_Read.gVoltMin) <= BMS_config.Pack_RW_config.gBatBalanceDiffVoltEnd)||BMSVoltTemperature.g_Volt_call[i]<BMS_config.Pack_RW_config.gBatBalanceVoltEnd)) //����ر�
					//		{
					//			ChannelBuf &= ~((u32)1 << i); //�������ͨ��
					//		}
					//}	

					if(ChannelBuf !=0)
					{
						BAT_State = BalanceMode;				//����ģʽ	
					}				
				}

			}	
			//////////////////////////////////////////////////////////			
			if(gBalance_UP ) //����������ʱ�����־���ھ���ģʽ�����л��жϾ����Ƿ������������ñ�־�ᱻ��1
			{
				gBalance_UP = 0;//���������ڶ��ξ���ʱ�����
				is_balance = false;
				time_start=0;
				time_differ=0;	
				dealy_gBatBalance_Event_Hour = 0;  //��������Էŵ��ʱ��
			}
								
		}
		////////////////////////////////////////�Էŵ��ʱ///////////////////////////////////////////////////////////////////////											
		if(Falg_gSelf_Hour == 0)	//��δ�����Էŵ�ȴ�ʱ�����
		{
			dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Hour;	//���浱ǰʱ��							
			Falg_gSelf_Hour = 1;//�����Էŵ�ʱ�����
			//dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Minute;	//���浱ǰʱ��							
			//Falg_gSelf_Hour = 1;//�����Էŵ�ʱ�����
		}
		else//�Էŵ�ʱ������Ѿ���ʼ
		{
			//////////////////////////////////////�Էŵ����/////////////////////////////////////////////////////////////////////////												
			if(RTC_DateTimeStructure_NOW.Hour >= dealy_gSelf_Hour_staty)//ʱ������24Сʱ��
				dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Hour-dealy_gSelf_Hour_staty;
			else
				dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Hour+24-dealy_gSelf_Hour_staty;					
			dealy_gSelf_Event_Hour = dealy_gSelf_Event_Hour + dealy_gSelf_Hour_stop;   //����ʱ���ֹʱ��Խ��
			dealy_gSelf_Hour_stop = 0;      
			dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Hour;
			///////////////////////////////////////////////////						
			if(BMS_config.Pack_RW_config.gSelfDchgStartTime<1)//�궨�Էŵ�ȴ�ʱ��С��1Сʱ����Ĭ�ϳ�һСʱ
				BMS_config.Pack_RW_config.gSelfDchgStartTime = 1 ;

			//if(RTC_DateTimeStructure_NOW.Minute >= dealy_gSelf_Hour_staty)//ʱ������24Сʱ��
			//dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Minute-dealy_gSelf_Hour_staty;
			//else
			//dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Minute+60-dealy_gSelf_Hour_staty;					
			//dealy_gSelf_Event_Hour = dealy_gSelf_Event_Hour + dealy_gSelf_Hour_stop;   //����ʱ���ֹʱ��Խ��
			//dealy_gSelf_Hour_stop = 0;      
			//dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Minute;
			/////////////////////////////////////////////////////						
			//if(BMS_config.Pack_RW_config.gSelfDchgStartTime<1)//�궨�Էŵ�ȴ�ʱ��С��1Сʱ����Ĭ�ϳ�һСʱ
			//BMS_config.Pack_RW_config.gSelfDchgStartTime = 1 ;




			if((dealy_gSelf_Event_Hour>BMS_config.Pack_RW_config.gSelfDchgStartTime)&&(gSelf_Dchg_UP == 0)) //��ֹʱ������趨ʱ�䣬���Էŵ���δ��
			{
				if(BAT_State != BalanceMode) //��ǰģʽ���ھ���ģʽ������û�п����������Էŵ�
				{
					if(BMS_config.Pack_RW_config.gSelfDchgStartVolt< BMS_config.BMS_Read.gVoltMin)//��С��ѹ�����Էŵ翪����ѹ
					{
						BAT_State = Self_DchgMode;  	//�л����Էŵ�ģʽ	
					}																		 
				}
			}	
			if(gSelf_Dchg_UP ) //�Էŵ���������Էŵ�ģʽ�����У��Էŵ������ñ�־λ�ᱻ��1
			{
				gSelf_Dchg_UP = 0;
				Falg_gSelf_Hour = 0;
				dealy_gSelf_Hour_staty=0;
				dealy_gSelf_Hour_stop=0;	
				dealy_gSelf_Event_Hour = 0; //��������Էŵ��ʱ��
			}											


		}

	}
	else//�г����߷ŵ磬���������Էŵ��ʱ
	{
		is_balance = false;
		time_start=0;
		time_differ=0;		
		Falg_gSelf_Hour = 0;
		dealy_gSelf_Hour_staty=0;
		dealy_gSelf_Hour_stop=0;	
		dealy_gSelf_Event_Hour = 0;//��������Էŵ��ʱ��
		dealy_gBatBalance_Event_Hour = 0;  //��������Էŵ��ʱ��

	}	 
		 
			
}
//=============================================================================
//��������: ��ֹģʽ��ִ�к���
//�������: 
//�������: ��
//����ֵ     : 
//��������: 
//ע������:
//=============================================================================
void Free_Mode(void)   //����ģʽ
{
	//////���ڼ��㾲ֹʱ��//////////
	static u8 Time_StaticStart=0;
	static u8 Time_Differ=0;
	static u8 Time_Total=0;
	static bool Is_StaticMode=false;							//�Ƿ��Ǿ���ģʽ��־
	static u8 ForcedShut = 0;									//�����������

	//////////////////////////////////////////////
	if(!Is_StaticMode) 											//��δ������ֹʱ����� 
	{
		Time_StaticStart=RTC_DateTimeStructure_NOW.Second;		//���浱ǰʱ����
		Is_StaticMode = true;									//������ֹʱ�����
	}
	else														//�Ѿ������˾�ֹʱ�����
	{	//////////////////////���㾲ֹʱ��////////////////////////					
		if(RTC_DateTimeStructure_NOW.Second >= Time_StaticStart)
			Time_Differ=RTC_DateTimeStructure_NOW.Second - Time_StaticStart;
		else
			Time_Differ=RTC_DateTimeStructure_NOW.Second + 60 - Time_StaticStart;												
		Time_Total =  Time_Total+ Time_Differ;
		Time_Differ = 0;
		Time_StaticStart = RTC_DateTimeStructure_NOW.Second;
		//////////////////////////////////////////////////////////													
		if(BMS_config.Pack_RW_config.gStaticSleepTimer<5)			//�궨��ֹ�������ߵȴ�ʱ��С��5s����Ĭ��Ϊ5s
			BMS_config.Pack_RW_config.gStaticSleepTimer = 5 ;					
		if(Time_Total>=BMS_config.Pack_RW_config.gStaticSleepTimer)	//��ֹʱ����ڱ궨ʱ�䣬��ʼ��������ģʽ
		{
			////////////////////ʱ���ʱ��־�ͱ�������/////////////////////
			Is_StaticMode = true;		
			Time_StaticStart = 0;
			Time_Differ = 0;		
			Time_Total = 0;	
			////////////////////�л�������ģʽ////////////////////////////	
			delay_ms(35);
			BAT_State = Sleep ;  //	
			///������λ��ǿ�ƾ���
			gSOC_Balance_Pc = 0;		
			BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//�������״̬λ																										
			gBalance_pc = 0;
			gBalance_falg = 0;													//�������
			BQ769X0_BalanceCtrl(0x00000000);																										
			//GPIO_ENABLE(DISABLE);
			//������λ��ǿ���Էŵ�	
			gDchg_falg_PC = 0;
			GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    				//�Էŵ����Ϊ1������ Ϊ0�ر�
			GPIO_SetBits(GPIOx_W5_6,LED_W6);	 								//�Էŵ����
			g_FaultAlarmState[6]&= ~(1<<3);	 
			//BAT_Working = Shut_down;   										// ����ػ�״̬

		}
		/////////////////////////////////////
		//if((g_Can_falg)||(ReadEvent_enable==1)||(Key_set.Key_state==KEY_Lock))//��ͨѶ���߰�����ֹͣ��ֹʱ���ʱ
		if((g_Can_falg)||(ReadEvent_enable==1)||(key_ack_status!=0)) 			//��ͨѶ���߰�����ֹͣ��ֹʱ���ʱ						
		{
			Is_StaticMode = false;		
			Time_StaticStart = 0;
			Time_Differ = 0;	
			Time_Total = 0 ;
			g_Can_falg = 0 ;
		}	
	}		
	////�ж��Ƿ���Ҫ�л�����������Էŵ�	
	Alternate_Mode();  															//�ж��Ƿ���Ҫ�л�����������Էŵ�ģʽ	
	//����ģʽ
	//if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<((BMS_config.Pack_RW_config.gForcedShutdown+200)*BMS_config.Pack_RW_config.gBatNumber/100))) //�ﵽ�ػ�����
	if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<(3300*BMS_config.Pack_RW_config.gBatNumber/100))) //�ﵽ�ػ�����
	{
		ForcedShut++;
		if(ForcedShut>=200)														//�˲�200��
		{
			BAT_State = Power_Dowm;												//�л����ػ�ģʽ׼���ػ�
			ForcedShut = 0;
		}	
	}										
	else if(ForcedShut>0)
		ForcedShut--;
	

	////////////////////////�ж��Ƿ���Ҫ�л�����ŵ�ģʽ//////////////////////
	if(BAT_STATE_Curren != 0)
	{
		BAT_State = ChgMode;  													//������/�ŵ�ģʽ
		gSOC_OCVupdata_falsh = 0; 												//���OCV�ļ�¼ʱ��
		/////////////////////��ֹ��ʱ��־�ͼ���������//////////////
		Is_StaticMode = false;		
		Time_StaticStart = 0;
		Time_Differ = 0;	
		Time_Total = 0 ;		
		//��ǿ�ƾ���
		gSOC_Balance_Pc = 0;		
		BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;																											
		gBalance_pc = 0;
		gBalance_falg = 0;														//�������
		BQ769X0_BalanceCtrl(0x00000000);																										
		//���Էŵ�	
		gDchg_falg_PC = 0;
		GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    					//�Էŵ����Ϊ1������ Ϊ0�ر�
		GPIO_SetBits(GPIOx_W5_6,LED_W6);	 									//�Էŵ����
		g_FaultAlarmState[6]&= ~(1<<3);	 
	}
				
}
//=============================================================================
//��������: void Balance_Mode
//�������: ��ǰʵ�ʵ���ֵ
//�������: ��
//����ֵ     : 
//��������: ����ģʽ��ִ�к���
//ע������:
//=============================================================================
	void Balance_Mode(void)   //����ģʽ
			{
				static u16 cunt = 0 ; //�ȴ�����״̬λ����
				if(BMS_config.Pack_RW_config.gBatBalanceVoltStart < BMS_config.BMS_Read.gVoltMin) //���ھ��⿪����ѹ����
				{
				 gBalance_falg = 1;
					BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0xff;
				}
				///�����������������������
			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			//�е�����������ģʽ
				if(BAT_STATE_Curren != 0)
				{
				BAT_State = ChgMode;  //������ŵ�ģʽ
				gSOC_Balance_Pc = 0;  
				gBalance_UP = 1;	
				BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//�������״̬λ																										
				gBalance_pc = 0;
				gBalance_falg = 0;	//�������
				BQ769X0_BalanceCtrl(0x00000000);
				GPIO_SetBits(GPIOx_W5_6,LED_W6);	//�سȵ�
				cunt=0;	
				}
			//�ж���������	 С�ھ����ѹ����
				if(cunt<1600)
				cunt++;
				if(cunt==1600)  //����һ��ʱ��ȴ������־λ����
				{
					if((BMS_config.Pack_RW_config.gBatBalanceVoltEnd>BMS_config.BMS_Read.gVoltMin)||(( BMSVoltTemperature.Balance[0]==0x00)&& (BMSVoltTemperature.Balance[1]==0x00)))    //���ھ���رյ�ѹ�ر�
					{
						gBalance_falg = 0;	//�������
						cunt=0;	
						gBalance_UP = 1;
						BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//�������״̬λ																										
				    gBalance_falg = 0;	//�������
				    BQ769X0_BalanceCtrl(0x00000000);
						LED_W6_l;	//�سȵ�
					//	BAT_State = FreeMode;	 //��������ģʽ	
						BAT_State = ChgMode;
					}
				}
			}
//=============================================================================
//��������: Self_Dchg_Mode
//�������: 
//�������: ��
//����ֵ     : 
//��������:�Էŵ�ģʽ��ִ�к���
//ע������:
//=============================================================================
		void Self_Dchg_Mode(void)   //�Էŵ�ģʽ
					{
						
						if(BMS_config.Pack_RW_config.gSelfDchgStartVolt< BMS_config.BMS_Read.gVoltMin)
						{
							GPIO_SetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
							LED_W6_H;	//���ȵ�
							g_FaultAlarmState[6]|= 1<<3;
							gBalance_Dchg_falg = 1;
						}
					///��������������������Էŵ�
					/////////////////////////////////////////////////////////////////////////////////////////////////////////
						if(BMS_config.Pack_RW_config.gSelfDchgEndVolt>BMS_config.BMS_Read.gVoltMin) ///�����ĵ�һ������ ��ѹ�ﵽ�ر�ֵ
						{
							GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
							LED_W6_l;	 //�Էŵ����
							g_FaultAlarmState[6]&= ~(1<<3);	 
							gSelf_Dchg_UP = 1;
							BAT_State = FreeMode;	 //��������ģʽ	
						} 
							//�ж���������	 2�е���
						if(BAT_STATE_Curren != 0)
						{
						BAT_State = ChgMode;  //������ŵ�ģʽ
						GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
					  LED_W6_l;	 //�Էŵ����
						g_FaultAlarmState[6]&= ~(1<<3);	
						gSelf_Dchg_UP = 1;
						}
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////
					}
//=============================================================================
//��������: Sleep_Mode
//�������: ��ǰʵ�ʵ���ֵ
//�������: ��
//����ֵ     : 
//��������:����ģʽ��ִ�к���
//ע������:
//=============================================================================
	void Sleep_Mode(void)   //����ģʽ
		{
			static u8 bit = 0 ;
			static u8 bit1 = 0 ;
			static u8 ForcedShut = 0;//�����������
			u8 i = 0 ;
			u8 falg_Current = 0;
			static u8  Current_daly_d = 0;
			static u8  Current_daly_c = 0;
			static u8  Current_daly_s = 0;
			////////////////////////
		
			LED_1_4_OFF;	//���ùر�LED	 
			LED_OFF; 	//���ùر�LED	
			GPIO_ENABLE(DISABLE);	
			CAN_DeInit(CAN);	
			TIM_Cmd(TIM2, DISABLE);
			TIM_Cmd(TIM3, DISABLE);
			SPI_Cmd(SPI1, DISABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);//ʹ��TIM1ʱ��
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);//ʹ��TIM2ʱ��
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);//����SPIʱ��ʹ��
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, DISABLE);//CAN�˿�ʱ��ʹ��
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, DISABLE);
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, DISABLE);
			TIM_DeInit(TIM2); TIM_DeInit(TIM3);
			SPI_I2S_DeInit(SPI1);
			CAN_DeInit(CAN);	
			USART_DeInit(USART1);
			////////////////ִ������ָ��////////////////////						
			Sleep_Bat();//ִ������
			
		//////////////////////���߻��ѻ�ȡ�������ź�ͨѶ��������״̬/////////////////////////////	
			bit = !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
			bit1 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);
		//if(bit|bit1)//�а�������ͨѶ�л�����ֹģʽ
		WKUP_enable = 0; //���ӻ���
	   	if(bit1)
			{
			  BAT_State = FreeMode;	 //��������ģʽ	
        if(key_ack_status ==4)
				key_ack_status = 0;
				WKUP_enable =1;  //�����ӻ���
        CAN_enable= 1; //��CAN�Ļ��ѹ���
				key_ack_status = 0;
			}	
		 if(bit)
			{
			  SEELP_cnt = 500;	 //��������ģʽ	
				if(key_ack_status ==4)
				key_ack_status = 0;
				WKUP_enable =1;  //�����ӻ���
			}	
			
//////////////////////����ϵͳ��ʼ��//////////////////////
			SystemInit();								        
			BSP_Configuration();//����Ӳ����ʼ������//
			Uart_Init(115200);
//	    GPIO_ResetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4|LED_W6);
//			while(1);
			//		GPIO_ENABLE(ENABLE);	
////////////////����Ҫ����Ӷ�ȡ����/////////////////////////
			for(i = 0;i < 6;i++)
						{
							falg_Current = BQ769X0_SampCurr(&Current_f);    //��ȡ����
							if(falg_Current == TRUE )   //���صı�־λ��ȷ��ʼ����
							{    
									if(Current_f<-30)       //��������100MA
									gCurrentOperation =(s32)(Calibration.gDchgCurrent_double*Current_f);  //�������Էŵ����ϵ��
									else if(Current_f>30)
									gCurrentOperation =(s32)(Calibration.gChgCurrent_double*Current_f);   //�������Գ�����ϵ��
									else
									gCurrentOperation = 0;
									BMS_config.BMS_Read.gCurrent = gCurrentOperation/10;                 //���µ���ֵ
									///////////////////////////////////////////
									if (BMS_config.BMS_Read.gCurrent<-10)   //�ŵ�
									{
										if(Current_daly_d<3)
										Current_daly_d++;
												if(Current_daly_d == 3)
												{		
												 BAT_STATE_Curren = 2;  //�ŵ�״̬
												}
									} 
									else
									{
										if(Current_daly_d>0)
										Current_daly_d--;			
									}
									///////////////////////////////////////////////		
									if (BMS_config.BMS_Read.gCurrent>8)  //���
									{
										if(Current_daly_c<3)
										Current_daly_c++;
											if(Current_daly_c == 3)
											{			
											BAT_STATE_Curren = 1;  
											}

									}
									else
									{
											if(Current_daly_c>0)
											Current_daly_c--;
									}
									///////////////////////////////////////////////
									if(gCurrentOperation == 0)  //����
									{		
									if(Current_daly_s<3)	
									Current_daly_s++;
											if(Current_daly_s == 3)
											{
											BAT_STATE_Curren = 0; 					
			
											}
									}
									else
									{
									if(Current_daly_s>0)	
									Current_daly_s--;	  
									}

								}							
					
							}
			///��������������������Էŵ�
			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(BAT_STATE_Curren != 0)
			{
					BAT_State = ChgMode;	 //��������ģʽ	
				  WKUP_enable =1;  //�����ӻ���
					if(key_ack_status ==4)
				   key_ack_status = 0;
				//	GPIO_SetBits(GPIOB, LNVCC_EN);   //���ܵ�Դ
					if(BAT_STATE_Curren== 2)
						CAN_enable= 1; //��CAN�Ļ��ѹ���
			}
			//����ģʽ
//			if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<((BMS_config.Pack_RW_config.gForcedShutdown+200)*BMS_config.Pack_RW_config.gBatNumber/100))) //�������ģʽ
      if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<(3300*BMS_config.Pack_RW_config.gBatNumber/100))) //�ﵽ�ػ�����
			{
					ForcedShut++;   //������ѹС��2.5
					if(ForcedShut>=200)
					{
					 BAT_State = Power_Dowm;	
					 ForcedShut = 0;
					}					
			 }										
			else
			ForcedShut = 0;		
				//�ж���������	
			Alternate_Mode();  //�ж��Ƿ�Ҫ������������Էŵ�ģʽ
				 /////////////////////////////////////////////////////////////////////////////////////////////////////////	
				 }
//=============================================================================
//��������: Power_Dowm_Mode
//�������: 
//�������: 
//����ֵ     : 
//��������: 
//ע������:
//=============================================================================
	void Power_Dowm_Mode(void)   //����ģʽ
	{
	//	GPIO_ENABLE(DISABLE);	
		BQ769X0_BalanceCtrl(0x00000000);//�ؾ���
		BQ769X0_Sleep();//�ص�ģ��ǰ��
		
	}
//=============================================================================
//��������: open_vvvvv
//�������: ��ǰʵ�ʵ���ֵ
//�������: ��
//����ֵ     : 
//��������: 
//ע������:
//=============================================================================
void open_vvvvv(void)
{
	static u8 BQ76940_num=0;
	if(BQ76940_num<200)	//�״��ϵ����ʵ���ʱ
		BQ76940_num++;
	if((BQ76940_num == 200)&&(gSOC_OCVupdata_open==0))//�ϵ���ʱʱ�䵽���һ�û���й�OCV����
	{
		gSOC_OCVupdata_open= 1;
		BMS_capacity.SOC = openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMin);//��·��ѹSOC����
		if(BMS_capacity.SOC>0)
		{
			BMS_config.Pack_RW_config.gSOC = (u8)(BMS_capacity.SOC/10);        //��ذ�SOC ����
			BMS_capacity.BatCapacity =  BMS_capacity.PackNewCapacity/100*BMS_config.Pack_RW_config.gSOC; //��ذ�������	
			BMS_config.Pack_RW_config.gSurpulsPackCapacity = (u16)(BMS_capacity.BatCapacity/3600/100);  //ʣ������ 0.01AH
		}
		else
		{
			BMS_config.Pack_RW_config.gSOC = 0;        //��ذ�SOC ����
			BMS_capacity.BatCapacity =  0; //��ذ�������
			BMS_config.Pack_RW_config.gSurpulsPackCapacity = 0;  //ʣ������ 0.1AH
		}
		BMS_config_Write();//д���ò���			 
	} 
	
}
//=============================================================================
//��������: open_vvvvv
//�������: 
//�������: ��
//����ֵ     : 
//��������: 
//ע������:
//=============================================================================
void Bat_State(void)
{
	//static u8 falg_down = 0;  //ǿ�ƹػ���־
	static u8 Power_Dowm_num = 0 ;
	u8 OCV_up=0;
	//////////////////////////
	BQ76940_TASK();
	BAT_Processing();		
	open_vvvvv();					//��·��ѹ����SOC �ϵ��ʼ���꣬����200�ι���һ��SOC,
	if(BAT_State !=Sleep)
	{
		CAN_Task();		
	}
	//								if(BAT_State != Sleep)  //��ǰ����������״̬LED��ʾ CAN��Դ��
	//								{
	LED_GPIO_TASK();//����ɨ��LED��ʾ 
	//									GPIO_SetBits(GPIOB, LNVCC_EN);   //��5v��Դ	
	//								}
	//����ָʾ�ƹر�
	//								if((BAT_State == BalanceMode)||(gSOC_Balance_Pc == 1))
	//								{
	//											if((timer3/1000)%2)		  //��������ָʾ��
	//											GPIO_ResetBits(GPIOx_W5_6,LED_W6);	//���ȵ�
	//											else
	//											GPIO_SetBits(GPIOx_W5_6,LED_W6);	//�سȵ�		
	//								}
	if(BMS_capacity.BatCapacity>0)//ʣ������   SOC  ���͸���λ������
	{
		if(BMS_capacity.BatCapacity>=BMS_capacity.PackNewCapacity)//��ֹʣ�����������������
		{
			BMS_capacity.BatCapacity=BMS_capacity.PackNewCapacity;
		}
		BMS_config.Pack_RW_config.gSurpulsPackCapacity = (u16)(BMS_capacity.BatCapacity/3600/100);  //ʣ������ 0.01AH
		BMS_capacity.SOC =  (u16)((double)BMS_capacity.BatCapacity*1000/BMS_capacity.PackNewCapacity);										
		BMS_config.Pack_RW_config.gSOC  = (u8)(BMS_capacity.SOC/10);  // ���ⷢ��SOCֵ				
	}
	else
	{
		BMS_config.Pack_RW_config.gSurpulsPackCapacity = 0;  //ʣ������ 0.1AH
		BMS_capacity.SOC =  0;										
		BMS_config.Pack_RW_config.gSOC  = 0;  // ���ⷢ��SOCֵ																				
	}	
	//ǿ�ƹػ���С��ѹС��2V   �ܵ�ѹС��2.1*����
	if(((BMS_config.BMS_Read.gVoltMin<2000)&&(2100*BMS_config.Pack_RW_config.gBatNumber/100)>BMS_config.BMS_Read.gVoltSum)&&(BAT_STATE_Curren == 0 ))	
	{
		Power_Dowm_num++;
		if(Power_Dowm_num == 200)
			BAT_State = Power_Dowm;	

	}
	else
	{
		if(Power_Dowm_num > 0)
			Power_Dowm_num--;
	}

	switch( BAT_State )  						//ģʽ����л�
	{
		case ReadyMode:   
			BAT_State = FreeMode;
		break;
		case ChgMode:  							//���ģʽ
		case DchgMode:  						//�ŵ�ģʽ 
			BAT_Working = Working_Start	;		//ת���ɹ���ģʽ											 
			Dchg_Chg_Mode();					//�����ŵ�ʱ��		�ۻ��������    											
		break;
		case FreeMode:   						//����״̬
			//falg_down = 1;
			Free_Mode();   						//����ģʽ
			if(gSOC_OCVupdata_falsh == 0)		//��·��ѹ���
			{													 
				OCV_up = sleep_soc_up();
				if(OCV_up==0)
					gSOC_OCVupdata_falsh = 1; 
			}
			//�����޵��� 
			if(gSOC_OCVupdata_falsh)			//OCV����
				OCV_up = sleep_soc_up();
			if(OCV_up == 1)
				gSOC_OCVupdata_falsh = 0; 		//�޵�����ͨѶ																									
		break;	
		case BalanceMode:						//��������
			Balance_Mode();   					//����ģʽ
		break;	
		case Self_DchgMode:
			Self_Dchg_Mode();   				//�Էŵ�ģʽ												
		break;	
		case Sleep:
			if((SEELP_cnt==0)||(gSleep_pc==1))
			{
				//if(( key_ack_status == 0 )&&(WK_UP)&&(key_down_FLAG==0))//�ް�����Ӧ����ִ�����ް�������
				if(( key_ack_status ==4 )||(gSleep_pc==1)||((key_ack_status ==0)&&(WK_UP)&&(key_down_FLAG==0)))//�ް�����Ӧ����ִ�����ް�������
				{		
					gSleep_pc = 0 ;														
					//key_finish_flg=0;
					//key_ack_status =0 ;
					//												          if(key_ack_status ==4)
					//																	key_ack_status = 0;					
					LED_1_4_OFF;							//���ùر�LED	 
					LED_OFF; 								//���ùر�LED	
					while(key_finish_flg) 					//�ȴ�����������������
					{
						key_event_hander();
#if(DUBAG == 1)
						IWDG_ReloadCounter();  				//�忴�Ź�
#endif
					}
					Sleep_Mode();
					gSleep = 1;
					//key_ack_status=0;
					if(BAT_State != Sleep)
						GPIO_SetBits(GPIOB, LNVCC_EN);   	//��5v��Դ	
				}
			}
			else 
				if(SEELP_cnt>0)	
					SEELP_cnt--;
		break;	
		case Power_Dowm:
			BMS_config_Write(); 							//delay_ms(10);
			BMS_Calibration_Write(); 						//delay_ms(10);
			BMS_Alm_Number_Write(); 
			delay_ms(35);
			Power_Dowm_Mode();   							//����ģʽ
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
		break;																		
		default:
			BAT_State = ReadyMode ;
		break;	
	}

}
