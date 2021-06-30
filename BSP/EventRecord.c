//=============================================================================
//				           深圳市格瑞普电池有限公司
//                         电子产品部			     
//-----------------------------------------------------------------------------
//文件名称: EventRecord.c
//文件标识:
//摘要    : flash数据存储
//-----------------------------------------------------------------------------
//当前版本: V1.0
//作者    : dangxinkai
//完成日期: 2020.10.15
//=============================================================================


//=============================================================================================
//头文件
//=============================================================================================
#include "pbdata.h"
#include "EventRecord.h"
#include "Uart.h"
#include "stdbool.h"
//=============================================================================================
//宏参数设定
//=============================================================================================


//=============================================================================================
//全局变量
//=============================================================================================
STR_EVENTDATA tCopyChrg;    //开始充电时的数据备份
STR_EVENTDATA tEvtDataW;   //写入EEPROM日志的缓存
STR_EVENTDATA tEvtDataR;   //读出EEPROM日志的缓存

STR_EVENTHEAD tAlmPrtEvtHead_W;	  //故障事件头写
STR_EVENTHEAD tAlmPrtEvtHead_R;	  //故障事件头读
//STR_EVENTHEAD tEvtHead;    //历史日志事件头  

static u32 NUM_CNT =0;
extern bool Is_UsartSend ;
extern unsigned  	int  UART_Data_addr;
//static u32 NUM_CNT =0;

//=============================================================================
//函数名称: static u8 WriteEventEeprom(void)
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 事件记录
//注意事项: 
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
		u16 temp_Sector_addr;//扇区入口地址
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
							      //以小时记录			
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
                     			if((Event_Second >= BMS_config.CustomerCode.Chg_tim)||(gFault_UP))//充电5秒中一次    //故障记录
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
                     			if((Event_Second >= BMS_config.CustomerCode.DChg_tim)||(gFault_UP))//放电1秒中一次
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
		 if((tAlmPrtEvtHead_W.WriteEventAddr%4096)==0)//扇区入口地址
		 {
			 temp_Sector_addr = tAlmPrtEvtHead_W.WriteEventAddr/4096;//一个扇区16页
			 W25QXX_Erase_Sector(temp_Sector_addr);//擦除待写的扇区
		 }
  		tAlmPrtEvtHead_W.EventNum = tEvtDataW.index;
	    W25QXX_Write_Page(DATA,tAlmPrtEvtHead_W.WriteEventAddr,0x100);
      Count = 0; //四次数据完成		
//	W25QXX_Read(data,temp_ADDR,20);
//		W25QXX_Read(DATA_a,tAlmPrtEvtHead_W.WriteEventAddr,0x100);
	    tAlmPrtEvtHead_W.WriteEventAddr=tAlmPrtEvtHead_W.WriteEventAddr+0x100;//下一次要存储的页地址
		//	if(tAlmPrtEvtHead_W.WriteEventAddr>EVENT_MAX_ADDR) //越界从新开始
		 if(tAlmPrtEvtHead_W.EventNum>=250000)
			{	tAlmPrtEvtHead_W.WriteEventAddr = 0 ;
			   tAlmPrtEvtHead_W.EventNum = 0 ;
			}				
			        Alm_Number.WriteEventAddr  = tAlmPrtEvtHead_W.WriteEventAddr;
							Alm_Number.EventNum = tAlmPrtEvtHead_W.EventNum;
	}
		
}


