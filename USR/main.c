//=============================================================================
//				           深圳市格瑞普电池有限公司
//                         电子产品部			     
//-----------------------------------------------------------------------------
//文件名称: Main.c
//文件标识:
//摘要    : 
//-----------------------------------------------------------------------------
//当前版本: V1.0
//作者    : dangxinkai
//完成日期: 2021.02.25
//=============================================================================
		#include "pbdata.h"
		#include "data.h"
		#include "soc.h"
		#include "Uart.h"
		#include "stdbool.h"
		#define DUBAG 1
		extern R_CAN PC_CAN_Data_R;//can接收数据机构体
		extern volatile u8 Broadcast_Falg;  //名称更新标志
		//gBatCodeNumber
		//unsigned char Broadcast_name[]="gerpow";
    unsigned char BT_RESET[]="TTM:RST-SYS";
		unsigned char broadcast_cmd[]="TTM:REN-";
		unsigned char broadcast_name[16]="123456789";
		u16 broadcast_name1[8];
		unsigned char cnt=0;
		//BMS_config.CustomerCode.gBatCodeNumber[8]
		u8 SEELP_falg = 0;//系统休眠指示标志
		u16 SEELP_cnt = 0;//系统休眠指示标志
		BAT_STATE_enum BAT_State = ReadyMode;//系统运行模式
		BAT_STATE_enum BAT_State_old = ReadyMode;//系统运行模式备份
		RTC_DateTimeTypeDef RTC_DateTimeStructure_NOW;	//RTC实时时钟数据机构
///////////////////////////////////////////////////
		void BSP_Configuration(void);//硬件初始化函数声明
		void RCC_Configuration(void);//时钟初始化函数声明
		void NVIC_Configuration(void);//中断初始化函数声明
		void TIM3_Configuration(void);
		void TIM2_Configuration(void);
		void Dchg_Chg_Mode(void); //充电模式
		void Alternate_Mode(void);   //模式切换 
		void Free_Mode(void);   //静置模式
		void Balance_Mode(void);   //均衡模式
		void Self_Dchg_Mode(void);   //自放电模式
		void Sleep_Mode(void);   //休眠模式
		void Power_Dowm_Mode(void);   //掉电模式
		void open_vvvvv(void);    //开路电压检测
		void Bat_State(void);
		void PVD_Init(void);  //低压掉电中断开启
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
	memcpy((uint32_t*)0x20000000,(uint32_t*)0x08004000,48*4);//重定义中断向量表，主要用于BOOTLOADER下载
	SYSCFG->CFGR1|=0X03;//设置从SRAM启动，主要用于BOOTLOADER
	BSP_Configuration();   //调用硬件初始化函数
	RTC_Initializes();     //RTC初始化
	GPIO_ENABLE(ENABLE);//IO口初始化并使能
	//					delay_ms(1000);
	//					delay_ms(1000);//做两秒延时，主要等待前端采集芯片准备就绪 
	GPIO_LED_dely();
	BQ769X0Init();         //BQ76940初始化 
	Board_int(); 
	//初始化默认参数
	Uart_Init(115200);
	CAN_enable= 1;
	Broadcast_Falg=1;
	//				  for(cnt=0 ; cnt<8; cnt++)//使用PACK条形码作为蓝牙广播名字
	//							{
	//			         broadcast_name[2*cnt]=(unsigned char)BMS_config.CustomerCode.gBatCodeNumber[cnt];
	//	             broadcast_name[2*cnt+1]=(unsigned char)(BMS_config.CustomerCode.gBatCodeNumber[cnt]>>8); 
	//							}
	while(1)
	{
		if(Broadcast_Falg==1)//有参数标定，更改蓝牙广播名称
		{
			Broadcast_Falg=0;
			delay_ms(100);
			memcpy(&broadcast_name1,&BMS_config.CustomerCode.gBatCodeNumber,16);	
			for(cnt=0 ; cnt<8; cnt++)//使用PACK条形码作为蓝牙广播名字
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
		RTC_GetDateTime(&RTC_DateTimeStructure_NOW);   //读取当前时间	
		Bat_State();   //主循环函数           //
		WriteEventEeprom();       //存储历史数据
#if(DUBAG == 1)
		IWDG_ReloadCounter();  //清看门狗
#endif
		USART_FRAME_PROCESS();	
	}
}
//=============================================================================
//函数名称: void BSP_Configuration(void)//硬件初始化函数
//输入参数: 
//输出参数: 无
//返回值  : 无
//功能描述: 初始化硬件资源
//注意事项:
//=============================================================================		
		void BSP_Configuration(void)//硬件初始化函数
		{	  
			RCC_Configuration();	//初始化RCC	
			NVIC_Configuration();//调用中断优先级初始化函数
			#if(DUBAG == 1)
			WWDG_Config();       //初始化看门狗
			#endif
			CAN_GPIO();          //初始化 CAN GPIO 
			CAN_Configation();   //初始化CAN口
			LED_GPIO();	         //初始化 CAN GPIO 
			TIM3_Configuration();  //初始化 定时器3
			TIM2_Configuration();   //初始化 定时器2
			SPI1_GPIO();//调用SPI标准通讯格式初始化函数
			SPI1_Configation();//调用SPI通讯格式设置函数
			W25QXX_GPIO();//调用25Qxx控制管脚初始化函数
			PVD_Init();	    //初始化单片机低电压检测功能
		}
