//
// Created by Yongz on 2018/12/27.
//
#include "soc.h"

#include "flash.h"


#pragma MESSAGE DISABLE C1420
#pragma MESSAGE DISABLE C2705

long soc1In1000;// ռ������ǧ��֮һ��������10ms��λ��A*10ms =AH*3600*100/1000����ע��Ҫѡ��long����
long socDifference = 0;
int soc;
word soh;
?//=============================================================================
//				           �����и����յ�����޹�˾
//                         ���Ӳ�Ʒ��			     
//-----------------------------------------------------------------------------
//�ļ�����: soc.c
//�ļ���ʶ:
//ժҪ    : ���ڽ��մ���
//-----------------------------------------------------------------------------
//��ǰ�汾: V1.0
//����    : dangxinkai
//�������: 2020.10.15
//=============================================================================
SOCHistory socHistory;
CapacityInfo BMS_capacity;
word chargeTime = 0, dischargeTime = 0;
byte needOpenCircuitVoltageMethod = 0;
char soc100CurrentCondition = -60;
/***************************************************************************
*   �� �� ��: openCircuitVoltageMethod(word voltage)
*   ����˵��: ��·��ѹ�������ʽ��
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/

u16  openCircuitVoltageMethod(word voltage){
    byte i;
    if (voltage >= BMS_capacity.socTable[0])
        return 1000;
    else if (voltage <= BMS_capacity.socTable[SOC_TABLE_SIZE - 1])
        return 0;
    for (i = 0; i < SOC_TABLE_SIZE; i++) {
        if (voltage >= BMS_capacity.socTable[i]) {
            break;
        }
    }
    return (20 - i) * 50 + (voltage - BMS_capacity.socTable[i]) * 50 / (BMS_capacity.socTable[i - 1] - BMS_capacity.socTable[i]);
}

#define CURRENT_CHARGE -8
/***************************************************************************
*   �� �� ��: socIntegralRoutine(void)
*   ����˵��: �������ֺ���
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void socIntegralRoutine(void) {
    // ��������С��100MA���ŵ������Ϊ0����Ч����
    if (Current <= CURRENT_CHARGE || Current > 0) {
        // 10ms�ۻ�
        BMS_capacity.BatCapacity += Current;
    }
}
/***************************************************************************
*   �� �� ��: readSoc(void)
*   ����˵��: ��ʼ��SOC
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void readSoc(void) {
    W25QXX_Read(BMS_capacity,0,sizeof(BMS_capacity));
 //Ч��



    if (BMS_capacity.SOC < 0 || BMS_capacity.SOC > 1000)
        gSOC = BMS_capacity.SOC;
    if (BMS_capacity.SOH < 0 || BMS_capacity.SOH > 1000) 
        gSOH = BMS_capacity.SOH;

        if (socHistory.soc > 1000) 
        {
            socHistory.soc = 1000;
        } 
         else if (socHistory.soc < 0) 
       {
            socHistory.soc = 0;
        }
    }
}
/***************************************************************************
*   �� �� ��: setSoc100CurrentCondition()
*   ����˵��: �������ĵ���С��2����6AʱSOC����Ϊ100%
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
/*
 �����ѹ���ʱ�ж�soc100%����С����������2A~6A֮�䡣
void setSoc100CurrentCondition() {
    soc100CurrentCondition = bmsConfig.batteryCapacity * 4 / 100;
    if (soc100CurrentCondition < 200) {
        soc100CurrentCondition = 200;
    } else if (soc100CurrentCondition > 600) {
        soc100CurrentCondition = 600;
    }
    soc100CurrentCondition = -soc100CurrentCondition;
}
*/
void socInit(void) {
    readSoc();
}
/***************************************************************************
*   �� �� ��: socCalculateTime()
*   ����˵��: �������ĵ���С��2����6AʱSOC����Ϊ100%
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
#define DENOMINATOR 100

// ����ʣ���ŵ�ʱ��
void socCalculateTime(void) {
    switch (systemState) {
        case STATE_DISCHARGE:// �ŵ�
            chargeTime = 0;
            dischargeTime = (word) (6UL * soc * capacityInfo.effective / currentInfo.current_x10 / DENOMINATOR);
            break;
        case STATE_CHARGE: // ���
            dischargeTime = 0;
            chargeTime = (word) (6UL * (1000 - soc) * capacityInfo.effective / (-currentInfo.current_x10) /
                                 DENOMINATOR);
            break;
    }
}
/***************************************************************************
*   �� �� ��: saveSoc(void)
*   ����˵��: ����SOC
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void saveSoc(void) {

 //Ч��


W25QXX_Write_Page(BMS_capacity,0,sizeof(BMS_capacity));

}

#define saveCapacityInfo() {writeParamEEPROMSectorRound(EEPROM_CAPACITY_INFO_BASE,&capacityInfo.saveIndex,(FlashWriteData *)&capacityInfo,EEPROM_CAPACITY_INFO_SECTOR_SIZE);}

// ���������Ч����
void calculateEffective(byte type) {
    if (type) {// �ŵ�,���㵱ǰ�ķŵ��������浽�ŵ�����
        capacityInfo.discharge =
                (word) ((long) capacityInfo.effective * (1000 - capacityInfo.residual) /
                        1000);
        if (capacityInfo.residual <= -30 || capacityInfo.residual >= 50) {// �ŵ������ϴ�3%�����С��5%����Ҫ����
            capacityInfo.effective = capacityInfo.discharge;
        }
        capacityInfo.residual = 0;
    } else {// ���,���㵱ǰ��ʣ���������浽�������
        capacityInfo.charge = (long) capacityInfo.effective * capacityInfo.residual / 1000;
        if (capacityInfo.residual >= 1030 || capacityInfo.residual <= 950) {// ��������ϴ�3%�����С��5%����Ҫ����
            capacityInfo.effective = capacityInfo.charge;
        }
        capacityInfo.residual = 1000;
    }
    if (capacityInfo.effective > bmsConfig.batteryCapacity) {// ��Ч���������������
        capacityInfo.effective = bmsConfig.batteryCapacity;
    }
    soh = capacityInfo.effective * 1000UL / bmsConfig.batteryCapacity;
    soc1In1000 = capacityInfo.effective * 3600L;
}

// ����������ƽ��У��
void socCalculateRoutine(void) {
   int socOld = soc;
    int residualOld = capacityInfo.residual;

    checkAgain:
    if (socDifference >= soc1In1000) {
        socDifference -= soc1In1000;
        if (soc > 1) {//������0%���ñ�����ƽ��ȥ����
            soc--;
        }
        if (capacityInfo.saveIndex != 0xFF) {
            if (capacityInfo.residual > 1000) {//�ŵ��ʱ��û�й�ѹ����������ѹ��������Ϊ1000������ͳ�Ƶĳ�����Ƚϴ�����������ֹ�ۻ��ŵ�������С
                capacityInfo.residual = 1000;
            }
            capacityInfo.residual--;
        }
        goto checkAgain;
    } else if (-socDifference >= soc1In1000) {
        socDifference += soc1In1000;
        if (soc < 999) {//������100%���ñ�����ƽ��ȥ����
            soc++;
        }
        if (capacityInfo.saveIndex != 0xFF) {
            if (capacityInfo.residual < 0) {//����ʱ��û��Ƿѹ��������Ƿѹ��������㣩����ͳ�Ƶķŵ����Ƚϴ�����������ֹ�ۻ����������С
                capacityInfo.residual = 0;
            }
            capacityInfo.residual++;
        }
        goto checkAgain;
    }

    if (!(systemErrorFlag & ERR_SAMPLE)) {
        // ƽ������
        long benckPercent;// ʹ��long����
        int cmp;
        static byte eventCount = 0;

        if (needOpenCircuitVoltageMethod) {
            needOpenCircuitVoltageMethod = 0;
            soc = openCircuitVoltageMethod(voltageMin[0].value);
            if (soc) {
                soc = openCircuitVoltageMethod(voltageMax[0].value);
                if (soc != 1000) {
                    soc = openCircuitVoltageMethod(sampleVoltageAverage);
                }
            }
        }

        cmp = systemWarnConfig.underVoltageCell.levels[WARN_LEVEL_1] + 400 - voltageMin[0].value;
        if (cmp > 0) {// ���裨�����Լ��裩������ѹ���ڣ�����+400mv���ĵ�ѹ����10%�ĵ磬�����뱣��ֵ��ѹ�Ĳ�࣬����soc
            benckPercent = cmp * 100L / 400;// 400mv����
            cmp = benckPercent + soc - 100; // �����ʵ��soc������soc�Ĳ�ֵ
            if (cmp > 0) {// ʵ��soc�ϴ󣬿۵�1/3
                if (eventCount > 2) {
                    eventCount = 0;
                    soc -= cmp * benckPercent / 300;
                    if (soc < 0)
                        soc = 0;
                    cmp = capacityInfo.residual - soc;
                    if (cmp > 50) { // ��Чsoc����Ƶ�soc���5%��ȥ�������2.5%��Чsoc���
                        capacityInfo.residual -= 25;
                    }
                } else {
                    eventCount++;
                }
            } else {
                eventCount = 0;
            }
        } else {// ƽ������
            cmp = voltageMax[0].value - (systemWarnConfig.overVoltageCell.levels[WARN_LEVEL_1] - 150);
            if (cmp > 0) {
                benckPercent = cmp * 100L / 150; // 150mv����
                cmp = benckPercent + 900 - soc;
                if (cmp > 0) { // ʵ��soc��С������1/3
                    if (eventCount > 2) {
                        eventCount = 0;
                        soc += cmp * benckPercent / 300;
                        if (soc > 1000) {
                            soc = 1000;
                        }
                    } else {
                        eventCount++;
                    }
                } else {
                    eventCount = 0;
                }
            }
        }
    }

#ifndef SIMULATION
    if (residualOld != capacityInfo.residual) {
        saveCapacityInfo();
    }

    if (socOld != soc) {
        saveSoc();
    }
#endif
}

void resetSOCEvent(byte type) {
    if (type == TYPE_CHARGE) {
        soc = 1000;
        if (capacityInfo.saveIndex == 0xFF) {//û�м�¼���״μ�¼��ʼ������������Ϊ��������
            capacityInfo.discharge = capacityInfo.charge = capacityInfo.effective = bmsConfig.batteryCapacity;
            capacityInfo.residual = 1000;
        } else {// �м�¼
            calculateEffective(TYPE_CHARGE);
        }
    } else {
        soc = 0;
        if (capacityInfo.saveIndex == 0xFF) {//û�м�¼���״μ�¼��ʼ������������Ϊ��������
            capacityInfo.discharge = capacityInfo.charge = capacityInfo.effective = bmsConfig.batteryCapacity;
            capacityInfo.residual = 0;
        } else {// �м�¼
            calculateEffective(TYPE_DISCHARGE);
        }
    }

#ifndef SIMULATION
    if (socHistory.soc != soc)
        saveSoc();
    saveCapacityInfo();
#endif
}

void setBatterCreateCapacity(word capacity) {// ����ص������仯ʱ���������Ч��������ؼ�¼
    if (bmsConfig.batteryCapacity != capacity) {
        capacityInfo.discharge
                = capacityInfo.charge
                = capacityInfo.effective
                = bmsConfig.batteryCapacity
                = capacity ? capacity : BATTERY_CAPACITY;
        capacityInfo.saveIndex = 0xFF;
        capacityInfo.residual = 0;
        soh = 1000;
        soc1In1000 = capacityInfo.effective * 3600L;
        setSoc100CurrentCondition();
        eepromErase(EEPROM_CAPACITY_INFO_BASE);
        eepromErase(EEPROM_CAPACITY_INFO_BASE + 0x300);
    }
}