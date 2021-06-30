//=============================================================================
//				           �����и����յ�����޹�˾
//                         ���Ӳ�Ʒ��			     
//-----------------------------------------------------------------------------
//�ļ�����: BQ769x0.c
//�ļ���ʶ:
//ժҪ    : �ɼ�IC BQ769x0�ĵײ�����
//-----------------------------------------------------------------------------
//��ǰ�汾: V1.0
//����    : dangxinkai
//�������: 2020.10.15
//=============================================================================

//=============================================================================
//������ͷ�ļ�
//=============================================================================
#include "pbdata.h"
#include "BQ769x0.h"
//#include "SYSTypeDef.h"
#include <stdint.h>
//=============================================================================
//ȫ�ֱ�������
//=============================================================================
//static const u16 TempVoltTable[131] =
//{
//    3074, 3061, 3048, 3035, 3020, 3006, 2991, 2975, 2958, 2941, 2924, //-30��~-20��
//	2906, 2887, 2867, 2847, 2827, 2805, 2784, 2761, 2738, 2715,       //-19��~-10��
//	2690, 2666, 2640, 2614, 2588, 2561, 2533, 2505, 2477, 2448,       //-9��~-0��
//	2419, 2389, 2359, 2328, 2297, 2266, 2235, 2203, 2171, 2139,       //1��~10��
//	2107, 2074, 2041, 2009, 1976, 1943, 1910, 1877, 1844, 1812,       //11��~20��
//	1779, 1747, 1714, 1682, 1650, 1618, 1587, 1555, 1524, 1494,       //21��~30��
//	1463, 1433, 1403, 1374, 1345, 1316, 1288, 1260, 1233, 1206,       //31��~40��
//	1179, 1153, 1127, 1102, 1077, 1052, 1028, 1005, 982,  959,        //41��~50��
//	937,  915,  894,  873,  853,  833,  813,  794,  775,  757,        //51��~60��
//	739,  722,  705,  688,  672,  656,  640,  625,  610,  596,        //61��~70��
//	582,  568,  555,  542,  529,  516,  504,  493,  481,  470,        //71��~80��
//	459,  448,  438,  427,  418,  408,  399,  389,  380,  372,        //81��~90��
//	363,  355,  347,  339,  331,  324,  317,  310,  303,  296         //91��~100��
//};
#define   ShortCircuit(x)    (x<0x8000?x:(~(x-1)))
//static const u8 sAddrBq769x0[NUM_BQ769X0] = {0x18, 0x08};
static const u8 sAddrBq769x0[NUM_BQ769X0] = {0x08};
//static const u16 sBqConnect[NUM_BQ769X0] = {0x5EFF};

static u16 sBqGain[NUM_BQ769X0] = {0}; 		//BQ769X0��AD����
static u8 sErrorBq = 0;
static u8 sErrBqComm = 0; 					//��BQ769X0ͨѶ�����־
static STR_BQ769X0_REG tBqReg[NUM_BQ769X0]; //BQ769X0�ļĴ���
static uBits uStaSysAFE;
u16 gBlanceState[gCellNum];

//��������
u16  gConnectNum    = IC_CONNECT_14; 

//=============================================================================
//��̬��������
//=============================================================================
//=============================================================================
//��������: static u8 BQ769X0_GetADCGainOffset(void)
//�������: void
//�������: ��ȡ���(TRUE�ɹ� FALSEʧ��)
//��������: ��ȡBQ769X0���ڲ��̻��������ƫ����
//ע������: void
//=============================================================================
static u8 BQ769X0_GetADCGainOffset(void);

//=============================================================================
//��������: static u8 BQ769X0_WriteRegister(u8 DeviceAddr, u8 RegAddr, u8 *CfgData, u8 Length)
//�������: DeviceAddr:�豸��ַ��RegAddr�Ĵ�����ַ, *CfgData��Ҫ���óɵ�ֵ��Length���õĳ���
//�������: д����(TRUE�ɹ� FALSEʧ��)
//��������: дB1769X0�Ĵ���ֵ
//ע������:
//=============================================================================
static u8 BQ769X0_WriteRegister(u8 DeviceAddr, u8 RegAddr, u8 *CfgData, u8 Length);

//=============================================================================
//��������: static u8 BQ769X0_ConfigPara(u8 AddrIndex)
//�������: AddrIndex��ַ����
//�������: ���ý��(TRUE�ɹ� FALSEʧ��)
//��������: BQ769X0���ò���
//ע������:
//=============================================================================
u8 BQ769X0_ConfigPara(u8 AddrIndex);

//=============================================================================
//��������: static void BQ769X0_ParaConfirm(void)
//�������: void
//�������: void
//����ֵ     : void
//��������: BQ769X0�������
//ע������:
//=============================================================================
static void BQ769X0_ParaConfirm(void);


//=============================================================================
//��������: static void ClearBqNoActSC(u8 AddrDev)
//�������: void
//�������: void
//����ֵ  : void
//��������: ���BQ769X0û�н��ж�·����IC�Ķ�·״̬
//ע������:
//=============================================================================
static void BQ769X0_ClrNoActSC(void);

//==============================================================================
//��������: static s8 TempSearchTab(u16 code)
//�������: code�ǲɼ���ȡ�ĵ�ѹֵ
//�������:
//����ֵ  : ����ȡ���¶�ֵ
//��������: ʹ�òɼ��ĵ�ѹֵ�����ҵ�ѹ�¶ȶ�Ӧ���ȡ�¶�ֵ
//ע������: �ö��ַ��ҵ��ӽ�λ�ã�����ǰһ������һ����ֵ�Ƚϵõ���ӽ���ֵ
//==============================================================================
//static s8 TempSearchTab(u16 code);

//=============================================================================
//��������: static u8 GetSubscriptU8(u8 Value, u8 *Table, u8 Length)
//�������: Value����Ҫ���ҵ�ֵ��*Table���ұ�Length��ĳ���
//�������: void
//����ֵ  :  ���ҵ��±�
//��������: ���Ҷ�Ӧֵ(�������ֵ)�ڱ��е��±�
//ע������: void
//=============================================================================
static u8 GetSubscriptU8(u8 Value, u8 *Table, u8 Length);

//=============================================================================
//��������: static u8 GetSubscriptU16(u16 Value, u16 *Table, u8 Length)
//�������: Value����Ҫ���ҵ�ֵ��*Table���ұ�Length��ĳ���
//�������: void
//����ֵ  :  ���ҵ��±�
//��������: ���Ҷ�Ӧֵ(�������ֵ)�ڱ��е��±�,U16�ı�
//ע������: void
//=============================================================================
static u8 GetSubscriptU16(u16 Value, u16 *Table, u8 Length);

//s16 FIFO_AVEREG_CURR(s16 NOW_curr);
s32 FIFO_AVEREG_CURR(s32 NOW_curr);
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
void BQ769X0Init(void)
{
  //  static u16 sCntInit = 0;
    u8 Result = FALSE;
    u8 i = 0;

    while(FALSE == Result)
    {
        Result = TRUE;
       // BQ769X0_Wakeup();

       // delay_us(500);
			 //  HAL_Delay(1);

       BQ769X0_BalanceCtrl(0x00000000); //ֹͣ����

       BQ769X0_ClearStatus(CLR_ALL);

        if(FALSE == BQ769X0_GetADCGainOffset())
        {
            Result = FALSE;
        }

        if(FALSE == BQ769X0_ConfigSys())
        {
        	Result = FALSE;
        }

        for(i = 0; i < NUM_BQ769X0; i++)
        {
            tBqReg[i].SysCtrl1.SysCtrl1Bit.ADC_EN = BQ_ADC_EN;
            tBqReg[i].SysCtrl1.SysCtrl1Bit.TEMP_SEL = BQ_TEMP_SEL;


            tBqReg[i].SysCtrl2.SysCtrl2Bit.DELAY_DIS = BQ_DELAY_DIS;
            tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_EN = BQ_CC_EN;
            tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_ONESHOT = BQ_CC_ONESHOT;
            tBqReg[i].SysCtrl2.SysCtrl2Bit.DSG_ON = BQ_DSG_ON;
            tBqReg[i].SysCtrl2.SysCtrl2Bit.CHG_ON = BQ_CHG_ON;

            if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 2))
            {
            	Result = FALSE;
            }
        }
				BQ769X0_MosControl(DISC_MOS,CONNECT,1);
				
	
   }
}

//=============================================================================
//��������: u8 BQ769X0_ConfigSys(void)
//�������: void
//�������: ���ý��(TRUE�ɹ� FALSEʧ��)
//��������: BQ769X0ϵͳ����������BQ769X0
//ע������:
//=============================================================================
u8 BQ769X0_ConfigSys(void)
{
    u8 Index = 0;
    u8 Result = TRUE;

    for(Index = 0; Index < NUM_BQ769X0; Index++)
    {
    	if(BQ769X0_ConfigPara(Index))
    	{
    		Result = FALSE;
    	}
    }

    return(Result);
}

