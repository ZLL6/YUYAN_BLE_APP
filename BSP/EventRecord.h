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

#ifndef _EVENTRECORD_H
#define _EVENTRECORD_H

//=============================================================================================
//包含的头文件
//=============================================================================================
#include "pbdata.h"
#include "SYSTypeDef.h"

//=============================================================================
//数据结构声明
//=============================================================================

//日志事件头结构体
#pragma pack (push)
#pragma pack(2)
typedef struct
{  u32 EventNum;	
	 u32 WriteEventAddr;
	 u32 ReadEventAddr_st;
	 u32 ReadEvent_Num;
	u16  ReadEvent_loop;
}STR_EVENTHEAD;
#pragma pack(pop)

//故障次数记录结构体
typedef struct
{
u16 gNewPackCapacity;
}STR_AlmPrtDATA;
//日志事件数据结构体
#pragma pack (push)
#pragma pack(1)
typedef struct
			{
			u16 gsCycleCnt;               //循环次数
			u32 index;                    // 
			RTC_DateTimeTypeDef RTC_DateTimeStructure;		
			//u16 gNewPackCapacity;     //最新满充容量
			//u16 gSurpulsPackCapacity;   //剩余容量
			u8  state;
			u8  gSOC;                  //电池包SOC
			u8  gSOH;                  //电池包SOH
			s16  gCurrent;        //当前电流值 
			u8 FaultAlarmState[8];  //告警信息
			//u16  gVoltSum;           //总电压1mV       
			//u16  gVoltAvg;          //平均电压mV  
			//u16  gVoltMin;          //输出最小电压值mV  
			//u16  gVoltMax;         //输出最大电压值mV 
			//u8   gChVoltMin;       //最小电压通道    
			//u8   gChVoltMax;       //最大电压通道
			//u16  gVoltdiff;        //压差  
			//u16  gDCHGTime;        //预计放电剩余时间
			//u16  gCHGTime;         //预计充电剩余时间
			
			//s16  gCurrentAvg;     //平均电流值     
			//s16  gCurrentDCHGMax;  //放电最大电流  
			//s16  gCurrentCHGMax;   //充电最大电流
			//s8   gTemAvg;          //平均温度
			//s8   gTemMin;          	//最低温度
			//s8   gTemMax;           //最高温度
			//u8   gChTemMin;         //最低温度通道
			//u8   gChTemMax;         //最高温度通道
			//s8   gChTemdiff;        //温度差
			BMSVoltTemperature_call_typedef  FALSH_BMSVoltTemperature_call;   //所有的电压与温度

			}STR_EVENTDATA;
#pragma pack(pop)


//=============================================================================
//宏定义
//=============================================================================
#define EVENT_RCD_EN        	1	    //事件记录使能
#define EVENT_FULL_COVER    	1	    //事件存储满后覆盖最早记录的事件

//事件数据记录地址
#define EVENT_MAX_NUM       	160000    //事件记录最大的事件数
#define EVENT_HEAD_ADDR     	4096  	//记录重要事件头的开始地址
#define EVENT_START_ADDR    	(EVENT_HEAD_ADDR + sizeof(tEvtHead))  //记录重要事件信息存放区域的开始地址
#define EVENT_END_ADDR    	    (EVENT_START_ADDR +((EVENT_MAX_NUM - 1) * sizeof(tEvtDataW)))    //事件记录最大的事件数


#define EVENT_MAX_ADDR      	0xF42400   //事件在EEPROM中存放的最大地址
#define EVENT_MAX_NUM_SUM      250000  //事件记录条目最大值

//=============================================================================================
//全局变量声明
//=============================================================================================
extern STR_EVENTDATA tEvtDataR;   		//读出EEPROM日志的缓存
extern STR_EVENTDATA tEvtDataW;   //写入EEPROM日志的缓存
extern STR_EVENTHEAD tAlmPrtEvtHead_W;	  	//日志事件头
extern STR_EVENTHEAD tAlmPrtEvtHead_R;	  	//日志事件头
extern STR_EVENTHEAD tEvtHead;    		//日志事件头  

//=============================================================================
//接口函数声明
//=============================================================================
//=============================================================================
//函数名称: void EventRecordInit(void)
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 日志事件上电初始
//注意事项: 
//=============================================================================
extern void EventRecordInit(void);
//=============================================================================
//函数名称: u8 EventRecordCheck(void) 
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 写日志事件的判断处理
//注意事项: 
//=============================================================================
extern u8 EventRecordTask(void);

//=============================================================================
//函数名称: void ClrEventRecordCheck(void)
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 清除EEPROM中的事件头信息
//注意事项: 
//=============================================================================
void ClrEventRecordCheck(void);

//=============================================================================
//函数名称: void ReadEventRecord(void)
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 读出EEPROM中的事件计数器
//注意事项: 
//=============================================================================
void ReadEventRecord(void);
void  WriteEventEeprom(void);
//=============================================================================
//函数名称: void ReadCHGEventRecord(void)
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 读出EEPROM中的事件计数器
//注意事项: 
//=============================================================================
void ReadCHGEventRecord(void);


void ReadEventRecord_TX(void);

#endif