//=============================================================================
//函数名称: void RCC_Configuration(void)//时钟初始化函数
//输入参数: 无
//输出参数: 无
//返回值     :
//功能描述: 初始化系统时钟
//注意事项:
//=============================================================================
	void RCC_Configuration(void)//时钟初始化函数
			{  
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
				//RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//设置SYSCFG复用使能
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//设置PWR复用使能
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能TIM1时钟
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能TIM2时钟
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//设置SPI时钟使能
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);//CAN端口时钟使能
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
			}
//=============================================================================
//函数名称: void PVD_Init(void)
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 初始化单片机低电压检测
//注意事项:
//=============================================================================			
		void PVD_Init(void)
			{
				 PWR_PVDLevelConfig(PWR_PVDLevel_2); //低电压复位设置
				 PWR_PVDCmd(ENABLE);
			}
			
//=============================================================================
//函数名称: void TIM3_Configuration(void)
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 设置1MS的定时
//注意事项:
//=============================================================================						
void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//定义一个TIM_InitTypeDef类型的结构体
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//清中断标志，以备下次中断到来使用
	TIM_TimeBaseStructure.TIM_Period =3;//1秒钟机2000个脉冲 20ms中断一次 40  10ms中断一次 20  1ms中断一次 2
	TIM_TimeBaseStructure.TIM_Prescaler =11999; //24000分频
	TIM_TimeBaseStructure.TIM_ClockDivision =0; //TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//初始化定时器
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//使能溢出中断
	TIM_Cmd(TIM3,ENABLE);//定时器使能
}
//=============================================================================
//函数名称: void TIM2_Configuration(void)
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 设置10MS的定时
//注意事项:
//=============================================================================		
void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//定义一个TIM_InitTypeDef类型的结构体
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清中断标志，以备下次中断到来使用
	TIM_TimeBaseStructure.TIM_Period =20;//1秒钟机2000个脉冲 20ms中断一次 40  10ms中断一次  20
	TIM_TimeBaseStructure.TIM_Prescaler =23999; //24000分频
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1; //TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//初始化定时器
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//使能溢出中断
	TIM_Cmd(TIM2,ENABLE);//定时器使能
}
//=============================================================================
//函数名称: static void NVIC_Configuration(void)
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 设置中断的优先级
//注意事项:
//=============================================================================		
static void NVIC_Configuration(void)//中断优先级初始化函数
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
	EXTI_ClearITPendingBit(0xFFFFFFFF);            //清除外部中断标志
}
//=============================================================================
//函数名称: sleep_soc_up(void)
//输入参数: 无
//输出参数: 无
//返回值  : 返回0 开启SOC更新计时功能;返回1 SOC更新成功 返回2 SOC更新时间不到
//功能描述: 设置休眠时更新SOC，当SOC的容量值与OCV比对值相差正负3%的时候，更新SOC
//注意事项: 
//=============================================================================		
			u8 sleep_soc_up(void)
			{
				static u8 dealy_Hour_staty=0;
				static u8 dealy_Hour_stop=0;
				static u8 dealy_Hour_UP=0;
				static u8 Up_falg=0;
				u16 soc=0;
				if(gSOC_OCVupdata_falsh==0)//只要有充放电退出开路电压检测
				{
				 Up_falg = 0;
				 dealy_Hour_staty = 0;
				 dealy_Hour_stop = 0;	
				}
				
				if(Up_falg == 0)//尚未计时开始
				{
					dealy_Hour_staty=RTC_DateTimeStructure_NOW.Hour;
					Up_falg = 1;//启动计时
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
						  if(dealy_Hour_UP>24)//静止时间大于24小时
							{
									if(BMS_capacity.FCC_UPdatat) //FCC计算已经启动
									{
											if(BMS_capacity.charge>36000000) //电池已经进行过充电，但没有满充
											{
												BMS_capacity.FCC_UPdatat = 0;//停止本次FCC计算	
												BMS_capacity.charge = 0; 
											}
										
									}
									/////////////////释放标志位，等待下一次更新
									Up_falg = 0;
									dealy_Hour_staty = 0;
									dealy_Hour_stop = 0;	
									dealy_Hour_UP=  0;
									////////////////////////////
									if(BMS_config.Pack_RW_config.gSOC>95)//算法SOC高于95%
									soc =openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMax);//采用最大电压计算开路电压SOC估算
									else
									soc =openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMin);//采用最小电压计算开路电压SOC估算
									soc=soc/10;//转换1%分辨率
									/////////////////////////////////////
									if(BMS_config.Pack_RW_config.gSOC>soc)
									soc =BMS_config.Pack_RW_config.gSOC-soc;
									else
									soc =soc-BMS_config.Pack_RW_config.gSOC;
									//////////////////////////////////////////
									if(soc>3)//开路电压计算的SOC与软件算法SOC偏差大于3%才使用OCV对应的SOC进行替换
									{
									 BMS_capacity.SOC = openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMin);//开路电压SOC估算
									 BMS_config.Pack_RW_config.gSOC = (u8)(BMS_capacity.SOC/10);        //电池包SOC 对外
									 BMS_capacity.BatCapacity =  BMS_capacity.PackNewCapacity/100*BMS_config.Pack_RW_config.gSOC; //电池包计算容	
									 BMS_config.Pack_RW_config.gSurpulsPackCapacity = (u16)(BMS_capacity.BatCapacity/100/3600);  //剩余容量 0.1AH	
									}
									return 1;
								}
								
								return 2;
					}
						
			}
