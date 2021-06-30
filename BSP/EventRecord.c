//=============================================================================
//				           �����и����յ�����޹�˾
//                         ���Ӳ�Ʒ��			     
//-----------------------------------------------------------------------------
//�ļ�����: EventRecord.c
//�ļ���ʶ:
//ժҪ    : flash���ݴ洢
//-----------------------------------------------------------------------------
//��ǰ�汾: V1.0
//����    : dangxinkai
//�������: 2020.10.15
//=============================================================================


//=============================================================================================
//ͷ�ļ�
//=============================================================================================
#include "pbdata.h"
#include "EventRecord.h"
#include "Uart.h"
#include "stdbool.h"
//=============================================================================================
//������趨
//=============================================================================================


//=============================================================================================
//ȫ�ֱ���
//=============================================================================================
STR_EVENTDATA tCopyChrg;    //��ʼ���ʱ�����ݱ���
STR_EVENTDATA tEvtDataW;   //д��EEPROM��־�Ļ���
STR_EVENTDATA tEvtDataR;   //����EEPROM��־�Ļ���

STR_EVENTHEAD tAlmPrtEvtHead_W;	  //�����¼�ͷд
STR_EVENTHEAD tAlmPrtEvtHead_R;	  //�����¼�ͷ��
//STR_EVENTHEAD tEvtHead;    //��ʷ��־�¼�ͷ  

static u32 NUM_CNT =0;
extern bool Is_UsartSend ;
extern unsigned  	int  UART_Data_addr;
//static u32 NUM_CNT =0;

//=============================================================================
//��������: static u8 WriteEventEeprom(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: �¼���¼
//ע������: 
//=============================================================================
void  WriteEventEeprom(void)
{	
		static u8 Event_dealy_timer_staty=0;
		static u8 Event_dealy_timer_stop=0;
		static u8 Event_falg=0;
	  static u16 Event_Second = 0;
	  static u8 Count = 0;
	  static u8 DATA[256]={0};
  //  u8 DATA_a[256]={0};
		u16 temp_Sector_addr;//������ڵ�ַ
	if(ReadEvent_enable)
	{return;}
	if(Event_falg==0)  
	{
		Event_dealy_timer_staty=RTC_DateTimeStructure_NOW.Second;
		Event_falg = 1;
	}
	else
	{			
			if(RTC_DateTimeStructure_NOW.Second >= Event_dealy_timer_staty)
			Event_dealy_timer_stop=RTC_DateTimeStructure_NOW.Second - Event_dealy_timer_staty;
			else
			Event_dealy_timer_stop=RTC_DateTimeStructure_NOW.Second + 60 - Event_dealy_timer_staty;	
	}				
	Write_falsh_EvtData();
	if(tEvtDataW.index == 0 ) Count = 0;
	
			Event_Second = Event_Second+Event_dealy_timer_stop;
			Event_dealy_timer_staty=RTC_DateTimeStructure_NOW.Second;
			Event_dealy_timer_stop = 0;		
	            
	 switch( BAT_STATE_Curren ) 
							{
							 case 0:   										     
							      //��Сʱ��¼			
                    if(((BMS_config.CustomerCode.Free_tim <= Event_Second)&&(BMS_config.CustomerCode.Free_tim!=0))||(gFault_UP))
										{
	
														memcpy(&DATA[Count*64],&tEvtDataW,sizeof(tEvtDataW)); 
														Count++;
														tEvtDataW.index++;
														gFault_UP = 0;	
											      Event_Second = 0;
											
										}
								
										break;
	             case 1:   
  					 
								            if(BMS_config.CustomerCode.Chg_tim==0)
									          BMS_config.CustomerCode.Chg_tim= 1;
                     			if((Event_Second >= BMS_config.CustomerCode.Chg_tim)||(gFault_UP))//���5����һ��    //���ϼ�¼
			                   	{												
														memcpy(&DATA[Count*64],&tEvtDataW,sizeof(tEvtDataW)); 
														Count++;
														tEvtDataW.index++;
														 gFault_UP = 0;
														Event_Second = 0;
												
													}						 
										break;
							 case 2:   
								         if(BMS_config.CustomerCode.DChg_tim == 0)
													BMS_config.CustomerCode.DChg_tim = 1; 							 
                     			if((Event_Second >= BMS_config.CustomerCode.DChg_tim)||(gFault_UP))//�ŵ�1����һ��
			                   	{
														memcpy(&DATA[Count*64],&tEvtDataW,sizeof(tEvtDataW)); 
														Count++;
														tEvtDataW.index++;	
                             gFault_UP = 0;	
														Event_Second = 0;
                    												
													}		
							 
										break;
							 default:										
							      BAT_STATE_Curren = 0;
										break;	
						 }
	if(Count > 3)
	{	
		//memset(&DATA_a,0xff,256);
		
		//	W25QXX_Write_Page(DATA_a,tAlmPrtEvtHead_W.WriteEventAddr,0x100);
		 if((tAlmPrtEvtHead_W.WriteEventAddr%4096)==0)//������ڵ�ַ
		 {
			 temp_Sector_addr = tAlmPrtEvtHead_W.WriteEventAddr/4096;//һ������16ҳ
			 W25QXX_Erase_Sector(temp_Sector_addr);//������д������
		 }
  		tAlmPrtEvtHead_W.EventNum = tEvtDataW.index;
	    W25QXX_Write_Page(DATA,tAlmPrtEvtHead_W.WriteEventAddr,0x100);
      Count = 0; //�Ĵ��������		
//	W25QXX_Read(data,temp_ADDR,20);
//		W25QXX_Read(DATA_a,tAlmPrtEvtHead_W.WriteEventAddr,0x100);
	    tAlmPrtEvtHead_W.WriteEventAddr=tAlmPrtEvtHead_W.WriteEventAddr+0x100;//��һ��Ҫ�洢��ҳ��ַ
		//	if(tAlmPrtEvtHead_W.WriteEventAddr>EVENT_MAX_ADDR) //Խ����¿�ʼ
		 if(tAlmPrtEvtHead_W.EventNum>=250000)
			{	tAlmPrtEvtHead_W.WriteEventAddr = 0 ;
			   tAlmPrtEvtHead_W.EventNum = 0 ;
			}				
			        Alm_Number.WriteEventAddr  = tAlmPrtEvtHead_W.WriteEventAddr;
							Alm_Number.EventNum = tAlmPrtEvtHead_W.EventNum;
	}
		
}