//=============================================================================
//��������: u8 BQ769X0_SampVolt(u16 *Volt, u8 Length)
//�������: *VoltAd�������ĵ�ѹADֵ Length������о��Ŀ
//�������: �ɼ����(TRUE�ɹ� FALSEʧ��)
//��������: ��ص����ѹ�ɼ�
//ע������: 
//=============================================================================
u8 BQ769X0_SampVolt(u16 *Volt, u8 Length)
{
    u8 AdValue[NUM_BQ769X0][NUM_CELL_BQ * 2];
    u8 Result = TRUE;
    u16 BufAd = 0;
    u16 BufVolt = 0;
    u8 ValidCh = 0;
    u8 i = 0;
    u8 j = 0;

    //����IC���ɼ���Ŀ
    if(Length > NUM_CELL_BQ * NUM_BQ769X0)
    {
        return(FALSE);
    }
    
    for(i = 0; i < NUM_BQ769X0; i++)
    {
		
        if(FALSE == SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], VC1_HI,1, AdValue[i], (NUM_CELL_BQ * 2)))
        {
        	Result = FALSE;
        	sErrBqComm |= 1 << i;
        	for(j = 0; j < NUM_CELL_BQ * 2; j++)
        	{
        		AdValue[i][j] = 0;
        	}
        }
        else
        {
            sErrBqComm &= ~(1 << i);
        }
    }

	for(i = 0; i < NUM_BQ769X0; i++)
	{
		for(j = 0; j < NUM_CELL_BQ; j++)
		{
			BufAd = ((u16)(AdValue[i][2 * j]) << 8) + (u16)(AdValue[i][2 * j + 1]);
			BufVolt = (u16)((u32)BufAd * sBqGain[i] / 1000);
	 
					if(tBqReg[i].ADCOffset > 0)
					{
							BufVolt += (u8)(tBqReg[i].ADCOffset);
					}
					else if(BufVolt > ABS(tBqReg[i].ADCOffset, 0))
					{
							 BufVolt -= (u8)(ABS(tBqReg[i].ADCOffset, 0));
					}
					else
					{
						BufVolt = 0;
					}

					if(BITGET16(gConnectNum, j))//��Ӧ�Ĵ���
					{
						Volt[ValidCh] = BufVolt;
						ValidCh++;
					}
		}
	}
	
    return(Result);
}

//=============================================================================
//��������: u8 BQ769X0_SampCurrAD(u16 *CurrAd)
//�������: *Current�ɼ����ĵ���ֵ
//�������: �ɼ��Ƿ����(TRUE����ɣ� FALSEδ���)
//��������: ��ص�������
//ע������: ����ֵ��λΪ10mA
//=============================================================================
u8 BQ769X0_SampCurr(s32 *Current)
{
    u8 Result = 0;
    u8 BufAD[2] = {0};
    s32 BufCur = 0;
	//��ȡϵͳ״̬
    if(FALSE == SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[0], SYS_STAT,1, &(tBqReg[0].SysStatus.StatusByte), 1))
    {
        return FALSE;
    }
	//�ж��µĿ��ؼ�������ȡ�Ƿ����
    if(0 == tBqReg[0].SysStatus.StatusBit.CC_READY)
    {
    	return FALSE;
    }
	
    Result = SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[0], CC_HI,1, BufAD, 2);
    
    if(TRUE == Result)
    {
	    BufCur = (s32)((s16)((BufAD[0]) << 8) + BufAD[1]);
	  }
	  else
		{
				BufCur = 0;
		}

    if(gRsense > 0)
    {
		
    	*Current = (BufCur * 844 / BMS_config.Pack_RW_config.gBatPackShunt); //8.44uv/gRsense  (gRsense�ĵ�λΪ0.1m��)
    }
    else
    {
    	*Current = 0;
    }

	BQ769X0_ClearStatus(CLR_CC_RDY);
	
    return(TRUE);
}
/////////////////////////////////////////////////////////////////
//��������: u8 BQ769X0_SampCurrAD(u16 *CurrAd)
//�������: *Current�ɼ����ĵ���ֵ
//�������: �ɼ��Ƿ����(TRUE����ɣ� FALSEδ���)
//��������: ��ص�������
//ע������: ����ֵ��λΪ10mA
/////////////////////////////////////////////////////////////////