//=============================================================================
//函数名称: void gSOCupdata_falsh_Write()
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 充电结束后，当更新标志被置起后，更新电池的满充容量，设置容量，剩余容量 SOH,SOC，并保
//注意事项: 
//=============================================================================		
		void gSOCupdata_falsh_Write()
		{
					if(gSOCupdata_falsh)//gSOCupdata_falsh为1后允许更新一次FLASH数据，更新满充容量
					{
						 gSOCupdata_falsh = 0;
						 BMS_config.Pack_RW_config.gNewPackCapacity =  BMS_capacity.PackNewCapacity/360000;//更新最新电池容量
						 BMS_config.Pack_RW_config.gSurpulsPackCapacity  = BMS_config.Pack_RW_config.gNewPackCapacity; //满充容量等于剩余容量         
						 BMS_config.Pack_RW_config.gSOH = BMS_config.Pack_RW_config.gNewPackCapacity*100/BMS_config.Pack_RW_config.gSetPackCapacity;
						 BMS_config.Pack_RW_config.gSOC = 100;
						 BMS_config_Write();//写配置参数
					}		
		}	
	//=============================================================================
//函数名称: void Chg_Mode(void) 
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 计算充放电时间		累积充电容量  写FALSH  没有电流时调到静置模式
//注意事项: 
//=============================================================================		
		void Chg_Mode(void) //充电模式 
		{
			if(BAT_STATE_Curren == 0)
			{
			
				if((BAT_State_old == Sleep)&&(gSOCupdata_falsh))
				{
				  CAN_enable = 0;
						delay_ms(35);
					BAT_State = Sleep;  //调到静置模式
				}
				else
				{
					 BAT_State = FreeMode;  //调到静置模式
				}
					
		 }
		}
//=============================================================================
//函数名称: void Dchg_Chg_Mode(void) 
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 计算充放电时间		累积充电容量  写FALSH  没有电流时调到静置模式
//注意事项: 
//=============================================================================		
		void Dchg_Chg_Mode(void) //充电模式 
		{
				GPIO_SetBits(GPIOB, LNVCC_EN);   //打开5v电源
				socCalculateTime(); //计算充放电时间
			 if(BAT_STATE_Curren==1)//充电累积容量计算
				{										
					if(BMS_capacity.Bat_charge>36000000)
					{
						BMS_capacity.Bat_charge =	BMS_capacity.Bat_charge-36000000;
						BMS_config.Pack_RW_config.gCHGPackCapacity+=1;	
						BMS_config.Pack_RW_config.gsCycleCnt =	BMS_config.Pack_RW_config.gCHGPackCapacity*1000/( BMS_config.Pack_RW_config.gSetPackCapacity*8);
					}
				}		
       Chg_Mode();				
			 gSOCupdata_falsh_Write();	//更新满充容量并写在FALSH中
				//跳出条件判断
//			 if(BAT_STATE_Curren==0) //只要没有电流就跳出
//			 BAT_State = FreeMode;  //调到静置模式
		}
