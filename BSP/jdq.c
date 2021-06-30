#include "pbdata.h"
#include "data.h"
Key_TypeDef Key_set;
#define key_down 0
#define key_up 1
#define key_filter_time 10 
//static u8 key_led_num = 0;
 u8 key_down_FLAG=0;
static u16 key_down_long_time =0;
u8 key_ack_status=0;//当前事件响应，该状态为不为0时，不要休眠
volatile u8 key_finish_flg=0;
/*********************************************************************************
*   函 数 名: JDQ_GPIO
*   功能说明: JDQ的GPIO管脚初始化
*   形    参：无
*   返 回 值: 无
*********************************************************************************/
void LED_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	//LED驱动7个GPIO										    
	GPIO_InitStructure.GPIO_Pin = LED_W1|LED_W2|LED_W3|LED_W4|GPIO_Pin_15;           	  /*选择要控制的GPIOE引脚*/				
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;       	/*设置要控制的GPIOE引脚为输出模式*/	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	/*设置引脚速率为50MHz */ 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      /*设置引脚模式为上拉*/
	GPIO_Init(GPIOx_W1_4, &GPIO_InitStructure);	         /*调用库函数，初始化GPIOB*/
	GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
	GPIO_InitStructure.GPIO_Pin = LED_W5|LED_W6|HOLD;           	  /*选择要控制的GPIOA引脚*/				
	GPIO_Init(GPIOx_W5_6, &GPIO_InitStructure);	         /*调用库函数，初始化GPIOA*/
	GPIO_SetBits(GPIOx_W5_6,LED_W5|LED_W6);
	GPIO_InitStructure.GPIO_Pin = LED_W7;           	  /*选择要控制的GPIOA引脚*/				
	GPIO_Init(GPIOx_W7, &GPIO_InitStructure);	         /*调用库函数，初始化GPIOA*/
	GPIO_SetBits(GPIOx_W7,LED_W7);	
	 //W25q128控制
	GPIO_InitStructure.GPIO_Pin = W25Q128_EN;
	GPIO_Init(GPIOx_W25Q128_EN, &GPIO_InitStructure); 
	GPIO_InitStructure.GPIO_Pin = W25Q128_WP;
	GPIO_Init(GPIOx_W25Q128_WP, &GPIO_InitStructure); 
	GPIO_ResetBits(GPIOx_W25Q128_EN,W25Q128_EN);    //W25q128控制电源为0开启， 为1关闭					
	GPIO_SetBits(GPIOx_HOLD,HOLD);    //自放电控制为0开启， 为1关闭
	GPIO_SetBits(GPIOx_W25Q128_WP,W25Q128_WP);    //WP电平职高
	
//	//总电源控制
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Pin = LNVCC_EN;            //总电源控制
	GPIO_Init(GPIOx_LNVCC_EN, &GPIO_InitStructure);	
	
	//BQ76940模拟I2C
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;   /*!< Configure Sim1_I2C pins: SCL */
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;    /*!< Configure Sim1_I2C pins: SDA */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14);   //打开总电源
	//初始化按键					
	GPIO_InitStructure.GPIO_Pin =  WKUP_PIN;  //初始化按键
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(WKUP_PORT, &GPIO_InitStructure);		
 //CAN的激活信号
	GPIO_InitStructure.GPIO_Pin =  COMWK_PIN;  //CAN的激活信号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(COMWK_PORT, &GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Pin =  ALERT;  //BQ76940故障引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOx_ALERT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = btdate;           	  /*选择要控制的GPIOB引脚*/				
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;       	/*设置要控制的GPIOB引脚为输出模式*/	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	/*设置引脚速率为50MHz */ 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      /*设置引脚模式为上拉*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	         /*调用库函数，初始化GPIOB*/
	//GPIO_ResetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
	GPIO_ResetBits(GPIOB,btdate);
	//BQ76940控制上拉
	GPIO_InitStructure.GPIO_Pin = POW_SAVE; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_Init(GPIOx_POW_SAVE, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOx_POW_SAVE,POW_SAVE);  //BQ76940控制上拉 为0开启， 为1关闭
	//自放电控制
	GPIO_InitStructure.GPIO_Pin = SELF_DISCHAR;
	GPIO_Init(GPIOx_SELF_DISCHAR, &GPIO_InitStructure); 
	GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭

}
void GPIO_ENABLE( FunctionalState x)
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	if(x == ENABLE)
	{
  	GPIO_SetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14);//操作BQ76940
	  GPIO_ResetBits(GPIOx_POW_SAVE,POW_SAVE);  //BQ76940控制上拉 为0开启， 为1关闭
		GPIO_SetBits(GPIOB, LNVCC_EN);   //打开总电源
		GPIO_ResetBits(GPIOx_W25Q128_EN,W25Q128_EN);    //W25q128控制电源为0开启， 为1关闭
		GPIO_SetBits(GPIOx_HOLD,HOLD);    //自放电控制为0开启， 为1关闭
		GPIO_SetBits(GPIOx_W25Q128_WP,W25Q128_WP);    //WP电平职高		
	}
  else
	{
	GPIO_InitStructure.GPIO_Pin = LED_W1|LED_W2|LED_W3|LED_W4;  //LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, LED_W1|LED_W2|LED_W3|LED_W4); 	
	
	GPIO_InitStructure.GPIO_Pin = LED_W5|LED_W6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, LED_W5|LED_W6);

	GPIO_InitStructure.GPIO_Pin = LED_W7;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_SetBits(GPIOF, LED_W7);		
		
 
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_15;  //can
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_ResetBits(GPIOB, GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_15); 
		
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//spi
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	//2021/5/14 修改
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);///全低输入
	GPIO_InitStructure.GPIO_Pin = W25Q128_EN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOx_W25Q128_EN,W25Q128_EN);  ///	
	GPIO_InitStructure.GPIO_Pin = HOLD|W25Q128_WP;           	 
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	         
	GPIO_ResetBits(GPIOx_HOLD,HOLD|W25Q128_WP);    //

