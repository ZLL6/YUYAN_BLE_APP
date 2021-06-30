//=============================================================================
//				           �����и����յ�����޹�˾
//                         ���Ӳ�Ʒ��			     
//-----------------------------------------------------------------------------
//�ļ�����: BQ769x0.H
//�ļ���ʶ:
//ժҪ    : �ɼ�IC BQ769x0�ĵײ�����
//-----------------------------------------------------------------------------
//��ǰ�汾: V1.0
//����    : dangxinkai
//�������: 2020.10.15
//=============================================================================
//ȡ���汾:
//ԭ����  :
//�������:
//=============================================================================
#ifndef __BQ769X0_H
#define __BQ769X0_H

//=============================================================================
//������ͷ�ļ�
//=============================================================================
#include "pbdata.h"
//=============================================================================
//���ݽṹ����
//=============================================================================
typedef struct _Register_Group
{
    union
	{
		struct
		{
			u8 OCD          :1;
			u8 SC           :1;
			u8 OV           :1;
			u8 UV			 :1;
			u8 OVRD_ALERT	 :1;
			u8 DEVICE_XREADY:1;
			u8 WAKE		 :1;
			u8 CC_READY	 :1;
		}StatusBit;
		u8 StatusByte;
	}SysStatus;

	union
	{
		struct
		{
			u8 RSVD		:3;
			u8 CB5			:1;
			u8 CB4			:1;
			u8 CB3			:1;
			u8 CB2			:1;
			u8 CB1			:1;
		}CellBal1Bit;
		u8 CellBal1Byte;
	}CellBal1;

	union
	{
		struct
		{
			u8 RSVD		:3;
			u8 CB10		:1;
			u8 CB9			:1;
			u8 CB8			:1;
			u8 CB7			:1;
			u8 CB6			:1;
		}CellBal2Bit;
		u8 CellBal2Byte;
	}CellBal2;

	union
	{
		struct
		{
			u8 RSVD			:3;
			u8 CB15			:1;
			u8 CB14			:1;
			u8 CB13			:1;
			u8 CB12			:1;
			u8 CB11			:1;
		}CellBal3Bit;
		u8 CellBal3Byte;
	}CellBal3;

	union
	{
		struct
		{
			u8 SHUT_B		:1;
			u8 SHUT_A		:1;
			u8 RSVD1			:1;
			u8 TEMP_SEL		:1;
			u8 ADC_EN		:1;
			u8 RSVD2			:2;
			u8 LOAD_PRESENT	:1;
		}SysCtrl1Bit;
		u8 SysCtrl1Byte;
	}SysCtrl1;

	union
	{
		struct
		{
			u8 CHG_ON		:1;
			u8 DSG_ON		:1;
			u8 WAKE_T		:2;
			u8 WAKE_EN		:1;
			u8 CC_ONESHOT	:1;
			u8 CC_EN			:1;
			u8 DELAY_DIS		:1;
		}SysCtrl2Bit;
		u8 SysCtrl2Byte;
	}SysCtrl2;

	union
	{
		struct
		{
			u8 SCD_THRESH	:3;
			u8 SCD_DELAY		:2;
			u8 RSVD			:2;
			u8 RSNS			:1;
		}Protect1Bit;
		u8 Protect1Byte;
	}Protect1;

	union
	{
		struct
		{
			u8 OCD_THRESH	:4;
			u8 OCD_DELAY		:3;
			u8 RSVD			:1;
		}Protect2Bit;
		u8 Protect2Byte;
	}Protect2;

	union
	{
		struct
		{
			u8 RSVD			:4;
			u8 OV_DELAY		:2;
			u8 UV_DELAY		:2;
		}Protect3Bit;
		u8 Protect3Byte;
	}Protect3;

	u8 OVTrip;
	u8 UVTrip;
	u8 CCCfg;

	union
	{
		struct
		{
			u8 RSVD1			:2;
			u8 ADCGAIN_4_3	:2;
			u8 RSVD2			:4;
		}ADCGain1Bit;
		u8 ADCGain1Byte;
	}ADCGain1;

	s8 ADCOffset;

	union
	{
		struct
		{
			u8 RSVD			:5;
			u8 ADCGAIN_2_0	:3;
		}ADCGain2Bit;
		u8 ADCGain2Byte;
	}ADCGain2;
}STR_BQ769X0_REG;