//=============================================================================
//函数名称: void Alternate_Mode(void) 
//输入参数: 无
//输出参数: 无
//返回值  : 无
//功能描述: 没有电流， 开始记录均衡与自放电起始的各自时间    在均衡时不开自放电 
//注意事项: 
//=============================================================================		
void Alternate_Mode(void)   //模式切换 
{ 
	u8 i=0;
	u32 ChannelBuf = 0; 
	////////////////// 均衡开启时间计算
	static bool is_balance = false;
	static u8 time_start=0;
	static u8 time_differ=0;
	static u16 dealy_gBatBalance_Event_Hour = 0;
	////////////////自放电时间计算
	static u8 Falg_gSelf_Hour = 0;
	static u8 dealy_gSelf_Hour_staty=0;
	static u8 dealy_gSelf_Hour_stop=0;	
	static u16 dealy_gSelf_Event_Hour = 0;
	//////////////////////////////////////////////
	if(BAT_STATE_Curren == 0)  //静置时允许进入均衡与自放电
	{
		if(!is_balance)	//启动均衡静止时间计算
		{
			time_start = RTC_DateTimeStructure_NOW.Hour;
			is_balance = true;
			//dealy_Hour_staty = RTC_DateTimeStructure_NOW.Minute;
			//Falg_Hour = 1;

		}
		else
		{
			//////////////////////////////////////均衡操作/////////////////////////////////////////////////////////////////////////												
			if(RTC_DateTimeStructure_NOW.Hour >= time_start)
				time_differ=RTC_DateTimeStructure_NOW.Hour-time_start;//当前时间减上次进来的时间，求得时间间隔
			else
				time_differ=RTC_DateTimeStructure_NOW.Hour+24-time_start;	 
			dealy_gBatBalance_Event_Hour = dealy_gBatBalance_Event_Hour + time_differ;    //计算时间防止时间越界
			time_differ = 0 ;
			time_start =  RTC_DateTimeStructure_NOW.Hour;
			////////////////////////////////////////							
			if(BMS_config.Pack_RW_config.gBatBalanceStateTime < 1 ) //均衡启动时间标定小于1小时，等于1小时
				BMS_config.Pack_RW_config.gBatBalanceStateTime = 1;


			//if(RTC_DateTimeStructure_NOW.Minute >= dealy_Hour_staty)
			//dealy_Hour_stop=RTC_DateTimeStructure_NOW.Minute-dealy_Hour_staty;//当前时间减上次进来的时间，求得时间间隔
			//else
			//dealy_Hour_stop=RTC_DateTimeStructure_NOW.Minute+60-dealy_Hour_staty;	 
			//dealy_gBatBalance_Event_Hour = dealy_gBatBalance_Event_Hour + dealy_Hour_stop;    //计算时间防止时间越界
			//dealy_Hour_stop = 0 ;
			//dealy_Hour_staty =  RTC_DateTimeStructure_NOW.Minute;
			//								
			//////////////////////////////////////////							
			//if(BMS_config.Pack_RW_config.gBatBalanceStateTime < 1 ) //均衡启动时间标定小于1小时，等于1小时
			//BMS_config.Pack_RW_config.gBatBalanceStateTime = 1;


			///////////////////////////////////////////////////////////									
			if((dealy_gBatBalance_Event_Hour>BMS_config.Pack_RW_config.gBatBalanceStateTime)&&(gBalance_UP == 0)) //静止时间大于均衡开启标定时间，且静止以来没开过均衡
			{
				if(BMS_config.Pack_RW_config.gBatBalanceVoltStart < BMS_config.BMS_Read.gVoltMin) //最小电压高于均衡开启电压开启标定允许电压							
				{
					for(i = 0; i < gCellNum; i++)
					{
						if((((BMSVoltTemperature.g_Volt_call[i] - BMS_config.BMS_Read.gVoltMin) >= BMS_config.Pack_RW_config.gBatBalanceDiffVoltState) && (BMSVoltTemperature.g_Volt_call[i] >= BMS_config.Pack_RW_config.gBatBalanceVoltStart)))//该节电压达到均衡开启条件
						{
							ChannelBuf |= ((u32)1 << i); //置均衡通道开启标志
						}
						else
						{
							ChannelBuf &= ~((u32)1 << i); //清除均衡通道
						}
					}

					//for(i = 0; i < gCellNum; i++)
					//{
					//		if(((BMSVoltTemperature.g_Volt_call[i] - BMS_config.BMS_Read.gVoltMin) <= BMS_config.Pack_RW_config.gBatBalanceDiffVoltEnd)||BMSVoltTemperature.g_Volt_call[i]<BMS_config.Pack_RW_config.gBatBalanceVoltEnd)) //均衡关闭
					//		{
					//			ChannelBuf &= ~((u32)1 << i); //清除均衡通道
					//		}
					//}	

					if(ChannelBuf !=0)
					{
						BAT_State = BalanceMode;				//均衡模式	
					}				
				}

			}	
			//////////////////////////////////////////////////////////			
			if(gBalance_UP ) //均衡完成清除时间与标志，在均衡模式函数中会判断均衡是否结束，结束后该标志会被置1
			{
				gBalance_UP = 0;//允许启动第二次均衡时间计算
				is_balance = false;
				time_start=0;
				time_differ=0;	
				dealy_gBatBalance_Event_Hour = 0;  //清除进入自放电的时间
			}
								
		}
		////////////////////////////////////////自放电计时///////////////////////////////////////////////////////////////////////											
		if(Falg_gSelf_Hour == 0)	//尚未启动自放电等待时间计算
		{
			dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Hour;	//保存当前时间							
			Falg_gSelf_Hour = 1;//启动自放电时间计算
			//dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Minute;	//保存当前时间							
			//Falg_gSelf_Hour = 1;//启动自放电时间计算
		}
		else//自放电时间计算已经开始
		{
			//////////////////////////////////////自放电操作/////////////////////////////////////////////////////////////////////////												
			if(RTC_DateTimeStructure_NOW.Hour >= dealy_gSelf_Hour_staty)//时间间隔在24小时内
				dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Hour-dealy_gSelf_Hour_staty;
			else
				dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Hour+24-dealy_gSelf_Hour_staty;					
			dealy_gSelf_Event_Hour = dealy_gSelf_Event_Hour + dealy_gSelf_Hour_stop;   //计算时间防止时间越界
			dealy_gSelf_Hour_stop = 0;      
			dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Hour;
			///////////////////////////////////////////////////						
			if(BMS_config.Pack_RW_config.gSelfDchgStartTime<1)//标定自放电等待时间小于1小时，则默认成一小时
				BMS_config.Pack_RW_config.gSelfDchgStartTime = 1 ;

			//if(RTC_DateTimeStructure_NOW.Minute >= dealy_gSelf_Hour_staty)//时间间隔在24小时内
			//dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Minute-dealy_gSelf_Hour_staty;
			//else
			//dealy_gSelf_Hour_stop=RTC_DateTimeStructure_NOW.Minute+60-dealy_gSelf_Hour_staty;					
			//dealy_gSelf_Event_Hour = dealy_gSelf_Event_Hour + dealy_gSelf_Hour_stop;   //计算时间防止时间越界
			//dealy_gSelf_Hour_stop = 0;      
			//dealy_gSelf_Hour_staty = RTC_DateTimeStructure_NOW.Minute;
			/////////////////////////////////////////////////////						
			//if(BMS_config.Pack_RW_config.gSelfDchgStartTime<1)//标定自放电等待时间小于1小时，则默认成一小时
			//BMS_config.Pack_RW_config.gSelfDchgStartTime = 1 ;




			if((dealy_gSelf_Event_Hour>BMS_config.Pack_RW_config.gSelfDchgStartTime)&&(gSelf_Dchg_UP == 0)) //静止时间大于设定时间，且自放电尚未打开
			{
				if(BAT_State != BalanceMode) //当前模式不在均衡模式，均衡没有开启才允许自放电
				{
					if(BMS_config.Pack_RW_config.gSelfDchgStartVolt< BMS_config.BMS_Read.gVoltMin)//最小电压大于自放电开启电压
					{
						BAT_State = Self_DchgMode;  	//切换到自放电模式	
					}																		 
				}
			}	
			if(gSelf_Dchg_UP ) //自放电结束，在自放电模式函数中，自放电结束后该标志位会被置1
			{
				gSelf_Dchg_UP = 0;
				Falg_gSelf_Hour = 0;
				dealy_gSelf_Hour_staty=0;
				dealy_gSelf_Hour_stop=0;	
				dealy_gSelf_Event_Hour = 0; //清除进入自放电的时间
			}											


		}

	}
	else//有充电或者放电，清除均衡和自放电计时
	{
		is_balance = false;
		time_start=0;
		time_differ=0;		
		Falg_gSelf_Hour = 0;
		dealy_gSelf_Hour_staty=0;
		dealy_gSelf_Hour_stop=0;	
		dealy_gSelf_Event_Hour = 0;//清除进入自放电的时间
		dealy_gBatBalance_Event_Hour = 0;  //清除进入自放电的时间

	}	 
		 
			
}
//=============================================================================
//函数名称: 静止模式下执行函数
//输入参数: 
//输出参数: 无
//返回值     : 
//功能描述: 
//注意事项:
//=============================================================================
void Free_Mode(void)   //静置模式
{
	//////用于计算静止时间//////////
	static u8 Time_StaticStart=0;
	static u8 Time_Differ=0;
	static u8 Time_Total=0;
	static bool Is_StaticMode=false;							//是否是静置模式标志
	static u8 ForcedShut = 0;									//掉电次数计算

	//////////////////////////////////////////////
	if(!Is_StaticMode) 											//尚未启动静止时间计算 
	{
		Time_StaticStart=RTC_DateTimeStructure_NOW.Second;		//保存当前时间秒
		Is_StaticMode = true;									//启动静止时间计算
	}
	else														//已经启动了静止时间计算
	{	//////////////////////计算静止时间////////////////////////					
		if(RTC_DateTimeStructure_NOW.Second >= Time_StaticStart)
			Time_Differ=RTC_DateTimeStructure_NOW.Second - Time_StaticStart;
		else
			Time_Differ=RTC_DateTimeStructure_NOW.Second + 60 - Time_StaticStart;												
		Time_Total =  Time_Total+ Time_Differ;
		Time_Differ = 0;
		Time_StaticStart = RTC_DateTimeStructure_NOW.Second;
		//////////////////////////////////////////////////////////													
		if(BMS_config.Pack_RW_config.gStaticSleepTimer<5)			//标定静止进入休眠等待时间小于5s，则默认为5s
			BMS_config.Pack_RW_config.gStaticSleepTimer = 5 ;					
		if(Time_Total>=BMS_config.Pack_RW_config.gStaticSleepTimer)	//静止时间大于标定时间，开始进入休眠模式
		{
			////////////////////时间计时标志和变量清零/////////////////////
			Is_StaticMode = true;		
			Time_StaticStart = 0;
			Time_Differ = 0;		
			Time_Total = 0;	
			////////////////////切换到休眠模式////////////////////////////	
			delay_ms(35);
			BAT_State = Sleep ;  //	
			///结束上位机强制均衡
			gSOC_Balance_Pc = 0;		
			BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//清除均衡状态位																										
			gBalance_pc = 0;
			gBalance_falg = 0;													//均衡完成
			BQ769X0_BalanceCtrl(0x00000000);																										
			//GPIO_ENABLE(DISABLE);
			//结束上位机强制自放电	
			gDchg_falg_PC = 0;
			GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    				//自放电控制为1开启， 为0关闭
			GPIO_SetBits(GPIOx_W5_6,LED_W6);	 								//自放电完成
			g_FaultAlarmState[6]&= ~(1<<3);	 
			//BAT_Working = Shut_down;   										// 进入关机状态

		}
		/////////////////////////////////////
		//if((g_Can_falg)||(ReadEvent_enable==1)||(Key_set.Key_state==KEY_Lock))//有通讯或者按键，停止静止时间计时
		if((g_Can_falg)||(ReadEvent_enable==1)||(key_ack_status!=0)) 			//有通讯或者按键，停止静止时间计时						
		{
			Is_StaticMode = false;		
			Time_StaticStart = 0;
			Time_Differ = 0;	
			Time_Total = 0 ;
			g_Can_falg = 0 ;
		}	
	}		
	////判断是否需要切换到均衡或者自放电	
	Alternate_Mode();  															//判断是否需要切换到均衡或者自放电模式	
	//掉电模式
	//if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<((BMS_config.Pack_RW_config.gForcedShutdown+200)*BMS_config.Pack_RW_config.gBatNumber/100))) //达到关机条件
	if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<(3300*BMS_config.Pack_RW_config.gBatNumber/100))) //达到关机条件
	{
		ForcedShut++;
		if(ForcedShut>=200)														//滤波200次
		{
			BAT_State = Power_Dowm;												//切换到关机模式准备关机
			ForcedShut = 0;
		}	
	}										
	else if(ForcedShut>0)
		ForcedShut--;
	

	////////////////////////判断是否需要切换到充放电模式//////////////////////
	if(BAT_STATE_Curren != 0)
	{
		BAT_State = ChgMode;  													//调到充/放电模式
		gSOC_OCVupdata_falsh = 0; 												//清除OCV的记录时间
		/////////////////////静止计时标志和计数器清零//////////////
		Is_StaticMode = false;		
		Time_StaticStart = 0;
		Time_Differ = 0;	
		Time_Total = 0 ;		
		//关强制均衡
		gSOC_Balance_Pc = 0;		
		BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;																											
		gBalance_pc = 0;
		gBalance_falg = 0;														//均衡完成
		BQ769X0_BalanceCtrl(0x00000000);																										
		//关自放电	
		gDchg_falg_PC = 0;
		GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    					//自放电控制为1开启， 为0关闭
		GPIO_SetBits(GPIOx_W5_6,LED_W6);	 									//自放电完成
		g_FaultAlarmState[6]&= ~(1<<3);	 
	}
				
}
//=============================================================================
//函数名称: void Balance_Mode
//输入参数: 当前实际电流值
//输出参数: 无
//返回值     : 
//功能描述: 均衡模式下执行函数
//注意事项:
//=============================================================================
	void Balance_Mode(void)   //均衡模式
			{
				static u16 cunt = 0 ; //等待均衡状态位置起
				if(BMS_config.Pack_RW_config.gBatBalanceVoltStart < BMS_config.BMS_Read.gVoltMin) //高于均衡开启电压开启
				{
				 gBalance_falg = 1;
					BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0xff;
				}
				///以下两种情况可以跳出均衡
			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			//有电流跳出均衡模式
				if(BAT_STATE_Curren != 0)
				{
				BAT_State = ChgMode;  //调到充放电模式
				gSOC_Balance_Pc = 0;  
				gBalance_UP = 1;	
				BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//清除均衡状态位																										
				gBalance_pc = 0;
				gBalance_falg = 0;	//均衡完成
				BQ769X0_BalanceCtrl(0x00000000);
				GPIO_SetBits(GPIOx_W5_6,LED_W6);	//关橙灯
				cunt=0;	
				}
			//判断跳出条件	 小于均衡电压跳出
				if(cunt<1600)
				cunt++;
				if(cunt==1600)  //过滤一段时间等待均衡标志位置起
				{
					if((BMS_config.Pack_RW_config.gBatBalanceVoltEnd>BMS_config.BMS_Read.gVoltMin)||(( BMSVoltTemperature.Balance[0]==0x00)&& (BMSVoltTemperature.Balance[1]==0x00)))    //低于均衡关闭电压关闭
					{
						gBalance_falg = 0;	//均衡完成
						cunt=0;	
						gBalance_UP = 1;
						BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//清除均衡状态位																										
				    gBalance_falg = 0;	//均衡完成
				    BQ769X0_BalanceCtrl(0x00000000);
						LED_W6_l;	//关橙灯
					//	BAT_State = FreeMode;	 //调到静置模式	
						BAT_State = ChgMode;
					}
				}
			}
