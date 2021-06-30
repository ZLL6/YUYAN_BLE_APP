#include "pbdata.h"
#include "data.h"
Key_TypeDef Key_set;
#define key_down 0
#define key_up 1
#define key_filter_time 10 
//static u8 key_led_num = 0;
 u8 key_down_FLAG=0;
static u16 key_down_long_time =0;
u8 key_ack_status=0;//��ǰ�¼���Ӧ����״̬Ϊ��Ϊ0ʱ����Ҫ����
volatile u8 key_finish_flg=0;
/*********************************************************************************
*   �� �� ��: JDQ_GPIO
*   ����˵��: JDQ��GPIO�ܽų�ʼ��
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************/
void LED_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	//LED����7��GPIO										    
	GPIO_InitStructure.GPIO_Pin = LED_W1|LED_W2|LED_W3|LED_W4|GPIO_Pin_15;           	  /*ѡ��Ҫ���Ƶ�GPIOE����*/				
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;       	/*����Ҫ���Ƶ�GPIOE����Ϊ���ģʽ*/	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	/*������������Ϊ50MHz */ 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      /*��������ģʽΪ����*/
	GPIO_Init(GPIOx_W1_4, &GPIO_InitStructure);	         /*���ÿ⺯������ʼ��GPIOB*/
	GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
	GPIO_InitStructure.GPIO_Pin = LED_W5|LED_W6|HOLD;           	  /*ѡ��Ҫ���Ƶ�GPIOA����*/				
	GPIO_Init(GPIOx_W5_6, &GPIO_InitStructure);	         /*���ÿ⺯������ʼ��GPIOA*/
	GPIO_SetBits(GPIOx_W5_6,LED_W5|LED_W6);
	GPIO_InitStructure.GPIO_Pin = LED_W7;           	  /*ѡ��Ҫ���Ƶ�GPIOA����*/				
	GPIO_Init(GPIOx_W7, &GPIO_InitStructure);	         /*���ÿ⺯������ʼ��GPIOA*/
	GPIO_SetBits(GPIOx_W7,LED_W7);	
	 //W25q128����
	GPIO_InitStructure.GPIO_Pin = W25Q128_EN;
	GPIO_Init(GPIOx_W25Q128_EN, &GPIO_InitStructure); 
	GPIO_InitStructure.GPIO_Pin = W25Q128_WP;
	GPIO_Init(GPIOx_W25Q128_WP, &GPIO_InitStructure); 
	GPIO_ResetBits(GPIOx_W25Q128_EN,W25Q128_EN);    //W25q128���Ƶ�ԴΪ0������ Ϊ1�ر�					
	GPIO_SetBits(GPIOx_HOLD,HOLD);    //�Էŵ����Ϊ0������ Ϊ1�ر�
	GPIO_SetBits(GPIOx_W25Q128_WP,W25Q128_WP);    //WP��ƽְ��
	