typedef struct
{
    u8 BaseHigh; //�����ĸ�λ(ʮ����)  ��1684��BaseHigh = 16��BaseLow = 84
    u8 BaseLow;  //�����ĵ�λ(ʮ����)
    u8 BaseCnt;  //Ԫ�ظ���
    u8 BaseDiff; //���ڼ�����Ԫ�ز�
}STR_TEMPVOLT;

typedef enum 
{
    eALERT    = 0,
    eCONFIGUE = 1
}ENUM_TASK;


typedef enum
{
	eADC_MODULE = 0,
	eCC_MODULE = 1,
	eADC_CC_MODULE = 2
}ENUM_MODULE;

typedef enum
{
    eERR_DEVICE = 0,
    eERR_CONFIG = 1,
    eERR_COMM_BQ = 2,
    eERR_MOS_STA = 3,
    eERR_PARA = 4,
    eERR_INIT = 5
}ENUM_BQ_ERROR;


//=============================================================================
//�궨��
//=============================================================================
#define  Bq769X0_IIC_CHANNEL 	eSIMCHANNEL_1

//BQ769x0Ӳ������ʹ��
#define BQ769X0_PRT_EN 			0
//����������
#define    gRsense   1.5
#define    gCellNum   14
#define gHardODCTimer                10 		 	//���طŵ��������ʱ��  1byte
#define gODCRlsTimer                   10  		//�ŵ�����ָ�ʱ�� 	  1byte
#define gDisShortTimer                	2  		//��·����ʱ��    				1byte
#define gDisShortRlsTimer           10  			//��·�ָ�ʱ��  	  			1byte
#define gDisShortCirCur           	200        			//��·��������
#define gDiscOvCurHard1         	100        			//�ŵ��������
#define gHardOVTimer             		10	  		//Ӳ����ѹ����ʱ��      		 1byte
#define gHardUVTimer             		10  	  		//Ӳ��Ƿѹ����ʱ��      		 1byte
#define gHighVoltHardPrt            4250        			//Ӳ����ѹ����ֵ     mV        2byte
#define gLowVoltHardPrt             3000        			//Ӳ��Ƿѹ����ֵ     mV        2byte
//�����ʼ����־
//u16 gDeviceInitFlag = 0;

//�豸��ַ
#define ADDR_BQ76940        0x08
#define NUM_BQ769X0 		1


//IC��ȡ��о��Ϣ������
#define IC_READ_CELL_NUM    15

//IC�ɼ�ͨ�����о���������ӷֲ�״̬
#define IC_CONNECT_15     ((u16)0x7fff) 
#define IC_CONNECT_14     ((u16)0x5fff) 
#define IC_CONNECT_13     ((u16)0x5eff) 
#define IC_CONNECT_12     ((u16)0x5ef7) 
#define IC_CONNECT_11     ((u16)0x4ef7) 
#define IC_CONNECT_10     ((u16)0x4e77) 
#define IC_CONNECT_9   	  ((u16)0x4e73) 
#define IC_CONNECT_6   	  ((u16)0x0273) 

//IC����
#define BQ76920             1
#define BQ76930             2
#define BQ76940             3
#define BQ_TYPE             BQ76940

#define BQ769X0_WITH_CRC //�������BQ76X0����CRC��Ҫ����ú꣬��֮������

//��������
#define NUM_BQ769X0 		1


//IC���ɼ���о����
#if(BQ_TYPE == BQ76920)
#define NUM_CELL_BQ         5
#define NUM_TEMP_BQ         1
#elif(BQ_TYPE == BQ76930)
#define NUM_CELL_BQ         10
#define NUM_TEMP_BQ         2
#elif(BQ_TYPE == BQ76940)
#define NUM_CELL_BQ         15
#define NUM_TEMP_BQ         2
#endif

//ͨѶ��ʱ
#define BQ769X0_Delay(time) {volatile u16 Count = time; while(Count--);}

