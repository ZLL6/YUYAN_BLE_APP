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

#ifndef _EVENTRECORD_H
#define _EVENTRECORD_H

//=============================================================================================
//������ͷ�ļ�
//=============================================================================================
#include "pbdata.h"
#include "SYSTypeDef.h"

//=============================================================================
//���ݽṹ����
//=============================================================================

//��־�¼�ͷ�ṹ��
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

//���ϴ�����¼�ṹ��
typedef struct
{
u16 gNewPackCapacity;
}STR_AlmPrtDATA;
//��־�¼����ݽṹ��
#pragma pack (push)
#pragma pack(1)
typedef struct
			{
			u16 gsCycleCnt;               //ѭ������
			u32 index;                    // 
			RTC_DateTimeTypeDef RTC_DateTimeStructure;		
			//u16 gNewPackCapacity;     //������������
			//u16 gSurpulsPackCapacity;   //ʣ������
			u8  state;
			u8  gSOC;                  //��ذ�SOC
			u8  gSOH;                  //��ذ�SOH
			s16  gCurrent;        //��ǰ����ֵ 
			u8 FaultAlarmState[8];  //�澯��Ϣ
			//u16  gVoltSum;           //�ܵ�ѹ1mV       
			//u16  gVoltAvg;          //ƽ����ѹmV  
			//u16  gVoltMin;          //�����С��ѹֵmV  
			//u16  gVoltMax;         //�������ѹֵmV 
			//u8   gChVoltMin;       //��С��ѹͨ��    
			//u8   gChVoltMax;       //����ѹͨ��
			//u16  gVoltdiff;        //ѹ��  
			//u16  gDCHGTime;        //Ԥ�Ʒŵ�ʣ��ʱ��
			//u16  gCHGTime;         //Ԥ�Ƴ��ʣ��ʱ��
			
			//s16  gCurrentAvg;     //ƽ������ֵ     
			//s16  gCurrentDCHGMax;  //�ŵ�������  
			//s16  gCurrentCHGMax;   //���������
			//s8   gTemAvg;          //ƽ���¶�
			//s8   gTemMin;          	//����¶�
			//s8   gTemMax;           //����¶�
			//u8   gChTemMin;         //����¶�ͨ��
			//u8   gChTemMax;         //����¶�ͨ��
			//s8   gChTemdiff;        //�¶Ȳ�
			BMSVoltTemperature_call_typedef  FALSH_BMSVoltTemperature_call;   //���еĵ�ѹ���¶�

			}STR_EVENTDATA;
#pragma pack(pop)


//=============================================================================
//�궨��
//=============================================================================
#define EVENT_RCD_EN        	1	    //�¼���¼ʹ��
#define EVENT_FULL_COVER    	1	    //�¼��洢���󸲸������¼���¼�

//�¼����ݼ�¼��ַ
#define EVENT_MAX_NUM       	160000    //�¼���¼�����¼���
#define EVENT_HEAD_ADDR     	4096  	//��¼��Ҫ�¼�ͷ�Ŀ�ʼ��ַ
#define EVENT_START_ADDR    	(EVENT_HEAD_ADDR + sizeof(tEvtHead))  //��¼��Ҫ�¼���Ϣ�������Ŀ�ʼ��ַ
#define EVENT_END_ADDR    	    (EVENT_START_ADDR +((EVENT_MAX_NUM - 1) * sizeof(tEvtDataW)))    //�¼���¼�����¼���


#define EVENT_MAX_ADDR      	0xF42400   //�¼���EEPROM�д�ŵ�����ַ
#define EVENT_MAX_NUM_SUM      250000  //�¼���¼��Ŀ���ֵ

//=============================================================================================
//ȫ�ֱ�������
//=============================================================================================
extern STR_EVENTDATA tEvtDataR;   		//����EEPROM��־�Ļ���
extern STR_EVENTDATA tEvtDataW;   //д��EEPROM��־�Ļ���
extern STR_EVENTHEAD tAlmPrtEvtHead_W;	  	//��־�¼�ͷ
extern STR_EVENTHEAD tAlmPrtEvtHead_R;	  	//��־�¼�ͷ
extern STR_EVENTHEAD tEvtHead;    		//��־�¼�ͷ  

//=============================================================================
//�ӿں�������
//=============================================================================
//=============================================================================
//��������: void EventRecordInit(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: ��־�¼��ϵ��ʼ
//ע������: 
//=============================================================================
extern void EventRecordInit(void);
//=============================================================================
//��������: u8 EventRecordCheck(void) 
//�������: void
//�������: void
//����ֵ  : void
//��������: д��־�¼����жϴ���
//ע������: 
//=============================================================================
extern u8 EventRecordTask(void);

//=============================================================================
//��������: void ClrEventRecordCheck(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: ���EEPROM�е��¼�ͷ��Ϣ
//ע������: 
//=============================================================================
void ClrEventRecordCheck(void);

//=============================================================================
//��������: void ReadEventRecord(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: ����EEPROM�е��¼�������
//ע������: 
//=============================================================================
void ReadEventRecord(void);
void  WriteEventEeprom(void);
//=============================================================================
//��������: void ReadCHGEventRecord(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: ����EEPROM�е��¼�������
//ע������: 
//=============================================================================
void ReadCHGEventRecord(void);


void ReadEventRecord_TX(void);

#endif