//	
	
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
		  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_4;   //BQ76940
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_Init(GPIOB, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOB, LNVCC_EN);   //5V电源
			GPIO_ResetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14); 
			GPIO_InitStructure.GPIO_Pin = POW_SAVE;
		  GPIO_Init(GPIOA, &GPIO_InitStructure);
      GPIO_SetBits(GPIOx_POW_SAVE,POW_SAVE);  //BQ76940
			GPIO_InitStructure.GPIO_Pin =  ALERT;  //BQ76940故障引脚
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	    GPIO_Init(GPIOx_ALERT, &GPIO_InitStructure);	
		  GPIO_ResetBits(GPIOx_ALERT,ALERT);
				
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;	
		  GPIO_InitStructure.GPIO_Pin =  SELF_DISCHAR;	//自放电	
			GPIO_Init(GPIOx_SELF_DISCHAR, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR); 
			
//没有用到的IO口
	    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; 
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(GPIOA, &GPIO_InitStructure);
		  GPIO_ResetBits(GPIOA,GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_14);	
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
      GPIO_Init(GPIOF, &GPIO_InitStructure);
		  GPIO_ResetBits(GPIOF,GPIO_Pin_0);	
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_11;
      GPIO_Init(GPIOB, &GPIO_InitStructure);
		  GPIO_ResetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_11);	
	
	
//	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_Pin    =GPIO_Pin_6|GPIO_Pin_7	;
//			GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_DOWN;
//	GPIO_Init(GPIOA, &GPIO_InitStructure); 
//GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5);  
//GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9); 
//×?·?μ??????a1?a??￡? ?a01?±?	
//		GPIO_ResetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14);//2ù×÷BQ76940
//W25q128????μ??′?a0?a??￡? ?a11?±?
		
	}
}

/********************************************
*   函 数 名: HAL_GetTick
*   功能说明: 获取系统时钟
*   形    参：无
*   返 回 值: 无
********************************************/
__weak uint32_t HAL_GetTick(void)
{
  return timer1;
}