//=============================================================================
//��������: void ReadEventRecord(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: ����EEPROM�е��¼�������
//ע������: 
//=============================================================================
void ReadEventRecord(void)
{	
	u32 temp_cnt;
	u32 temp_ADDR;
	u32 temp_LOOP_NO;
	u32 record_num=0;//?????????
	u32 read_shart_addr;//��ȡ��������ַ
	u8  finded_shart_flag=0;//???????
	u32  RecordIndex = 0; 
	u8  data[20];
	RecordIndex = tAlmPrtEvtHead_W.EventNum;
	if(tAlmPrtEvtHead_R.ReadEvent_loop==0)//��ȡ������ʷ����
	{

		if(RecordIndex>=(EVENT_MAX_ADDR/64))//���������ݸ���
		{
			if((tAlmPrtEvtHead_W.WriteEventAddr%4096)==0)//д������û��������
			{
				record_num = (EVENT_MAX_ADDR/64);
				read_shart_addr = tAlmPrtEvtHead_W.WriteEventAddr ;
			}
			else//��ʱ��ҳ��ַλ��ĳ���������м�
			{
				record_num = (EVENT_MAX_ADDR/64)-64+(((tAlmPrtEvtHead_W.WriteEventAddr%4096)/256)*4);//��
				read_shart_addr = (tAlmPrtEvtHead_W.WriteEventAddr/4096) ;//???????????
				read_shart_addr = read_shart_addr*4096+4096;//���ڱ�̵�������Ӧ����һ��������ڵ�ַ
			}

		}
		else//??????FLASH
		{
			read_shart_addr=0;//????????????0
			record_num = RecordIndex;//??????????????
		}
		tAlmPrtEvtHead_R.ReadEventAddr_st =read_shart_addr;
		tAlmPrtEvtHead_R.ReadEvent_Num = record_num;
	}
	else 
	{

		if(RecordIndex>=(EVENT_MAX_ADDR/64))//??FLASH????????
		{
			////////FLASH�ڵļ�¼��������ʼ��ַ
			if((tAlmPrtEvtHead_W.WriteEventAddr%4096)==0)//д������û��������
			{
				record_num = (EVENT_MAX_ADDR/64);
				read_shart_addr = tAlmPrtEvtHead_W.WriteEventAddr ;
			}
			else//��ʱ��ҳ��ַλ��ĳ���������м�
			{
				record_num = (EVENT_MAX_ADDR/64)-64+(((tAlmPrtEvtHead_W.WriteEventAddr%4096)/256)*4);//��
				read_shart_addr = (tAlmPrtEvtHead_W.WriteEventAddr/4096) ;//???????????
				read_shart_addr = read_shart_addr*4096+4096;//���ڱ�̵�������Ӧ����һ��������ڵ�ַ����ѯ���ݵ���ڵ�ַ
			}
			////////////////////////////////////////	
			temp_ADDR=read_shart_addr;//???????????? 
			for(temp_cnt=0;temp_cnt<(record_num);temp_cnt++)
			{
				// temp_LOOP_NO = read_flash(temp_ADDR+?);//?????????????
				W25QXX_Read(data,temp_ADDR,2);
				temp_LOOP_NO = (data[0]|(((u16)(data[1]))<<16));
				if(temp_LOOP_NO==(tAlmPrtEvtHead_R.ReadEvent_loop))
				{
					if(finded_shart_flag==0)//????????????
					{
						finded_shart_flag=1; //???????????
						read_shart_addr = temp_ADDR;//???????????
					}
					record_num++;//?????????
				}
				temp_ADDR=temp_ADDR+(64);
				if(temp_ADDR>=(EVENT_MAX_ADDR))
				{
					temp_ADDR=0;
				}
				if((finded_shart_flag==1)&&(temp_LOOP_NO!=(tAlmPrtEvtHead_R.ReadEvent_loop)))
				{
					tAlmPrtEvtHead_R.ReadEventAddr_st =read_shart_addr;
					tAlmPrtEvtHead_R.ReadEvent_Num = record_num;
					break;//????,?????????read_shart_addr,???????record_num?
				}
			}
		}
		else//û�в��������ݸ���
		{ 
			temp_ADDR=0;//????????????
			for(temp_cnt=0;temp_cnt<(RecordIndex);temp_cnt++)
			{
				// temp_LOOP_NO = read_flash(temp_ADDR);//?????????????
				W25QXX_Read(data,temp_ADDR,2);
				temp_LOOP_NO = (data[0]|(((u16)(data[1]))<<16));
				if(temp_LOOP_NO==(tAlmPrtEvtHead_R.ReadEvent_loop))
				{
					if(finded_shart_flag==0)//????????????
					{
						finded_shart_flag=1; //???????????
						read_shart_addr = temp_ADDR;//???????????
					}
					record_num++;//?????????
				}
				temp_ADDR=temp_ADDR+(64);//??????????
				if(((finded_shart_flag==1)&&(temp_LOOP_NO!=(tAlmPrtEvtHead_R.ReadEvent_loop)))||(temp_cnt==(RecordIndex-1)))
				{
					tAlmPrtEvtHead_R.ReadEventAddr_st =read_shart_addr;
					tAlmPrtEvtHead_R.ReadEvent_Num = record_num;
					break;//????,?????????read_shart_addr,???????record_num?
				}
			}
		}


	}
	/*test*/
	//tAlmPrtEvtHead_R.ReadEvent_Num=500;   
	if(!Is_UsartSend)//��������Դ���
	{
		data[2] = 0x01;
		data[3] = 0xff;
		data[4] = 0x4f;
		data[5] = 0x04;
		data[6] = (u8)tAlmPrtEvtHead_R.ReadEvent_Num;
		data[7] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>8);
		data[8] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>16);
		data[9] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>24);
		CCITT_CRC16Init(&data[2],8); //����Ч��ֵ
		data[0]=	CCITT_CRC16&0x00ff;//CRC16��λ
		data[1]=	(CCITT_CRC16>>8)&0x00ff;//CRC16��λ	
		//         while(CAN_TX_Falg);
		timer6 =0;
		while(CAN_TX_Falg)
		{
			if(timer6>35)
			break;
		}
		memcpy(&CAN_T,&data,10);  //����汾��	
		CAN_T_LEN_SUM = 10;
		//		NUM_CNT =0;
		//									 CAN_TX_Falg = 1;

	}
	else 				//��������ģ��,�����ж�������ʷ��¼
	{
		char temp_i=0;
		UsartReturn_Buffer[temp_i++]=0x55;
		UsartReturn_Buffer[temp_i++]=0xAA;	
		UsartReturn_Buffer[temp_i++]=0X00;
		UsartReturn_Buffer[temp_i++]=0X00;
		UsartReturn_Buffer[temp_i++]=0XAA;
		UsartReturn_Buffer[temp_i++]=0X55;
		UsartReturn_Buffer[temp_i++]=1;
		UsartReturn_Buffer[temp_i++]=0xff;
		UsartReturn_Buffer[temp_i++]=0x4f;
		UsartReturn_Buffer[temp_i++]=4;//���ݳ��ȹ̶�4
		UsartReturn_Buffer[temp_i++] = (u8)tAlmPrtEvtHead_R.ReadEvent_Num & 0xFF;
		UsartReturn_Buffer[temp_i++] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>8) & 0xFF;
		UsartReturn_Buffer[temp_i++] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>16) & 0xFF;
		UsartReturn_Buffer[temp_i++] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>24) & 0xFF;
		CCITT_CRC16Init(&UsartReturn_Buffer[6],8); 
		UsartReturn_Buffer[temp_i++]=(char)CCITT_CRC16;
		UsartReturn_Buffer[temp_i++]=(char)(CCITT_CRC16>>8);
		USART_Send_string(UsartReturn_Buffer,16);
	}

	if(tAlmPrtEvtHead_R.ReadEvent_Num>0)
	{ 
		ReadEvent_enable =1;					//��ʷ���ݷ���ʹ��
		NUM_CNT = 0;							//��ʷ��������������			
	}
	
}
//////////////////////////////
//��ʷ����ÿһ�췢��
//////////////////////////////