static float NtcConverTemperature( u16 v )
{	 
//	static u8 Ntc_EER=0;
	// -40 ~ +150 Degree centigrade
	const float rtable[] = {
		206.1, 194.6, 183.9, 173.8, 164.4, 155.5, 147.1, 139.3, 131.9, 125.0,
		118.4, 112.3, 106.5, 101.0, 95.85, 91.00, 86.43, 82.12, 78.05, 74.20,
		70.58, 67.16, 63.93, 60.87, 57.98, 55.24, 52.65, 50.20, 47.87, 45.66,
		43.56, 41.58, 39.69, 37.90, 36.20, 34.58, 33.05, 31.59, 30.21, 28.89,
		27.63, 26.42, 25.27, 24.18, 23.15, 22.17, 21.25, 20.36, 19.53, 18.73,
		17.97, 17.25, 16.56, 15.91, 15.28, 14.69, 14.12, 13.57, 13.05, 12.56,
		12.08, 11.63, 11.20, 10.78, 10.38, 10.00, 9.635, 9.285, 8.949, 8.627,
		8.319, 8.022, 7.738, 7.465, 7.203, 6.952, 6.710, 6.478, 6.255, 6.040,
		5.834, 5.636, 5.446, 5.262, 5.086, 4.917, 4.753, 4.596, 4.445, 4.300,
		4.160, 4.025, 3.895, 3.770, 3.649, 3.533, 3.421, 3.313, 3.210, 3.109,
		3.013, 2.920, 2.830, 2.743, 2.660, 2.579, 2.502, 2.427, 2.354, 2.285,
		2.217, 2.152, 2.089, 2.029, 1.970, 1.914, 1.859, 1.807, 1.756, 1.707,
		1.659, 1.613, 1.569, 1.526, 1.484, 1.451, 1.406, 1.369, 1.333, 1.299,
		1.265, 1.232, 1.201, 1.170, 1.140, 1.111, 1.083, 1.056, 1.030, 1.004,
		0.9788,0.9546,0.9331,0.9082,0.8860,0.8644,0.8434,0.8229,0.8031,0.7838,
		0.7650,0.7467,0.7290,0.7117,0.6949,0.6785,0.6626,0.6471,0.6320,0.6173,
		0.6030,0.5891,0.5756,0.5624,0.5496,0.5371,0.5249,0.5131,0.5016,0.4904,
		0.4795,0.4688,0.4585,0.4484,0.4386,0.4291,0.4198,0.4107,0.4019,0.3933,
		0.3850,0.3769,0.3690,0.3613,0.3538,0.3465,0.3394,0.3325,0.3258,0.3192,
		0.3129   
	};
	
	float ftemp;
	u8 i;
	u8 last = sizeof(rtable) / sizeof(rtable[0]);
	u8 top = 0;
	u8 bot = last - 1;
	
	ftemp = v;
	// VTSX = (ADC in Decimal) x 382uV/LSB
	ftemp *= 382;
	ftemp /= 1000000;
	
	// RTS = (10,000 * VTSX)/(3.3 - VTSX)
	// kRTS = (10 * VTSX)/(3.3 - VTSX)
	ftemp = ((10 * ftemp) / ( 3.3 - ftemp ));
//	if((ftemp >= 194.6 )|(ftemp <= 0.7838))
//	{
//	  if(Ntc_EER<50)
//		Ntc_EER++;
//		if(Ntc_EER==50)
//		{
//	   gBitTemFailure = 1;            //�¶Ȳ�������
//     return -40;					
//		}		
//	}
//	else if(Ntc_EER>0)
//	{
//		Ntc_EER--;
//		 if(Ntc_EER == 0)
//		gBitTemFailure = 0;            //�¶Ȳ�����������
//	}
//	else
//	{
//		
//	}
		
			
		if ( ftemp >= rtable[0] ) 		
		return -40;			
	else if ( ftemp <= rtable[last - 1] ) return 127;
	
	while(1)
	{
		i = top + (bot - top) / 2;
		
		if ( (rtable[i] >= ftemp)&&\
				 (rtable[i + 1] <= ftemp) )
		{
			ftemp = 1 - (ftemp - rtable[i + 1]) / (rtable[i] - rtable[i + 1]);
			break;
		}
		
		if ( rtable[i] > ftemp ) top = i;
		else bot = i;
	}
	
	return ftemp + i - 40;
}
//=============================================================================
//��������: u8 BQ769X0_SampleTemp(s8* Temperature, u8 Length)
//�������: *Temperature�¶ȴ�����䣬Length��ȡ�¶�ͨ������
//�������: void
//����ֵ     : void
//��������: BQ769X0�¶Ȳ���
//ע������:
//=============================================================================
u8 BQ769X0_SampleTemp(s8* Temperature, u8 Length)
{
	u8 BufAD[NUM_BQ769X0][NUM_TEMP_BQ * 2] = {0};
	u16 BufTemp = 0;
	static s8 temp=0;
	static u8 Ntc_EER[NUM_TEMP_BQ]={0};
    u8 Result = TRUE;
    u8 i = 0;
    u8 j = 0;
    u8 k = 0;

    if(Length > NUM_BQ769X0 * NUM_TEMP_BQ)
    {
    	return (FALSE);
    }

    for(i = 0; i < NUM_BQ769X0; i++)
    {
					if(FALSE == SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], TS2_HI,1, BufAD[i], NUM_TEMP_BQ * 2))
					{
						for(j = 0; j < NUM_TEMP_BQ * 2; j++)
						{
							BufAD[i][j] = 0;
						}
						Result = FALSE;
					}
    }

    for(i = 0; i < NUM_BQ769X0; i++)
    {
    	for(j = 0; j < NUM_TEMP_BQ; j++)
    	{
    		BufTemp = ((u16)(BufAD[i][2 * j]) << 8) | (u16)(BufAD[i][2 * j + 1]);
    		//BufTemp = (u16)((u32)BufTemp * 382 / 1000);
        //	*(Temperature + k) = TempSearchTab(BufTemp);
				    *(Temperature + k) = NtcConverTemperature(BufTemp);
				    temp = *(Temperature + k);
            k++;
						if(temp == -40)
						{
						if(Ntc_EER[j]<50)
							Ntc_EER[j]++;
					
						}
						else
						{
					   if(Ntc_EER[j]>0)
							Ntc_EER[j]--;			
						}
            if(k >= Length)
            {
            	break;
            }
	
    	}
    }

			
     if((Ntc_EER[0]==0)&&(Ntc_EER[1]==0))
			gBitTemFailure = 0;            //�¶Ȳ�������					
     if((Ntc_EER[0]==50)||(Ntc_EER[1]==50))
			gBitTemFailure = 1;            //�¶Ȳ�������		


    return Result;
}
////////////////////////////////////////////////////
//?SCD_cacl
//
//1:Risister ��������ֵ
//2:Short_curr,��·���� 0.1A
//3:delay,��ʱʱ��0x00\0x01\0x02\0x03;??????70us\100us\200us\400us
///////////////////////////////////////////////////
u8 SCD_cacl(u16 Risister,u16 Short_curr,u8 delay)//??????????????BQ76940??????
{ 
				u8 SCD_T,RSNS_BIT,protect1=0;
				u8 tmp_i;
				u16 short_Volt;
				u8 RSNS0[8]={22,33,44,56,67,78,89,100};
				u8 RSNS1[8]={44,67,89,111,133,155,178,200};
				short_Volt = Short_curr*Risister/10000;
				//short_Volt = 2000*150/10000;
				if(short_Volt <= 100)
				{
				 RSNS_BIT =0;
				 for(tmp_i=0;tmp_i<8;tmp_i++)
				 {
					 if(short_Volt <= RSNS0[tmp_i])
				 {
					 SCD_T = tmp_i;
					 break;
				 }
				 }
				 protect1 = SCD_T;
				}
				else
				{
					RSNS_BIT =1;
					for(tmp_i=0;tmp_i<8;tmp_i++)
					{
							 if(short_Volt <= RSNS1[tmp_i])
						 {
							 SCD_T = tmp_i;
							 break;
						 }
					}
				 protect1=(RSNS_BIT<<7)+SCD_T;
				 protect1 |= (delay<<3);
				}
				 return protect1;
}
//=============================================================================
//��������: void HardFaultCheck(void)
//�������: void
//�������: void
//��������: ALERT�źŹ���
//ע������:
//=============================================================================
void HardFaultCheck(void)
{
    u8 i = 0;
    for(i = 0; i < NUM_BQ769X0; i++)
    {  	 
        (void)SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_STAT, 1,&(tBqReg[i].SysStatus.StatusByte), 1);
    }
    uStaSysAFE.Byte = 0;
    for(i = 0; i < NUM_BQ769X0; i++)
    {
        if(1 == tBqReg[i].SysStatus.StatusBit.OCD)
        {
            AFE_OCD = 1;
        }

        if(1 == tBqReg[i].SysStatus.StatusBit.OV)
        {
            AFE_OV = 1;
        }

        if(1 == tBqReg[i].SysStatus.StatusBit.UV)
        {
            AFE_UV = 1;
        }

        if(1 == tBqReg[i].SysStatus.StatusBit.OVRD_ALERT)
        {
            AFE_OVRD_ALT = 1;
        }

        if(1 == tBqReg[i].SysStatus.StatusBit.DEVICE_XREADY)
        {
            AFE_DEV_ERR = 1;
        }

        if(1 == tBqReg[i].SysStatus.StatusBit.CC_READY)
        {
            AFE_CC_RDY = 1;
        }
    }

    if(1 == tBqReg[0].SysStatus.StatusBit.SC)
    {
          AFE_SC = 1;
					gBitDchgShortCircuit = AFE_SC;	
					BQ769X0_ClrNoActSC();
    }
			else
				{
            AFE_SC = 0;
						gBitDchgShortCircuit =AFE_SC;
				}	
		

    if(AFE_DEV_ERR)
    {

			BQ769X0_ClearStatus(CLR_DEV_ERR);
			delay_ms(1000);
			BQ769X0Init();
				delay_ms(1000);
		}

}

//=============================================================================
//��������: u8 BQ769X0_MosControl(u8 MosSel, u8 StaSel, u8 Order)
//�������: MosSel��Ҫ������MOS, StaSel���Ƶ�״̬, Order���ƶ��BQ769X0��˳��(�Ӹߵ��ͻ�ӵ͵���)
//�������: void
//����ֵ  : ���ƽ��
//��������: BQ769X0����MOS
//ע������:
//=============================================================================
u8 BQ769X0_MosControl(u8 MosSel, u8 StaSel, u8 Order)
{
    u8 FlgChrgMos = NO_ACTIVE;
    u8 FlgDiscMos = NO_ACTIVE;
    u8 BufCtrlChrg = 0;
    u8 BufCtrlDisc = 0;
    u8 Result = TRUE;
    u8 i = 0;
//
    switch(MosSel)
    {
        case CHRG_MOS:
            if(CONNECT == StaSel)
            {
                BufCtrlChrg = 1;
            }
            else
            {
                BufCtrlChrg = 0;
            }
            FlgChrgMos = ACTIVE;
            break;

        case DISC_MOS:
            if(CONNECT == StaSel)
            {
                BufCtrlDisc = 1;
            }
            else
            {
                BufCtrlDisc = 0;
            }
            FlgDiscMos = ACTIVE;
            break;

        case CHRG_DISC_MOS:
            if(CONNECT == StaSel)
            {
                BufCtrlChrg = 1;
                BufCtrlDisc = 1;
            }
            else
            {
                BufCtrlChrg = 0;
                BufCtrlDisc = 0;
            }
            FlgChrgMos = ACTIVE;
            FlgDiscMos = ACTIVE;
            break;

        default:
            Result = FALSE;
            break;
    }

    if(FALSE == Result)
    {
        return FALSE;
    }

    for(i = 0; i < NUM_BQ769X0; i++)
    {
        if(ACTIVE == FlgChrgMos)
        {
             tBqReg[i].SysCtrl2.SysCtrl2Bit.CHG_ON = BufCtrlChrg;
        }

        if(ACTIVE == FlgDiscMos)
        {
            tBqReg[i].SysCtrl2.SysCtrl2Bit.DSG_ON = BufCtrlDisc;
        }

        if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL2, &(tBqReg[i].SysCtrl2.SysCtrl2Byte), 1))
        {
        	Result = FALSE;
        }
    }

    for(i = 0; i < NUM_BQ769X0; i++)
    {
        if(HIGH_FIRST == Order)
        {
            if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[NUM_BQ769X0 - 1- i], SYS_CTRL2, &(tBqReg[NUM_BQ769X0 - 1 - i].SysCtrl2.SysCtrl2Byte), 1))
            {
            	Result = FALSE;
            }
        }
        else
        {
            if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL2, &(tBqReg[i].SysCtrl2.SysCtrl2Byte), 1))
            {
            	Result = FALSE;
            }
        }
    }

    if(TRUE == Result)
    {
        if(ACTIVE == FlgChrgMos)
        {
           // CmosState = tBqReg[0].SysCtrl2.SysCtrl2Bit.CHG_ON;
        }

        if(ACTIVE == FlgDiscMos)
        {
           // DmosState = tBqReg[0].SysCtrl2.SysCtrl2Bit.DSG_ON;
        }
    }

    return Result;
}