///************************ 论坛地址 www.zxkjmcu.com ******************************/
//u8 key_check(void)//按键扫描
//  { 
//    static u8 key_status = key_up;
//    static u8 key_filter=0;
//	if(!WK_UP)
//	{
//	  if(key_filter<key_filter_time)
//	  key_filter++;
//	  if(key_filter==key_filter_time)
//	  {
//	    key_status = key_down;
//	  }
//	}
//	else
//	{
//	  if(key_filter>0)
//	  key_filter--;
//	  if(key_filter==0)
//	  {
//	    key_status = key_up;
//	  }
//	}
//	return key_status;
//  }
//??????
//?????
//mode:0,??????;1,?????;
//0,????????
//1,WKUP?? WK_UP
//???????????,KEY0>KEY1>KEY2>WK_UP!!
//Key_TypeDef KEY_Scan(void)
//{  
//static 	uint32_t tickstart_key = 0;
//static 	uint32_t ticksotp_key = 0;
//     switch (Key_set.Key_state) 
//    {
//    case KEY_Ready:          //允许按键按下
//	//	if(!key_check())
//			if(!WK_UP)
//		{
//		Key_set.Key_state = KEY_Down;
//		Key_set.Key_Down_Timer = 0;
//		}
//		 break;
//    case KEY_Down:           //按键按下
//		
//	// if(!key_check())
//			if(!WK_UP)
//	 {
//	 	  Key_set.Key_state = KEY_Lock;
//		 	tickstart_key = HAL_GetTick();  //读取按下时间
//	 }
//	 else
//	 	Key_set.Key_state = KEY_Ready;   //干扰高出
//		 break;
//    case KEY_Lock:         //按键锁定
//					ticksotp_key = HAL_GetTick();
//				if(ticksotp_key<tickstart_key)//
//				{
//				ticksotp_key=0xffffffff-tickstart_key+ticksotp_key;
//				}
//				else
//				ticksotp_key=ticksotp_key-tickstart_key;

