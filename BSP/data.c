#include "pbdata.h"
#include "SYSTypeDef.h"
#include "data.h"
#include "EventRecord.h"
#include "w25qxx.h"
//��Ʒ�ͺ�V1.01

#define TERM_TICK 120
#define Erase_Sector_FALG  0X23
#define Failure_DUBAG 0  //����ʧЧ�رչ��� 1���� 0�ر�
//Ӳ���汾V0.001
u8 g_Product_Version[10] = 
{
	'b', 
	'm', 
	's', 
	'_',
	'v',
	'1',
	'4',
	's',
  '1',
  '2'
}; 
//BMS����汾��
u8 g_BMS_Version[10] = 
{
  'b', 
	'm', 
	's', 
	'_',
	'v',
	'1',
	'.',
	'0',
  '0',
  '3'
}; 
//����汾V01
u8 g_SoftWare_Version[10] = 
{
	'S', 
	'O', 
	'F', 
	'T',
	'W',
	'a',
	'r',
	'e',
  '0',
  '1'
};
//�������� 
u8 g_BMS_Product_data[3] = 
{
	20, 
	10, 
	31

}; 
u8 g_FaultAlarmState[8];
u16 g_Volt_call[15]={0
};
s8 g_Temperature_call[2] = {
0	
};
BMS_typedef BMS_config;
BMSVoltTemperature_call_typedef  BMSVoltTemperature; //
Calibration_typedef  Calibration; //У׼����
Alm_typedef Alm_Number; // ���ϴ�����¼
Extremum_typedef  Extremum; //��ֵ�洢
u16 Volt_call[14];
s8 Temperature_call[2];
 s32  Current_f;
u32 timer1=0;
u32 timer2=0;
u32 timer3=0;
u32 timer4=0;
u32 timer5=0;
u32 timer6=0;
s32 gCurrentOperation=0;
u8  BAT_STATE_FALG[4]={0};
u8  BAT_STATE_Curren = 0;
BAT_Working_enum BAT_Working = Shut_down;
u16 gBalance_pc;
uint16_t CCITT_CRC16=0;

u16 u16_CalculateAverage(u16 *p,u8 len);
s8 s8_CalculateAverage(s8 *p,u8 len);
void u16_FindMaxAndMinValue(u16* p,u8 *pmax,u8 *pmin,u8 len);
void u8_FindMaxAndMinValue(s8* p,u8 *pmax,u8 *pmin,u8 len);
void Board_int(void);  //ϵͳ��ʼ��Ĭ�ϲ���
void BAT_AlmPrt_Processing(void);

//=============================================================================
//��������: void CCITT_CRC16Init(uint8_t const * bytes, uint16_t len) 
//�������: *bytes ����ָ�룬len���ݳ���
//�������: ��
//����ֵ  : ��
//��������: ��CRCֵ
//ע������:
//=============================================================================		
			void CCITT_CRC16Init(uint8_t const * bytes, uint16_t len)  	        
			{
				uint16_t j;
				uint8_t i=0;
					CCITT_CRC16 =CRC_CCITT_INIT;                                		    
				while (len--)	
				{
					CCITT_CRC16 ^= ((uint16_t)bytes[i++] << 8);
   				for (j = 0; j < 8; j++)
					{
					CCITT_CRC16=(CCITT_CRC16 & 0x8000U)?((CCITT_CRC16 << 1) ^ CRC_CCITT_POLY):(CCITT_CRC16  << 1);
					}
					
				}
				
		 }
//=============================================================================
//��������: Boying_CheckCRC16(uint8_t const * bytes, uint32_t len) 
//�������: *bytes ����ָ�룬len���ݳ���
//�������: ��
//����ֵ  : ��
//��������: ��CRCֵ
//ע������:
//=============================================================================				
uint16_t Boying_CheckCRC16(uint8_t const * bytes, uint32_t len) 
{
	uint16_t crc16 = 0xa635;
	
	const uint16_t table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
	};

	while (len--)
	{
			crc16 = table [(crc16 >> 8 ^ *bytes++) & 0xff] ^ (crc16 << 8);
	}

	return crc16;
}

/***************************************************************************
*   �� �� ��: u8 u16_CalculateAverage(u16 *p,u8 len)
*   ����˵��: 	���ƽ����ѹ
*   ��    �Σ�*p ����ָ���ַ   lenָ�볤��
*   �� �� ֵ: ����ƽ����
***************************************************************************/
u16 u16_CalculateAverage(u16 *p,u8 len)
{
	u8 i = 0;
	u8 ka = 0;
	u32  sum  = 0;

	if((len <= 0) || p == (void *)0)
	{
		return 0;
	}

	for(i = 0;i < len;i++)
	{
		if(p[i] !=0xffff) //ȥ����Чֵ
		sum += p[i];
		else
			ka++;
	}
	
	sum = sum / (len-ka);
	return (u16)(sum);
	
}

/***************************************************************************
*   �� �� ��: s8 s8_CalculateAverage(s8 *p,u8 len)
*   ����˵��: 	���ƽ���¶�
*   ��    �Σ�*p ����ָ���ַ   lenָ�볤��
*   �� �� ֵ: ����ƽ����
***************************************************************************/
s8 s8_CalculateAverage(s8 *p,u8 len)
{
	u8  i = 0;
	s32  sum  = 0;
	u8 kk_l =0 ;

	if((len <= 0) || p == (void *)0)
	{
		return 0;
	}

	for(i = 0;i < len;i++)
	{
			if( p[i] != -40 )
			{
		sum += p[i];
			}
			else
			kk_l++;	
	}	
	sum = sum / (len-kk_l);
	
	return (s8)(sum);	
}

/***************************************************************************
*   �� �� ��: u16_FindMaxAndMinValue(u16* p,u8 *pmax,u8 *pmin,u8 len)
*   ����˵��: 	��������С���ݵ��±�
*   ��    �Σ�   *p ����ָ���ַ   *pmax ����ָ���ַ   *pmin  lenָ�볤��
*   �� �� ֵ: ��
***************************************************************************/
void u16_FindMaxAndMinValue(u16* p,u8 *pmax,u8 *pmin,u8 len)
{
				u8 maxIndex = 0;
				u8 minIndex = 0;
				u8 i = 0;

				if((len <= 0) || p == (void *)0)
				{
					return;
				}

				for(i = 0;i < len;i++)
				{
					if( p[i] !=0xffff)    //  ȥ����Чֵ
					{
								if(p[maxIndex] < p[i])
								{
									maxIndex = i;
								}
								if(p[minIndex] > p[i])
								{
									minIndex = i;
								}
			  	}
				}

				*pmax = maxIndex;
				*pmin = minIndex;
	
}

