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
    u32 BatCapacity;// ��Ч����
	  u32 PackNewCapacity;// ������������
    u32 PackCapacity;// �������
    u16 SOC;      //��ǰSOCֵ
    u16 SOH;      //��ǰSOHֵ
    u32 charge;// ���������
	  u32 Bat_charge;// ����ۻ�����
//    u32 discharge;// ����ʱʣ��ų�������
//    u16 BatCycleCnt;  //��ص�ѭ������
} CapacityInfo;  
#pragma pack(pop)

extern CapacityInfo BMS_capacity;
u16  openCircuitVoltageMethod(u16 voltage);
void socIntegralRoutine(void);

void socCalculateRoutine(void);

void socCalculateTime(void);

void socInit(void);

//void setBatterCreateCapacity(word capacity);


//void resetSOCEvent(byte type);// �����������ص�

#endif //BMS_SOC_H