//=============================================================================
//函数名称: Self_Dchg_Mode
//输入参数: 
//输出参数: 无
//返回值     : 
//功能描述:自放电模式下执行函数
//注意事项:
//=============================================================================
		void Self_Dchg_Mode(void)   //自放电模式
					{
						
						if(BMS_config.Pack_RW_config.gSelfDchgStartVolt< BMS_config.BMS_Read.gVoltMin)
						{
							GPIO_SetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
							LED_W6_H;	//开橙灯
							g_FaultAlarmState[6]|= 1<<3;
							gBalance_Dchg_falg = 1;
						}
					///以下两种情况可以跳出自放电
					/////////////////////////////////////////////////////////////////////////////////////////////////////////
						if(BMS_config.Pack_RW_config.gSelfDchgEndVolt>BMS_config.BMS_Read.gVoltMin) ///跳出的第一个条件 电压达到关闭值
						{
							GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
							LED_W6_l;	 //自放电完成
							g_FaultAlarmState[6]&= ~(1<<3);	 
							gSelf_Dchg_UP = 1;
							BAT_State = FreeMode;	 //调到静置模式	
						} 
							//判断跳出条件	 2有电流
						if(BAT_STATE_Curren != 0)
						{
						BAT_State = ChgMode;  //调到充放电模式
						GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
					  LED_W6_l;	 //自放电完成
						g_FaultAlarmState[6]&= ~(1<<3);	
						gSelf_Dchg_UP = 1;
						}
						//////////////////////////////////////////////////////////////////////////////////////////////////////////////
					}
