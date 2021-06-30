//
// Created by Yongz on 2018/12/27.
//
#include "soc.h"

#include "flash.h"


#pragma MESSAGE DISABLE C1420
#pragma MESSAGE DISABLE C2705

long soc1In1000;// 占总容量千分之一的容量（10ms单位：A*10ms =AH*3600*100/1000），注意要选择long类型
long socDifference = 0;
int soc;
word soh;
?//=============================================================================
//				           深圳市格瑞普电池有限公司
//                         电子产品部			     
//-----------------------------------------------------------------------------
//文件名称: soc.c
//文件标识:
//摘要    : 串口接收处理
//-----------------------------------------------------------------------------
//当前版本: V1.0
//作者    : dangxinkai
//完成日期: 2020.10.15
//=============================================================================
SOCHistory socHistory;
CapacityInfo BMS_capacity;
word chargeTime = 0, dischargeTime = 0;
byte needOpenCircuitVoltageMethod = 0;
char soc100CurrentCondition = -60;
/***************************************************************************
*   函 数 名: openCircuitVoltageMethod(word voltage)
*   功能说明: 开路电压法【查表方式】
*   形    参：无
*   返 回 值: 无
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
*   函 数 名: socIntegralRoutine(void)
*   功能说明: 电流积分函数
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void socIntegralRoutine(void) {
    // 充电电流不小于100MA，放电电流不为0，有效积分
    if (Current <= CURRENT_CHARGE || Current > 0) {
        // 10ms累积
        BMS_capacity.BatCapacity += Current;
    }
}
/***************************************************************************
*   函 数 名: readSoc(void)
*   功能说明: 初始化SOC
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void readSoc(void) {
    W25QXX_Read(BMS_capacity,0,sizeof(BMS_capacity));
 //效验



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
*   函 数 名: setSoc100CurrentCondition()
*   功能说明: 当充电机的电流小于2——6A时SOC更新为100%
*   形    参：无
*   返 回 值: 无
***************************************************************************/
/*
 计算恒压充电时判断soc100%的最小电流条件，2A~6A之间。
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
*   函 数 名: socCalculateTime()
*   功能说明: 当充电机的电流小于2——6A时SOC更新为100%
*   形    参：无
*   返 回 值: 无
***************************************************************************/
#define DENOMINATOR 100