/***************************************************************************
*   �� �� ��: void u8_FindMaxAndMinValue(s8* p,u8 *pmax,u8 *pmin,u8 len)
*   ����˵��: 	��������С���ݵ��±�
*   ��    �Σ�*p ����ָ���ַ   *pmax ����ָ���ַ   *pmin  lenָ�볤��
*   �� �� ֵ: ��
***************************************************************************/
void u8_FindMaxAndMinValue(s8* p,u8 *pmax,u8 *pmin,u8 len)
{
	u8 maxIndex = 0;
	u8 minIndex = 0;
	u8 i = 0;

	if((len <= 0) || p == (void *)0)
	{
		return;
	}

	for(i = 0;i < len;i++)
	{
		if( p[i] != -40 )
		{
				if(p[maxIndex] < p[i])
				{
					maxIndex = i;
				}

				if(p[minIndex] > p[i])
				{
					minIndex = i;
				}
	  }
	}

	*pmax = maxIndex;
	*pmin = minIndex;
	
}
/***************************************************************************
*   �� �� ��: BMS_config_Write
*   ����˵��: BMS���ò���д��  ��CRC д������ַ
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void BMS_config_Write(void)
{
	u16 config_num = 0;

	config_num = ((sizeof(BMS_typedef)) - (sizeof(Read_typedef)));
	CCITT_CRC16Init((u8 *)(&BMS_config.Pack_RW_config),(config_num-2));
	BMS_config.CRC16= CCITT_CRC16;	
	FLASH_Write(FALSH_BMS_config_ADDR,&BMS_config.Pack_RW_config.gSetPackCapacity,config_num/2);
	delay_ms(10);
	FLASH_Write(FALSH_BMS_config_old_ADDR,&BMS_config.Pack_RW_config.gSetPackCapacity,config_num/2);

}
/***************************************************************************
*   �� �� ��: void BMS_Calibration_Write(void)
*   ����˵��: У׼����д��  ��CRC д������ַ
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void BMS_Calibration_Write(void)
{	
	u16 num = 0; 	
	CCITT_CRC16Init((u8 *)(&Calibration.gDchgCurrent_double),(sizeof(Calibration)-2));
	Calibration.CRC16= CCITT_CRC16;	
	num = sizeof(Calibration)/2; 
	FLASH_Write(FALSH_BMS_Calibration_ADDR,(u16 *)(&Calibration),num);
	delay_ms(10);
	FLASH_Write(FALSH_BMS_Calibration_old_ADDR,(u16 *)(&Calibration),num);
	//W25QXX_Write((u8 *)(&Calibration),60,sizeof(Calibration));	
}
/***************************************************************************
*   �� �� ��: BMS_Calibration_Read_Write(void)
*   ����˵��: У׼������ȡ
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void BMS_Calibration_Read(void)
{
u16 num = 0; 
	 num = sizeof(Calibration)/2;
   STMFLASH_Read(FALSH_BMS_Calibration_ADDR,(u16 *)(&Calibration),num);
	CCITT_CRC16Init((u8 *)(&Calibration.gDchgCurrent_double),(sizeof(Calibration)-2));
   if(Calibration.CRC16 != CCITT_CRC16)
	 {
		 delay_ms(10);
     STMFLASH_Read(FALSH_BMS_Calibration_old_ADDR,(u16 *)(&Calibration),num);
		 CCITT_CRC16Init((u8 *)(&Calibration.gDchgCurrent_double),(sizeof(Calibration)-2));
		 if(Calibration.CRC16 != CCITT_CRC16)
		 {
			gBitStorageFailure = 1; 
		 }
		 else
		 {
			FLASH_Write(FALSH_BMS_Calibration_ADDR,(u16 *)(&Calibration),num);			 
		 }
		 
	 }		 
}
/***************************************************************************
*   �� �� ��: void BMS_Alm_Number_Write(void)
*   ����˵��: ���ϴ�����¼ ��ѹ����ͳ�� Ƿѹ����ͳ�� ѹ�����H ����ͳ��   ѹ�����L ����ͳ��  
����������ͳ�� �ŵ��������ͳ�� �����������ͳ��
*   ��    �Σ���
*   �� �� ֵ: ��    
***************************************************************************/
void BMS_Alm_Number_Write(void)
{	
	static uBits bits ;
	static u16 num = sizeof(Alm_Number)/2;
	if((gBitOverVoltAlm|gBitOverVoltAlmPrt)&&(bits.Bits.B0==0))
	{
		Alm_Number.OverVoltNumber++;   //��ѹ����
		bits.Bits.B0= 1;
	}
	else if((bits.Bits.B0)&&(!(gBitOverVoltAlm|gBitOverVoltAlmPrt)))
	{
		bits.Bits.B0= 0;
	}
	

	if((gBitUnderVoltAlm|gBitUnderVoltAlmPrt)&&(bits.Bits.B1==0))
	{
		Alm_Number.UnderVoltNumber++;     //Ƿѹ����
		bits.Bits.B1= 1;			
	}
	else if((bits.Bits.B1)&&(!(gBitUnderVoltAlm|gBitUnderVoltAlmPrt)))
		bits.Bits.B1= 0;	
	
	if((gBitVoltDiff1MaxAlm|gBitVoltDiff1MaxAlmPrt)&&(bits.Bits.B2== 0	))
	{
		Alm_Number.VoltDiffMaxNumber++;        //ѹ���1����
		bits.Bits.B2= 1;			
	}
	else if((bits.Bits.B2)&&(!(gBitVoltDiff1MaxAlm|gBitVoltDiff1MaxAlmPrt)))
		bits.Bits.B2=  0;
	

	if((gBitVoltDiff2MaxAlm|gBitVoltDiff2MaxAlmPrt)&&(bits.Bits.B3== 0	))
	{
		Alm_Number.VoltDiffMax2Number++;       //ѹ���2����
		bits.Bits.B3= 1;
	}
	else if((bits.Bits.B3)&&(!(gBitVoltDiff2MaxAlm|gBitVoltDiff2MaxAlmPrt)))
		bits.Bits.B3 = 0;
	

	if((gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt|gBitChgShortCircuit)&&(bits.Bits.B4== 0	))	
	{			
		Alm_Number.ChgOverCurrenNumber++;              //����������
		bits.Bits.B4= 1;
	}
	else if((bits.Bits.B4)&&(!(gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt|gBitChgShortCircuit)))
		bits.Bits.B4 = 0;
	

	if((gBitDchgOverCurrentAlm|gBitDchgOverCurrentAlmPrt|gBitDchgShortCircuit)	&&(bits.Bits.B5== 0	))	
	{
		Alm_Number.DchgOverCurrenNumber++;        //�ŵ��������
		bits.Bits.B5= 1;
	}
	else if((bits.Bits.B5)&&(!(gBitDchgOverCurrentAlm|gBitDchgOverCurrentAlmPrt|gBitDchgShortCircuit)))
		bits.Bits.B5 = 0;
	
	if((gBit_Chg_Error)&&(bits.Bits.B6== 0))
	{
		Alm_Number.InformalChgMosNumber++;        //������ĳ�����
		bits.Bits.B6= 1;
	}
	else if((bits.Bits.B6)&&(!(gBitDchgOverCurrentAlm|gBitDchgOverCurrentAlmPrt|gBitDchgShortCircuit)))
		bits.Bits.B6 = 0;
	
	//Alm_Number.ChgHighTemNumber++;              		//�����´���
	//Alm_Number.ChgLowTemNumber++;           			//�����´���
	//Alm_Number.DchgHighTemNumber++;        			//�ŵ���´���
	//Alm_Number.DchgLowTemNumber++;                	//�ŵ���´���
	//Alm_Number.ChgMosHighTemNumber++;                	//���MOS���´���
	//Alm_Number.ChgMosLowTemNumber++;                	//���MOS���´���
	//Alm_Number.DchgMosHighNumber++;                	//�ŵ�MOS���´���
	//Alm_Number.DchgMosLowNumber++;                	//�ŵ�MOS���� ����
	//Alm_Number.InformalChgMosNumber;              	//������ĳ�����
	///W25QXX_Write((u8 *)(&Alm_Number),20,sizeof(Alm_Number));	
	Alm_Number.Failure = gBit_BatHighTemFailure|(gBit_BatlowTemFailure<<1)|(gBit_BatOverVoltFailure<<2)|(gBit_BatUnderVoltFailure<<3)|(gBit_BatVoltDiffMaxFailure<<4);

	CCITT_CRC16Init((u8 *)(&Alm_Number.OverVoltNumber),(sizeof(Alm_Number)-2));
	Alm_Number.CRC16= CCITT_CRC16;		 
	FLASH_Write(FALSH_BMS_Alm_Number_ADDR,&Alm_Number.OverVoltNumber,num);	
	delay_ms(10);		 
	FLASH_Write(FALSH_BMS_Alm_Number_old_ADDR,&Alm_Number.OverVoltNumber,num);	
		 
}
/***************************************************************************
*   �� �� ��: void BMS_Alm_Number_Write(void)
*   ����˵��: ���ϴ�����ȡ
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void BMS_Alm_Number_Read(void)
{
	static u16 num = sizeof(Alm_Number)/2;	
	STMFLASH_Read(FALSH_BMS_Alm_Number_ADDR,&Alm_Number.OverVoltNumber,num);	
	CCITT_CRC16Init((u8 *)(&Alm_Number.OverVoltNumber),(sizeof(Alm_Number)-2));

	if(Alm_Number.CRC16 != CCITT_CRC16)
	{
		delay_ms(10);
		STMFLASH_Read(FALSH_BMS_Calibration_old_ADDR,(u16 *)(&Calibration),num);
		CCITT_CRC16Init((u8 *)(&Alm_Number.OverVoltNumber),(sizeof(Alm_Number)-2));
		if(Alm_Number.CRC16 != CCITT_CRC16)
		{
			gBitStorageFailure = 1; 
		}
		else
		{
			FLASH_Write(FALSH_BMS_Alm_Number_ADDR,&Alm_Number.OverVoltNumber,num);	 			 
		}

	}		 
	gBit_BatHighTemFailure =((u8)Alm_Number.Failure&0x01);
	g_FaultAlarmState[4] = ((u8)(Alm_Number.Failure>>1)&0x1f);
	
}
/***************************************************************************
*   �� �� ��: Board_int
*   ����˵��: ��ʼ��ϵͳ����
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
							void Board_int(void)
							{  	
								 u8 i=0;
								 static u16 num = ((sizeof(BMS_typedef)) - (sizeof(Read_typedef)));	

								 STMFLASH_Read(FALSH_BMS_config_ADDR,&BMS_config.Pack_RW_config.gSetPackCapacity,num/2);	
								 CCITT_CRC16Init((u8 *)(&BMS_config.Pack_RW_config),(num-2));
								
								if(	BMS_config.CRC16 != CCITT_CRC16)//Ч�鲻��ȷ�˳�
								{		
									STMFLASH_Read(FALSH_BMS_config_old_ADDR,&BMS_config.Pack_RW_config.gSetPackCapacity,num/2);	
									CCITT_CRC16Init((u8 *)(&BMS_config.Pack_RW_config),(num-2));
											 if(	BMS_config.CRC16 == CCITT_CRC16)//Ч�鲻��ȷ�˳�
											 {
											  BMS_config_Write();      //���±�������
											 }		 
												else	
												{					
																				gBitStorageFailure = 1; 
																				BMS_config.Pack_RW_config.gBatNumber = 14;                 //��ش���
																			  for(i = 0; i < 100; i++)  //��һ�γ�ʼ������SOC
																				{
																				 BQ76940_TASK();
																				 BAT_Processing();	
																				 }
																				 BMS_capacity.SOC = openCircuitVoltageMethod(BMS_config.BMS_Read.gVoltMin);//��·��ѹSOC����
																				 if(BMS_capacity.SOC>0)
																				 {
																				 BMS_config.Pack_RW_config.gSOC = (u8)(BMS_capacity.SOC/10);        //��ذ�SOC ����
																				 BMS_capacity.BatCapacity =  Capacity_Max/100*BMS_config.Pack_RW_config.gSOC; //��ذ�������
																				 BMS_config.Pack_RW_config.gSurpulsPackCapacity = (u16)(BMS_capacity.BatCapacity/3600/100);  //ʣ������ 0.1AH
																				 }
																				 else
																				 {
																				 BMS_config.Pack_RW_config.gSOC = 0;        //��ذ�SOC ����
																				 BMS_capacity.BatCapacity =  0; //��ذ�������
																				 BMS_config.Pack_RW_config.gSurpulsPackCapacity = 0;  //ʣ������ 0.1AH
																				 }				 			 
																				///������������
																				 BMS_config.Pack_RW_config.gSetPackCapacity = 2100; //��ذ����������
																				 BMS_config.Pack_RW_config.gNewPackCapacity = 2100;  //������������
																				 BMS_config.Pack_RW_config.gCHGPackCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity/100;      //�����ذ�����
																				 BMS_config.Pack_RW_config.gSOH = 100;                   //��ذ�SOH
																				 BMS_config.Pack_RW_config.gsCycleCnt =0x0000;                 //ѭ������
																				 BMS_config.Pack_RW_config.gBatType = 0;                  //�������
																				 BMS_config.Pack_RW_config.gBatNumber = 14;                 //��ش���
																				 BMS_config.Pack_RW_config.gBatFront = 0;                    //��زɼ�ǰ�ˣ�BQ7940��BQ76930�ȣ�
																				 BMS_config.Pack_RW_config.gBatMode = 0;                    //�����𡢡���������������������������������������������
																				 BMS_config.Pack_RW_config.gBatPackShunt = 150;                //������ֵ
																				 BMS_config.Pack_RW_config.gVoltDiff1 = 3700;
																				 BMS_config.Pack_RW_config.gBatBalanceVoltStart = 3800;       //��ؾ��⿪����ѹ
																				 BMS_config.Pack_RW_config.gBatBalanceVoltEnd = 3700;          //��ؾ���رյ�ѹ
																				 BMS_config.Pack_RW_config.gBatBalanceDiffVoltState=15;        ///���⿪��ѹ��
																				 BMS_config.Pack_RW_config.gBatBalanceDiffVoltEnd=5;          ///����ر�ѹ��
																				 BMS_config.Pack_RW_config.gBatBalanceStateTime = 30;                //���⿪��ʱ��
																				 BMS_config.Pack_RW_config.gForcedShutdown = 2500;              //ǿ�ƹػ���ѹ
																				 BMS_config.Pack_RW_config.gStaticSleepTimer = 250;              //��������ʱ��
																				 BMS_config.Pack_RW_config.gStaticResumeTimer = 5;              //���û���ʱ��
																				 
																				 BMS_config.Pack_RW_config.gTemAcquisitionEnable = 1;             //�¶ȼ��ʹ��
																				 BMS_config.Pack_RW_config.gTemNumber = 2;                         //�¶Ȳɼ�����
																				 BMS_config.Pack_RW_config.gTemType = 0;                            //�¶ȴ���������
																				 BMS_config.Pack_RW_config.gSoclow = 30;                            //SOC���͸澯��ֵ
																				 BMS_config.Pack_RW_config.gChgEndVolt = 588;                      //����ֹ�ܵ�ѹ
																				 BMS_config.Pack_RW_config.gDchgEndVolt = 518;                     //�ŵ��ֹ�ܵ�ѹ
																				 BMS_config.Pack_RW_config.gChgShortCircuit = 700;                 //����·����ֵ
																				 BMS_config.Pack_RW_config.gDchgShortCircuit = -8000;                 //�ŵ��·����ֵ
																				 BMS_config.Pack_RW_config.gChgDiffhTemMax = 30;                      //����²����
																				 BMS_config.Pack_RW_config.gDchgDiffhTemMax = 30;                      //�ŵ��²����
																				 BMS_config.Pack_RW_config.gSelfDchgStartTime = 40;                     //�Էŵ翪��ʱ��
																				 BMS_config.Pack_RW_config.gSelfDchgStartVolt = 3900;                    //�Էŵ翪����ѹ
																				 BMS_config.Pack_RW_config.gSelfDchgEndVolt = 3700;                       //�Էŵ�رյ�ѹ
																				 BMS_config.Pack_RW_config.gBatHighTemFailure = 70;                      //��ظ���ʧЧ
																				 BMS_config.Pack_RW_config.gBatLowTemFailure =  -35;                      //��ص���ʧЧ
																				 BMS_config.Pack_RW_config.gBatOverVoltFailure =  4500;                  //��ع�ѹʧЧ
																				 BMS_config.Pack_RW_config.gBatUnderVoltFailure =  2500;                  //���ǷѹʧЧ
																				 BMS_config.Pack_RW_config.gBatVoltDiffMaxFailure =  500;                  //���ѹ�����ʧЧ		
																		 //��ѹ�ı�������
																				 BMS_config.OverVolt.gVoltAlm = 4200;       //��ع�ѹ�澯
																				 BMS_config.OverVolt.gVoltAlmReset = 4150;  //��ع�ѹ�澯�ָ�ֵ
																				 BMS_config.OverVolt.gVoltAlmPrt = 4250;    //��ع�ѹ����ֵ
																				 BMS_config.OverVolt.gVoltAlmPrtReset = 4200;  //��ع�ѹ�����ָ�ֵ
																				 BMS_config.OverVolt.gVoltAlmPrtTimer = 3;      //��ع�ѹ�����ָ�ʱ��
																				 BMS_config.OverVolt.gVoltAlmPrtNumber = 5;      //��ع�ѹ�����ָ�����
																			//Ƿѹ�ı�������
																				 BMS_config.UnderVolt.gVoltAlm = 3900;       //���Ƿѹ�澯
																				 BMS_config.UnderVolt.gVoltAlmReset = 4000;  //���Ƿѹ�澯�ָ�ֵ
																				 BMS_config.UnderVolt.gVoltAlmPrt = 3700;    //���Ƿѹ����ֵ
																				 BMS_config.UnderVolt.gVoltAlmPrtReset = 3900;  //���Ƿѹ�����ָ�ֵ
																				 BMS_config.UnderVolt.gVoltAlmPrtTimer = 3;      //���Ƿѹ�����ָ�ʱ��
																				 BMS_config.UnderVolt.gVoltAlmPrtNumber = 5;      //���Ƿѹ�����ָ�����		 
																			//ѹ�����ı�������
																				 BMS_config.VoltDiffMax.gVoltAlm = 200;       //���ѹ�����澯
																				 BMS_config.VoltDiffMax.gVoltAlmReset = 150;  //���ѹ�����澯�ָ�ֵ
																				 BMS_config.VoltDiffMax.gVoltAlmPrt = 300;    //���ѹ����󱣻�ֵ
																				 BMS_config.VoltDiffMax.gVoltAlmPrtReset = 250;  //���ѹ����󱣻��ָ�ֵ
																				 BMS_config.VoltDiffMax.gVoltAlmPrtTimer = 3;      //���ѹ����󱣻��ָ�ʱ��
																				 BMS_config.VoltDiffMax.gVoltAlmPrtNumber = 5;      //���ѹ����󱣻��ָ�����	
																				//ѹ�����ı�������
																				 BMS_config.VoltDiffMax1.gVoltAlm = 200;       //���ѹ�����澯
																				 BMS_config.VoltDiffMax1.gVoltAlmReset = 150;  //���ѹ�����澯�ָ�ֵ
																				 BMS_config.VoltDiffMax1.gVoltAlmPrt = 300;    //���ѹ����󱣻�ֵ
																				 BMS_config.VoltDiffMax1.gVoltAlmPrtReset = 250;  //���ѹ����󱣻��ָ�ֵ
																				 BMS_config.VoltDiffMax1.gVoltAlmPrtTimer = 3;      //���ѹ����󱣻��ָ�ʱ��
																				 BMS_config.VoltDiffMax1.gVoltAlmPrtNumber = 5;      //���ѹ����󱣻��ָ�����		 
																				 //�������ı�������
																				 BMS_config.ChgOverCurren.gCurrentAlm = 1600;       //�������澯
																				 BMS_config.ChgOverCurren.gCurrentAlmReset = 1500;    //�������澯�ָ�
																				 BMS_config.ChgOverCurren.gCurrentAlmPrt = 2000;      //�������澯����
																				 BMS_config.ChgOverCurren.gCurrentAlmPrtReset = 1600;    //�������澯�����ָ�
																				 BMS_config.ChgOverCurren.gCurrentAlmPrtTimer = 3;         //�������澯����ʱ��
																				 BMS_config.ChgOverCurren.gCurrentAlmPrtNumber = 5;         //�������澯��������
																				 //�ŵ�����ı�������
																				 BMS_config.DchgOverCurren.gCurrentAlm = -1600;       //�ŵ�����澯
																				 BMS_config.DchgOverCurren.gCurrentAlmReset = -1500;    //�ŵ�����澯�ָ�
																				 BMS_config.DchgOverCurren.gCurrentAlmPrt = -2000;      //�ŵ�����澯����
																				 BMS_config.DchgOverCurren.gCurrentAlmPrtReset = -1600;    //�ŵ�����澯�����ָ�
																				 BMS_config.DchgOverCurren.gCurrentAlmPrtTimer = 3;         //�ŵ�����澯����ʱ��
																				 BMS_config.DchgOverCurren.gCurrentAlmPrtNumber = 5;         //�ŵ�������������� 
																				 //�����µı�������
																				 BMS_config.ChgHighTem.gTemAlm = 50;             //�����¸澯
																				 BMS_config.ChgHighTem.gTemAlmReset = 40;        //�����¸澯�ָ�
																				 BMS_config.ChgHighTem.gTemAlmPrt = 55;          //�����±���
																				 BMS_config.ChgHighTem.gTemAlmPrtReset = 50;     //�����±����ָ�
																				 BMS_config.ChgHighTem.gTemAlmPrtTimer = 3;       //�����±���ʱ��
																				 BMS_config.ChgHighTem.gTemAlmPrtNumber = 5;       ////�����±�������
																				 //�����µı�������
																				 BMS_config.ChgLowTem.gTemAlm = 0;             //�����¸澯
																				 BMS_config.ChgLowTem.gTemAlmReset = 5;        //�����¸澯�ָ�
																				 BMS_config.ChgLowTem.gTemAlmPrt = -5;          //�����±���
																				 BMS_config.ChgLowTem.gTemAlmPrtReset = 0;     //�����±����ָ�
																				 BMS_config.ChgLowTem.gTemAlmPrtTimer = 3;       //�����±���ʱ��
																				 BMS_config.ChgLowTem.gTemAlmPrtNumber = 5;       ////�����±�������			 
																					 //�ŵ���µı�������
																				 BMS_config.DchgHighTem.gTemAlm = 50;             //�ŵ���¸澯
																				 BMS_config.DchgHighTem.gTemAlmReset = 40;        //�ŵ���¸澯�ָ�
																				 BMS_config.DchgHighTem.gTemAlmPrt = 55;          //�ŵ���±���
																				 BMS_config.DchgHighTem.gTemAlmPrtReset = 50;     //�ŵ���±����ָ�
																				 BMS_config.DchgHighTem.gTemAlmPrtTimer = 3;       //�ŵ���±���ʱ��
																				 BMS_config.DchgHighTem.gTemAlmPrtNumber = 5;       ////�ŵ���±�������
																				 //�ŵ���µı�������
																				 BMS_config.DchgLowTem.gTemAlm = 0;             //�ŵ���¸澯
																				 BMS_config.DchgLowTem.gTemAlmReset = 5;        //�ŵ���¸澯�ָ�
																				 BMS_config.DchgLowTem.gTemAlmPrt = -5;          //�ŵ���±���
																				 BMS_config.DchgLowTem.gTemAlmPrtReset = 0;     //�ŵ���±����ָ�
																				 BMS_config.DchgLowTem.gTemAlmPrtTimer = 3;       //�ŵ���±���ʱ��
																				 BMS_config.DchgLowTem.gTemAlmPrtNumber = 5;       ////�ŵ���±�������	
																				 //���MOS���µı�������
																				 BMS_config.ChgMosHighTem.gTemAlm = 50;             //���MOS���¸澯
																				 BMS_config.ChgMosHighTem.gTemAlmReset = 40;        //���MOS���»ָ�
																				 BMS_config.ChgMosHighTem.gTemAlmPrt = 55;          //���MOS���±���
																				 BMS_config.ChgMosHighTem.gTemAlmPrtReset = 50;     //���MOS���±����ָ�
																				 BMS_config.ChgMosHighTem.gTemAlmPrtTimer = 3;       //���MOS���±���ʱ��
																				 BMS_config.ChgMosHighTem.gTemAlmPrtNumber = 5;       ////���MOS���±�������
																					//���MOS���µı�������
																				 BMS_config.ChgMosLowTem.gTemAlm = 0;             //���MOS���¸澯
																				 BMS_config.ChgMosLowTem.gTemAlmReset = 5;        //���MOS���¸澯�ָ�
																				 BMS_config.ChgMosLowTem.gTemAlmPrt = -5;          //���MOS���±���
																				 BMS_config.ChgMosLowTem.gTemAlmPrtReset = 0;     //���MOS���±����ָ�
																				 BMS_config.ChgMosLowTem.gTemAlmPrtTimer = 3;       //���MOS���±���ʱ��
																				 BMS_config.ChgMosLowTem.gTemAlmPrtNumber = 5;       ////���MOS���±�������		 
																				 //�ŵ�MOS�ĸ��±�������
																				 BMS_config.DchgMosHighTem.gTemAlm = 50;             //�ŵ�MOS�ĸ��¸澯
																				 BMS_config.DchgMosHighTem.gTemAlmReset = 40;        //�ŵ�MOS�ĸ��¸澯�ָ�
																				 BMS_config.DchgMosHighTem.gTemAlmPrt = 55;          //�ŵ�MOS�ĸ��±���
																				 BMS_config.DchgMosHighTem.gTemAlmPrtReset = 50;     //�ŵ�MOS�ĸ��»ָ�
																				 BMS_config.DchgMosHighTem.gTemAlmPrtTimer = 3;       //�ŵ�MOS�ĸ��±���ʱ��
																				 BMS_config.DchgMosHighTem.gTemAlmPrtNumber = 5;       ////�ŵ�MOS�ĸ��±�������				 
																				 //�ŵ�MOS�ĵ��±�������
																				 BMS_config.DchgMosLowTem.gTemAlm = 0;             //�ŵ�MOS�ĵ��¸澯
																				 BMS_config.DchgMosLowTem.gTemAlmReset = 5;        //�ŵ�MOS�ĵ��¸澯�ָ�
																				 BMS_config.DchgMosLowTem.gTemAlmPrt = -5;          //�ŵ�MOS�ĵ��±���
																				 BMS_config.DchgMosLowTem.gTemAlmPrtReset = 0;     //�ŵ�MOS�ĵ��±����ָ�
																				 BMS_config.DchgMosLowTem.gTemAlmPrtTimer = 3;       //�ŵ�MOS�ĵ��±���ʱ��
																				 BMS_config.DchgMosLowTem.gTemAlmPrtNumber = 5;       ////�ŵ�MOS�ĵ��±�������			 		
																				 memset(&BMS_config.CustomerCode,0x55,sizeof(CustomerCode_typedef));	//�ͻ�����ȫΪ0;		
																				 BMS_config.CustomerCode.DChg_tim = 1;	
																				 BMS_config.CustomerCode.Chg_tim = 5;		
																				 BMS_config.CustomerCode.Free_tim = 3600;												 
																				memset(&Alm_Number,0,sizeof(Alm_Number));//��ʼ�����ϼ�¼����			
														}
								 }

										memset(&BMS_config.BMS_Read,0,sizeof(Read_typedef));
										memset(&BMSVoltTemperature,0,sizeof(BMSVoltTemperature_call_typedef));
										memcpy(&BMS_config.Product_Version,&g_Product_Version,sizeof(g_Product_Version));  //Ӳ���汾��
										memcpy(&BMS_config.SoftWare_Version,&g_BMS_Version,sizeof(g_BMS_Version));  //����汾��	
										BMS_config.BMS_Read.gCurrentDCHGMax = 1800;
										BMS_config.BMS_Read.gCurrentCHGMax = 440;
										BMS_Alm_Number_Read();
										//tEvtDataW.index = tAlmPrtEvtHead_W.EventNum ;
										if(Alm_Number.CRC16 == 0xffff)
										memset(&Alm_Number,0,sizeof(Alm_Number));
										BMS_Calibration_Read();  
										
										for(i = 0;i < 14;i++)
										{
												if(Calibration.g_Volt_double_max[i]< 0 )
												Calibration.g_Volt_double_max[i] = 1.0f;
										}	
										for(i = 0;i < 14;i++)
										{
											if(Calibration.g_Volt_double_min[i] < 0)
											Calibration.g_Volt_double_min[i] = 1.0f;	
										}	
										if(Calibration.gChgCurrent_double<0)
										Calibration.gChgCurrent_double	= 1.0f;	
										if(Calibration.gDchgCurrent_double<0)	
										Calibration.gDchgCurrent_double	= 1.0f;	
										if(Calibration.gTem_double[0]<0)	
										Calibration.gTem_double[0] =1.0f;
										if(Calibration.gTem_double[1]<0)	
										Calibration.gTem_double[1] =1.0f;
								
										BMS_capacity.BatCapacity = (u32)(((double)BMS_config.Pack_RW_config.gNewPackCapacity)*3600* BMS_config.Pack_RW_config.gSOC);
										BMS_capacity.PackNewCapacity =(u32)(BMS_config.Pack_RW_config.gNewPackCapacity)*360000;

										gBit_BatHighTemFailure =Alm_Number.Failure&(0x01);          //��ظ���ʧЧ
										gBit_BatlowTemFailure	=(Alm_Number.Failure>>1)&(0x01);   								//��ص���ʧЧ 
										gBit_BatOverVoltFailure =(Alm_Number.Failure>>2)&(0x01);    							//��ص�ع�ѹ��������ʧЧ
										gBit_BatUnderVoltFailure	=(Alm_Number.Failure>>3)&(0x01);						//��ص��Ƿѹ��������ʧЧ
										gBit_BatVoltDiffMaxFailure	=(Alm_Number.Failure>>4)&(0x01);						//���ѹ�������ʧЧ
										tAlmPrtEvtHead_W.WriteEventAddr= Alm_Number.WriteEventAddr ;
										
										if(((tAlmPrtEvtHead_W.WriteEventAddr%64) !=0)||(tAlmPrtEvtHead_W.WriteEventAddr>=(EVENT_MAX_ADDR)))
										{
										tAlmPrtEvtHead_W.WriteEventAddr= 0;
										}
										tAlmPrtEvtHead_W.EventNum =  Alm_Number.EventNum ;
										tEvtDataW.index = tAlmPrtEvtHead_W.EventNum ;
										memset(&Extremum,0,sizeof(Extremum));
										STMFLASH_Read(FALSH_BMS_Extremum_ADDR,&Extremum.MAX__Extremum_Chg_Volt.value[0][0],(sizeof(Extremum)/2));
										delay_ms(10);
										STMFLASH_Read(FALSH_BMS_Extremum_old_ADDR,&Extremum.MAX__Extremum_Chg_Volt.value[0][0],(sizeof(Extremum)/2));		
          	}		
/***************************************************************************
*   �� �� ��: u8	u8	BAT_gVoltAlmPrt( BatVolt_typedef* Volt, u16 BatVolt,u8 timer)
*   ����˵��: ��ѹ�澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/
	u8	BAT_gVoltAlmPrt( BatVolt_typedef* Volt, u16 BatVolt)
	{
  static u8 timer_Alm = 0;
	static	AlmPrtx_enum X = NO_AlmPrt;		
		switch(X)
		{
			case NO_AlmPrt:
					if(Volt->gVoltAlm < BatVolt)//�ﵽ�澯��ѹ
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1									
								timer_Alm=0; //����������
							} 
						}
							else
						 timer_Alm = 0;			
			       break;			
			case Alm_OUT:
				   	if(Volt->gVoltAlmReset > BatVolt)   //�澯��ѹ�ָ�����
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //��ѹ�澯��־λ��1		
							   timer_Alm=0; //����������
							} 
						}
						
						if(Volt->gVoltAlmPrt < BatVolt)       //�ﵽ������ѹ
						{
						  timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //��ѹ�澯��־λ��1			
								timer_Alm=0; //����������
							} 	
						}	
			       break;
      case AlmPrt_OUT:

					if(Volt->gVoltAlmPrtReset > BatVolt)   //������ѹ�ָ�
					{
						 timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;			 
			       break;
			default:
				     X=NO_AlmPrt;      //��ѹ�澯��־λ��0
             break;
		}		
		return X;		
	}
/***************************************************************************
*   �� �� ��: u8	BAT_gUVoltAlmPrt( BatVolt_typedef* Volt, u16 BatVolt)
*   ����˵��: Ƿѹ��ѹ�澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/	
	u8	BAT_gUVoltAlmPrt( BatVolt_typedef* Volt, u16 BatVolt)
	{
  static u8 timer_Alm = 0;
	static	AlmPrtx_enum X = NO_AlmPrt;		
		switch(X)
		{
			case NO_AlmPrt:
					if(Volt->gVoltAlm >BatVolt)//�ﵽ�澯��ѹ
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1									
								timer_Alm=0; //����������
							} 
						}
							else
						 timer_Alm = 0;			
			       break;			
			case Alm_OUT:
				   	if(Volt->gVoltAlmReset < BatVolt)   //�澯��ѹ�ָ�����
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //��ѹ�澯��־λ��1		
							   timer_Alm=0; //����������
							} 
						}
						
						if(Volt->gVoltAlmPrt >BatVolt)       //�ﵽ������ѹ
						{
						  timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //��ѹ�澯��־λ��1			
								timer_Alm=0; //����������
							} 	
						}	
			       break;
      case AlmPrt_OUT:

					if(Volt->gVoltAlmPrtReset < BatVolt)   //������ѹ�ָ�
					{
						 timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;			 
			       break;
			default:
				     X=NO_AlmPrt;      //��ѹ�澯��־λ��0
                      break;
		}		
		return X;	
	}
/***************************************************************************
*   �� �� ��: u8	BAT_gVoltDiffAlmPrt( BatVolt_typedef* Volt, u16 BatVolt)
*   ����˵��: ѹ����ѹ�澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/	
u8	BAT_gVoltDiffAlmPrt( BatVolt_typedef* Volt, u16 BatVolt)
	{
   static u8 timer_Alm = 0;
   static	AlmPrtx_enum X = NO_AlmPrt;		
		switch(X)
		{
			case NO_AlmPrt:
					if(Volt->gVoltAlm < BatVolt)//�ﵽ�澯��ѹ
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1									
								timer_Alm=0; //����������
							} 
						}
							else
						 timer_Alm = 0;			
			       break;			
			case Alm_OUT:
				   	if(Volt->gVoltAlmReset > BatVolt)   //�澯��ѹ�ָ�����
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //��ѹ�澯��־λ��1		
							   timer_Alm=0; //����������
							} 
						}
						
						if(Volt->gVoltAlmPrt < BatVolt)       //�ﵽ������ѹ
						{
						  timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //��ѹ�澯��־λ��1			
								timer_Alm=0; //����������
							} 	
						}	
			       break;
      case AlmPrt_OUT:

					if(Volt->gVoltAlmPrtReset > BatVolt)   //������ѹ�ָ�
					{
						 timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;			 
			       break;
			default:
				     X=NO_AlmPrt;      //��ѹ�澯��־λ��0
                      break;
		}		
		return X;	
	}
/***************************************************************************
*   �� �� ��: u8	BAT_gVoltDiffAlmPrt1( BatVolt_typedef* Volt, u16 BatVolt)
*   ����˵��: ѹ����ѹ�澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/	
u8	BAT_gVoltDiffAlmPrt1( BatVolt_typedef* Volt, u16 BatVolt)
	{
  static u8 timer_Alm = 0;
	static	AlmPrtx_enum X = NO_AlmPrt;		
		switch(X)
		{
			case NO_AlmPrt:
					if(Volt->gVoltAlm < BatVolt)//�ﵽ�澯��ѹ
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1									
								timer_Alm=0; //����������
							} 
						}
							else
						 timer_Alm = 0;			
			       break;			
			case Alm_OUT:
				   	if(Volt->gVoltAlmReset > BatVolt)   //�澯��ѹ�ָ�����
						{
							timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //��ѹ�澯��־λ��1		
							   timer_Alm=0; //����������
							} 
						}
						
						if(Volt->gVoltAlmPrt < BatVolt)       //�ﵽ������ѹ
						{
						  timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //��ѹ�澯��־λ��1			
								timer_Alm=0; //����������
							} 	
						}	
			       break;
      case AlmPrt_OUT:

					if(Volt->gVoltAlmPrtReset > BatVolt)   //������ѹ�ָ�
					{
						 timer_Alm++;
							if(Volt->gVoltAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //��ѹ�澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;			 
			       break;
			default:
				     X=NO_AlmPrt;      //��ѹ�澯��־λ��0
                      break;
		}		
		return X;	
	}
	/***************************************************************************
*   �� �� ��: u8	BAT_gCurrentAlmPrt( BatCurrent_typedef* Current, u16 BatCurrent)
*   ����˵��: �����澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/
u8	BAT_gCurrentAlmPrt( BatCurrent_typedef* Current, s16 BatCurrent)  ///����������
	{
	static u8 timer_Alm = 0;
	static	AlmPrtx_enum X = NO_AlmPrt;
		
		switch(X)
		{
			case NO_AlmPrt:
					if(Current->gCurrentAlm <= BatCurrent)//�ﵽ�澯����
						{
							timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�����澯��־λ��1		
								timer_Alm=0; //����������
							} 
						}	
          else	
               timer_Alm = 0;						
			       break;
			
			case Alm_OUT:
				   	if(Current->gCurrentAlmReset >= BatCurrent)   //�澯�����ָ�����
						{
							timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //�����澯��־λ��0	
								timer_Alm=0; //����������
							} 
						}
						
						if(Current->gCurrentAlmPrt <= BatCurrent)       //�ﵽ��������
						{
						  timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //��ѹ�澯��־λ��2		
								timer_Alm=0; //����������
							} 	
						}	
			       break;

      case AlmPrt_OUT:
					if(Current->gCurrentAlmPrtReset >= BatCurrent)   //������ѹ����
					{
						 timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�����澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;
			 
			       break;
			default:			
			        X=NO_AlmPrt; //�����澯��־λ��0
                      break;
		}				
		return X;	
	}
	
	/***************************************************************************
*   �� �� ��: u8	BAT_gCurrentAlmPrt( BatCurrent_typedef* Current, u16 BatCurrent)
*   ����˵��: �����澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/
u8	BAT_gCurrentAlmPrt1( BatCurrent_typedef* Current, s16 BatCurrent)
	{
	static u8 timer_Alm = 0;
	static	AlmPrtx_enum X = NO_AlmPrt;
		
		switch(X)
		{
			case NO_AlmPrt:
					if(Current->gCurrentAlm >= BatCurrent)//�ﵽ�澯����
						{
							timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�����澯��־λ��1		
								timer_Alm=0; //����������
							} 
						}	
          else	
               timer_Alm = 0;						
			       break;
			
			case Alm_OUT:
				   	if(Current->gCurrentAlmReset <= BatCurrent)   //�澯�����ָ�����
						{
							timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //�����澯��־λ��0	
								timer_Alm=0; //����������
							} 
						}
						
						if(Current->gCurrentAlmPrt >= BatCurrent)       //�ﵽ��������
						{
						  timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //��ѹ�澯��־λ��2		
								timer_Alm=0; //����������
							} 	
						}	
			       break;

      case AlmPrt_OUT:
					if(Current->gCurrentAlmPrtReset <= BatCurrent)   //������ѹ����
					{
						 timer_Alm++;
							if(Current->gCurrentAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�����澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;
			 
			       break;
			default:			
			        X=NO_AlmPrt; //�����澯��־λ��0
                      break;
		}				
		return X;	
	}
		/***************************************************************************
*   �� �� ��: u8	BAT_gCurrentAlmPrt( BatCurrent_typedef* Current, u16 BatCurrent)
*   ����˵��: �����澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/
u8	BAT_gTemAlmPrt( BatTem_typedef* Tem, s8 BatTem)
	{
	static u8 timer_Alm = 0;
	static	AlmPrtx_enum X = NO_AlmPrt;
		switch(X)
		{
			case NO_AlmPrt:
					if(Tem->gTemAlm <= BatTem)//�ﵽ�澯�¶�
						{
							timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�¶ȸ澯��־λ��1		
								timer_Alm=0; //����������
							} 
						}	
          else	
               timer_Alm = 0;						
			       break;
			
			case Alm_OUT:
				   	if(Tem->gTemAlmReset >= BatTem)   //�澯�¶Ȼָ�����
						{
							timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //�¶ȸ澯��־λ��0	
								timer_Alm=0; //����������
							} 
						}
						
						if(Tem->gTemAlmPrt <= BatTem)       //�ﵽ�����¶�
						{
						  timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //�¶ȸ澯��־λ��2		
								timer_Alm=0; //����������
							} 	
						}	
			       break;

      case AlmPrt_OUT:
					if(Tem->gTemAlmPrtReset >= BatTem)   //�����¶�
					{
						 timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�¶ȸ澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;
			 
			       break;
			default:			
			        X=NO_AlmPrt; //�¶ȸ澯��־λ��0
                      break;
		}				
		return X;	
	}
	
/***************************************************************************
*   �� �� ��: u8	BAT_gCurrentAlm1Prt( BatCurrent_typedef* Current, u16 BatCurrent)
*   ����˵��: �����澯������־λ������
*   ��    �Σ���
*   �� �� ֵ: ���ظ澯�����ȼ�
***************************************************************************/
u8	BAT_gTemAlm1Prt( BatTem_typedef* Tem, s8 BatTem)
	{
	static u8 timer_Alm = 0;
	static	AlmPrtx_enum X = NO_AlmPrt;
		switch(X)
		{
			case NO_AlmPrt:
					if((Tem->gTemAlm )>= BatTem)//�ﵽ�澯�¶�
						{
							timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�¶ȸ澯��־λ��1		
								timer_Alm=0; //����������
							} 
						}	
          else	
               timer_Alm = 0;						
			       break;
			
			case Alm_OUT:
				   	if(Tem->gTemAlmReset <= BatTem)   //�澯�¶Ȼָ�����
						{
							timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=NO_AlmPrt; //�¶ȸ澯��־λ��0	
								timer_Alm=0; //����������
							} 
						}
						
						if(Tem->gTemAlmPrt >= BatTem)       //�ﵽ�����¶�
						{
						  timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=AlmPrt_OUT; //�¶ȸ澯��־λ��2		
								timer_Alm=0; //����������
							} 	
						}	
			       break;

      case AlmPrt_OUT:
					if(Tem->gTemAlmPrtReset <= BatTem)   //�����¶�
					{
						 timer_Alm++;
							if(Tem->gTemAlmPrtTimer == timer_Alm)
							{
								 X=Alm_OUT; //�¶ȸ澯��־λ��1		
								timer_Alm=0; //����������
							} 
					}	
					else
						 timer_Alm = 0;
			 
			       break;
			default:			
			        X=NO_AlmPrt; //�¶ȸ澯��־λ��0
                      break;
		}				
		return X;	
	}
	