//=============================================================================
//��������: void BQ769X0_MosConfirm(void)
//�������: void
//�������: void
//����ֵ     : void
//��������: MOS״̬ȷ��
//ע������:
//=============================================================================
void BQ769X0_MosConfirm(void)
{
    static u8 sCntErr = 0;
    u8 StaMos[NUM_BQ769X0] = {0};
    u8 ErrNoEqual = 0;
    u8 i = 0;

    for(i = 0; i < NUM_BQ769X0; i++)
    {
        (void)SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_CTRL2,1, StaMos + i, 1);
        StaMos[i] &= 0x03;

//        if((BITGET(StaMos[i], 0) != CmosState) || (BITGET(StaMos[i], 1) != DmosState))
//        {
//            ErrNoEqual = 1;
//            break;
//        }
    }

    if(1 == ErrNoEqual)
    {
        sCntErr++;
        if(sCntErr >= 6)
        {
            BQ769X0_ClearStatus(CLR_ALL);
//            (void)BQ769X0_MosControl(CHRG_MOS, CmosState, HIGH_FIRST);
//            (void)BQ769X0_MosControl(DISC_MOS, DmosState, HIGH_FIRST);
            sCntErr = 0;
        }
    }
    else
    {
        sCntErr = 0;
    }
}

//=============================================================================
//��������: void BQ769X0_BalanceCtrl(u32 Channel)
//�������: Channel����ͨ��
//�������: void
//��������: ���Ƶ�ص��������ͣ
//ע������: void
//=============================================================================
void BQ769X0_BalanceCtrl(u32 Channel)
{
    u16 BufCtrl[NUM_BQ769X0];
    u8 BufWrt[NUM_BQ769X0][3];
    u8 i = 0;
    u8 j = 0;
    u8 k = 0;
    
    for(i = 0; i < NUM_BQ769X0; i++)
    {
        BufCtrl[i] = 0;
    }

    for(i = 0; i < NUM_BQ769X0; i++)
    {
        for(j = 0; j < NUM_CELL_BQ; j++)
        {
            if(BITGET16(gConnectNum, j)) //��Чͨ��
            {
                if(BITGET32(Channel, k))
                {
                    BufCtrl[i] |= ((u16)1 << j);
                }

                k++;
            }
        }
    }

    for(i = 0; i < NUM_BQ769X0; i++)
    {
        BufWrt[i][0] = (u8)(BufCtrl[i] & 0x001F);
        BufWrt[i][1] = (u8)((BufCtrl[i] >> 5) & 0x001F);
        BufWrt[i][2] = (u8)((BufCtrl[i] >> 10) & 0x001F);
    	  BQ769X0_WriteRegister(sAddrBq769x0[i], CELLBAL1, BufWrt[i], 3);
    }

	//�������ر�������־Ҳ����
//	if(0 == Channel)
//	{
//		for(i = 0; i < gCellNum; i++)
//		{			
//			gBlanceState[i] = 0;			
//		}        
//    	gStaBalance = 0;   //���µ�о��Ϣ�����еľ���״̬
//	}
}

//=============================================================================
//��������: void BQ769X0_VoltBalance(void)
//�������: void
//�������: void
//����ֵ     : void
//��������: ��ѹ����
//ע������: 
//=============================================================================
void BQ769X0_VoltBalance(void)
{
    static u32 sTimeWindow = 0x55555555; //��ʱ����
    u32 ChannelBuf = 0; 
    u8 i;
 if(!gSOC_Balance_Pc)
	{
    for(i = 0; i < gCellNum; i++)
    {
		if((((BMSVoltTemperature.g_Volt_call[i] - BMS_config.BMS_Read.gVoltMin) >= BMS_config.Pack_RW_config.gBatBalanceDiffVoltState) && (BMSVoltTemperature.g_Volt_call[i] >= BMS_config.Pack_RW_config.gBatBalanceVoltStart)))//���⿪��
        {
            ChannelBuf |= ((u32)1 << i); //�þ���ͨ��
        }
    }
    
	for(i = 0; i < gCellNum; i++)
	{
		if(((BMSVoltTemperature.g_Volt_call[i] - BMS_config.BMS_Read.gVoltMin) <= BMS_config.Pack_RW_config.gBatBalanceDiffVoltEnd)|(BMSVoltTemperature.g_Volt_call[i]<BMS_config.Pack_RW_config.gBatBalanceVoltEnd)) //����ر�
		{
			ChannelBuf &= ~((u32)1 << i); //�������ͨ��
		}
	}	
			//	 if(ChannelBuf ==0) //����������
//			gBalance_UP= 1;;				
		
    if(timer4>1000)
		{
			timer4 = 0 ;
    sTimeWindow = ~sTimeWindow;
		}
	
		 BMSVoltTemperature.Balance[0] = (u8)(ChannelBuf&0xff);
		 BMSVoltTemperature.Balance[1] = (u8)((ChannelBuf>>8)&0xff);
		ChannelBuf = ChannelBuf & sTimeWindow;
   BQ769X0_BalanceCtrl(ChannelBuf); //������ֹͣ��Ӧͨ���ľ���
	}
	else
	{
		sTimeWindow = ~sTimeWindow;
		 BMSVoltTemperature.Balance[0] = (u8)(gBalance_pc&0xff);
		 BMSVoltTemperature.Balance[1] = (u8)((gBalance_pc>>8)&0xff);
		ChannelBuf = gBalance_pc & sTimeWindow;
		 BQ769X0_BalanceCtrl(ChannelBuf); //������ֹͣ��Ӧͨ���ľ���		
	}
}

//=============================================================================
//��������: void BQ769X0_PowerSave(u8 Module)
//�������: Moduleģ��
//�������: void
//��������: BQ769X0���ߴ���
//ע������:
//=============================================================================
void BQ769X0_PowerSave(u8 Module)
{
    u8 Style = 0;
    u8 i = 0;

    Style = Module;
    
    switch(Style)
    {
        case eADC_MODULE:
        	for(i = 0; i < NUM_BQ769X0; i++)
        	{
        		tBqReg[i].SysCtrl1.SysCtrl1Bit.TEMP_SEL = 1;
        		tBqReg[i].SysCtrl1.SysCtrl1Bit.ADC_EN = 0;
        		(void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1);
        	}
            break;

        case eCC_MODULE:
        	/*
        	for(i = 0; i < NUM_BQ769X0; i++)
        	{
        		tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_EN = 0;
        		tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_ONESHOT = 0;
        		(void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL2, &(tBqReg[i].SysCtrl2.SysCtrl2Byte), 1);
        	}
        	*/
    		tBqReg[0].SysCtrl2.SysCtrl2Bit.CC_EN = 0;
    		tBqReg[0].SysCtrl2.SysCtrl2Bit.CC_ONESHOT = 0;
    		(void)BQ769X0_WriteRegister(sAddrBq769x0[0], SYS_CTRL2, &(tBqReg[0].SysCtrl2.SysCtrl2Byte), 1);

            break;

        case eADC_CC_MODULE:
        	for(i = 0; i < NUM_BQ769X0; i++)
        	{
                tBqReg[i].SysCtrl1.SysCtrl1Bit.TEMP_SEL = 1;
                tBqReg[i].SysCtrl1.SysCtrl1Bit.ADC_EN = 0;

                tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_EN = 0;
                tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_ONESHOT = 0;
                (void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 2);
        	}
            break;

        default:
        	break;
    }
}

//=============================================================================
//��������: void BQ769X0_Resume(u8 Module)
//�������: Moduleģ��
//�������: void
//��������: BQ769X0���ѻָ�
//ע������:
//=============================================================================
void BQ769X0_Resume(u8 Module)
{
    u8 Style = 0;
    u8 i = 0;
  
    Style = Module;   
    switch(Style)
    {
        case eADC_MODULE:
            for(i = 0; i < NUM_BQ769X0; i++)
            {
            	tBqReg[i].SysCtrl1.SysCtrl1Bit.TEMP_SEL = 1;
            	tBqReg[i].SysCtrl1.SysCtrl1Bit.ADC_EN = 1;
            	(void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1);
            }
            break;

        case eCC_MODULE:
        	/*
        	for(i = 0; i < NUM_BQ769X0; i++)
        	{
        		tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_EN = 1;
        		tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_ONESHOT = 0;
        		(void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL2, &(tBqReg[i].SysCtrl2.SysCtrl2Byte), 1);
        	}
        	*/
        	tBqReg[0].SysCtrl2.SysCtrl2Bit.CC_EN = 1;
        	tBqReg[0].SysCtrl2.SysCtrl2Bit.CC_ONESHOT = 0;
        	(void)BQ769X0_WriteRegister(sAddrBq769x0[0], SYS_CTRL2, &(tBqReg[0].SysCtrl2.SysCtrl2Byte), 1);

            break;

        case eADC_CC_MODULE:
            for(i = 0; i < NUM_BQ769X0; i++)
            {
            	tBqReg[i].SysCtrl1.SysCtrl1Bit.TEMP_SEL = 1;
            	tBqReg[i].SysCtrl1.SysCtrl1Bit.ADC_EN = 1;

            	tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_EN = 1;
            	tBqReg[i].SysCtrl2.SysCtrl2Bit.CC_ONESHOT = 0;

            	(void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 2);
            }
            break;

        default:
        	break;
    }
}