//	//�ܵ�Դ����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Pin = LNVCC_EN;            //�ܵ�Դ����
	GPIO_Init(GPIOx_LNVCC_EN, &GPIO_InitStructure);	
	
	//BQ76940ģ��I2C
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;   /*!< Configure Sim1_I2C pins: SCL */
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;    /*!< Configure Sim1_I2C pins: SDA */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14);   //���ܵ�Դ
	//��ʼ������					
	GPIO_InitStructure.GPIO_Pin =  WKUP_PIN;  //��ʼ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(WKUP_PORT, &GPIO_InitStructure);		
 //CAN�ļ����ź�
	GPIO_InitStructure.GPIO_Pin =  COMWK_PIN;  //CAN�ļ����ź�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(COMWK_PORT, &GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Pin =  ALERT;  //BQ76940��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOx_ALERT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = btdate;           	  /*ѡ��Ҫ���Ƶ�GPIOB����*/				
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;       	/*����Ҫ���Ƶ�GPIOB����Ϊ���ģʽ*/	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	/*������������Ϊ50MHz */ 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 	    /*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;      /*��������ģʽΪ����*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	         /*���ÿ⺯������ʼ��GPIOB*/
	//GPIO_ResetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
	GPIO_ResetBits(GPIOB,btdate);
	//BQ76940��������
	GPIO_InitStructure.GPIO_Pin = POW_SAVE; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_Init(GPIOx_POW_SAVE, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOx_POW_SAVE,POW_SAVE);  //BQ76940�������� Ϊ0������ Ϊ1�ر�
	//�Էŵ����
	GPIO_InitStructure.GPIO_Pin = SELF_DISCHAR;
	GPIO_Init(GPIOx_SELF_DISCHAR, &GPIO_InitStructure); 
	GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�

}
void GPIO_ENABLE( FunctionalState x)
{
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	if(x == ENABLE)
	{
  	GPIO_SetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14);//����BQ76940
	  GPIO_ResetBits(GPIOx_POW_SAVE,POW_SAVE);  //BQ76940�������� Ϊ0������ Ϊ1�ر�
		GPIO_SetBits(GPIOB, LNVCC_EN);   //���ܵ�Դ
		GPIO_ResetBits(GPIOx_W25Q128_EN,W25Q128_EN);    //W25q128���Ƶ�ԴΪ0������ Ϊ1�ر�
		GPIO_SetBits(GPIOx_HOLD,HOLD);    //�Էŵ����Ϊ0������ Ϊ1�ر�
		GPIO_SetBits(GPIOx_W25Q128_WP,W25Q128_WP);    //WP��ƽְ��		
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
	//2021/5/14 �޸�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);///ȫ������
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
			GPIO_ResetBits(GPIOB, LNVCC_EN);   //5V��Դ
			GPIO_ResetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14); 
			GPIO_InitStructure.GPIO_Pin = POW_SAVE;
		  GPIO_Init(GPIOA, &GPIO_InitStructure);
      GPIO_SetBits(GPIOx_POW_SAVE,POW_SAVE);  //BQ76940
			GPIO_InitStructure.GPIO_Pin =  ALERT;  //BQ76940��������
	    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	    GPIO_Init(GPIOx_ALERT, &GPIO_InitStructure);	
		  GPIO_ResetBits(GPIOx_ALERT,ALERT);
				
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;	
		  GPIO_InitStructure.GPIO_Pin =  SELF_DISCHAR;	//�Էŵ�	
			GPIO_Init(GPIOx_SELF_DISCHAR, &GPIO_InitStructure);
			GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR); 
			
//û���õ���IO��
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
//��?��?��??????a1?a??��? ?a01?��?	
//		GPIO_ResetBits(GPIOB, GPIO_Pin_13|GPIO_Pin_14);//2������BQ76940
//W25q128????��??��?a0?a??��? ?a11?��?
		
	}
}

/********************************************
*   �� �� ��: HAL_GetTick
*   ����˵��: ��ȡϵͳʱ��
*   ��    �Σ���
*   �� �� ֵ: ��
********************************************/
__weak uint32_t HAL_GetTick(void)
{
  return timer1;
}


///************************ ��̳��ַ www.zxkjmcu.com ******************************/
//u8 key_check(void)//����ɨ��
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
//    case KEY_Ready:          //����������
//	//	if(!key_check())
//			if(!WK_UP)
//		{
//		Key_set.Key_state = KEY_Down;
//		Key_set.Key_Down_Timer = 0;
//		}
//		 break;
//    case KEY_Down:           //��������
//		
//	// if(!key_check())
//			if(!WK_UP)
//	 {
//	 	  Key_set.Key_state = KEY_Lock;
//		 	tickstart_key = HAL_GetTick();  //��ȡ����ʱ��
//	 }
//	 else
//	 	Key_set.Key_state = KEY_Ready;   //���Ÿ߳�
//		 break;
//    case KEY_Lock:         //��������
//					ticksotp_key = HAL_GetTick();
//				if(ticksotp_key<tickstart_key)//
//				{
//				ticksotp_key=0xffffffff-tickstart_key+ticksotp_key;
//				}
//				else
//				ticksotp_key=ticksotp_key-tickstart_key;

//				Key_set.Key_Down_Timer = ticksotp_key;
//		//if(key_check())  //�����ͷ�
//		if(WK_UP)
//		{
//				Key_set.Key_state = KEY_Ulock;
//		}
//       	 break;
//    case KEY_Ulock:       //û�а�������
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
*   �� �� ��: void  set_led_Alm(void)
*   ����˵��: 
*   ��    �Σ�
  �� �� ֵ: 