////////////////////////////////////////////////////////////////////////////////	
//��ֵ����	
///////////////////////////////////////////////////////////////////////////////
void caculatFiveMax(u16 MAX_VALUE[5][2],u16 NOW_MAX_VALUE,u16 NOW_cycle)
{
 u16 temp,temp1;
	u8 i,j;
//	static u16 old_MAX_VALUE;
//	
//	if(old_MAX_VALUE !=NOW_MAX_VALUE)
//	old_MAX_VALUE =NOW_MAX_VALUE;
//	else
//		return;
if(((u16)MAX_VALUE[0][0]==0xffff)&&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[1][0]) &&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[2][0])&&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[3][0]) &&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[4][0]))
  {
   MAX_VALUE[0][0]=MAX_VALUE[1][0]=MAX_VALUE[2][0]=MAX_VALUE[3][0]=MAX_VALUE[4][0]=0;
  } 	

//for(i=0;i<5;i++)
//{	
//if(MAX_VALUE[i][0]==0xFFFF) 
//{
//  MAX_VALUE[i][0]=0;
//}
//}
if(NOW_MAX_VALUE>MAX_VALUE[0][0])//???????????
 {
 
 MAX_VALUE[0][0] = NOW_MAX_VALUE;
 MAX_VALUE[0][1] = NOW_cycle;
 }
  else
	 return;
	 for(i=0;i<4;i++)
 { 
   for(j=0;j<(4-i);j++)
   {
     if(MAX_VALUE[j][0] > MAX_VALUE[j+1][0])
	 {
	  temp = MAX_VALUE[j][0];
	  temp1 = MAX_VALUE[j][1];
	  MAX_VALUE[j][0] = MAX_VALUE[j+1][0];
	  MAX_VALUE[j][1] = MAX_VALUE[j+1][1];
	  MAX_VALUE[j+1][0] = temp;
	  MAX_VALUE[j+1][1] = temp1;
	 }
	
   }
 }

 }