//=============================================================================
//函数名称: Sleep_Mode
//输入参数: 当前实际电流值
//输出参数: 无
//返回值     : 
//功能描述:休眠模式下执行函数
//注意事项:
//=============================================================================
	void Sleep_Mode(void)   //休眠模式
		{
			static u8 bit = 0 ;
			static u8 bit1 = 0 ;
			static u8 ForcedShut = 0;//掉电次数计算
			u8 i = 0 ;
			u8 falg_Current = 0;
			static u8  Current_daly_d = 0;
			static u8  Current_daly_c = 0;
			static u8  Current_daly_s = 0;
			////////////////////////
		
			LED_1_4_OFF;	//静置关闭LED	 
			LED_OFF; 	//静置关闭LED	
			GPIO_ENABLE(DISABLE);	
			CAN_DeInit(CAN);	
			TIM_Cmd(TIM2, DISABLE);
			TIM_Cmd(TIM3, DISABLE);
			SPI_Cmd(SPI1, DISABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);//使能TIM1时钟
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);//使能TIM2时钟
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);//设置SPI时钟使能
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, DISABLE);//CAN端口时钟使能
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, DISABLE);
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, DISABLE);
			TIM_DeInit(TIM2); TIM_DeInit(TIM3);
			SPI_I2S_DeInit(SPI1);
			CAN_DeInit(CAN);	
			USART_DeInit(USART1);
			////////////////执行休眠指令////////////////////						
			Sleep_Bat();//执行休眠
			
		//////////////////////休眠唤醒获取按键引脚和通讯唤醒引脚状态/////////////////////////////	
			bit = !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
			bit1 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);
		//if(bit|bit1)//有按键或者通讯切换到静止模式
		WKUP_enable = 0; //闹钟唤醒
	   	if(bit1)
			{
			  BAT_State = FreeMode;	 //调到静置模式	
        if(key_ack_status ==4)
				key_ack_status = 0;
				WKUP_enable =1;  //非闹钟唤醒
        CAN_enable= 1; //打开CAN的唤醒功能
				key_ack_status = 0;
			}	
		 if(bit)
			{
			  SEELP_cnt = 500;	 //调到静置模式	
				if(key_ack_status ==4)
				key_ack_status = 0;
				WKUP_enable =1;  //非闹钟唤醒
			}	
			