//ϵͳǰ�˹���״̬
#define AFE_OCD       uStaSysAFE.Bits.B0
#define AFE_SC        uStaSysAFE.Bits.B1
#define AFE_OV        uStaSysAFE.Bits.B2
#define AFE_UV        uStaSysAFE.Bits.B3
#define AFE_OVRD_ALT  uStaSysAFE.Bits.B4
#define AFE_DEV_ERR   uStaSysAFE.Bits.B5
#define AFE_CC_RDY    uStaSysAFE.Bits.B6
#define AFE_SC_NOACT  uStaSysAFE.Bits.B7


//----------------------------�Ĵ������---------------------------------------------------
#define SYS_STAT      0x00
#define CELLBAL1      0x01
#define CELLBAL2      0x02
#define CELLBAL3      0x03
#define SYS_CTRL1     0x04
#define SYS_CTRL2     0x05
#define PROTECT1      0x06
#define PROTECT2      0x07
#define PROTECT3      0x08
#define OV_TRIP       0x09
#define UV_TRIP       0x0A
#define VC1_HI        0x0C
#define TS1_HI        0x2C
#define TS2_HI        0x2E
#define TS3_HI        0x30

#define CC_HI         0x32
#define CC_LO         0x33

#define ADCGAIN1      0x50
#define ADCOFFSET     0x51
#define ADCGAIN2      0x59

#define SCD_DELAY_50us		0x00
#define SCD_DELAY_100us		0x01
#define SCD_DEALY_200us		0x02
#define SCD_DELAY_400us		0x03

#define SCD_THRESH_44mV_22mV	0
#define SCD_THRESH_67mV_33mV	1
#define SCD_THRESH_89mV_44mV	2
#define SCD_THRESH_111mV_56mV	3
#define SCD_THRESH_133mV_67mV	4
#define SCD_TRHESH_155mV_78mV	5
#define SCD_THRESH_178mV_89mV	6
#define SCD_THRESH_200mV_100mV	7

#define OCD_DEALY_10ms		    0x00
#define OCD_DELAY_20ms		    0x01
#define OCD_DELAY_40ms		    0x02
#define OCD_DELAY_80ms		    0x03
#define OCD_DELAY_160ms		    0x04
#define OCD_DELAY_320ms		    0x05
#define OCD_DELAY_640ms		    0x06
#define OCD_DELAY_1280ms	    0x07

#define OCD_THRESH_17mV_8mV		0
#define OCD_THRESH_22mV_11mV	1
#define OCD_THRESH_28mV_14mV	2
#define OCD_THRESH_33mV_17mV	3
#define OCD_THRESH_39mV_19mV	4
#define OCD_THRESH_44mV_22mV	5
#define OCD_THRESH_50mV_25mV	6
#define OCD_THRESH_56mV_28MV	7
#define OCD_THRESH_61mV_31mV	8
#define OCD_THRESH_67mV_33mV	9
#define OCD_THRESH_72mV_36mV	0x0A
#define OCD_THRESH_78mV_39mV	0x0B
#define OCD_THRESH_83mV_42mV	0x0C
#define OCD_THRESH_89mV_44mV	0x0D
#define OCD_THRESH_94mV_47mV	0x0E
#define OCD_THRESH_100mV_50mV	0x0F

#define VAL_CC_CFG              0x19

#define UV_DELAY_1s				0
#define UV_DELAY_4s				1
#define UV_DELAY_8s				2
#define UV_DELAY_16s			3

#define OV_DELAY_1s				0
#define OV_DELAY_2s				1
#define OV_DELAY_4s				2
#define OV_DELAY_8s				3

#define OV_THRESH_BASE			((u16)0x2008)
#define UV_THRESH_BASE			((u16)0x1000)
#define OV_STEP					0x10
#define UV_STEP					0x10

#define BQ769X0_ADCGAIN_BASE	((u16)365)

//���Bq769x0ϵͳ״̬
#define CLR_CC_RDY              (0x80)
#define CLR_DEV_ERR             (0x20)
#define CLR_OVRD_ALERT          (0x10)
#define CLR_UV                  (0x08)
#define CLR_OV                  (0x04)
#define CLR_SC                  (0x02)
#define CLR_OCD                 (0x01)
#define CLR_ALL                 (0xFF)