//////////////////////////////////////////////////////////// 
 void caculatFiveMin(u16 Min_VALUE[5][2],u16 NOW_Min_VALUE,u16 NOW_cycle)
{
  u16 temp,temp1;
	u8 i,j;
//	static u16 old_MIN_VALUE;
//	if(old_MIN_VALUE !=NOW_Min_VALUE)
//		old_MIN_VALUE = NOW_Min_VALUE;
//	else
//		return ;
//	if(((u16)Min_VALUE[0][0]==0xffff)&&((u16)Min_VALUE[0][0]==(u16)Min_VALUE[1][0]) &&((u16)Min_VALUE[0][0]==(u16)Min_VALUE[2][0])&&((u16)Min_VALUE[0][0]==(u16)Min_VALUE[3][0]) &&((u16)Min_VALUE[0][0]==(u16)Min_VALUE[4][0]))
//  {
//   Min_VALUE[0][0]=Min_VALUE[1][0]=Min_VALUE[2][0]=Min_VALUE[3][0]=Min_VALUE[4][0]=3800;
//  } 
	
 if(NOW_Min_VALUE<Min_VALUE[0][0])//???????????
 {
 
 Min_VALUE[0][0] = NOW_Min_VALUE;
 Min_VALUE[0][1] = NOW_cycle;
 }
 else
	 return ;
 for(i=0;i<4;i++)
 { 
   for(j=0;j<(4-i);j++)
   {
     if(Min_VALUE[j][0] < Min_VALUE[j+1][0])
	 {
	  temp = Min_VALUE[j][0];
	  temp1 = Min_VALUE[j][1];
	  Min_VALUE[j][0] = Min_VALUE[j+1][0];
	  Min_VALUE[j][1] = Min_VALUE[j+1][1];
	  Min_VALUE[j+1][0] = temp;
	  Min_VALUE[j+1][1] = temp1;
	 }
   }
 }
 
}