//=============================================================================
//��������: void ClearBqStatus(u8 Status)
//�������: Status��Ҫ�����״̬
//�������: void
//����ֵ  : void
//��������: ���BQ769X0��ϵͳ״̬
//ע������:
//=============================================================================
void BQ769X0_ClearStatus(u8 Status)
{
    u8 BufWrite = 0;
    u8 i = 0;

    BufWrite = Status;

    for(i = 0; i < NUM_BQ769X0; i++)
    {
    	(void)SimI2C_WriteBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_STAT, 1, &BufWrite, 1);
    }
}

//==============================================================================
//��������: void BQ769X0_Diagnose(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: Ba769x0�������
//ע������:
//==============================================================================
void BQ769X0_Diagnose(void)
{
    static u8 sCntCommErr = 0;
    //static u8 sCntPowerDown = 0;

    if(sErrBqComm)
    {
       // ErrCommBq = 1;
        sCntCommErr++;
        if(sCntCommErr >= 5)
        {
          //  gDeviceInitFlag = ENABLE_YS;
            sCntCommErr = 0;
        }
    }
    else
    {
       // ErrCommBq = 0;
    }

    BQ769X0_ParaConfirm();
}

//=============================================================================
//��������: void BQ769X0_Wakeup(void)
//�������: void
//�������: void
//��������: ����BQ769X0
//ע������: void
//=============================================================================
void BQ769X0_Wakeup(void)
{
//	GPIO_ResetBits(GPIOx_ALERT, ALERT);
//			delay_ms(1000);
//		GPIO_SetBits(GPIOx_ALERT, ALERT);
//		delay_ms(1000);
}


//=============================================================================
//��������: u8 BQ769X0_Sleep(void)
//�������: void
//�������: ������(TRUE�ɹ� FALSEʧ��)
//��������: BQ769X0��Normalģʽ����Shipģʽ
//ע������:
//=============================================================================
u8 BQ769X0_Sleep(void)
{
    u8 Result = TRUE;
    u8 i = 0;
//     u8 link = 0;
//    for(i = 0; i < NUM_BQ769X0; i++)
//    {
//			tBqReg[i].SysCtrl1.SysCtrl1Byte=0X00;
//			BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1);
//			SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_CTRL1,1, &link, 1);
//			if((link&0x03) == 0)
//			{		
//							tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_A = 0;
//							tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_B = 1;
//							if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1))
//							{
//								Result = FALSE;
//							}
//              SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_CTRL1,1, &link, 1);
//								if((link&0x03) == 0x01)
//								{
//											if(TRUE == Result)
//											{
//												tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_A = 1;
//												tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_B = 0;
//													if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1))
//													{
//														Result = FALSE;
//														break;
//													}
//													SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_CTRL1,1, &link, 1);
//													if((link&0x03) != 0x02)
//													{
//															tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_A = 1;
//												      tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_B = 0;
//												      BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1);
//														
//													}
//													
//											}
//						   	}
//							 }
//				}
//     BQ769X0_Resume(eADC_CC_MODULE);
 for(i = 0; i < NUM_BQ769X0; i++)
    {
    	tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_A = 0;
    	tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_B = 1;
    	if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1))
    	{
    		Result = FALSE;
    	}

        if(TRUE == Result)
        {
        	tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_A = 1;
        	tBqReg[i].SysCtrl1.SysCtrl1Bit.SHUT_B = 0;
            if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1))
            {
            	Result = FALSE;
            	break;
            }
        }
    }

    return(Result);
}
//=============================================================================
//��������: u8 GetBQ769x0Status(void)
//�������: void
//�������: BQ769X0��оƬ�ڲ�״̬
//��������: ��ȡBQ769X0оƬ�ڲ�״̬
//ע������:
//=============================================================================
u8 GetBQ769X0Status(void)
{
    return(uStaSysAFE.Byte);
}

//=============================================================================
//��������: u8 GetBQ769x0Status(void)
//�������: void
//�������: BQ769X0�Ĵ���״̬
//��������: ��ȡBQ769X0�����г��ֵĴ���״̬
//ע������:
//=============================================================================
u8 GetBQ769X0Error(void)
{
    return(sErrorBq);
}


//=============================================================================
//��̬��������
//=============================================================================
//=============================================================================
//��������: static u8 BQ769X0_GetADCGainOffset(void)
//�������: void
//�������: ��ȡ���(TRUE�ɹ� FALSEʧ��)
//��������: ��ȡBQ769X0���ڲ��̻��������ƫ����
//ע������: void
//=============================================================================
static u8 BQ769X0_GetADCGainOffset(void)
{
    u8 Result = TRUE;
    u8 i = 0;

	for(i = 0; i < NUM_BQ769X0; i++)
	{
		if(FALSE == SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], ADCGAIN1,1, &(tBqReg[i].ADCGain1.ADCGain1Byte), 1))
		{
			Result = FALSE;
		}

		if(FALSE == SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], ADCGAIN2,1,&(tBqReg[i].ADCGain2.ADCGain2Byte), 1))
		{
			Result = FALSE;
		}

		if(FALSE == SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], ADCOFFSET,1,(u8*)(&(tBqReg[i].ADCOffset)), 1))
		{
			Result = FALSE;
		}

		if(TRUE == Result)
		{
			sBqGain[i] = ADCGAIN_BASE + ((tBqReg[i].ADCGain1.ADCGain1Byte & 0x0C) << 1) + ((tBqReg[i].ADCGain2.ADCGain2Byte & 0xE0)>> 5);
		}
	}

	return(Result);
}


//=============================================================================
//��������: static u8 BQ769X0_WriteRegister(u8 DeviceAddr, u8 RegAddr, u8 *CfgData, u8 Length)
//�������: DeviceAddr:�豸��ַ��RegAddr�Ĵ�����ַ, *CfgData��Ҫ���óɵ�ֵ��Length���õĳ���
//�������: д����(TRUE�ɹ� FALSEʧ��)
//��������: дB1769X0�Ĵ���ֵ
//ע������: 
//=============================================================================
static u8 BQ769X0_WriteRegister(u8 DeviceAddr, u8 RegAddr, u8 *CfgData, u8 Length)
{
	u8 BufBack[12] = {0};
	u8 Result = TRUE;
	u8 i =0;
	u8 j= 0;

    if((RegAddr + Length) > 12)
    {
    	BITSET(sErrorBq, eERR_CONFIG);
    return(FALSE);
	}
	for(j = 0; j < 3; j++)
	{

			Result = SimI2C_WriteBuffer_CRC(Bq769X0_IIC_CHANNEL, DeviceAddr, RegAddr, 1,CfgData, Length);
			if(TRUE == Result)
			{
				Result = SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, DeviceAddr, RegAddr, 1,BufBack, Length);
			}
			
			


				//����ȡ������ֵ�Ƿ�Ϊ��д���ֵ
				for(i = 0; i < Length; i++)
				{
						if(SYS_CTRL1 == (RegAddr + i))
						{
								if((BufBack[i] & 0x18) != (*(CfgData + i) & 0x18))
								{
									Result = FALSE;
										break;
								}
						}
						else if(SYS_CTRL2 == (RegAddr + i))
						{
								if((BufBack[i] & 0xE3) != (*(CfgData + i) & 0xE3))
								{
									Result = FALSE;
										break;
								}
						}
						else
						{
								if(BufBack[i] != *(CfgData + i))
								{
									Result = FALSE;
										break;
								 }
						}
			  }
				if(Result==TRUE)
				{
					break;
				}
			}
	return(Result);
}