//=============================================================================
//函数名称: void ReadEventRecord(void)
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 读出EEPROM中的事件计数器
//注意事项: 
//=============================================================================
void ReadEventRecord(void)
{	
	u32 temp_cnt;
	u32 temp_ADDR;
	u32 temp_LOOP_NO;
	u32 record_num=0;//?????????
	u32 read_shart_addr;//读取的启动地址
	u8  finded_shart_flag=0;//???????
	u32  RecordIndex = 0; 
	u8  data[20];
	RecordIndex = tAlmPrtEvtHead_W.EventNum;
	if(tAlmPrtEvtHead_R.ReadEvent_loop==0)//读取所有历史数据
	{

		if(RecordIndex>=(EVENT_MAX_ADDR/64))//发生过数据覆盖
		{
			if((tAlmPrtEvtHead_W.WriteEventAddr%4096)==0)//写函数还没擦除扇区
			{
				record_num = (EVENT_MAX_ADDR/64);
				read_shart_addr = tAlmPrtEvtHead_W.WriteEventAddr ;
			}
			else//此时的页地址位于某个扇区的中间
			{
				record_num = (EVENT_MAX_ADDR/64)-64+(((tAlmPrtEvtHead_W.WriteEventAddr%4096)/256)*4);//有
				read_shart_addr = (tAlmPrtEvtHead_W.WriteEventAddr/4096) ;//???????????
				read_shart_addr = read_shart_addr*4096+4096;//正在编程的扇区对应的下一个扇区入口地址
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
			////////FLASH内的记录总数和起始地址
			if((tAlmPrtEvtHead_W.WriteEventAddr%4096)==0)//写函数还没擦除扇区
			{
				record_num = (EVENT_MAX_ADDR/64);
				read_shart_addr = tAlmPrtEvtHead_W.WriteEventAddr ;
			}
			else//此时的页地址位于某个扇区的中间
			{
				record_num = (EVENT_MAX_ADDR/64)-64+(((tAlmPrtEvtHead_W.WriteEventAddr%4096)/256)*4);//有
				read_shart_addr = (tAlmPrtEvtHead_W.WriteEventAddr/4096) ;//???????????
				read_shart_addr = read_shart_addr*4096+4096;//正在编程的扇区对应的下一个扇区入口地址即查询数据的入口地址
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
		else//没有产生过数据覆盖
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
	if(!Is_UsartSend)//命令不是来自串口
	{
		data[2] = 0x01;
		data[3] = 0xff;
		data[4] = 0x4f;
		data[5] = 0x04;
		data[6] = (u8)tAlmPrtEvtHead_R.ReadEvent_Num;
		data[7] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>8);
		data[8] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>16);
		data[9] = (u8)(tAlmPrtEvtHead_R.ReadEvent_Num>>24);
		CCITT_CRC16Init(&data[2],8); //计算效验值
		data[0]=	CCITT_CRC16&0x00ff;//CRC16低位
		data[1]=	(CCITT_CRC16>>8)&0x00ff;//CRC16低位	
		//         while(CAN_TX_Falg);
		timer6 =0;
		while(CAN_TX_Falg)
		{
			if(timer6>35)
			break;
		}
		memcpy(&CAN_T,&data,10);  //软件版本号	
		CAN_T_LEN_SUM = 10;
		//		NUM_CNT =0;
		//									 CAN_TX_Falg = 1;

	}
	else 				//来自蓝牙模块,发送有多少条历史记录
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
		UsartReturn_Buffer[temp_i++]=4;//数据长度固定4
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
		ReadEvent_enable =1;					//历史数据发送使能
		NUM_CNT = 0;							//历史数据条数计数器			
	}
	
}
//////////////////////////////
//历史数据每一天发送
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
		
		if(tAlmPrtEvtHead_R.ReadEventAddr_st>=EVENT_MAX_ADDR)//判断地址是否到顶
		{
			tAlmPrtEvtHead_R.ReadEventAddr_st=0;
		}
		W25QXX_Read(data,tAlmPrtEvtHead_R.ReadEventAddr_st,64);
		for(i=0;i<64;i++)//计算累加校验和放到最后一个字节
		{
			data[64] = data[64]+data[i];
		}
		NUM_CNT++;
		if(!Is_UsartSend)//不是串口发送
		{			
			timer6 =0;
			while(CAN_TX_Falg)
			{
				if(timer6>35)
					break;
			}
			memcpy(&CAN_T[6],&data,sizeof(data));  //软件版本号	
			CAN_T[2] = 0x01;
			CAN_T[3] = (u8)tAlmPrtEvtHead_R.ReadEvent_loop;
			CAN_T[4] = ((u8)tAlmPrtEvtHead_R.ReadEvent_loop>>8)|0x40;
			CAN_T[5] = 65;//有效载荷65个字节
			CAN_T_LEN_SUM =71;
			CAN_TX_Falg = 1;
			timer5 = 0;	
			CAN_id_add=0x016E416;//装入数据包29位ID
		}
		else//命令来自串口
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
			UsartReturn_Buffer[temp_i++]=65;//数据长度固定65,只有校验和
			for(i=0;i<65;i++)//
			{
				UsartReturn_Buffer[temp_i++] = data[i];
			}
			CCITT_CRC16Init(&UsartReturn_Buffer[6],71); //计算效验值
			UsartReturn_Buffer[temp_i++]=	CCITT_CRC16;//CRC16低位
			UsartReturn_Buffer[temp_i++]=	(CCITT_CRC16>>8);//CRC16低位	
			USART_Send_string(UsartReturn_Buffer,77);
			timer5 = 0;
		}
		tAlmPrtEvtHead_R.ReadEventAddr_st=tAlmPrtEvtHead_R.ReadEventAddr_st+(64);

   	}
	if(NUM_CNT==tAlmPrtEvtHead_R.ReadEvent_Num)
	{
		NUM_CNT=0;
		ReadEvent_enable=0;//禁止再调用此函数
		tAlmPrtEvtHead_R.ReadEvent_Num = 0;
		tAlmPrtEvtHead_R.ReadEventAddr_st=0;
		if(!Is_UsartSend)//命令不是来自串口	
		{
			data[2] = 0x01;
			data[3] = 0xff;
			data[4] = 0x4f;
			data[5] = 0x04;
			data[6] = 0xff;
			data[7] = 0xff;
			data[8] = 0xff;
			data[9] = 0xff;
			CCITT_CRC16Init(&data[2],8); //计算效验值
			data[0] = CCITT_CRC16&0x00ff;//CRC16低位
			data[1] = (CCITT_CRC16>>8)&0x00ff;//CRC16低位	
			timer6 =0;
			while(CAN_TX_Falg)
			{
				if(timer6>35)
					break;
			}
			memcpy(&CAN_T,&data,10);  //软件版本号	
			CAN_id_add=0x016E416;//装入数据包29位ID
			timer5 = 0;	
			CAN_T_LEN_SUM = 10;
			CAN_TX_Falg = 1;

			//发送结束指令
		}
	//	else//来自串口的命令
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
			UsartReturn_Buffer[temp_i++]=04;//数据长度固定4
			UsartReturn_Buffer[temp_i++] = 0xff;
			UsartReturn_Buffer[temp_i++] = 0xff;
			UsartReturn_Buffer[temp_i++] = 0xff;
			UsartReturn_Buffer[temp_i++] = 0xff;
			CCITT_CRC16Init(&UsartReturn_Buffer[6],8); //计算效验值
			UsartReturn_Buffer[temp_i++]=	CCITT_CRC16;//CRC16低位
			UsartReturn_Buffer[temp_i++]=	(CCITT_CRC16>>8);//CRC16低位	
			USART_Send_string(UsartReturn_Buffer,16);
			
		}
	}	
}