///////////////////////////////////////////////////////////////////////////////
///�����5�γ�����
////////////////////////////////////
void CurrentFiveMax(s16 MAX_VALUE[5][2],s16 NOW_MAX_Current,u16 NOW_cycle)
{
  s16 temp,temp1;
	u8 i,j;
//	static u16 old_MAX_Current;
//	
//	if(old_MAX_Current !=NOW_MAX_Current)
//	old_MAX_Current =NOW_MAX_Current;
//	else
//		return;
 
	if(NOW_MAX_Current>MAX_VALUE[0][0])//???????????
 {
 
 MAX_VALUE[0][0] = NOW_MAX_Current;
 MAX_VALUE[0][1] = NOW_cycle;
 }
  else
	 return;
					 for(i=0;i<4;i++)
				 { 
					 for(j=0;j<(4-i);j++)
					 {
						 if(MAX_VALUE[j][0] > MAX_VALUE[j+1][0])
					 {
						temp = MAX_VALUE[j][0];
						temp1 = MAX_VALUE[j][1];
						MAX_VALUE[j][0] = MAX_VALUE[j+1][0];
						MAX_VALUE[j][1] = MAX_VALUE[j+1][1];
						MAX_VALUE[j+1][0] = temp;
						MAX_VALUE[j+1][1] = temp1;
					 }
					
					 }
				 }
 }
///////////////////////////////////////////////////////////////////////////////
 //��5�����ŵ����
 //////////////////