// 计算剩余充放电时间
void socCalculateTime(void) {
    switch (systemState) {
        case STATE_DISCHARGE:// 放电
            chargeTime = 0;
            dischargeTime = (word) (6UL * soc * capacityInfo.effective / currentInfo.current_x10 / DENOMINATOR);
            break;
        case STATE_CHARGE: // 充电
            dischargeTime = 0;
            chargeTime = (word) (6UL * (1000 - soc) * capacityInfo.effective / (-currentInfo.current_x10) /
                                 DENOMINATOR);
            break;
    }
}
/***************************************************************************
*   函 数 名: saveSoc(void)
*   功能说明: 保存SOC
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void saveSoc(void) {

 //效验


W25QXX_Write_Page(BMS_capacity,0,sizeof(BMS_capacity));

}

#define saveCapacityInfo() {writeParamEEPROMSectorRound(EEPROM_CAPACITY_INFO_BASE,&capacityInfo.saveIndex,(FlashWriteData *)&capacityInfo,EEPROM_CAPACITY_INFO_SECTOR_SIZE);}

// 计算更新有效容量
void calculateEffective(byte type) {
    if (type) {// 放电,计算当前的放电容量保存到放电容量
        capacityInfo.discharge =
                (word) ((long) capacityInfo.effective * (1000 - capacityInfo.residual) /
                        1000);
        if (capacityInfo.residual <= -30 || capacityInfo.residual >= 50) {// 放电容量较大（3%）或过小（5%）需要更新
            capacityInfo.effective = capacityInfo.discharge;
        }
        capacityInfo.residual = 0;
    } else {// 充电,计算当前的剩余容量保存到充电容量
        capacityInfo.charge = (long) capacityInfo.effective * capacityInfo.residual / 1000;
        if (capacityInfo.residual >= 1030 || capacityInfo.residual <= 950) {// 充电容量较大（3%）或过小（5%）需要更新
            capacityInfo.effective = capacityInfo.charge;
        }
        capacityInfo.residual = 1000;
    }
    if (capacityInfo.effective > bmsConfig.batteryCapacity) {// 有效容量不超过额定容量
        capacityInfo.effective = bmsConfig.batteryCapacity;
    }
    soh = capacityInfo.effective * 1000UL / bmsConfig.batteryCapacity;
    soc1In1000 = capacityInfo.effective * 3600L;
}

// 电流积分与平滑校正
void socCalculateRoutine(void) {
   int socOld = soc;
    int residualOld = capacityInfo.residual;

    checkAgain:
    if (socDifference >= soc1In1000) {
        socDifference -= soc1In1000;
        if (soc > 1) {//不减到0%，让保护和平滑去处理
            soc--;
        }
        if (capacityInfo.saveIndex != 0xFF) {
            if (capacityInfo.residual > 1000) {//放电的时候，没有过压保护过（过压保护会置为1000），但统计的充电量比较大，舍弃掉，防止累积放电容量过小
                capacityInfo.residual = 1000;
            }
            capacityInfo.residual--;
        }
        goto checkAgain;
    } else if (-socDifference >= soc1In1000) {
        socDifference += soc1In1000;
        if (soc < 999) {//不增到100%，让保护和平滑去处理
            soc++;
        }
        if (capacityInfo.saveIndex != 0xFF) {
            if (capacityInfo.residual < 0) {//充电的时候，没有欠压保护过（欠压保护会归零），但统计的放电量比较大，舍弃掉，防止累积充电容量过小
                capacityInfo.residual = 0;
            }
            capacityInfo.residual++;
        }
        goto checkAgain;
    }

    if (!(systemErrorFlag & ERR_SAMPLE)) {
        // 平滑处理
        long benckPercent;// 使用long类型
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
        if (cmp > 0) {// 假设（纯线性假设）工作电压低于（保护+400mv）的电压还有10%的电，计算与保护值电压的差距，补偿soc
            benckPercent = cmp * 100L / 400;// 400mv区间
            cmp = benckPercent + soc - 100; // 计算出实际soc与计算的soc的差值
            if (cmp > 0) {// 实际soc较大，扣掉1/3
                if (eventCount > 2) {
                    eventCount = 0;
                    soc -= cmp * benckPercent / 300;
                    if (soc < 0)
                        soc = 0;
                    cmp = capacityInfo.residual - soc;
                    if (cmp > 50) { // 有效soc与估计的soc差超过5%，去掉少算的2.5%有效soc误差
                        capacityInfo.residual -= 25;
                    }
                } else {
                    eventCount++;
                }
            } else {
                eventCount = 0;
            }
        } else {// 平滑处理
            cmp = voltageMax[0].value - (systemWarnConfig.overVoltageCell.levels[WARN_LEVEL_1] - 150);
            if (cmp > 0) {
                benckPercent = cmp * 100L / 150; // 150mv区间
                cmp = benckPercent + 900 - soc;
                if (cmp > 0) { // 实际soc较小，补上1/3
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
        if (capacityInfo.saveIndex == 0xFF) {//没有记录，首次记录开始，所有容量设为出厂容量
            capacityInfo.discharge = capacityInfo.charge = capacityInfo.effective = bmsConfig.batteryCapacity;
            capacityInfo.residual = 1000;
        } else {// 有记录
            calculateEffective(TYPE_CHARGE);
        }
    } else {
        soc = 0;
        if (capacityInfo.saveIndex == 0xFF) {//没有记录，首次记录开始，所有容量设为出厂容量
            capacityInfo.discharge = capacityInfo.charge = capacityInfo.effective = bmsConfig.batteryCapacity;
            capacityInfo.residual = 0;
        } else {// 有记录
            calculateEffective(TYPE_DISCHARGE);
        }
    }

#ifndef SIMULATION
    if (socHistory.soc != soc)
        saveSoc();
    saveCapacityInfo();
#endif
}

void setBatterCreateCapacity(word capacity) {// 当电池的容量变化时，清除掉有效容量的相关记录
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