//=============================================================================
//��������: static u8 BQ769X0_ConfigPara(u8 AddrIndex)
//�������: AddrIndex��ַ����
//�������: ���ý��(TRUE�ɹ� FALSEʧ��)
//��������: BQ769X0���ò���
//ע������:
//=============================================================================
u8 BQ769X0_ConfigPara(u8 AddrIndex)
{
	u8 OCThreshTable[16] = {8, 11, 14, 17, 19, 22, 25, 28, 31, 33, 36, 39, 42, 44, 47, 50};//��λmv
	u16 OCDelayTable[8] = {10, 20, 40, 80, 160, 320, 640, 1280}; //��λms
	//u8 SCThreshTable[8] = {22, 33, 44, 56, 67, 78, 89, 100};//��λmv
	u8 SCThreshTable[8] = {44, 67, 89, 111, 133, 155, 178, 200};//��λmv
 //   u16 SCDelayTable[4] = {70, 100, 200, 400};//��λus
    u8 OVDelayTable[4] = {1, 2, 4, 8}; //��λs
    u8 UVDelayTable[4] = {1, 4, 8, 16};//��λs
    u8 BufSC = 0;
    u8 BufOC = 0;
    u8 Result = 0;
    u8 i = 0;
u16 gChgShortCircuit_temp=0;
    if(AddrIndex >= NUM_BQ769X0)
    {
    	return FALSE;
    }


		gChgShortCircuit_temp =	(u16)BMS_config.Pack_RW_config.gDchgShortCircuit;
	//��������������������ò�����������Ĭ��ֵ 
		tBqReg[AddrIndex].Protect1.Protect1Byte = SCD_cacl(ShortCircuit(gChgShortCircuit_temp),BMS_config.Pack_RW_config.gBatPackShunt,2);

		tBqReg[AddrIndex].Protect1.Protect1Bit.SCD_THRESH = GetSubscriptU8(BufSC, SCThreshTable, 8);
		tBqReg[AddrIndex].Protect2.Protect2Bit.OCD_DELAY = GetSubscriptU16(640, OCDelayTable, 8);
		BufOC = (u8)((u16)BMS_config.Pack_RW_config.gChgShortCircuit * BMS_config.Pack_RW_config.gBatPackShunt / 1000);
		tBqReg[AddrIndex].Protect2.Protect2Bit.OCD_THRESH = GetSubscriptU8(BufOC, OCThreshTable, 16);
		
		tBqReg[AddrIndex].Protect3.Protect3Bit.OV_DELAY = GetSubscriptU8(2, OVDelayTable, 4);
		tBqReg[AddrIndex].Protect3.Protect3Bit.UV_DELAY = GetSubscriptU8(2, UVDelayTable, 4);
		
		if(15 == gCellNum)
		{
			gConnectNum = IC_CONNECT_15;
		}
		else if(14 == gCellNum)
		{
			gConnectNum = IC_CONNECT_14;
		}
		else if(13 == gCellNum)
		{
			gConnectNum = IC_CONNECT_13;
		}
		else if(12 == gCellNum)
		{
			gConnectNum = IC_CONNECT_12;
		}
		else if(11 == gCellNum)
		{
			gConnectNum = IC_CONNECT_11;
		}
		else if(10 == gCellNum)
		{
			gConnectNum = IC_CONNECT_10;
		}
		else if(9 == gCellNum)
		{
			gConnectNum = IC_CONNECT_9;
		}
		
	if(tBqReg[AddrIndex].ADCOffset > 0)
    {
    	tBqReg[AddrIndex].OVTrip = (u8)(((((u32)gHighVoltHardPrt - (u32)tBqReg[AddrIndex].ADCOffset) * 1000 / sBqGain[AddrIndex]  - OV_THRESH_BASE) >> 4) & 0xFF);
    	tBqReg[AddrIndex].UVTrip = (u8)(((((u32)gLowVoltHardPrt - (u32)tBqReg[AddrIndex].ADCOffset) * 1000 / sBqGain[i] - UV_THRESH_BASE) >> 4) & 0xFF);
    }
    else
    {
    	tBqReg[AddrIndex].OVTrip = (u8)(((((u32)gHighVoltHardPrt + (u32)(-tBqReg[AddrIndex].ADCOffset)) * 1000 / sBqGain[AddrIndex]  - OV_THRESH_BASE) >> 4) & 0xFF);
    	tBqReg[AddrIndex].UVTrip = (u8)(((((u32)gLowVoltHardPrt + (u32)(-tBqReg[AddrIndex].ADCOffset)) * 1000 / sBqGain[AddrIndex] - UV_THRESH_BASE) >> 4) & 0xFF);
	  }
	
    tBqReg[AddrIndex].CCCfg = VAL_CC_CFG;
    if(FALSE == BQ769X0_WriteRegister(sAddrBq769x0[AddrIndex], PROTECT1, &(tBqReg[AddrIndex].Protect1.Protect1Byte), 6))
    {
    	Result = FALSE;
        sErrBqComm |= 1 << (AddrIndex + 4);
    }
    else
    {
    	sErrBqComm &= ~(1 << (AddrIndex + 4));
    }

    return(Result);
}
//=============================================================================
//��������: static void BQ769X0_ParaConfirm(void)
//�������: void
//�������: void
//����ֵ     : void
//��������: BQ769X0�������
//ע������:
//=============================================================================
static void BQ769X0_ParaConfirm(void)
{
    static u8 sCntCycle = 0;
    static u8 sCntCompare = 0;
    static u8 sCntErr[NUM_BQ769X0] = {0};
    u8 SysCtrl[NUM_BQ769X0][2];
    u8 SysPrt[NUM_BQ769X0][6];
    u8 FlgErr = 0;
    u8 i = 0;

    if(sCntCycle >= 240)
    {
        if(sCntCompare < 2)
        {
            sCntCompare++;
        }
        else
        {
            sCntCycle = 0;
            sCntCompare = 0;
        }

        for(i = 0; i < NUM_BQ769X0; i++)
        {
            (void)SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_CTRL1,1, SysCtrl[i], 2);
            (void)SimI2C_ReadBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], PROTECT1,1,SysPrt[i], 6);

            if(((SysCtrl[i][0] & 0x18) != (tBqReg[i].SysCtrl1.SysCtrl1Byte & 0x18))
                || ((SysCtrl[i][1] & 0x80) != tBqReg[i].SysCtrl2.SysCtrl2Bit.DELAY_DIS)
                || ((SysPrt[i][0] & 0x9F) != (tBqReg[i].Protect1.Protect1Byte & 0x9F))
                || ((SysPrt[i][1] & 0x7F) != (tBqReg[i].Protect2.Protect2Byte & 0x7F))
                || ((SysPrt[i][2] & 0xF0) != (tBqReg[i].Protect3.Protect3Byte & 0xF0))
                || (SysPrt[i][3] != tBqReg[i].OVTrip)
                || (SysPrt[i][4] != tBqReg[i].UVTrip)
                || (SysPrt[i][5] != VAL_CC_CFG))
            {
                sCntErr[i]++;
            }
            else
            {
                sCntErr[i] = 0;
            }
        }

        for(i = 0; i < NUM_BQ769X0; i++)
        {
            if(1 == sCntErr[i])
            {
                FlgErr = 1;
                break;
            }
        }

        if(0 == FlgErr)
        {
        	gBitFAE_Error = 0;
        }
    }
    else
    {
        sCntCycle++;
    }

    for(i = 0; i < NUM_BQ769X0; i++)
    {
        if(sCntErr[i] >= 3)
        {
            (void)BQ769X0_ConfigPara(i);
            (void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 2);
        	  gBitFAE_Error = 1;
            sCntErr[i] = 0;
        }
    }
}


//=============================================================================
//��������: static void ClearBqNoActSC(u8 AddrDev)
//�������: void
//�������: void
//����ֵ  : void
//��������: ���BQ769X0û�н��ж�·����IC�Ķ�·״̬
//ע������:
//=============================================================================
static void BQ769X0_ClrNoActSC(void)
{
    u8 BufWrite= 0;
    u8 i = 0;

   // BufWrite = CLR_SC;
	BufWrite = CLR_ALL;

//    for(i = 1; i < NUM_BQ769X0; i++)
//    {
           (void) SimI2C_WriteBuffer_CRC(Bq769X0_IIC_CHANNEL, sAddrBq769x0[i], SYS_STAT,1, &BufWrite, 1);
//    }
}