void ReadEventRecord_TX(void)
{
//	u32 temp_cnt,i;
//	static u32  temp_ADDR = 0 ; tAlmPrtEvtHead_R.ReadEventAddr_st ;
	//static u32 NUM_CNT =0;
	u8 i = 0;
	u8 data[65]= {0};

	if(NUM_CNT<tAlmPrtEvtHead_R.ReadEvent_Num)
	{
		
		if(tAlmPrtEvtHead_R.ReadEventAddr_st>=EVENT_MAX_ADDR)//�жϵ�ַ�Ƿ񵽶�
		{
			tAlmPrtEvtHead_R.ReadEventAddr_st=0;
		}
		W25QXX_Read(data,tAlmPrtEvtHead_R.ReadEventAddr_st,64);
		for(i=0;i<64;i++)//�����ۼ�У��ͷŵ����һ���ֽ�
		{
			data[64] = data[64]+data[i];
		}
		NUM_CNT++;
		if(!Is_UsartSend)//���Ǵ��ڷ���
		{			
			timer6 =0;
			while(CAN_TX_Falg)
			{
				if(timer6>35)
					break;
			}
			memcpy(&CAN_T[6],&data,sizeof(data));  //����汾��	
			CAN_T[2] = 0x01;
			CAN_T[3] = (u8)tAlmPrtEvtHead_R.ReadEvent_loop;
			CAN_T[4] = ((u8)tAlmPrtEvtHead_R.ReadEvent_loop>>8)|0x40;
			CAN_T[5] = 65;//��Ч�غ�65���ֽ�
			CAN_T_LEN_SUM =71;
			CAN_TX_Falg = 1;
			timer5 = 0;	
			CAN_id_add=0x016E416;//װ�����ݰ�29λID
		}
		else//�������Դ���
		{
			char temp_i=0;
			UsartReturn_Buffer[temp_i++]=0x55;
			UsartReturn_Buffer[temp_i++]=0xAA;	
			UsartReturn_Buffer[temp_i++]=0X00;
			UsartReturn_Buffer[temp_i++]=0X00;
			UsartReturn_Buffer[temp_i++]=0XAA;
			UsartReturn_Buffer[temp_i++]=0X55;
			UsartReturn_Buffer[temp_i++]=1;
			UsartReturn_Buffer[temp_i++]=(char)UART_Data_addr;
			UsartReturn_Buffer[temp_i++]=(char)(UART_Data_addr>>8);
			UsartReturn_Buffer[temp_i++]=65;//���ݳ��ȹ̶�65,ֻ��У���
			for(i=0;i<65;i++)//
			{
				UsartReturn_Buffer[temp_i++] = data[i];
			}
			CCITT_CRC16Init(&UsartReturn_Buffer[6],71); //����Ч��ֵ
			UsartReturn_Buffer[temp_i++]=	CCITT_CRC16;//CRC16��λ
			UsartReturn_Buffer[temp_i++]=	(CCITT_CRC16>>8);//CRC16��λ	
			USART_Send_string(UsartReturn_Buffer,77);
			timer5 = 0;
		}
		tAlmPrtEvtHead_R.ReadEventAddr_st=tAlmPrtEvtHead_R.ReadEventAddr_st+(64);

   	}
	if(NUM_CNT==tAlmPrtEvtHead_R.ReadEvent_Num)
	{
		NUM_CNT=0;
		ReadEvent_enable=0;//��ֹ�ٵ��ô˺���
		tAlmPrtEvtHead_R.ReadEvent_Num = 0;
		tAlmPrtEvtHead_R.ReadEventAddr_st=0;
		if(!Is_UsartSend)//��������Դ���	
		{
			data[2] = 0x01;
			data[3] = 0xff;
			data[4] = 0x4f;
			data[5] = 0x04;
			data[6] = 0xff;
			data[7] = 0xff;
			data[8] = 0xff;
			data[9] = 0xff;
			CCITT_CRC16Init(&data[2],8); //����Ч��ֵ
			data[0] = CCITT_CRC16&0x00ff;//CRC16��λ
			data[1] = (CCITT_CRC16>>8)&0x00ff;//CRC16��λ	
			timer6 =0;
			while(CAN_TX_Falg)
			{
				if(timer6>35)
					break;
			}
			memcpy(&CAN_T,&data,10);  //����汾��	
			CAN_id_add=0x016E416;//װ�����ݰ�29λID
			timer5 = 0;	
			CAN_T_LEN_SUM = 10;
			CAN_TX_Falg = 1;

			//���ͽ���ָ��
		}
	//	else//���Դ��ڵ�����
		if((NUM_CNT==tAlmPrtEvtHead_R.ReadEvent_Num)&&(Is_UsartSend))
		{
			char temp_i=0;
			UsartReturn_Buffer[temp_i++]=0x55;
			UsartReturn_Buffer[temp_i++]=0xAA;	
			UsartReturn_Buffer[temp_i++]=0X00;
			UsartReturn_Buffer[temp_i++]=0X00;
			UsartReturn_Buffer[temp_i++]=0XAA;
			UsartReturn_Buffer[temp_i++]=0X55;
			UsartReturn_Buffer[temp_i++]=1;
			UsartReturn_Buffer[temp_i++]=0xff;
			UsartReturn_Buffer[temp_i++]=0x4f;
			UsartReturn_Buffer[temp_i++]=04;//���ݳ��ȹ̶�4
			UsartReturn_Buffer[temp_i++] = 0xff;
			UsartReturn_Buffer[temp_i++] = 0xff;
			UsartReturn_Buffer[temp_i++] = 0xff;
			UsartReturn_Buffer[temp_i++] = 0xff;
			CCITT_CRC16Init(&UsartReturn_Buffer[6],8); //����Ч��ֵ
			UsartReturn_Buffer[temp_i++]=	CCITT_CRC16;//CRC16��λ
			UsartReturn_Buffer[temp_i++]=	(CCITT_CRC16>>8);//CRC16��λ	
			USART_Send_string(UsartReturn_Buffer,16);
			
		}
	}	
}