//////////////////////唤醒系统初始化//////////////////////
			SystemInit();								        
			BSP_Configuration();//调用硬件初始化函数//
			Uart_Init(115200);
//	    GPIO_ResetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4|LED_W6);
//			while(1);
			//		GPIO_ENABLE(ENABLE);	
////////////////以下要补充加读取电流/////////////////////////
			for(i = 0;i < 6;i++)
						{
							falg_Current = BQ769X0_SampCurr(&Current_f);    //读取电流
							if(falg_Current == TRUE )   //返回的标志位正确开始处理
							{    
									if(Current_f<-30)       //电流大于100MA
									gCurrentOperation =(s32)(Calibration.gDchgCurrent_double*Current_f);  //电流乘以放电电流系数
									else if(Current_f>30)
									gCurrentOperation =(s32)(Calibration.gChgCurrent_double*Current_f);   //电流乘以充电电流系数
									else
									gCurrentOperation = 0;
									BMS_config.BMS_Read.gCurrent = gCurrentOperation/10;                 //更新电流值
									///////////////////////////////////////////
									if (BMS_config.BMS_Read.gCurrent<-10)   //放电
									{
										if(Current_daly_d<3)
										Current_daly_d++;
												if(Current_daly_d == 3)
												{		
												 BAT_STATE_Curren = 2;  //放电状态
												}
									} 
									else
									{
										if(Current_daly_d>0)
										Current_daly_d--;			
									}
									///////////////////////////////////////////////		
									if (BMS_config.BMS_Read.gCurrent>8)  //充电
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
									if(gCurrentOperation == 0)  //静置
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
			///以下两种情况可以跳出自放电
			/////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(BAT_STATE_Curren != 0)
			{
					BAT_State = ChgMode;	 //调到静置模式	
				  WKUP_enable =1;  //非闹钟唤醒
					if(key_ack_status ==4)
				   key_ack_status = 0;
				//	GPIO_SetBits(GPIOB, LNVCC_EN);   //打开总电源
					if(BAT_STATE_Curren== 2)
						CAN_enable= 1; //打开CAN的唤醒功能
			}
			//掉电模式
//			if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<((BMS_config.Pack_RW_config.gForcedShutdown+200)*BMS_config.Pack_RW_config.gBatNumber/100))) //进入掉电模式
      if((BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gForcedShutdown)&&(BMS_config.BMS_Read.gVoltSum<(3300*BMS_config.Pack_RW_config.gBatNumber/100))) //达到关机条件
			{
					ForcedShut++;   //持续电压小于2.5
					if(ForcedShut>=200)
					{
					 BAT_State = Power_Dowm;	
					 ForcedShut = 0;
					}					
			 }										
			else
			ForcedShut = 0;		
				//判断跳出条件	
			Alternate_Mode();  //判断是否要调到均衡或者自放电模式
				 /////////////////////////////////////////////////////////////////////////////////////////////////////////	
				 }
//=============================================================================
//函数名称: Power_Dowm_Mode
//输入参数: 
//输出参数: 
//返回值     : 
//功能描述: 
//注意事项:
//=============================================================================
	void Power_Dowm_Mode(void)   //掉电模式
	{
	//	GPIO_ENABLE(DISABLE);	
		BQ769X0_BalanceCtrl(0x00000000);//关均衡
		BQ769X0_Sleep();//关掉模拟前断
		
	}
//=============================================================================
//函数名称: open_vvvvv
//输入参数: 当前实际电流值
//输出参数: 无
//返回值     : 
//功能描述: 
//注意事项:
//=============================================================================
void open_vvvvv(void)
{
	static u8 BQ76940_num=0;
	if(BQ76940_num<200)	//首次上电做适当延时
		BQ76940_num++;
	if((BQ76940_num == 200)&&(gSOC_OCVupdata_open==0))//上电延时时间到，且还没进行过OCV计算
	{
		gSOC_OCVupdata_open= 1;
		BMS_capacity.SOC = openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMin);//开路电压SOC估算
		if(BMS_capacity.SOC>0)
		{
			BMS_config.Pack_RW_config.gSOC = (u8)(BMS_capacity.SOC/10);        //电池包SOC 对外
			BMS_capacity.BatCapacity =  BMS_capacity.PackNewCapacity/100*BMS_config.Pack_RW_config.gSOC; //电池包计算容	
			BMS_config.Pack_RW_config.gSurpulsPackCapacity = (u16)(BMS_capacity.BatCapacity/3600/100);  //剩余容量 0.01AH
		}
		else
		{
			BMS_config.Pack_RW_config.gSOC = 0;        //电池包SOC 对外
			BMS_capacity.BatCapacity =  0; //电池包计算容
			BMS_config.Pack_RW_config.gSurpulsPackCapacity = 0;  //剩余容量 0.1AH
		}
		BMS_config_Write();//写配置参数			 
	} 
	
}
//=============================================================================
//函数名称: open_vvvvv
//输入参数: 
//输出参数: 无
//返回值     : 
//功能描述: 
//注意事项:
//=============================================================================
void Bat_State(void)
{
	//static u8 falg_down = 0;  //强制关机标志
	static u8 Power_Dowm_num = 0 ;
	u8 OCV_up=0;
	//////////////////////////
	BQ76940_TASK();
	BAT_Processing();		
	open_vvvvv();					//开路电压估算SOC 上电初始化完，运行200次估算一个SOC,
	if(BAT_State !=Sleep)
	{
		CAN_Task();		
	}
	//								if(BAT_State != Sleep)  //当前不处于休眠状态LED显示 CAN电源打开
	//								{
	LED_GPIO_TASK();//按键扫描LED显示 
	//									GPIO_SetBits(GPIOB, LNVCC_EN);   //打开5v电源	
	//								}
	//均衡指示灯关闭
	//								if((BAT_State == BalanceMode)||(gSOC_Balance_Pc == 1))
	//								{
	//											if((timer3/1000)%2)		  //开启均衡指示灯
	//											GPIO_ResetBits(GPIOx_W5_6,LED_W6);	//开橙灯
	//											else
	//											GPIO_SetBits(GPIOx_W5_6,LED_W6);	//关橙灯		
	//								}
	if(BMS_capacity.BatCapacity>0)//剩余容量   SOC  发送给上位机数据
	{
		if(BMS_capacity.BatCapacity>=BMS_capacity.PackNewCapacity)//防止剩余容量大于设计容量
		{
			BMS_capacity.BatCapacity=BMS_capacity.PackNewCapacity;
		}
		BMS_config.Pack_RW_config.gSurpulsPackCapacity = (u16)(BMS_capacity.BatCapacity/3600/100);  //剩余容量 0.01AH
		BMS_capacity.SOC =  (u16)((double)BMS_capacity.BatCapacity*1000/BMS_capacity.PackNewCapacity);										
		BMS_config.Pack_RW_config.gSOC  = (u8)(BMS_capacity.SOC/10);  // 对外发送SOC值				
	}
	else
	{
		BMS_config.Pack_RW_config.gSurpulsPackCapacity = 0;  //剩余容量 0.1AH
		BMS_capacity.SOC =  0;										
		BMS_config.Pack_RW_config.gSOC  = 0;  // 对外发送SOC值																				
	}	
	//强制关机最小电压小于2V   总电压小于2.1*串数
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

	switch( BAT_State )  						//模式间的切换
	{
		case ReadyMode:   
			BAT_State = FreeMode;
		break;
		case ChgMode:  							//充电模式
		case DchgMode:  						//放电模式 
			BAT_Working = Working_Start	;		//转换成工作模式											 
			Dchg_Chg_Mode();					//计算充放电时间		累积充电容量    											
		break;
		case FreeMode:   						//静置状态
			//falg_down = 1;
			Free_Mode();   						//静置模式
			if(gSOC_OCVupdata_falsh == 0)		//开路电压检测
			{													 
				OCV_up = sleep_soc_up();
				if(OCV_up==0)
					gSOC_OCVupdata_falsh = 1; 
			}
			//静置无电流 
			if(gSOC_OCVupdata_falsh)			//OCV更新
				OCV_up = sleep_soc_up();
			if(OCV_up == 1)
				gSOC_OCVupdata_falsh = 0; 		//无电流无通讯																									
		break;	
		case BalanceMode:						//均衡条件
			Balance_Mode();   					//均衡模式
		break;	
		case Self_DchgMode:
			Self_Dchg_Mode();   				//自放电模式												
		break;	
		case Sleep:
			if((SEELP_cnt==0)||(gSleep_pc==1))
			{
				//if(( key_ack_status == 0 )&&(WK_UP)&&(key_down_FLAG==0))//无按键响应正在执行且无按键按下
				if(( key_ack_status ==4 )||(gSleep_pc==1)||((key_ack_status ==0)&&(WK_UP)&&(key_down_FLAG==0)))//无按键响应正在执行且无按键按下
				{		
					gSleep_pc = 0 ;														
					//key_finish_flg=0;
					//key_ack_status =0 ;
					//												          if(key_ack_status ==4)
					//																	key_ack_status = 0;					
					LED_1_4_OFF;							//静置关闭LED	 
					LED_OFF; 								//静置关闭LED	
					while(key_finish_flg) 					//等待按键弹起后进入休眠
					{
						key_event_hander();
#if(DUBAG == 1)
						IWDG_ReloadCounter();  				//清看门狗
#endif
					}
					Sleep_Mode();
					gSleep = 1;
					//key_ack_status=0;
					if(BAT_State != Sleep)
						GPIO_SetBits(GPIOB, LNVCC_EN);   	//打开5v电源	
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
			Power_Dowm_Mode();   							//掉电模式
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