/*
//=============================================================================
//��������: static s8 TempSearchTab(u16 ValAD)
//�������: ValAD��AD�ɼ�ֵ,LowestTemp����¶�,HighestTemp����¶�,
//          *Table�¶���ADֵ�Ķ�Ӧ��,LenTable��Ӧ��ĳ���
//�������: void
//����ֵ  : ��о�¶�
//��������: ����ȡ�¶�ֵ
//ע������: void
//=============================================================================
static s8 TempSearchTab(u16 ValAD, s8 LowestTemp, s8 HighestTemp, STR_TEMPVOLT* Table, u8 LenTable)
{
    u8 result = 0, remainder = 0;
    u8 index = 0, i = 0, cnt = 0;
    u16  basenum = 0;
    s8 temp = 0;
    u16 AdBuf = 0;

    if(((void*)0 == Table) || (LowestTemp > HighestTemp) || (LenTable < 1))
    {
        return 0x7F; //���ش����¶�ֵ127
    }

    AdBuf = ((u16)(Table[0].BaseHigh) << 8 ) + Table[0].BaseLow;
    if(ValAD > AdBuf) //�������ADֵ
    {
        return(LowestTemp - 1);  //��������¶ȣ���Ϊ����¶�-1
    }

    AdBuf = ((u16)(Table[LenTable - 1].BaseHigh) << 8) + Table[LenTable - 1].BaseLow;
	AdBuf -= (u16)(Table[LenTable - 1].BaseCnt - 1) * Table[LenTable - 1].BaseDiff;
    if(ValAD < AdBuf)  //�������ADֵ
    {
        return(HighestTemp + 1); //��������¶ȣ���Ϊ����¶�+1
    }

    while(index < (LenTable - 1))
    {
        if((ValAD <= (((u16)(Table[index].BaseHigh) << 8) + Table[index].BaseLow))
          && (ValAD > (((u16)(Table[index + 1].BaseHigh) << 8) + Table[index + 1].BaseLow)))
        {
            break;
        }
        else
        {
            index++;
        }
    }

    for(i = 0; i < index; i++)
    {
        cnt = cnt + Table[i].BaseCnt;
    }

    basenum = ((u16)(Table[index].BaseHigh) << 8) + Table[index].BaseLow;
    if(ValAD == basenum)
    {
        temp = (s8)cnt + LowestTemp;
        return temp;
    }
    else
    {
        result = (basenum - ValAD) / Table[index].BaseDiff;
        remainder = (basenum - ValAD) % Table[index].BaseDiff;
        if(remainder > (Table[index].BaseDiff / 2))
        {
            result += 1;
        }
        cnt += result;
        temp = (s8)cnt + LowestTemp;
        return temp;
    }
}
*/
//==============================================================================
//��������: static s8 TempSearchTab(u16 code)
//�������: code�ǲɼ���ȡ�ĵ�ѹֵ
//�������:
//����ֵ  : ����ȡ���¶�ֵ
//��������: ʹ�òɼ��ĵ�ѹֵ�����ҵ�ѹ�¶ȶ�Ӧ���ȡ�¶�ֵ
//ע������: �ö��ַ��ҵ��ӽ�λ�ã�����ǰһ������һ����ֵ�Ƚϵõ���ӽ���ֵ
//==============================================================================
//static s8 TempSearchTab(u16 code)
//{
//    u16 buff = 0;
//volatile    u8 low = 0;
// volatile   u8 high = LEN_TEMP_TABLE - 1;

//    if(code > TempVoltTable[0])
//    {
//        return(MIN_TEMP_TABLE - 1);  //��������¶ȣ���ΪMIN_TEMP_TAB - 1
//    }

//    if(code < TempVoltTable[LEN_TEMP_TABLE - 1])
//    {
//        return(MAX_TEMP_TABLE + 1);  //��������¶ȣ���ΪMAX_TEMP_TABLE + 1
//    }

//    while(low < high)
//    {
//        buff = (low + high) >> 1;  //(low+high)/2
//        if(code != TempVoltTable[buff])
//        {
//            if(code < TempVoltTable[buff])
//            {
//                low = buff + 1;
//            }
//            else if(buff > 1)
//            {
//                high = buff - 1;
//            }
//            else
//            {
//            	high = 0;
//            }
//        }
//        else
//        {
//            return((s8)buff - TEMP_ZERO_OFFSET);
//        }
//    }

//    buff = (low + high) >> 1;   //(low+high)/2

//    if(code > TempVoltTable[buff])
//    {
//        if(code > (TempVoltTable[buff] + TempVoltTable[buff - 1]) / 2)
//        {
//            return ((s8)(buff - 1) - TEMP_ZERO_OFFSET);
//        }
//        else
//        {
//            return ((s8)buff - TEMP_ZERO_OFFSET);
//        }
//    }
//    else
//    {
//        if(code > (TempVoltTable[buff + 1] + TempVoltTable[buff]) / 2)
//        {
//            return((s8)buff - TEMP_ZERO_OFFSET);
//        }
//        else
//        {
//            return((s8)(buff + 1) - TEMP_ZERO_OFFSET);
//        }
//    }
//}