************************************/
void  set_led_Alm(void)
{
	static u8 R_led= 0;
	static u8 B_led= 0;
	static u8 G_led= 0;
//	gBitDchgShortCircuit=1;
//	gBitChgShortCircuit=1; //��·
//	gBitDchgHighTemAlmPrt=gBitChgHighTemAlmPrt=1; //��ŵ��¶ȱ���
 // gBitChgOverCurrentAlm=1;
//	gBitChgOverCurrentAlmPrt=1;  //�������澯  ����������	
//  gBitUnderVoltAlmPrt =gBitUnderVoltAlm=1;   //Ƿѹ�澯  Ƿѹ����
//  gBitSOClow = 1;
//	gBitVoltDiff1MaxAlm=1;  //ѹ��L�澯	
//   gBitVoltDiff1MaxAlmPrt=gBitVoltDiff2MaxAlmPrt=gBitVoltDiff2MaxAlm=1; //ѹ��H����ѹ��L����ѹ��L�澯	
//   gBitChgHighTemAlm=gBitDchgHighTemAlm=1;                //һ������
//   gBitChgLowTemAlm=gBitChgLowTemAlmPrt=gBitDchgLowTemAlm=gBitDchgLowTemAlmPrt=1;   //���¸澯����
	
				if(gBitDchgShortCircuit|gBitChgShortCircuit)  //��·
				{
					LED_W5_H;LED_W6_l;LED_W7_H;  //��ɫ�Ӻ��
				}
				else if(gBitOverVoltAlmPrt) //����ѹ����
				{
					LED_W5_H;LED_W6_l;LED_W7_H;		//��ɫ�Ӻ��			
				}
			 else if(gBitDchgHighTemAlmPrt|gBitChgHighTemAlmPrt) //��ŵ��¶ȱ���
			 {            //�����˸
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
			 else if(gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt)  //�������澯  ����������
			 {
				
				 if(gBitChgOverCurrentAlmPrt)   //����������
				 {
          LED_W5_H;LED_W6_l;LED_W7_H;	 //�׵�+���
				 }	
         else		                      //�������澯
				 {
         LED_W5_H;LED_W6_l;LED_W7_l;	 //�׵�
				 }					 
				 
			 }
			 else if(gBitUnderVoltAlmPrt|gBitUnderVoltAlm)   //Ƿѹ�澯  Ƿѹ����
			 {
				 if(gBitUnderVoltAlmPrt)
				 {
					LED_W5_H;LED_W6_l;LED_W7_H;	 //�׵�+��� 
				 }
				 else                 //Ƿѹ�澯
				 {
					 
				 }
			 }
			else if(gBitSOClow)    //SOC��
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
			else if(gBitVoltDiff1MaxAlm|gBitVoltDiff1MaxAlmPrt|gBitVoltDiff2MaxAlmPrt|gBitVoltDiff2MaxAlm) //ѹ��H����ѹ��L����ѹ��L�澯
			{
			if(gBitVoltDiff1MaxAlm)
			{
				LED_W5_H;LED_W7_l;LED_W6_l;   //�׵�
			}			
			else
			{
				LED_W5_l;LED_W6_l;LED_W7_H;	  //���
			}
				
			}
			else if(gBitChgHighTemAlm|gBitDchgHighTemAlm)                 //һ������
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
			else if(gBitChgLowTemAlm|gBitChgLowTemAlmPrt|gBitDchgLowTemAlm|gBitDchgLowTemAlmPrt)   //���¸澯����
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
*   �� �� ��: led_display
*   ����˵��: 
*   ��    �Σ�
*   �� �� ֵ: 
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
	static u8 SOC_tick_cnt=0;//����SOC��ʾʱ��
	static u8 SOH_tick_cnt=0;//����SOH��ʾʱ��
	static u8 LED_tick_cnt=0;//����LEDȫ��ʱ��
	//////////////////////////////
	//��һ��ִ�в��ȴ�
	static u8 led_play_tick = 0;
//	GPIO_SetBits(GPIOB, LNVCC_EN);   //��5v��Դ	
	 if(gSleep) //�����߱�־
	 {
		 number=0;
		 number_R=0;
		 gSleep = 0;
		led_play_tick = 0; 		 
	 }
	 /////////////////////////////////////////////
	 if(led_play_tick==1)//��һ���Ȳ���ʱ������ÿ500msִ��һ�����ĳ���
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
	if(key_ack_status==1)//����Ҫ����ʾSOC 
	{
		if(SOC_tick_cnt<14)
			SOC_tick_cnt++;	
		if(SOC_tick_cnt>=14)//5��������߼�ȥ��ʾ
		{
			if(key_down_FLAG==0)//�ް�������
			{
		  	SOC_tick_cnt = 0;
			  key_ack_status = 0;
				LED_1_4_OFF;
			}
		}					
	}
 else{SOC_tick_cnt=0;}	
//////////////////////////////////////////////	
	if(key_ack_status==2) //����Ҫ����ʾSOH	
  {
		if(BMS_config.Pack_RW_config.gSOH <= 12)    //SOC������Ӧ ��λ
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
			if(SOH_tick_cnt >= 14)//500msִ��һ�飬10�����5s
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
	if(key_ack_status==3)   //����Ҫ��4��LEDȫ��
	{
				//if(LED_tick_cnt<20)//��10��
					LED_tick_cnt++;
					if(LED_tick_cnt<2)
					{						
		      LED_1_4_OFF; 
					}
					else
					{
		      LED_1_4_ON;	//���ùر�LED
					}

					if(LED_tick_cnt>=10)
					{
						if(key_down_FLAG==0)//�ް�������
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
if(key_ack_status<=1)//�ް���LED��ʾ���󣬻����е���������ʾSOC��SOC����ʾ��ŵ硢��ֹ����ʾ��ʽһ��
{
//////////////////////////////////////////////////////////////////////////////
  if((BAT_STATE_Curren == 1)&&(key_ack_status==0))  //���ģʽ�����ް���LED��ʾ����
				{ 
					if(BMS_config.Pack_RW_config.gSOC <= 12)   //SOC������Ӧ ��λ
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
							case 0:     //���0%-12%
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
							case 1:     //���13%-37%
													 switch(number_R)
																	{
																	case 0:   LED_W1_H;LED_W2_l;LED_W3_l;LED_W4_l;                  number_R=1;	break;
																	case 1:   LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;									number_R=2;break;
																	case 2:   LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;                 	number_R=3;break;
																	case 3:   LED_1_4_ON;                  	                        number_R=0;break; 
																	default:  number=0;                                           	break;
																	}	
										break;
							case 2:      //���38%-62%
													switch(number_R)
																	{
																	case 0:   LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;                 number_R=1;	break;
																	case 1:   LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;                  number_R=2;	break;
																	case 2:   LED_1_4_ON;                                           number_R=0;	break; 
																	default:  number=0;                                           	break;
																	}	
										break;
							case 3:     //���63%-94%
													 switch(number_R)
																	{
																	case 0:    LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;               number_R=1;	break;
																	case 1:    LED_1_4_ON;                                        number_R=0;	break;
																	default:  number=0;                                           	break;
																	}	
										 break;
							case 4:    //���95%-100%  
										LED_1_4_ON;
										break;
							default:
									 task=0;	
										break;	
					}
					
				}
		   else if(((BAT_STATE_Curren == 2)||(BAT_State==FreeMode)||(key_ack_status==1))&&(key_ack_status!=3))   //�ŵ�/��ֹ/����SOC��ʾ
        {
					if(BMS_config.Pack_RW_config.gSOC <= 12)    //SOC������Ӧ ��λ
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
							case 0:   //�ŵ�0%-12%  
																switch(number)
																	{
																	case 0:   LED_1_4_OFF;                                        number=1;	break;
																	case 1:   LED_W1_H;									                          number=0;	break;																
																	default:  number=0;                                           	break;
																	}									
										
										break;
							case 1:    //�ŵ�13%-24%
															LED_W1_H;LED_W2_l;LED_W3_l;LED_W4_l;
							
										break;
							case 2:    //�ŵ�25%-37%
																 switch(number)
																	{
																	case 0:   LED_W1_H;LED_W2_l;LED_W3_l;LED_W4_l;	               number=1;	break;
																	case 1:   LED_W2_H;									                           number=0;	break;
																	default:  number=0;                                           	break;
																	}	
							 

										break;
							case 3:    //�ŵ�38%-49% 
															LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;									
										break;
							case 4:     //�ŵ�50%-62% 
															switch(number)
																	{
																	case 0:   LED_W1_H;LED_W2_H;LED_W3_l;LED_W4_l;	               number=1;	break;
																	case 1:   LED_W3_H;									                           number=0;	break; 
																	default:  number=0;                                           	break;
																	}	

										break;
							case 5:      //�ŵ�63%-74%
															LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;
										break;
							case 6:      //�ŵ�75%-94%
														 switch(number)
																	{
																	case 0:   LED_W1_H;LED_W2_H;LED_W3_H;LED_W4_l;                 number=1;	 break;
																	case 1:   LED_W4_H;									                           number=2;	break;
																	default:  number=0;                                           	break;
																	}	
										break;
							case 7:      //�ŵ�95%-100%
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
u8 key_check(void)//����ɨ��
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
//����ֵΪ������а����¼�����ͨ�����β������ݰ�������ʱ����������ǰ״̬
////////////////////////////////////////////////
u8 key_scan(void)
{
 static u8 key_status_now = key_up;
 static u8 key_status_old =  key_up;
 static u16 key_down_star_time =0;
 u16   temp_time;
///////////////////////////////////////////////
 key_status_now=key_check();//��ȡ����״̬
//////////////��������/////////////////////
 if((key_status_now==key_down)&&(key_status_old==key_up)) //��⵽��������
 {
   key_down_star_time = HAL_GetTick();//����������ʼʱ��
   key_down_FLAG=1;//ָʾ�����Ѿ�����
 }
 //////////////////����һֱ����////////////////////////
 if(key_down_FLAG==1)//����һֱ���ڰ���״̬
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
  
   key_down_long_time= temp_time;//��ð�������ʱ��

 }
 /////////////////////��������//////////////////////////////
  if((key_status_now==key_up)&&(key_status_old==key_down))//��������
  {
    key_down_FLAG=0;//ָʾ�����Ѿ���
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
	if(key_down_FLAG==1)//�а����¼����
	//if(key_event_flag==1)//�а����¼����
	{
      if(BAT_State == Sleep)//��ǰϵͳ״̬Ϊ����
			{
				if((key_ack_status == 0)&&(key_finish_flg==0))//��һ�ΰ�����
				{
						if(key_down_long_time<5000)
						{
							if(key_finish_flg==0)
							{
								key_finish_flg=1;
								key_ack_status = 1;//LED�����Ǳ���ʾSOC��5���û���������Ѹñ�־��0 
								LED_1_4_OFF;
								gSleep = 1;
							}
						}
				}
				else if((key_ack_status==1)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=5000)
					 {
						 key_ack_status = 2;//LED�����Ǳ���ʾSOH��5��������������ñ�־Ҫ��0
						 gSleep = 1;
						 LED_1_4_OFF; 
					 }
					
				}
				else if((key_ack_status==2)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=10000)
					 {
						  key_ack_status = 5;//������Ч
							LED_1_4_OFF;	//���ùر�LED	 
							LED_OFF; 	//���ùر�LED	
					 }
					
				}
			//////////////////////////////////////////////////////////////////////////
       else if((key_ack_status==1)&&(key_finish_flg==0))	//����ڶ��ΰ���
				{
					if(2000<key_down_long_time)
					{
				    	key_finish_flg=1;
							LED_1_4_OFF;	//���ùر�LED	 
							LED_OFF; 	//���ùر�LED	
              delay_ms(500);						
  						LED_W1_H;  delay_ms(500);	                					
					    LED_W2_H;		delay_ms(500);								
						  LED_W3_H;   delay_ms(500);	            
						  LED_W4_H;   delay_ms(500);	              
						 //�ر�����LED�������ҵ�������ȫ��
						  key_ack_status = 0;
					  	BAT_State = FreeMode;//�л�����ֹģʽ
						  gSleep = 1; 
						  GPIO_SetBits(GPIOB, LNVCC_EN);   //��5v��Դ	 
					}
					
				}
	  	}
	/////////////////////////////////////////////////////////
		 else if(BAT_State == FreeMode)//��ǰϵͳ״̬Ϊ��ֹ
    	{
				
        if((key_ack_status == 0)&&(key_finish_flg==0))//��һ�ΰ�����
				{
					if(key_down_long_time<5000)
					{
					 key_finish_flg = 1;	
					 key_ack_status = 3;//LED�����Ǳ�ȫ�� ��10�������
           gSleep = 1; 
//					GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
					}
				}
				else if((key_ack_status == 3)&&(key_finish_flg==1))//
				{
					if(key_down_long_time>5000)
					{
					//  key_ack_status = 2;//LED�����Ǳ���ʾSOH��5��������������ñ�־Ҫ��0
							LED_1_4_OFF;	//���ùر�LED	 
							LED_OFF; 	//���ùر�LED	
					}
				}
				else if((key_ack_status == 3)&&(key_finish_flg==0))//�ڶ��ΰ�����
				{
				    
					  if(key_down_long_time>=2000)//�ڶ��ΰ���ʱ�����2��
						 { key_finish_flg=1;
							 LED_OFF;
							 LED_1_4_ON; delay_ms(500);		//���ùر�LED
               LED_W4_l;delay_ms(500);
							 LED_W3_l;delay_ms(500);
							 LED_W2_l;delay_ms(500);
							 LED_W1_l;delay_ms(500);
							 key_ack_status =4;
							 BAT_State = Sleep;//ϵͳ�ı䵽����ģʽ

							 }
				}
////////////////////////////////////////////////
       }
		/////////////////////////////////////////
			else//��ǰϵͳ״̬Ϊ��ŵ�ģʽ�� �������Էŵ���ʾSOH SOC
			{
				if((gBalance_falg == 1)||(gBalance_Dchg_falg == 1))
				{
					BAT_State = FreeMode;//�л�����ֹģʽ
					SEELP_cnt = 500;
					gBalance_UP = 1;
					gSelf_Dchg_UP = 1;	
					gSOC_Balance_Pc = 0;		
					BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;	//�������״̬λ																										
					gBalance_pc = 0;
					gBalance_falg = 0;	//�������
					BQ769X0_BalanceCtrl(0x00000000);																										
					//GPIO_ENABLE(DISABLE);
					//	������λ��ǿ���Էŵ�	
					gDchg_falg_PC = 0;
					GPIO_ResetBits(GPIOx_SELF_DISCHAR,SELF_DISCHAR);    //�Էŵ����Ϊ1������ Ϊ0�ر�
					GPIO_SetBits(GPIOx_W5_6,LED_W6);	 //�Էŵ����
					g_FaultAlarmState[6]&= ~(1<<3);	 
				  key_finish_flg=0;
					key_ack_status =0;
					return ;
				}
		   else	if((key_ack_status == 0)&&(key_finish_flg==0))//��һ�ΰ�����
				{
						if(key_down_long_time<5000)
						{
							if(key_finish_flg==0)
							{
								key_finish_flg=1;
								key_ack_status = 1;//LED�����Ǳ���ʾSOC��5���û���������Ѹñ�־��0 
								GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4);
								gSleep = 1;
							}
						}
				}
				else if((key_ack_status==1)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=5000)
					 {
						 key_ack_status = 2;//LED�����Ǳ���ʾSOH��5��������������ñ�־Ҫ��0
						 gSleep = 1;
						 LED_1_4_OFF; 
					 }
					
				}
				else if((key_ack_status==2)&&(key_finish_flg==1))	
				{
					 if(key_down_long_time>=10000)
					 {
						 key_ack_status = 5;//������Ч
						 //GPIO_SetBits(GPIOx_W1_4,LED_W1|LED_W2|LED_W3|LED_W4); 
					 }
					
				}
			}
			
	 }
	////////////////////////////////////////////////
  else if(key_event_flag)//��������
	 {
		 key_finish_flg=0;
		 key_down_long_time =0;
  	 g_Can_falg = 1;
		 if(key_ack_status==5)//��ǰ�¼���Ӧ����ʲô״̬
		  {
				key_ack_status=0;
		  }
	 }
	///////////////////////////////////////////
	else //�ް����¼���ɣ�����Ҳû�д��ڰ���״̬
	{
		;
	}
	
}
/*********************************************************************************
*   �� �� ��: void LED_GPIO_TASK(void)
*   ����˵��: ����ɨ����Ӧ����ʾ
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************/
void LED_GPIO_TASK(void)
{
	led_display();	
	key_event_hander();

}

/************************ ��̳��ַ www.zxkjmcu.com ******************************/