void CurrentFiveMin(s16 MAX_VALUE[5][2],s16 NOW_Min_Current,u16 NOW_cycle)
{
  s16 temp,temp1;
	u8 i,j;
//	static u16 old_MAX_Current;
//	
//	if(old_MAX_Current !=NOW_Min_Current)
//	old_MAX_Current =NOW_Min_Current;
//	else
//		return;
 
if(NOW_Min_Current<MAX_VALUE[4][0])//???????????
 {
 
 MAX_VALUE[4][0] = NOW_Min_Current;
 MAX_VALUE[4][1] = NOW_cycle;
 }
  else
	 return;
					 for(i=0;i<4;i++)
				 { 
					 for(j=0;j<(4-i);j++)
					 {
						 if(MAX_VALUE[j][0] > MAX_VALUE[j+1][0])
					 {
						temp = MAX_VALUE[j][0];
						temp1 = MAX_VALUE[j][1];
						MAX_VALUE[j][0] = MAX_VALUE[j+1][0];
						MAX_VALUE[j][1] = MAX_VALUE[j+1][1];
						MAX_VALUE[j+1][0] = temp;
						MAX_VALUE[j+1][1] = temp1;
					 }
					
					 }
				 }
 } 
///////////////////////////////////////////////////////////////////////////////
void TemFiveMax(s16 MAX_VALUE[5][2],s16 NOW_MAX_Tem,u16 NOW_cycle)
{
 s16 temp,temp1;
	u8 i,j;
//	static u16 old_MAX_Tem;
//	
//	if(old_MAX_Tem !=NOW_MAX_Tem)
//	old_MAX_Tem =NOW_MAX_Tem;
//	else
//		return;
if(((u16)MAX_VALUE[0][0]==0xffff)&&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[1][0]) &&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[2][0])&&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[3][0]) &&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[4][0]))
  {
   MAX_VALUE[0][0]=MAX_VALUE[1][0]=MAX_VALUE[2][0]=MAX_VALUE[3][0]=MAX_VALUE[4][0]=(-40);
  } 
	if(NOW_MAX_Tem>MAX_VALUE[0][0])//???????????
 {
 
 MAX_VALUE[0][0] = NOW_MAX_Tem;
 MAX_VALUE[0][1] = NOW_cycle;
 }
  else
	 return;
					 for(i=0;i<4;i++)
				 { 
					 for(j=0;j<(4-i);j++)
					 {
						 if(MAX_VALUE[j][0] > MAX_VALUE[j+1][0])
					 {
						temp = MAX_VALUE[j][0];
						temp1 = MAX_VALUE[j][1];
						MAX_VALUE[j][0] = MAX_VALUE[j+1][0];
						MAX_VALUE[j][1] = MAX_VALUE[j+1][1];
						MAX_VALUE[j+1][0] = temp;
						MAX_VALUE[j+1][1] = temp1;
					 }
					
					 }
				 }
 }
///////////////////////////////////////////////////////////////////////////////
void TemFiveMin(s16 MAX_VALUE[5][2],s16 NOW_Min_Tem,u16 NOW_cycle)
{
 s16 temp,temp1;
	u8 i,j;
//	static u16 old_MAX_Tem;
//	
//	if(old_MAX_Tem !=NOW_Min_Tem)
//	old_MAX_Tem =NOW_Min_Tem;
//	else
//		return;

 if(((u16)MAX_VALUE[0][0]==0xffff)&&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[1][0]) &&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[2][0])&&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[3][0]) &&((u16)MAX_VALUE[0][0]==(u16)MAX_VALUE[4][0]))
  {
   MAX_VALUE[0][0]=MAX_VALUE[1][0]=MAX_VALUE[2][0]=MAX_VALUE[3][0]=MAX_VALUE[4][0]=(127);
  }
 
	if(NOW_Min_Tem<MAX_VALUE[0][0])//???????????
 {
 
 MAX_VALUE[0][0] = NOW_Min_Tem;
 MAX_VALUE[0][1] = NOW_cycle;
 }
  else
	 return;
					 for(i=0;i<4;i++)
				 { 
					 for(j=0;j<(4-i);j++)
					 {
						 if(MAX_VALUE[j][0] < MAX_VALUE[j+1][0])
					 {
						temp = MAX_VALUE[j][0];
						temp1 = MAX_VALUE[j][1];
						MAX_VALUE[j][0] = MAX_VALUE[j+1][0];
						MAX_VALUE[j][1] = MAX_VALUE[j+1][1];
						MAX_VALUE[j+1][0] = temp;
						MAX_VALUE[j+1][1] = temp1;
					 }
					
					 }
				 }
 } 