//=============================================================================
//��������: static u8 GetSubscriptU8(u8 Value, u8 *Table, u8 Length)
//�������: Value����Ҫ���ҵ�ֵ��*Table���ұ�Length��ĳ���
//�������: void
//����ֵ  :  ���ҵ��±�
//��������: ���Ҷ�Ӧֵ(�������ֵ)�ڱ��е��±�
//ע������: void
//=============================================================================
static u8 GetSubscriptU8(u8 Value, u8 *Table, u8 Length)
{
    u8 Buffer;
    u8 low = 0;
    u8 high = Length - 1;

    if(Value <= Table[0])
    {
        return 0;
    }

    if(Value >= Table[Length - 1])
    {
        return(Length - 1);
    }

    while(low < high)
    {
        Buffer = (low + high) >> 1;  //(low+high)/2
        if(Value != Table[Buffer])
        {
            if(Value < Table[Buffer])
            {

                high = Buffer - 1;
            }
            else
            {
                 low = Buffer + 1 ;
            }
        }
        else
        {
            return(Buffer);
        }
    }

    Buffer = (low + high) >> 1;   //(low+high)/2

    if(0 == Buffer)
    {
        if(Value > (Table[0] + Table[1]) / 2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if(Value > Table[Buffer])
    {
        if(Value > (Table[Buffer + 1] + Table[Buffer]) / 2)
        {
            return(Buffer + 1);
        }
        else
        {
            return Buffer;
        }
    }
    else
    {
        if(Value > (Table[Buffer] + Table[Buffer - 1]) / 2)
        {
            return Buffer;
        }
        else
        {
            return (Buffer - 1);
        }
    }
}

//=============================================================================
//��������: static u8 GetSubscriptU16(u16 Value, u16 *Table, u8 Length)
//�������: Value����Ҫ���ҵ�ֵ��*Table���ұ�Length��ĳ���
//�������: void
//����ֵ  :  ���ҵ��±�
//��������: ���Ҷ�Ӧֵ(�������ֵ)�ڱ��е��±�,U16�ı�
//ע������: void
//=============================================================================
static u8 GetSubscriptU16(u16 Value, u16 *Table, u8 Length)
{
    u16 Buffer;
    u8 low = 0;
    u8 high = Length - 1;

    if(Value <= Table[0])
    {
        return 0;
    }

    if(Value >= Table[Length - 1])
    {
        return(Length - 1);
    }

    while(low < high)
    {
        Buffer = (low + high) >> 1;  //(low+high)/2
        if(Value != Table[Buffer])
        {
            if(Value < Table[Buffer])
            {

                high = Buffer - 1;
            }
            else
            {
                 low = Buffer + 1 ;
            }
        }
        else
        {
            return(Buffer);
        }
    }

    Buffer = (low + high) >> 1;   //(low+high)/2

    if(0 == Buffer)
    {
        if(Value > (Table[0] + Table[1]) / 2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if(Value > Table[Buffer])
    {
        if(Value > (Table[Buffer + 1] + Table[Buffer]) / 2)
        {
            return(Buffer + 1);
        }
        else
        {
            return Buffer;
        }
    }
    else
    {
        if(Value > (Table[Buffer] + Table[Buffer - 1]) / 2)
        {
            return Buffer;
        }
        else
        {
            return (Buffer - 1);
        }
    }
}
/*********************************************************************************
*   �� �� ��: BQ76940_TASK(void)
*   ����˵��: 
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************/
void BQ76940_TASK(void)
{ 
	static u8 Current_falg=0;
	static u8 Volt_falg=0;
	static u8 Temp_falg=0;
	static u8 task=0;
	static u8 BQ76940_falg = 1;
	static 	uint32_t tickstart_BQ76940 = 0;
	static 	uint32_t ticksotp_BQ76940 = 0;
	u16 Volt_call_0ff = 0;
	u8 i = 0 ;
	static u8 up_soc=0;
	static u8 tim_daly_d=0;
	static u8 tim_daly_c=0;
	static u8 tim_daly_S=0;
	static u16 Volt_off_line=0;
	static u16 off_line_num[gCellNum-1] ;
	//						for(i = 0;i < BMS_config.Pack_RW_config.gTemNumber;i++)
	//				BMSVoltTemperature.g_Temperature_call[i] = 25;
	//						for(i = 0;i < gCellNum;i++)
	//							BMSVoltTemperature.g_Volt_call[i] =4000;
	//			   gCurrentOperation = 0;
	//	     BMS_config.BMS_Read.gCurrent = gCurrentOperation/10;
	//	     BMS_config.BMS_Read.gCurrentAvg = BMS_config.BMS_Read.gCurrent;
	//						for(i = 0;i < BMS_config.Pack_RW_config.gTemNumber;i++)
	//				BMSVoltTemperature.g_Temperature_call[i] = 10;


	switch(task)
	{
		//�Ե���������
		case 0://�ɼ����������سɹ����   
			Current_falg = BQ769X0_SampCurr(&Current_f);
			if(Current_falg == TRUE )
			{    
				if(Current_f<-30)
					gCurrentOperation =(s32)(Calibration.gDchgCurrent_double*Current_f);
				else if(Current_f>30)
					gCurrentOperation =(s32)(Calibration.gChgCurrent_double*Current_f);
				else
					gCurrentOperation = 0;
				BMS_config.BMS_Read.gCurrent = gCurrentOperation/10;								//�õ���ǰ����
				BMS_config.BMS_Read.gCurrentAvg =FIFO_AVEREG_CURR(BMS_config.BMS_Read.gCurrent);	//�õ�ƽ������
				///////////////////////////////////////////
				if (BMS_config.BMS_Read.gCurrent<-10)
				{
					if(tim_daly_d < 3)
						tim_daly_d++;
					if(tim_daly_d == 3)
					{		
						BAT_STATE_Curren = 2;  //�ŵ�״̬
						up_soc = 1;
					}
				} 
				else
				{
					if(tim_daly_d>0)
						tim_daly_d--;			
				}
				///////////////////////////////////////////////		
				if (BMS_config.BMS_Read.gCurrent>8)
				{
					if(tim_daly_c<3)
						tim_daly_c++;
					if(tim_daly_c == 3)
					{			
						BAT_STATE_Curren = 1;  
						BAT_State_old = BAT_State;
						up_soc =1;
					}

				}
				else
				{
					if(tim_daly_c>0)
						tim_daly_c--;
				}
				///////////////////////////////////////////////
				if(gCurrentOperation == 0)
				{		
					if(tim_daly_S<3)	
						tim_daly_S++;
					if(tim_daly_S == 3)
					{
						BAT_STATE_Curren = 0; 					
						if(up_soc ==1)
						{          //д���ò���
							BMS_config_Write(); 
							BMS_Calibration_Write();
							BMS_Alm_Number_Write(); 
							up_soc =0;
						}						
					}
				}
				else
				{
					if(tim_daly_S>0)	
						tim_daly_S--;	  
				}

			}
			task=1;	
		break;
		//���¶ȵ���ش���	
		case 1:     
			Temp_falg = BQ769X0_SampleTemp(Temperature_call,BMS_config.Pack_RW_config.gTemNumber);
			if(Temp_falg)
			{
				for(i = 0;i < BMS_config.Pack_RW_config.gTemNumber;i++)
				{
					if((Temperature_call[i] >= -40 ) && (Temperature_call[i] < 127 ))
					{
						if(Temperature_call[i] > 0)
							BMSVoltTemperature.g_Temperature_call[i] = Calibration.gTem_double[i]*Temperature_call[i];
						else
							BMSVoltTemperature.g_Temperature_call[i] = Temperature_call[i];				
					}
					else
					{
						BMSVoltTemperature.g_Temperature_call[i] = -40;
					}
				}
			}
			task=2;	
		break;
		//�Ե�ѹ����ش���
		case 2:   	
			Volt_falg = BQ769X0_SampVolt(Volt_call, gCellNum);
			if(Volt_falg)
			{

				for(i = 0;i < gCellNum;i++)
				{
					if((Volt_call[i]>500)&&(Volt_call[i]<5000))
					{
						if(Volt_call[i] >3700)
							BMSVoltTemperature.g_Volt_call[i] = Calibration.g_Volt_double_max[i]*Volt_call[i];		
						else				
							BMSVoltTemperature.g_Volt_call[i] = Calibration.g_Volt_double_min[i]*Volt_call[i];						 
					}
				}		 
				//////////////////////////////////////////////////////////////////////////////////////////////////
				//�����������
				for(i = 0;i < (gCellNum-1);i++)
				{

					if(BMSVoltTemperature.g_Volt_call[i]>BMSVoltTemperature.g_Volt_call[i+1])
					{
						Volt_off_line =BMSVoltTemperature.g_Volt_call[i] - BMSVoltTemperature.g_Volt_call[i+1];
					}
					else
					{
						Volt_off_line = BMSVoltTemperature.g_Volt_call[i+1] - BMSVoltTemperature.g_Volt_call[i];
					}
					if(Volt_off_line>1500) //��ѹ�����1.5 v��¼��������
					{
						if(off_line_num[i]<100)	  //
							off_line_num[i]++;
						if(off_line_num[i]==100)
						{
							Volt_call_0ff |= 1>>i;
						}													
					}
					else
					{
						if(off_line_num>0)
						{
							off_line_num[i]--; 	
							if(off_line_num[i] == 0)
							{
								Volt_call_0ff &= ~(1>>i);
							}
						}

					}


				}
			//////////////////////////////////////////////////////////////////////////////////////////////////									 
			}
			if(Volt_call_0ff)
				gBitVoltFailure = 1;		//��ѹ�ɼ�ʧЧ
			else
				gBitVoltFailure = 0;
			task=3;	
			tickstart_BQ76940 = HAL_GetTick();  //��ȡ����ʱ��
			//			for(i = 0; i < NUM_BQ769X0; i++)
			//        	{
			////        	tBqReg[i].SysCtrl1.SysCtrl1Bit.TEMP_SEL = 1;
			//        		tBqReg[i].SysCtrl1.SysCtrl1Bit.ADC_EN = 0;
			//        		(void)BQ769X0_WriteRegister(sAddrBq769x0[i], SYS_CTRL1, &(tBqReg[i].SysCtrl1.SysCtrl1Byte), 1);
			//        	}

		break;
		case 3: 
			//��������־λ�� ���� ���;���λ��ָ��
			if((( gBalance_falg )&&(BQ76940_falg))|(gSOC_Balance_Pc))
			{						

				ticksotp_BQ76940 = HAL_GetTick();
				if(ticksotp_BQ76940<tickstart_BQ76940)//
				{
					ticksotp_BQ76940=0xffffffff-tickstart_BQ76940+ticksotp_BQ76940;
				}
				else
					ticksotp_BQ76940=ticksotp_BQ76940-tickstart_BQ76940;

				if(ticksotp_BQ76940>4000)
				{
					BQ76940_falg = 0;
					tickstart_BQ76940 = 0;
					ticksotp_BQ76940 = 0;

				}
				BQ769X0_VoltBalance();
			}	
			else
			{
				BQ769X0_BalanceCtrl(0x00000000);						
				if(BAT_STATE_Curren !=0)
					BMSVoltTemperature.Balance[0]  = BMSVoltTemperature.Balance[1] =0;
				BQ76940_falg = 1; //������һ�ξ���
				task=4;	
				timer2 = 0;	
			}		
		break;
		case 4:  
			if(gBalance_falg)	
			{	
				if(timer2>1000)	
				{
					task=0;
					timer2 = 0;		               								
				}
			}

			else
			{   
				HardFaultCheck();
				task=0;					 
			}

		break;
		default:
			task=0;	
		break;	
	}

}
//=============================================================================
//��������: s32 FIFO_AVEREG_CURR(s32 NOW_curr)
//�������: ��ǰʵ�ʵ���ֵ
//�������: ��
//����ֵ     : ���30��������ƽ��ֵ
//��������: ��ȡ����30�ε�����ƽ��ֵ
//ע������:
//=============================================================================
s32 FIFO_AVEREG_CURR(s32 NOW_curr)
{
	static s32 New_30curr[30];//�������30������
	static u8 start_p=0;//�����׵�ַ
	static u8 new_p=0;//���е�ǰ��ַ
	s32 curr=0,sum_curr=0;
	//s32 curr,sum_curr;
	u8 i,p;
	/*���ݽ������*/
	curr = NOW_curr;
	New_30curr[new_p++] = NOW_curr;
	if(new_p==30)//���30��
	new_p = 0;	
	if(new_p == start_p)
	start_p++;
  if(start_p==30)	
	start_p = 0;	
	/*��ƽ��ֵ*/
	p = start_p;
	for(i=0;i<30;i++)
	{
		 sum_curr = sum_curr+New_30curr[p++];
		 if(p==30)
		 p = 0;
	}
	curr = sum_curr/30;
	return curr;//����30��������ƽ��ֵ	
}