//////////////////////////////////////////////////////////////////////
///////////////////////////////////�豸����/////////////////////////////////////////
//SYS_CTRL1����
#define BQ_ADC_EN     ENABLE
#define BQ_TEMP_SEL   ENABLE
//SYS CTRL2����
#define BQ_DELAY_DIS  DISABLE
#define BQ_CC_EN      ENABLE
#define BQ_CC_ONESHOT DISABLE
#define BQ_DSG_ON     DISABLE
#define BQ_CHG_ON     DISABLE
//PROTECT1����
#define BQ_RSNS       0
#define BQ_SCD_DELAY  SCD_DELAY_400us
#define BQ_SCD_THRESH SCD_THRESH_200mV_100mV
//PROTECT2����
#define BQ_OCD_DELAY  OCD_DELAY_1280ms
#define BQ_OCD_THRESH OCD_THRESH_56mV_28MV //20AH
//#define BQ_OCD_THRESH OCD_THRESH_61mV_31mV //26AH
//#define BQ_OCD_THRESH OCD_THRESH_67mV_33mV //32AH

//PROTECT3����
#define BQ_OV_DELAY   OV_DELAY_4s
#define BQ_UV_DELAY   UV_DELAY_4s

#define ADCGAIN_BASE      ((u16)365)

//�¶ȶ��ձ�
#define LEN_TEMP_TABLE    131
#define MAX_TEMP_TABLE    100
#define MIN_TEMP_TABLE    ((s8)(-30))
#define TEMP_ZERO_OFFSET  30


//��������
#define BLC_START         40
#define BLC_STOP          20
#define BLC_SRT_VOLT1     ((u16)tUserData.VoltBalStr * 100)
#define BLC_SRT_VOLT2     (4100)


//MOS״̬
#ifndef CONNECT
#define CONNECT           1
#define CUT               0
#endif
#define LOAD_DETECT       ((P1IN & PORT_BIT5) ? (CUT) : (CONNECT))

#define CHRG_MOS          1
#define DISC_MOS          2
#define CHRG_DISC_MOS     3

//�������BQ769X0��˳��
#define HIGH_FIRST        1
#define LOW_FIRST         2


//=============================================================================
//ȫ�ֱ�������
//=============================================================================



//=============================================================================
//�ӿں�������
//=============================================================================
//=============================================================================
//��������: void BQ769X0Init(void)
//�������: void
//�������: void
//����ֵ  : ��ʼ�����
//��������: BQ769X0�ϵ��ʼ��
//ע������:
//=============================================================================
void BQ769X0Init(void);

//=============================================================================
//��������: u8 BQ769X0_ConfigSys(void)
//�������: void
//�������: ���ý��(TRUE�ɹ� FALSEʧ��)
//��������: BQ769X0ϵͳ����������BQ769X0
//ע������:
//=============================================================================
u8 BQ769X0_ConfigSys(void);
//=============================================================================
//��������: static u8 BQ769X0_ConfigPara(u8 AddrIndex)
//�������: AddrIndex��ַ����
//�������: ���ý��(TRUE�ɹ� FALSEʧ��)
//��������: BQ769X0���ò���
//ע������:
//=============================================================================
u8 BQ769X0_ConfigPara(u8 AddrIndex);
//=============================================================================
//��������: u8 BQ769X0_SampVolt(u16 *Volt, u8 Length)
//�������: *VoltAd�������ĵ�ѹADֵ Length������о��Ŀ
//�������: �ɼ����(TRUE�ɹ� FALSEʧ��)
//��������: ��ص����ѹ�ɼ�
//ע������:
//=============================================================================
u8 BQ769X0_SampVolt(u16 *Volt, u8 Length);

//=============================================================================
//��������: u8 BQ769X0_SampCurrAD(u16 *CurrAd)
//�������: *Current�ɼ����ĵ���ֵ
//�������: �ɼ����(TRUE�ɹ� FALSEʧ��)
//��������: ��ص�������
//ע������: ����ֵ��λΪ10mA
//=============================================================================
u8 BQ769X0_SampCurr(s32 *Current);

