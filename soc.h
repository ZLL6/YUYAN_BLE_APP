//
// Created by Yongz on 2018/12/27.
//

#ifndef BMS_SOC_H
#define BMS_SOC_H

#include "derivative.h"
#include "config.h"

#define SOC_TABLE_SIZE              101

typedef struct {
    u32 BatCapacity;// ��Ч����
    u32 PackCapacity;// �������
    u16 SOC;      //��ǰSOCֵ
    u16 SOH;      //��ǰSOHֵ
//    u32 charge;// ����ʱ���ʣ�������
//    u32 discharge;// ����ʱʣ��ų�������
//    u16 BatCycleCnt;  //��ص�ѭ������
    u16 socTable[101]; //��·��ѹֵ
    u16 CRC16;    //crcЧ��
} CapacityInfo;


void socIntegralRoutine(void);

void socCalculateRoutine(void);

void socCalculateTime(void);

void socInit(void);

void setBatterCreateCapacity(word capacity);


void resetSOCEvent(byte type);// �����������ص�

extern word soh;
extern int soc;
extern word chargeTime;
extern word dischargeTime;
extern CapacityInfo capacityInfo;
extern char soc100CurrentCondition;
#endif //BMS_SOC_H
