//
// Created by Yongz on 2018/12/27.
//

#ifndef BMS_SOC_H
#define BMS_SOC_H

//#include "derivative.h"
//#include "config.h"

#define SOC_TABLE_SIZE              101
#define   Capacity_Max           0x2f34f600ul 
#define   Capacity_Min           0x00000000ul
#pragma pack (push)
#pragma pack(1) 
typedef struct {
	  u8  FCC_UPdatat;
	  u16 soc_1000;
    u32 BatCapacity;// 有效容量
	  u32 PackNewCapacity;// 最新满充容量
    u32 PackCapacity;// 设计容量
    u16 SOC;      //当前SOC值
    u16 SOH;      //当前SOH值
    u32 charge;// 充进的容量
	  u32 Bat_charge;// 充电累积容量
//    u32 discharge;// 满放时剩余放出的容量
//    u16 BatCycleCnt;  //电池的循环次数
} CapacityInfo;  
#pragma pack(pop)

extern CapacityInfo BMS_capacity;
u16  openCircuitVoltageMethod(u16 voltage);
void socIntegralRoutine(void);

void socCalculateRoutine(void);

void socCalculateTime(void);

void socInit(void);

//void setBatterCreateCapacity(word capacity);


//void resetSOCEvent(byte type);// 充满电或放完电回调

#endif //BMS_SOC_H