////////////////////////////////��ֵ�洢����
void Alternate_Mode_Processing(void) 
{
	static u8 state_MODE_ODLE = 0;

	static u16 temp_min_volt=0xffff;
	static u16 temp_max_volt=0;
	static s16 temp_max_CH_T=(-40);
	static s16 temp_min_CH_T=127; 
	static s16 temp_max_DISCH_T=(-40);
	static s16 temp_min_DISCH_T=127;
	static s16 temp_max_CH_C=0;
	static s16 temp_max_DISCH_C=0;
	static u16 temp_max_DIFF_VOlt=0;
	if(BAT_STATE_Curren==2)//�ڷŵ�״̬��
	{
		if(state_MODE_ODLE!=2)//���δӾ����л����ŵ�
		{
			temp_max_DISCH_T=(-40);
			temp_min_DISCH_T=127;
			temp_max_DIFF_VOlt=0;
			temp_max_DISCH_C=0;
			temp_min_volt=0XFFFF;
		}
		else//�����ŵ�״̬
		{
			if(BMS_config.BMS_Read.gTemMax>temp_max_DISCH_T)
				temp_max_DISCH_T = BMS_config.BMS_Read.gTemMax; 
			if(BMS_config.BMS_Read.gTemMin<temp_min_DISCH_T)
				temp_min_DISCH_T = BMS_config.BMS_Read.gTemMin; 
			if(temp_max_DIFF_VOlt<BMS_config.BMS_Read.gVoltdiff)
				temp_max_DIFF_VOlt =BMS_config.BMS_Read.gVoltdiff;
			if(temp_max_DISCH_C>BMS_config.BMS_Read.gCurrent)	
				temp_max_DISCH_C = BMS_config.BMS_Read.gCurrent;	
			if(temp_min_volt>BMS_config.BMS_Read.gVoltMin)
				temp_min_volt = BMS_config.BMS_Read.gVoltMin; 					 
		}
	}
	if((BAT_STATE_Curren!=2)&&(state_MODE_ODLE==2))//�ŵ��л�������
	{
		caculatFiveMin(Extremum.MAX__Extremum_DChg_Volt.value,temp_min_volt,(BMS_config.Pack_RW_config.gsCycleCnt+1)); //�ŵ��ѹ��ֵ�洢
		CurrentFiveMin(Extremum.MAX__Extremum_DchgOverCurren.value,temp_max_DISCH_C,(BMS_config.Pack_RW_config.gsCycleCnt+1));//�ŵ������ֵ�洢
		TemFiveMax(Extremum.MAX__Extremum_DchgHighTem.value,(temp_max_DISCH_T),(BMS_config.Pack_RW_config.gsCycleCnt+1));   //�����¼�ֵ�洢
		TemFiveMin(Extremum.MAX__Extremum_DchgLowTem.value,(temp_min_DISCH_T),(BMS_config.Pack_RW_config.gsCycleCnt+1));   //�����¼�ֵ�洢	
		caculatFiveMax(Extremum.MAX__Extremum_VoltDiffMax.value,temp_max_DIFF_VOlt,(BMS_config.Pack_RW_config.gsCycleCnt+1)); //ѹ�ֵ�洢
		//////����FLASH
		FLASH_Write(FALSH_BMS_Extremum_ADDR,&Extremum.MAX__Extremum_Chg_Volt.value[0][0],(sizeof(Extremum)/2));
		delay_ms(10);
		FLASH_Write(FALSH_BMS_Extremum_old_ADDR,&Extremum.MAX__Extremum_Chg_Volt.value[0][0],(sizeof(Extremum)/2));
	}
	///////////////////////////////////	
	if(BAT_STATE_Curren==1)//�ڳ��ģʽ��
	{
		if(state_MODE_ODLE!=1)//���δӾ����л������
		{
			temp_max_CH_T=(-40);
			temp_min_CH_T=(127);
			temp_max_DIFF_VOlt=0;
			temp_max_CH_C=0;
			temp_max_volt=0X0000;
		}
		else//�����ų��
		{
			if(BMS_config.BMS_Read.gTemMax>temp_max_CH_T)
				temp_max_CH_T = BMS_config.BMS_Read.gTemMax; 
			if(BMS_config.BMS_Read.gTemMin<temp_min_CH_T)
				temp_min_CH_T = BMS_config.BMS_Read.gTemMin; 
			if(temp_max_DIFF_VOlt<BMS_config.BMS_Read.gVoltdiff)
				temp_max_DIFF_VOlt =BMS_config.BMS_Read.gVoltdiff;
			if(temp_max_CH_C<BMS_config.BMS_Read.gCurrent)	
				temp_max_CH_C = BMS_config.BMS_Read.gCurrent;	
			if(temp_max_volt<BMS_config.BMS_Read.gVoltMax)
				temp_max_volt = BMS_config.BMS_Read.gVoltMax; 					 
		}
	}
	if((BAT_STATE_Curren!=1)&&(state_MODE_ODLE==1))//����л�������
	{		
		caculatFiveMax(Extremum.MAX__Extremum_Chg_Volt.value,temp_max_volt,(BMS_config.Pack_RW_config.gsCycleCnt+1)); //����ѹ��ֵ�洢	
		CurrentFiveMax(Extremum.MAX__Extremum_ChgOverCurren.value,temp_max_CH_C,(BMS_config.Pack_RW_config.gsCycleCnt+1));  //��������ֵ�洢
		TemFiveMax(Extremum.MAX__Extremum_ChgHighTem.value,temp_max_CH_T,(BMS_config.Pack_RW_config.gsCycleCnt+1));   //�����¼�ֵ�洢
		TemFiveMin(Extremum.MAX__Extremum_ChgLowTem.value,temp_min_CH_T,(BMS_config.Pack_RW_config.gsCycleCnt+1));   //�����¼�ֵ�洢				
		caculatFiveMax(Extremum.MAX__Extremum_VoltDiffMax.value,temp_max_DIFF_VOlt,(BMS_config.Pack_RW_config.gsCycleCnt+1)); //ѹ�ֵ�洢
		//////����FLASH
		FLASH_Write(FALSH_BMS_Extremum_ADDR,&Extremum.MAX__Extremum_Chg_Volt.value[0][0],(sizeof(Extremum)/2));
		delay_ms(10);
		FLASH_Write(FALSH_BMS_Extremum_old_ADDR,&Extremum.MAX__Extremum_Chg_Volt.value[0][0],(sizeof(Extremum)/2));
	}

	///////////////////////////////////////////
	state_MODE_ODLE = BAT_STATE_Curren;
}
/***************************************************************************
*   �� �� ��: BAT_Processing
*   ����˵��: ������ݴ���
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
	
void BAT_Processing(void)
{
	static u8 Proces_dealy_timer_staty=0;
	static u8 Proces_dealy_timer_stop=0;
	static u8 Proces_falg=0;
	static u8 Proces_bat_falg = 0;

	u16_FindMaxAndMinValue( BMSVoltTemperature.g_Volt_call,&BMS_config.BMS_Read.gChVoltMax,&BMS_config.BMS_Read.gChVoltMin,BMS_config.Pack_RW_config.gBatNumber);  //�������С��ѹ
	BMS_config.BMS_Read.gVoltMax = BMSVoltTemperature.g_Volt_call[BMS_config.BMS_Read.gChVoltMax];  		//����ѹ
	BMS_config.BMS_Read.gVoltMin = BMSVoltTemperature.g_Volt_call[BMS_config.BMS_Read.gChVoltMin]; 			//��С��ѹ
	BMS_config.BMS_Read.gVoltdiff = BMS_config.BMS_Read.gVoltMax - BMS_config.BMS_Read.gVoltMin; 			//�����Сѹ�	
	BMS_config.BMS_Read.gVoltAvg = u16_CalculateAverage(BMSVoltTemperature.g_Volt_call,BMS_config.Pack_RW_config.gBatNumber);//���ƽ����ѹ
	BMS_config.BMS_Read.gVoltSum = BMS_config.BMS_Read.gVoltAvg * BMS_config.Pack_RW_config.gBatNumber/100; //����ܵ�ѹ

	u8_FindMaxAndMinValue(BMSVoltTemperature.g_Temperature_call,&BMS_config.BMS_Read.gChTemMax,&BMS_config.BMS_Read.gChTemMin,BMS_config.Pack_RW_config.gTemNumber); //�������С�¶�
	BMS_config.BMS_Read.gTemMax = BMSVoltTemperature.g_Temperature_call[BMS_config.BMS_Read.gChTemMax];//����¶�
	BMS_config.BMS_Read.gTemMin = BMSVoltTemperature.g_Temperature_call[BMS_config.BMS_Read.gChTemMin]; //����¶�
	BMS_config.BMS_Read.gChTemdiff = BMS_config.BMS_Read.gTemMax - BMS_config.BMS_Read.gTemMin; //�¶Ȳ�
	BMS_config.BMS_Read.gTemAvg =	s8_CalculateAverage(BMSVoltTemperature.g_Temperature_call,BMS_config.Pack_RW_config.gTemNumber); //���ƽ���¶�

	if(BAT_State <= 2)
		BMS_config.BMS_Read.state =  BAT_STATE_Curren;//��ǰBMS״̬
	else
		BMS_config.BMS_Read.state =  BAT_State;//��ǰBMS״̬
	if(Proces_bat_falg == 0)  //ǰ5�벻�������ж�
	{
		if(Proces_falg == 0)
		{
			Proces_dealy_timer_staty=RTC_DateTimeStructure_NOW.Second;
			Proces_falg = 1;
		}
		else
		{			
			if(RTC_DateTimeStructure_NOW.Second >= Proces_dealy_timer_staty)
				Proces_dealy_timer_stop=RTC_DateTimeStructure_NOW.Second - Proces_dealy_timer_staty;
			else
				Proces_dealy_timer_stop=RTC_DateTimeStructure_NOW.Second + 60 - Proces_dealy_timer_staty;	

			if(Proces_dealy_timer_stop >= 5)
				Proces_bat_falg = 1;

		}	
	}
	else
	{
		BAT_AlmPrt_Processing();
		//memcpy(&BMS_config.BMS_Read.FaultAlarmState,&g_FaultAlarmState,sizeof(g_FaultAlarmState)); 
		Alternate_Mode_Processing();		
	}	

}
	/***************************************************************************
*   �� �� ��: Write_falsh_EvtData(void)
*   ����˵��: �����ʷ��������
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void	Write_falsh_EvtData(void)
	{
		u8 da[8]= {0};
		tEvtDataW.gsCycleCnt = BMS_config.Pack_RW_config.gsCycleCnt+1;              //ѭ������
		tEvtDataW.RTC_DateTimeStructure = RTC_DateTimeStructure_NOW;
//				tEvtDataW.gNewPackCapacity = BMS_config.Pack_RW_config.gNewPackCapacity;     //������������
//				tEvtDataW.gSurpulsPackCapacity = BMS_config.Pack_RW_config.gSurpulsPackCapacity;   //ʣ������
		          tEvtDataW.state = BMS_config.BMS_Read.state;
				tEvtDataW.gSOC = BMS_config.Pack_RW_config.gSOC;                  //��ذ�SOC
				tEvtDataW.gSOH = BMS_config.Pack_RW_config.gSOH;                 //��ذ�SOH
		    da[0] = g_FaultAlarmState[0];
				da[1] = g_FaultAlarmState[1];
		    da[2] = g_FaultAlarmState[2];
		    da[3] = g_FaultAlarmState[3]&0xf0;
		    da[4] = g_FaultAlarmState[4]&0x1f;
		    da[5] = g_FaultAlarmState[5]&0x1f;
		    da[6] = g_FaultAlarmState[6]&0x08;
		    da[7] = 0x00;
				memcpy(&tEvtDataW.FaultAlarmState,&da,8); //�澯��Ϣ
//				tEvtDataW.gVoltSum = BMS_config.BMS_Read.gVoltSum;           //�ܵ�ѹ1mV       
//				tEvtDataW.gVoltAvg = BMS_config.BMS_Read.gVoltAvg;          //ƽ����ѹmV  
//				tEvtDataW.gVoltMin = BMS_config.BMS_Read.gVoltMin;         //�����С��ѹֵmV  
//				tEvtDataW.gVoltMax = BMS_config.BMS_Read.gVoltMin;        //�������ѹֵmV 
//				tEvtDataW.gChVoltMin = BMS_config.BMS_Read.gVoltMin;       //��С��ѹͨ��    
//				tEvtDataW.gChVoltMax = BMS_config.BMS_Read.gChVoltMax;      //����ѹͨ��
//				tEvtDataW.gVoltdiff = BMS_config.BMS_Read.gVoltdiff;       //ѹ��  
//				tEvtDataW.gDCHGTime = BMS_config.BMS_Read.gDCHGTime;      //Ԥ�Ʒŵ�ʣ��ʱ��
//				tEvtDataW.gCHGTime = BMS_config.BMS_Read.gCHGTime;        //Ԥ�Ƴ��ʣ��ʱ��
				tEvtDataW.gCurrent = BMS_config.BMS_Read.gCurrent;       //��ǰ����ֵ 
//				tEvtDataW.gCurrentAvg = BMS_config.BMS_Read.gCurrentAvg;     //ƽ������ֵ     
//				tEvtDataW.gCurrentDCHGMax = BMS_config.BMS_Read.gCurrentDCHGMax; //�ŵ�������  
//				tEvtDataW.gCurrentCHGMax = BMS_config.BMS_Read.gCurrentCHGMax;  //���������
//				tEvtDataW.gTemAvg = BMS_config.BMS_Read.gTemAvg;          //ƽ���¶�
//				tEvtDataW.gTemMin = BMS_config.BMS_Read.gTemMin;          	//����¶�
//				tEvtDataW.gTemMax = BMS_config.BMS_Read.gTemMax;          //����¶�
//				tEvtDataW.gChTemMin = BMS_config.BMS_Read.gChTemMin;         //����¶�ͨ��
//				tEvtDataW.gChTemMax = BMS_config.BMS_Read.gChTemMax;        //����¶�ͨ��
//				tEvtDataW.gChTemdiff = BMS_config.BMS_Read.gChTemdiff;        //�¶Ȳ�
				memcpy(&tEvtDataW.FALSH_BMSVoltTemperature_call,&BMSVoltTemperature,sizeof(BMSVoltTemperature_call_typedef)); //��ѹ�¶Ⱦ���		
	
	}
/***************************************************************************
*   �� �� ��: BAT_AlmPrt_Processing()
*   ����˵��: ��ع�����Ϣ
*   ��    �Σ���    BAT_gVoltAlmPrt( BatVolt_typedef* Volt, u16 BatVolt)
	BAT_gCurrentAlmPrt( BatCurrent_typedef* Current, u16 BatCurrent)
	BAT_gTemAlmPrt( BatTem_typedef* Tem, s8 BatTem)
	
*   �� �� ֵ: ��
***************************************************************************/
void BAT_AlmPrt_Processing(void)
{
	static 	uint32_t tickstart_AlmPrt = 0;
static 	uint32_t ticksotp_AlmPrt = 0;
static u8 falg_Alm[12]	= {0};
u8 old= 0;
u8 New_falg= 0;
u8 temp =0 ;
static u8 Old_falg= 0;
static u8 gSoclow_falg = 0;
//static u8 BitVoltFailure_falg = 0;
//u8 i=0;
	     if(tickstart_AlmPrt == 0)
			 {
				 tickstart_AlmPrt = HAL_GetTick();
			 }
	
			ticksotp_AlmPrt = HAL_GetTick();
				if(ticksotp_AlmPrt<tickstart_AlmPrt)//
				{
				ticksotp_AlmPrt=0xffffffff-tickstart_AlmPrt+ticksotp_AlmPrt;
				}
				else
				ticksotp_AlmPrt=ticksotp_AlmPrt-tickstart_AlmPrt;
				
				if(ticksotp_AlmPrt>1000)
				{
					tickstart_AlmPrt = 0;
					ticksotp_AlmPrt = 0;
				}
				else
				{
					return;
				}
	g_FaultAlarmState[0] =g_FaultAlarmState[1]=g_FaultAlarmState[2]	=0;
	g_FaultAlarmState[3]	&=0xA0;

		if(BMS_config.BMS_Read.gVoltMin>=3700)
		g_FaultAlarmState[0]	|=  BAT_gVoltDiffAlmPrt( &BMS_config.VoltDiffMax, BMS_config.BMS_Read.gVoltdiff)<<4;     //����ѹ��澯���� 
		else
		g_FaultAlarmState[0]	|=  BAT_gVoltDiffAlmPrt1( &BMS_config.VoltDiffMax1, BMS_config.BMS_Read.gVoltdiff)<<6;     //����ѹ��1�澯����

		if(BAT_STATE_Curren == 1)	
		{	
//		g_FaultAlarmState[0]	|= BAT_gVoltAlmPrt( &BMS_config.OverVolt, BMS_config.BMS_Read.gVoltMax);   //�����ѹ�澯����
//		g_FaultAlarmState[1]	=   BAT_gCurrentAlmPrt( &BMS_config.ChgOverCurren, BMS_config.BMS_Read.gCurrent); // �����������澯�뱣��
	//	g_FaultAlarmState[1] = g_FaultAlarmState[1]&0x07;
		/////////////////////////////////////////////����·Ԥ��
			if(BMS_config.BMS_Read.gCurrent>BMS_config.Pack_RW_config.gChgShortCircuit)
			gBitChgShortCircuit = 1;	
//////////////////////////////////������Ԥ��
//		g_FaultAlarmState[1] |=	(BAT_gTemAlmPrt(&BMS_config.ChgHighTem,BMS_config.BMS_Read.gTemMax)<<6);
			temp = BAT_gTemAlmPrt(&BMS_config.ChgHighTem,BMS_config.BMS_Read.gTemMax);
			if(temp == 1)
			gBitChgHighTemAlm = 1;
			else if (temp == 2)
			gBitChgHighTemAlmPrt = 1;
			else
			{
				gBitChgHighTemAlm = 0;
				 gBitChgHighTemAlmPrt = 0;
			}
//////////////////////////////////������Ԥ��	
//			       if(gBitTemFailure==0)			
//      g_FaultAlarmState[2] =	  BAT_gTemAlm1Prt(&BMS_config.ChgLowTem,BMS_config.BMS_Read.gTemMin);
			if(gBitTemFailure==0)	
			{
				 temp =  BAT_gTemAlm1Prt(&BMS_config.ChgLowTem,BMS_config.BMS_Read.gTemMin);
					if(temp ==1)
					gBitChgLowTemAlm = 1;
					else if (temp ==2)
					gBitChgLowTemAlmPrt = 1;
					else
					{
					gBitChgLowTemAlm = 0;
					gBitChgLowTemAlmPrt = 0;
					}						
			}
			

//////////////////////////////////����²�Ԥ��
			if(BMS_config.Pack_RW_config.gChgDiffhTemMax<BMS_config.BMS_Read.gChTemdiff)
       g_FaultAlarmState[2] |= 	1<<2;		
		
			//////////////////////////////////���MOS����Ԥ��
		}
	if((BAT_STATE_Curren == 2)|(BAT_STATE_Curren == 0))	
	{
				gBitChgShortCircuit = 0;	
//	g_FaultAlarmState[0]	|=	BAT_gUVoltAlmPrt( &BMS_config.UnderVolt, BMS_config.BMS_Read.gVoltMin)<<2;    //����Ƿѹ�澯���� 
//	g_FaultAlarmState[1]	|= BAT_gCurrentAlmPrt1(&BMS_config.DchgOverCurren, BMS_config.BMS_Read.gCurrent)<<3; // �ŵ���������澯�뱣��	
//	g_FaultAlarmState[1] = g_FaultAlarmState[1]&0x38;
	/////////////////////////////////////////////�ŵ��·Ԥ��
		//////////////////////////////////�ŵ����Ԥ��
		//		g_FaultAlarmState[2] |=	(BAT_gTemAlmPrt(&BMS_config.DchgHighTem,BMS_config.BMS_Read.gTemMax)<<3);
			temp = BAT_gTemAlmPrt(&BMS_config.DchgHighTem,BMS_config.BMS_Read.gTemMax);
			if(temp ==1)
			gBitDchgHighTemAlm = 1;
			else if (temp ==2)
			gBitDchgHighTemAlmPrt = 1;
			else
			{
				gBitDchgHighTemAlm = 0;
				 gBitDchgHighTemAlmPrt = 0;
			}
//////////////////////////////////�ŵ����Ԥ��	
//        if(gBitTemFailure==0)			
//        g_FaultAlarmState[2] |=	  BAT_gTemAlm1Prt(&BMS_config.DchgLowTem,BMS_config.BMS_Read.gTemMin)<<5;
			if(gBitTemFailure==0)	
			{
				 temp =  BAT_gTemAlm1Prt(&BMS_config.DchgLowTem,BMS_config.BMS_Read.gTemMin);
					if(temp ==1)
					gBitDchgLowTemAlm = 1;
					else if (temp ==2)
					gBitDchgLowTemAlmPrt = 1;
					else
					{
					gBitDchgLowTemAlm = 0;
					gBitDchgLowTemAlmPrt = 0;
					}						
			}
//////////////////////////////////�ŵ��²�Ԥ��
					if(BMS_config.Pack_RW_config.gDchgDiffhTemMax<BMS_config.BMS_Read.gChTemdiff)
     g_FaultAlarmState[2] |= 	1<<7;	
		//////////////////////////////////�ŵ�MOS����Ԥ��			
					

	}	
			if((BMS_config.Pack_RW_config.gSoclow>BMS_config.Pack_RW_config.gSOC))//soc��
		{
			gSoclow_falg =1;
	//	  g_FaultAlarmState[3] |= 	1<<6;	
		}	
		 if	((BMS_config.Pack_RW_config.gSOC>15))
		 {
//			g_FaultAlarmState[3] &= 	~(1<<6);
			 gSoclow_falg =0;
		 }	
			if(gSoclow_falg)		
			 g_FaultAlarmState[3] |= 	1<<6;	
			else
			g_FaultAlarmState[3] &= 	~(1<<6);	
	g_FaultAlarmState[0]	|= BAT_gVoltAlmPrt( &BMS_config.OverVolt, BMS_config.BMS_Read.gVoltMax);   //�����ѹ�澯����
	g_FaultAlarmState[1]	|=   BAT_gCurrentAlmPrt( &BMS_config.ChgOverCurren, BMS_config.BMS_Read.gCurrent); // �����������澯�뱣��
	g_FaultAlarmState[0]	|=	BAT_gUVoltAlmPrt( &BMS_config.UnderVolt, BMS_config.BMS_Read.gVoltMin)<<2;    //����Ƿѹ�澯���� 
	g_FaultAlarmState[1]	|= BAT_gCurrentAlmPrt1(&BMS_config.DchgOverCurren, BMS_config.BMS_Read.gCurrent)<<3; // �ŵ���������澯�뱣��	
	//g_FaultAlarmState[1] = g_FaultAlarmState[1]&0x3f;
	#if(Failure_DUBAG == 1)
//����ʧЧ��Ҫ����    
	if(BMS_config.BMS_Read.gTemMax>BMS_config.Pack_RW_config.gBatHighTemFailure)
	{	
		if(falg_Alm[0]<TERM_TICK)
		falg_Alm[0]++;
		if(falg_Alm[0]==TERM_TICK)
		{
	   gBit_BatHighTemFailure = 1;            //��ظ���ʧЧ	
		}			
	}
	else if(falg_Alm[0]>0)
	{
		falg_Alm[0]= 0 ;
	}
	else
	{
		
	}
	
	if((BMS_config.BMS_Read.gTemMin<BMS_config.Pack_RW_config.gBatLowTemFailure)&&(gBitTemFailure==0))  
	{
	if(falg_Alm[1]<TERM_TICK)
		falg_Alm[1]++;
		if(falg_Alm[1]==TERM_TICK)
	gBit_BatlowTemFailure = 1;             	//��ص���ʧЧ 
	}
	else if(falg_Alm[1]>0)
	{
		falg_Alm[1]= 0 ;		
	}
	else
	{
		
	}
		
	if(BMS_config.BMS_Read.gVoltMax>BMS_config.Pack_RW_config.gBatOverVoltFailure)
	{
		if(falg_Alm[2]<20)
		falg_Alm[2]++;
		if(falg_Alm[2]==20)
   gBit_BatOverVoltFailure = 1;  								//��ص�ع�ѹ��������ʧЧ	
	}
	else if(falg_Alm[2]>0)
	{
			falg_Alm[2]= 0 ;
	}
	else
  {
		
	}		
	if(BMS_config.BMS_Read.gVoltMin<BMS_config.Pack_RW_config.gBatUnderVoltFailure)
	{
		if(falg_Alm[3]<20)
		falg_Alm[3]++;
		if(falg_Alm[3]==20)
  gBit_BatUnderVoltFailure = 1;  							//��ص��Ƿѹ��������ʧЧ	
	}
	else if(falg_Alm[3]>0)
	{
		falg_Alm[3] = 0 ;
	}
	else
	{
		
	}
	if((BMS_config.BMS_Read.gVoltdiff>BMS_config.Pack_RW_config.gBatVoltDiffMaxFailure)&&(BMS_config.BMS_Read.gVoltMin>3600))
	{
		if(falg_Alm[4]<TERM_TICK)
		falg_Alm[4]++;
		if(falg_Alm[4]==TERM_TICK)
  gBit_BatVoltDiffMaxFailure = 1;  							//���ѹ�������ʧЧ
		
	}
	else if(falg_Alm[4]>0)
	{
	falg_Alm[4]=0;		
	}
	else
	{
		
	}
	#endif
	//���ڻ�С�ڱ��滷����ߵ���ʧЧֵһֱ
	if(BMS_config.BMS_Read.gTemMax>BMS_config.Pack_RW_config.gBatHighTemFailure) 
	{
  if(falg_Alm[5]<TERM_TICK)
		falg_Alm[5]++;
		if(falg_Alm[5]==TERM_TICK)
		gBitStorageHighTem= 1; 								//�������洢�����¶�		
	}
	else if(falg_Alm[5]>0)
	{
		falg_Alm[5]--;
		if(falg_Alm[5] == 0 )
			gBitStorageHighTem= 0;
	}
	else
	{
	}
	
if((BMS_config.BMS_Read.gTemMin<BMS_config.Pack_RW_config.gBatLowTemFailure)&&(gBitTemFailure==0))
{ 
 if(falg_Alm[6]<TERM_TICK)
falg_Alm[6]++;
if(falg_Alm[6]==TERM_TICK)	
	gBitStorageLowTem= 1; 								//С����͵Ĵ洢�Ļ����¶�	
}
else if(falg_Alm[6]>0)
{
falg_Alm[6]--;	
	if(falg_Alm[6] == 0)
		gBitStorageLowTem= 0;
}
else
{
	
}
/////////////////��ѹ�ɼ������¶Ȳ������߾���BQ76940��
if((gBitTemFailure)&&(gBitVoltFailure))
	gBitFAE_Error = 1;
else
	gBitFAE_Error = 0;
  if((!gBOying_IN)&&(BAT_STATE_Curren==1))//������ĳ�����
	{
		gBit_Chg_Error = 1;	
	}
	else
	{
		gBit_Chg_Error = 0;		
	}

////MOS�������汣��ʧЧ
gBitChgMosHighTemAlm = 0;								//���MOS����
gBitChgMosHighTemAlmPrt  = 0;								//���MOS���±���
gBitDchgMosHighTemAlm   = 0;								//�ŵ�MOS����
gBitDchgMosHighTemAlmPrt  = 0;								//�ŵ�MOS���±���
gBitSelfDchgMOSFailure 	 = 0;									 //�Էŵ�MOSʧЧ
gBitHeatTemMosFailure		 = 0;									//����MOSʧЧ
gBitChgMOSFailure 		   = 0;									 //���MOSʧЧ
gBitDchgMosFailure       = 0;	 								//�ŵ�MOSʧЧ
gBitPreDChargeMosFailure     = 0;									//Ԥ�ŵ�MOSʧЧ
gBitPreChargeMosFailure       = 0;									//Ԥ��MOSʧЧ
	

memcpy(&BMS_config.BMS_Read.FaultAlarmState,&g_FaultAlarmState,sizeof(g_FaultAlarmState)); 
	///���´�����־
	New_falg = (gBitOverVoltAlm|gBitOverVoltAlmPrt)     
	|((gBitUnderVoltAlm|gBitUnderVoltAlmPrt)<<1)
	|((gBitVoltDiff1MaxAlm|gBitVoltDiff1MaxAlmPrt)<<2)
	|((gBitVoltDiff2MaxAlm|gBitVoltDiff2MaxAlmPrt)<<3)
	|((gBitChgOverCurrentAlm|gBitChgOverCurrentAlmPrt|gBitChgShortCircuit)<<4)
	|((gBitDchgOverCurrentAlm|gBitDchgOverCurrentAlmPrt|gBitDchgShortCircuit)<<5)
	|(gBit_Chg_Error<<6);
	//����ʧЧ��־
	if((old !=Alm_Number.Failure)||(New_falg !=Old_falg))
	{
		Alm_Number.Failure = old;
	BMS_Alm_Number_Write(); //��¼���ϴ���
	Old_falg = New_falg;
	old = 	gBit_BatHighTemFailure|(gBit_BatlowTemFailure<<1)|(gBit_BatOverVoltFailure<<2)|(gBit_BatUnderVoltFailure<<3)|(gBit_BatVoltDiffMaxFailure<<4);
	}

}