//				Key_set.Key_Down_Timer = ticksotp_key;
//		//if(key_check())  //按键释放
//		if(WK_UP)
//		{
//				Key_set.Key_state = KEY_Ulock;
//		}
//       	 break;
//    case KEY_Ulock:       //没有按键按下
//		Key_set.Key_state = KEY_Ready;
//		 break;
//    default:
//       Key_set.Key_state = KEY_Ready;
//        break;
//    } 
//    
//    return Key_set;   //????????
//}
/**********************************
*   函 数 名: void  set_led_Alm(void)
*   功能说明: 
*   形    参：
  返 回 值: 
************************************/
void  set_led_Alm(void)
{
	static u8 R_led= 0;
	static u8 B_led= 0;
	static u8 G_led= 0;
//	gBitDchgShortCircuit=1;
//	gBitChgShortCircuit=1; //短路
//	gBitDchgHighTemAlmPrt=gBitChgHighTemAlmPrt=1; //充放电温度保护
 // gBitChgOverCurrentAlm=1;
//	gBitChgOverCurrentAlmPrt=1;  //充电过流告警  充电过流保护	
//  gBitUnderVoltAlmPrt =gBitUnderVoltAlm=1;   //欠压告警  欠压保护
//  gBitSOClow = 1;
//	gBitVoltDiff1MaxAlm=1;  //压差L告警	
//   gBitVoltDiff1MaxAlmPrt=gBitVoltDiff2MaxAlmPrt=gBitVoltDiff2MaxAlm=1; //压差H保护压差L保护压差L告警	
//   gBitChgHighTemAlm=gBitDchgHighTemAlm=1;                //一级高温
//   gBitChgLowTemAlm=gBitChgLowTemAlmPrt=gBitDchgLowTemAlm=gBitDchgLowTemAlmPrt=1;   //低温告警保护
	
				if(gBitDchgShortCircuit|gBitChgShortCircuit)  //短路
				{
					LED_W5_H;LED_W6_l;LED_W7_H;  //白色加红灯
				}
				else if(gBitOverVoltAlmPrt) //充电过压保护
				{
					LED_W5_H;LED_W6_l;LED_W7_H;		//白色加红灯			
				}
			 else if(gBitDchgHighTemAlmPrt|gBitChgHighTemAlmPrt) //充放电温度保护
			 {            //红灯闪烁
										if(R_led)
										{
											R_led =0;
											LED_W5_l;LED_W6_l;LED_W7_H;		
										}
										else 
										{
											R_led =1;
											LED_OFF;
										}					 
			 }
			 else if(gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt)  //充电过流告警  充电过流保护
			 {
				
				 if(gBitChgOverCurrentAlmPrt)   //充电过流保护
				 {
          LED_W5_H;LED_W6_l;LED_W7_H;	 //白灯+红灯
				 }	
         else		                      //充电过流告警
				 {
         LED_W5_H;LED_W6_l;LED_W7_l;	 //白灯
				 }					 
				 
			 }
			 else if(gBitUnderVoltAlmPrt|gBitUnderVoltAlm)   //欠压告警  欠压保护
			 {
				 if(gBitUnderVoltAlmPrt)
				 {
					LED_W5_H;LED_W6_l;LED_W7_H;	 //白灯+红灯 
				 }
				 else                 //欠压告警
				 {
					 
				 }
			 }
			else if(gBitSOClow)    //SOC低
			{
					if(B_led)
					{
						B_led =0;
						LED_W5_l;LED_W6_H;LED_W7_l;		
					}
					else 
					{
						B_led =1;
						LED_OFF;
					}						 
			}
			else if(gBitVoltDiff1MaxAlm|gBitVoltDiff1MaxAlmPrt|gBitVoltDiff2MaxAlmPrt|gBitVoltDiff2MaxAlm) //压差H保护压差L保护压差L告警
			{
			if(gBitVoltDiff1MaxAlm)
			{
				LED_W5_H;LED_W7_l;LED_W6_l;   //白灯
			}			
			else
			{
				LED_W5_l;LED_W6_l;LED_W7_H;	  //红灯
			}
				
			}
			else if(gBitChgHighTemAlm|gBitDchgHighTemAlm)                 //一级高温
			{
											if(G_led)
											{
												G_led =0;
                        LED_W5_H;LED_W6_l;LED_W7_l;			 									
											}
											else 
											{
												G_led =1;
												LED_OFF;			
											}				
			}
			else if(gBitChgLowTemAlm|gBitChgLowTemAlmPrt|gBitDchgLowTemAlm|gBitDchgLowTemAlmPrt)   //低温告警保护
			{
												if(G_led)
											{
												G_led =0;
                        LED_W5_H;LED_W6_l;LED_W7_l;												
											}
											else 
											{
												G_led =1;
												LED_OFF;			
											}				
			}
			else
			{
				LED_OFF;
			}
}
/*********************************************************************************
*   函 数 名: led_display
*   功能说明: 
*   形    参：
*   返 回 值: 
*********************************************************************************/
void led_display(void)
{
	static uint32_t tickstart_LED = 0;
	static uint32_t ticksotp_LED = 0;
	static u8 task=0;
	//static u8 delay_falg=0;
   // static  BitAction x=Bit_SET;
	static u8 number=0;
	static u8 number_R=0;
	//static u8 num = 0;
	///////////////////////////////
	static u8 SOC_tick_cnt=0;//控制SOC显示时间
	static u8 SOH_tick_cnt=0;//控制SOH显示时间
	static u8 LED_tick_cnt=0;//控制LED全闪时间
	//////////////////////////////
	//第一遍执行不等待
	static u8 led_play_tick = 0;
//	GPIO_SetBits(GPIOB, LNVCC_EN);   //打开5v电源	
	 if(gSleep) //清休眠标志
	 {
		 number=0;
		 number_R=0;
		 gSleep = 0;
		led_play_tick = 0; 		 
	 }
	 /////////////////////////////////////////////
	 if(led_play_tick==1)//第一次先不计时，后续每500ms执行一遍后面的程序
	 {
		 if(tickstart_LED == 0)
		 {      
			 tickstart_LED = HAL_GetTick();
		 }

			ticksotp_LED = HAL_GetTick();
			if(ticksotp_LED<tickstart_LED)//
			{
			ticksotp_LED=0xffffffff-tickstart_LED+ticksotp_LED;
			}
			else
			ticksotp_LED=ticksotp_LED-tickstart_LED;
			
			if(ticksotp_LED>500)
			{
				tickstart_LED = 0;
				ticksotp_LED = 0;
			}
			else
			{
				return;
			}
	 }	
	led_play_tick = 1 ;	 
	////////////////////////////////////////////	
	if(key_ack_status==1)//按键要求显示SOC 
	{
		if(SOC_tick_cnt<14)
			SOC_tick_cnt++;	
		if(SOC_tick_cnt>=14)//5秒后按正常逻辑去显示
		{
			if(key_down_FLAG==0)//无按键按下
			{
		  	SOC_tick_cnt = 0;
			  key_ack_status = 0;
				LED_1_4_OFF;
			}
		}					
	}
 else{SOC_tick_cnt=0;}	
//////////////////////////////////////////////	
	if(key_ack_status==2) //按键要求显示SOH	
  {
		if(BMS_config.Pack_RW_config.gSOH <= 12)    //SOC电量对应 档位
			task = 0;
			else if((12 < BMS_config.Pack_RW_config.gSOH )&&( BMS_config.Pack_RW_config.gSOH <= 24))
			task = 1;
			else if((24 < BMS_config.Pack_RW_config.gSOH )&&(BMS_config.Pack_RW_config.gSOH<= 37))
			task = 2;
			else if((37 < BMS_config.Pack_RW_config.gSOH )&&(BMS_config.Pack_RW_config.gSOH<= 49))
			task = 3;
			else if((49 < BMS_config.Pack_RW_config.gSOH )&&(BMS_config.Pack_RW_config.gSOH<= 62))
			task = 4;
			else if((62 < BMS_config.Pack_RW_config.gSOH )&&(BMS_config.Pack_RW_config.gSOH<= 74))
			task = 5;	
			else if((74 < BMS_config.Pack_RW_config.gSOH )&&(BMS_config.Pack_RW_config.gSOH<= 87))
			task = 6;
			else 
			task = 7;
			//////////////////////
			SOH_tick_cnt++;
			if(SOH_tick_cnt >= 14)//500ms执行一遍，10遍等于5s
					{
						SOH_tick_cnt = 0;
						key_ack_status =0;
						LED_1_4_OFF;
					}
		 	 switch(task)
					{
					case 0:   //SOH0%-12%  
														switch(number)
															{
															case 0:   LED_1_4_OFF;                                        number=1;	break;
															case 1:   LED_W1_H;									                          number=0;	break;																
															default:  number=0;                                           break;
															}									
								
								break;
					case 1:    //SOH13%-24%	
                          LED_W1_H;	LED_W2_l;LED_W3_l;LED_W4_l;					
								break;
					case 2:    //SOH25%-37%
														 switch(number)
															{
															case 0:    LED_W1_H;	LED_W2_l;LED_W3_l;LED_W4_l;		              number=1;	break;
															case 1:    LED_W2_H;									                            number=0;	break;
															default:  number=0;                                           	  break;
															}	
					 

								break;
					case 3:    //SOH38%-49% 
                         LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;					
								break;
					case 4:     //SOH50%-62% 
													switch(number)
															{
															case 0:   LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;	number=1;	break;
															case 1:   LED_W3_H;									number=0;	break; 
															default:  number=0;                                           	break;
															}	

								break;
					case 5:      //SOH63%-74%
					                 LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;
								break;
					case 6:      //SOH75%-94%
												 switch(number)
															{
															case 0:   LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;number=1;	break;
															case 1:   LED_W4_H;									number=2;	break;
															default:  number=0;                                           	break;
															}	
								break;
					case 7:      //SOH95%-100%
									LED_1_4_ON;
								break;
					default:
							 task=0;	
								break;	
					}				
	 }	
  else
		{
			SOH_tick_cnt=0;
	   }	
//////////////////////////////////////////////				
	if(key_ack_status==3)   //按键要求4个LED全闪
	{
				//if(LED_tick_cnt<20)//闪10秒
					LED_tick_cnt++;
					if(LED_tick_cnt<2)
					{						
		      LED_1_4_OFF; 
					}
					else
					{
		      LED_1_4_ON;	//静置关闭LED
					}

					if(LED_tick_cnt>=10)
					{
						if(key_down_FLAG==0)//无按键按下
			      {
		  	    LED_tick_cnt = 0;
						key_ack_status = 0;
						LED_1_4_OFF;
		      	}
						
					}
			//	return;
	}
	else
	{
		LED_tick_cnt=0;
	}
					
//////////////////////////////////////////////////////////////////////////
if(key_ack_status<=1)//无按键LED显示请求，或者有但是请求显示SOC，SOC的显示与放电、静止的显示方式一样
{
//////////////////////////////////////////////////////////////////////////////
  if((BAT_STATE_Curren == 1)&&(key_ack_status==0))  //充电模式下且无按键LED显示请求
				{ 
					if(BMS_config.Pack_RW_config.gSOC <= 12)   //SOC电量对应 档位
					task = 0;
					else if((12 < BMS_config.Pack_RW_config.gSOC )&&( BMS_config.Pack_RW_config.gSOC <= 37))
					task = 1;
          else if((37 < BMS_config.Pack_RW_config.gSOC )&&(BMS_config.Pack_RW_config.gSOC<= 62))
					task = 2;
					else if((62 < BMS_config.Pack_RW_config.gSOC )&&(BMS_config.Pack_RW_config.gSOC<= 94))
					task = 3;
					else 
					task = 4;
					switch(task)
					{
							case 0:     //充电0%-12%
													 switch(number_R)
																	{
																	case 0:   LED_1_4_OFF;                                        number_R=1; break;
																	case 1:   LED_W1_H;									                          number_R=2;	 break;
																	case 2:   LED_W1_H;LED_W2_H;                                  number_R=3;	 break;
																	case 3:   LED_W1_H;LED_W2_H; LED_W3_H;                        number_R=4;	 break; 
																	case 4:   LED_W1_H;LED_W2_H; LED_W3_H;LED_W4_H;               number_R=0;	 break;  
																	default:  number=0;                                           	break;
																	}										
										break;
							case 1:     //充电13%-37%
													 switch(number_R)
																	{
																	case 0:   LED_W1_H;LED_W2_l;LED_W3_l;LED_W4_l;                  number_R=1;	break;
																	case 1:   LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;									number_R=2;break;
																	case 2:   LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;                 	number_R=3;break;
																	case 3:   LED_1_4_ON;                  	                        number_R=0;break; 
																	default:  number=0;                                           	break;
																	}	
										break;
							case 2:      //充电38%-62%
													switch(number_R)
																	{
																	case 0:   LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;                 number_R=1;	break;
																	case 1:   LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;                  number_R=2;	break;
																	case 2:   LED_1_4_ON;                                           number_R=0;	break; 
																	default:  number=0;                                           	break;
																	}	
										break;
							case 3:     //充电63%-94%
													 switch(number_R)
																	{
																	case 0:    LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;               number_R=1;	break;
																	case 1:    LED_1_4_ON;                                        number_R=0;	break;
																	default:  number=0;                                           	break;
																	}	
										 break;
							case 4:    //充电95%-100%  
										LED_1_4_ON;
										break;
							default:
									 task=0;	
										break;	
					}
					
				}
		   else if(((BAT_STATE_Curren == 2)||(BAT_State==FreeMode)||(key_ack_status==1))&&(key_ack_status!=3))   //放电/静止/按键SOC显示
        {
					if(BMS_config.Pack_RW_config.gSOC <= 12)    //SOC电量对应 档位
					task = 0;
					else if((12 < BMS_config.Pack_RW_config.gSOC )&&( BMS_config.Pack_RW_config.gSOC <= 24))
					task = 1;
          else if((24 < BMS_config.Pack_RW_config.gSOC )&&(BMS_config.Pack_RW_config.gSOC<= 37))
					task = 2;
					else if((37 < BMS_config.Pack_RW_config.gSOC )&&(BMS_config.Pack_RW_config.gSOC<= 49))
					task = 3;
					else if((49 < BMS_config.Pack_RW_config.gSOC )&&(BMS_config.Pack_RW_config.gSOC<= 62))
					task = 4;
					else if((62 < BMS_config.Pack_RW_config.gSOC )&&(BMS_config.Pack_RW_config.gSOC<= 74))
					task = 5;	
					else if((74 < BMS_config.Pack_RW_config.gSOC )&&(BMS_config.Pack_RW_config.gSOC<= 87))
					task = 6;
					else 
					task = 7;		

					switch(task)
					{
							case 0:   //放电0%-12%  
																switch(number)
																	{
																	case 0:   LED_1_4_OFF;                                        number=1;	break;
																	case 1:   LED_W1_H;									                          number=0;	break;																
																	default:  number=0;                                           	break;
																	}									
										
										break;
							case 1:    //放电13%-24%
															LED_W1_H;LED_W2_l;LED_W3_l;LED_W4_l;
							
										break;
							case 2:    //放电25%-37%
																 switch(number)
																	{
																	case 0:   LED_W1_H;LED_W2_l;LED_W3_l;LED_W4_l;	               number=1;	break;
																	case 1:   LED_W2_H;									                           number=0;	break;
																	default:  number=0;                                           	break;
																	}	
							 

										break;
							case 3:    //放电38%-49% 
															LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;									
										break;
							case 4:     //放电50%-62% 
															switch(number)
																	{
																	case 0:   LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;	               number=1;	break;
																	case 1:   LED_W3_H;									                           number=0;	break; 
																	default:  number=0;                                           	break;
																	}	

										break;
							case 5:      //放电63%-74%
															LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;
										break;
							case 6:      //放电75%-94%
														 switch(number)
																	{
																	case 0:   LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;                 number=1;	 break;
																	case 1:   LED_W4_H;									                           number=2;	break;
																	default:  number=0;                                           	break;
																	}	
										break;
							case 7:      //放电95%-100%
											LED_1_4_ON;
										break;
							default:
									 task=0;	
										break;	
					}			
			}	
		}				

//////////////////////////////////////////////////////////////////////////////////////////					
	      if((BAT_State != Sleep)||(WKUP_enable==1))
			   {
					 set_led_Alm();
           if(gDchg_falg_PC)	
					 {
						 LED_W6_H;
					 }
			   	}

}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
u8 key_check(void)//按键扫描
  { 
    static u8 key_status = key_up;
    static u8 key_filter=0;
		if(!WK_UP)
		{
			if(key_filter<key_filter_time)
			key_filter++;
			if(key_filter==key_filter_time)
			{
				key_status = key_down;
			}
		}
		else
		{
			if(key_filter>0)
			key_filter--;
			if(key_filter==0)
			{
				key_status = key_up;
			}
		}
	  return key_status;
  }
