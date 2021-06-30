//
// Created by Yongz on 2018/12/27.
//

#ifndef BMS_SOC_H
#define BMS_SOC_H

#include "derivative.h"
#include "config.h"

#define SOC_TABLE_SIZE              101

typedef struct {
    u32 BatCapacity;// 有效容量
    u32 PackCapacity;// 设计容量
    u16 SOC;      //当前SOC值
    u16 SOH;      //当前SOH值
//    u32 charge;// 满充时充进剩余的容量
//    u32 discharge;// 满放时剩余放出的容量
//    u16 BatCycleCnt;  //电池的循环次数
    u16 socTable[101]; //开路电压值
    u16 CRC16;    //crc效验
} CapacityInfo;


void socIntegralRoutine(void);

void socCalculateRoutine(void);

void socCalculateTime(void);

void socInit(void);

void setBatterCreateCapacity(word capacity);


void resetSOCEvent(byte type);// 充满电或放完电回调

extern word soh;
extern int soc;
extern word chargeTime;
extern word dischargeTime;
extern CapacityInfo capacityInfo;
extern char soc100CurrentCondition;
#endif //BMS_SOC_H