//=============================================================================
//��������: u8 BQ769X0_SampleTemp(s8* Temperature, u8 Length)
//�������: *Temperature�¶ȴ�����䣬Length��ȡ�¶�ͨ������
//�������: void
//����ֵ     : void
//��������: BQ769X0�¶Ȳ���
//ע������:
//=============================================================================
u8 BQ769X0_SampleTemp(s8* Temperature, u8 Length);

//=============================================================================
//��������: void HardFaultCheck(void)
//�������: void
//�������: void
//��������: ALERT�źŹ���
//ע������:
//=============================================================================
void HardFaultCheck(void);

//=============================================================================
//��������: u8 BQ769X0_MosControl(u8 MosSel, u8 StaSel, u8 Order)
//�������: MosSel��Ҫ������MOS, StaSel���Ƶ�״̬, Order���ƶ��BQ769X0��˳��(�Ӹߵ��ͻ�ӵ͵���)
//�������: void
//����ֵ  : ���ƽ��
//��������: BQ769X0����MOS
//ע������:
//=============================================================================
u8 BQ769X0_MosControl(u8 MosSel, u8 StaSel, u8 Order);

//=============================================================================
//��������: void BQ769X0_VoltBalance(void)
//�������: void
//�������: void
//����ֵ     : void
//��������: ��ѹ����
//ע������:
//=============================================================================
void BQ769X0_VoltBalance(void);

//=============================================================================
//��������: void BQ769X0_PowerSave(u8 Module)
//�������: Moduleģ��
//�������: void
//��������: BQ769X0���ߴ���
//ע������:
//=============================================================================
void BQ769X0_PowerSave(u8 Module);

//=============================================================================
//��������: void BQ769X0_Resume(u8 Module)
//�������: Moduleģ��
//�������: void
//��������: BQ769X0���ѻָ�
//ע������:
//=============================================================================
void BQ769X0_Resume(u8 Module);

//=============================================================================
//��������: void ClearBqStatus(u8 Status)
//�������: Status��Ҫ�����״̬
//�������: void
//����ֵ  : void
//��������: ���BQ769X0��ϵͳ״̬
//ע������:
//=============================================================================
void BQ769X0_ClearStatus(u8 Status);

//==============================================================================
//��������: void BQ769X0_Diagnose(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: Ba769x0�������
//ע������:
//==============================================================================
void BQ769X0_Diagnose(void);


//=============================================================================
//��������: void BQ769X0_BalanceCtrl(u32 Channel)
//�������: Channel����ͨ��
//�������: void
//��������: ���Ƶ�ص��������ͣ
//ע������: void
//=============================================================================
void BQ769X0_BalanceCtrl(u32 Channel);

//=============================================================================
//��������: void BQ769X0_MosConfirm(void)
//�������: void
//�������: void
//����ֵ     : void
//��������: MOS״̬ȷ��
//ע������:
//=============================================================================
void BQ769X0_MosConfirm(void);

//=============================================================================
//��������: void BQ769X0_Wakeup(void)
//�������: void
//�������: void
//��������: ����BQ769X0
//ע������: void
//=============================================================================
void BQ769X0_Wakeup(void);

//=============================================================================
//��������: u8 BQ769X0_Sleep(void)
//�������: void
//�������: ������(TRUE�ɹ� FALSEʧ��)
//��������: BQ769X0��Normalģʽ����Shipģʽ
//ע������:
//=============================================================================
u8 BQ769X0_Sleep(void);

//=============================================================================
//��������: u8 GetBQ769x0Status(void)
//�������: void
//�������: BQ769X0��оƬ�ڲ�״̬
//��������: ��ȡBQ769X0оƬ�ڲ�״̬
//ע������:
//=============================================================================
u8 GetBQ769X0Status(void);

//=============================================================================
//��������: u8 GetBQ769x0Status(void)
//�������: void
//�������: BQ769X0�Ĵ���״̬
//��������: ��ȡBQ769X0�����г��ֵĴ���״̬
//ע������:
//=============================================================================
u8 GetBQ769X0Error(void);
/*********************************************************************************
*   �� �� ��: BQ76940_TASK(void)
*   ����˵��: 
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************/

void BQ76940_TASK(void);
#endif