/////////////////////////////////////////////////
//返回值为真代表有按键事件，并通过隐形参数传递按键按下时长及引进当前状态
////////////////////////////////////////////////
u8 key_scan(void)
{
 static u8 key_status_now = key_up;
 static u8 key_status_old =  key_up;
 static u16 key_down_star_time =0;
 u16   temp_time;
///////////////////////////////////////////////
 key_status_now=key_check();//获取引脚状态
//////////////按键按下/////////////////////
 if((key_status_now==key_down)&&(key_status_old==key_up)) //检测到按键按下
 {
   key_down_star_time = HAL_GetTick();//按键按下起始时间
   key_down_FLAG=1;//指示按键已经按下
 }
 //////////////////按键一直按着////////////////////////
 if(key_down_FLAG==1)//按键一直处于按下状态
 {
   temp_time = HAL_GetTick();
   if(temp_time<key_down_star_time)//
   {
	  temp_time=0xffffffff-key_down_star_time+temp_time;
   }
   else
   {
     temp_time = temp_time - key_down_star_time;
   }
  
   key_down_long_time= temp_time;//求得按键按下时长

 }
 /////////////////////按键弹起//////////////////////////////
  if((key_status_now==key_up)&&(key_status_old==key_down))//按键弹起
  {
    key_down_FLAG=0;//指示按键已经松
	  key_status_now=key_up;
	  key_status_old=key_up;
	  return 1;
 
  }else
  {
    key_status_old = key_status_now;
	  return 0;
  }
}	
void key_event_hander(void)
{
  static	u8 key_event_flag=0;

	//////////////////////////////////////
	key_event_flag = key_scan();
	 
/////////////////////////////////////
	if(key_down_FLAG==1)//有按键事件完成
	//if(key_event_flag==1)//有按键事件完成
	{
      if(BAT_State == Sleep)//当前系统状态为休眠
			{
				if((key_ack_status == 0)&&(key_finish_flg==0))//第一次按按键
				{
						if(key_down_long_time<5000)
						{
							if(key_finish_flg==0)
							{
								key_finish_flg=1;
								key_ack_status = 1;//LED函数那边显示SOC，5秒后没其他动作把该标志归0 
								LED_1_4_OFF;
								gSleep = 1;
							}
						}
				}
				else if((key_ack_status==1)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=5000)
					 {
						 key_ack_status = 2;//LED函数那边显示SOH，5秒后无其他动作该标志要归0
						 gSleep = 1;
						 LED_1_4_OFF; 
					 }
					
				}
				else if((key_ack_status==2)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=10000)
					 {
						  key_ack_status = 5;//按键无效
							LED_1_4_OFF;	//静置关闭LED	 
							LED_OFF; 	//静置关闭LED	
					 }
					
				}
			//////////////////////////////////////////////////////////////////////////
       else if((key_ack_status==1)&&(key_finish_flg==0))	//代表第二次按键
				{
					if(2000<key_down_long_time)
					{
				    	key_finish_flg=1;
							LED_1_4_OFF;	//静置关闭LED	 
							LED_OFF; 	//静置关闭LED	
              delay_ms(500);						
  						LED_W1_H;  delay_ms(500);	                					
					    LED_W2_H;		delay_ms(500);								
						  LED_W3_H;   delay_ms(500);	            
						  LED_W4_H;   delay_ms(500);	              
						 //关闭所有LED，随后从右到左依次全亮
						  key_ack_status = 0;
					  	BAT_State = FreeMode;//切换到静止模式
						  gSleep = 1; 
						  GPIO_SetBits(GPIOB, LNVCC_EN);   //打开5v电源	 
					}
					
				}
	  	}
	/////////////////////////////////////////////////////////
		 else if(BAT_State == FreeMode)//当前系统状态为静止
    	{
				
        if((key_ack_status == 0)&&(key_finish_flg==0))//第一次按按键
				{
					if(key_down_long_time<5000)
					{
					 key_finish_flg = 1;	
					 key_ack_status = 3;//LED函数那边全闪 闪10秒后清零
           gSleep = 1; 
//					GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
					}
				}
				else if((key_ack_status == 3)&&(key_finish_flg==1))//
				{
					if(key_down_long_time>5000)
					{
					//  key_ack_status = 2;//LED函数那边显示SOH，5秒后无其他动作该标志要归0
							LED_1_4_OFF;	//静置关闭LED	 
							LED_OFF; 	//静置关闭LED	
					}
				}
				else if((key_ack_status == 3)&&(key_finish_flg==0))//第二次按按键
				{
				    
					  if(key_down_long_time>=2000)//第二次按下时间大于2秒
						 { key_finish_flg=1;
							 LED_OFF;
							 LED_1_4_ON; delay_ms(500);		//静置关闭LED
               LED_W4_l;delay_ms(500);
							 LED_W3_l;delay_ms(500);
							 LED_W2_l;delay_ms(500);
							 LED_W1_l;delay_ms(500);
							 key_ack_status =4;
							 BAT_State = Sleep;//系统改变到休眠模式

							 }
				}
////////////////////////////////////////////////
       }
		/////////////////////////////////////////
			else//当前系统状态为充放电模式下 均衡与自放电显示SOH SOC
			{
				if((gBalance_falg == 1)||(gBalance_Dchg_falg == 1))
				{
					BAT_State = FreeMode;//切换到静止模式
					SEELP_cnt = 500;
					gBalance_UP = 1;
					gSelf_Dchg_UP = 1;	
					gSOC_Balance_Pc = 0;		
					BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//清除均衡状态位																										
					gBalance_pc = 0;
					gBalance_falg = 0;	//均衡完成
					BQ769X0_BalanceCtrl(0x00000000);																										
					//GPIO_ENABLE(DISABLE);
					//	结束上位机强制自放电	
					gDchg_falg_PC = 0;
					GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //自放电控制为1开启， 为0关闭
					GPIO_SetBits(GPIOx_W5_6,LED_W6);	 //自放电完成
					g_FaultAlarmState[6]&= ~(1<<3);	 
				  key_finish_flg=0;
					key_ack_status =0;
					return ;
				}
		   else	if((key_ack_status == 0)&&(key_finish_flg==0))//第一次按按键
				{
						if(key_down_long_time<5000)
						{
							if(key_finish_flg==0)
							{
								key_finish_flg=1;
								key_ack_status = 1;//LED函数那边显示SOC，5秒后没其他动作把该标志归0 
								GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
								gSleep = 1;
							}
						}
				}
				else if((key_ack_status==1)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=5000)
					 {
						 key_ack_status = 2;//LED函数那边显示SOH，5秒后无其他动作该标志要归0
						 gSleep = 1;
						 LED_1_4_OFF; 
					 }
					
				}
				else if((key_ack_status==2)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=10000)
					 {
						 key_ack_status = 5;//按键无效
						 //GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4); 
					 }
					
				}
			}
			
	 }
	////////////////////////////////////////////////
  else if(key_event_flag)//按键弹起
	 {
		 key_finish_flg=0;
		 key_down_long_time =0;
  	 g_Can_falg = 1;
		 if(key_ack_status==5)//当前事件响应处于什么状态
		  {
				key_ack_status=0;
		  }
	 }
	///////////////////////////////////////////
	else //无按键事件完成，按键也没有处于按下状态
	{
		;
	}
	
}
/*********************************************************************************
*   函 数 名: void LED_GPIO_TASK(void)
*   功能说明: 按键扫描响应及显示
*   形    参：无
*   返 回 值: 无
*********************************************************************************/
void LED_GPIO_TASK(void)
{
	led_display();	
	key_event_hander();

}

/************************ 论坛地址 www.zxkjmcu.com ******************************/
