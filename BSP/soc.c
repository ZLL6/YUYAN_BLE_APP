//
// Created by Yongz on 2018/12/27.
//
#include "pbdata.h"
#include "soc.h"
#include "data.h"
#include "stdio.h"
#include "math.h"
//#include <stdlid.h>
//#include "flash.h"


//#pragma MESSAGE DISABLE C1420
//#pragma MESSAGE DISABLE C2705

//long soc1In1000;// 占总容量千分之一的容量（10ms单位：A*10ms =AH*3600*100/1000），注意要选择long类型
//long socDifference = 0;
//int soc;
//word soh;
//=============================================================================
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
#define   qufan(x)    (x<0x8000?x:(~(x-1)))
void calculateEffective(void) ;
void saveSoc(void) ;
//SOCHistory socHistory;
CapacityInfo BMS_capacity;
//word chargeTime = 0, dischargeTime = 0;
//byte needOpenCircuitVoltageMethod = 0;
//char soc100CurrentCondition = -60;
//extern BMS_typedef BMS_config;
//extern u16 Current;
u8 systemState=0;
const u16 Gauge_Data[101] = 
{
 4190,
 4174,
 4162,
 4152,
 4142,
 4132,
 4123,
 4113,
 4104,
 4094,
 4086,
 4079,
 4074,
 4069,
 4062,
 4050,
 4031,
 4013,
 3999,
 3988,
 3979,
 3972,
 3966,
 3961,
 3957,
 3953,
 3949,
 3943,
 3938,
 3931,
 3925,
 3918,
 3912,
 3905,
 3898,
 3891,
 3884,
 3876,
 3869,
 3861,
 3854,
 3847,
 3841,
 3835,
 3830,
 3826,
 3821,
 3817,
 3812,
 3809,
 3806,
 3802,
 3799,
 3796,
 3793,
 3790,
 3787,
 3785,
 3783,
 3781,
 3778,
 3776,
 3774,
 3772,
 3770,
 3768,
 3767,
 3765,
 3764,
 3762,
 3760,
 3758,
 3756,
 3754,
 3751,
 3748,
 3744,
 3741,
 3738,
 3735,
 3731,
 3727,
 3721,
 3715,
 3710,
 3705,
 3699,
 3691,
 3686,
 3684,
 3683,
 3681,
 3679,
 3675,
 3666,
 3636,
 3591,
 3532,
 3454,
 3335,
 3200,
};
/***************************************************************************
*   函 数 名: openCircuitVoltageMethod(u16 voltage)
*   功能说明: 开路电压法【查表方式】
*   形    参：无
*   返 回 值: 无
***************************************************************************/

u16  openCircuitVoltageMethod(u16 voltage){
    u8 i;
    if (voltage >= Gauge_Data[0])
        return 1000;
    else if (voltage <= Gauge_Data[SOC_TABLE_SIZE - 1])
        return 0;
    for (i = 0; i < SOC_TABLE_SIZE; i++) {
        if (voltage >= Gauge_Data[i]) {
            break;
        }
    }
		return (100-i)*10+(voltage - Gauge_Data[i]) * 10 / (Gauge_Data[i - 1] - Gauge_Data[i]);
}
// 计算剩余充放电时间
void socCalculateTime(void) {
	u16  Current = 0;


		Current = (u16)(gCurrentOperation); 
		Current =qufan(Current);	//取绝对值
    switch (BAT_STATE_Curren) {
		
        case 2:// 放电
					if(BMS_capacity.BatCapacity <= Current)
					BMS_config.BMS_Read.gDCHGTime =0;
          else					
         BMS_config.BMS_Read.gDCHGTime =(u16) (BMS_capacity.BatCapacity/60/Current/100);
            break;
        case 1: // 充电
//BMS_config.BMS_Read.gCHGTime	= (u16) (BMS_capacity.PackNewCapacity*(100-BMS_config.Pack_RW_config.gSOC)/3600/gCurrentOperation);	
			if(BMS_capacity.BatCapacity>=BMS_capacity.PackNewCapacity)		
				BMS_config.BMS_Read.gCHGTime	= 0;
			else
			BMS_config.BMS_Read.gCHGTime	= (u16) (BMS_capacity.PackNewCapacity/60/Current/100*(100-BMS_config.Pack_RW_config.gSOC)/100);			
            break;
					default:
				 BMS_config.BMS_Read.gCHGTime	= BMS_config.BMS_Read.gDCHGTime = 0;
					break;		
    }
}
#define CURRENT_DISCHARGE -8
/***************************************************************************
*   函 数 名: socIntegralRoutine(void)
*   功能说明: 电流积分函数
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void socIntegralRoutine(void) 
	{
    // 充电电流不小于100MA，放电电流不为0，有效积分
    if (((s16)gCurrentOperation) <= CURRENT_DISCHARGE || ((s16)gCurrentOperation )> 0) {
		   calculateEffective();		
    }	
}
	////// 计算更新有效容量
void calculateEffective(void) 
	{
static	double k = 0;//电流系数
static	u16  Current = 0;
static		u16 Voltage[2]={0};  //比较电压
static		float Newsoc ;
static u8 full_falg=0;

		Current = (u16)(gCurrentOperation); 
		Current =qufan(Current);	//取绝对值
		//k = (double)Current/(BMS_config.Pack_RW_config.gSetPackCapacity/10);
	//	k = (double)Current*10000.0/22/100000;
    k = (double)Current*10000.0f/BMS_config.Pack_RW_config.gNewPackCapacity/10000;
		if(BAT_STATE_Curren==0x02)//放电
		{
			     full_falg = 0;
		       Voltage[0] =	Gauge_Data[90] - (u16)(k*(20+k*7));
//		if(BMS_capacity.BatCapacity<(Current))
//				BMS_capacity.BatCapacity = Current;
			
					if	(((Voltage[0]>BMS_config.BMS_Read.gVoltMin)&&(2500<BMS_config.BMS_Read.gVoltMin))&&(k<=3.0f))
					{
						Voltage[1] =	Gauge_Data[100] - (u16)(k*(20+k*7));					
				  	Newsoc = 10000.0f/(Voltage[0]-Voltage[1]);
						Newsoc = (Newsoc*(Voltage[0]-BMS_config.BMS_Read.gVoltMin))/1000;
						if(Newsoc>10)
						Newsoc = 10;	
						Newsoc = 10 -  Newsoc;
		///////////////////
		    BMS_capacity.FCC_UPdatat = 1; //允许更新FCC
		    BMS_capacity.soc_1000 = Newsoc;  //记录更新最新的SOC值
		  	BMS_capacity.charge = 0;    
    ////////////////////////////	
						if(Newsoc!=0)
						{
						 k = BMS_config.Pack_RW_config.gSOC*100/Newsoc/100;								
						}
						else if(BMS_capacity.BatCapacity>0)//动态估算的SOC已经为0，直接用10倍速度积分
						{k=10;}
						else{k=0;}
					
			 if(k>=1)
				{	
				 if(BMS_capacity.BatCapacity>(Current*(1+3*(k-1))))
					BMS_capacity.BatCapacity += (s16)gCurrentOperation*(1+3*(k-1));
				 else
					BMS_capacity.BatCapacity=0;
				}
				else
				{
				 if(BMS_capacity.BatCapacity>(Current))
				 {
				 BMS_capacity.BatCapacity += (s16)gCurrentOperation;					 
				 }
				 else
				 BMS_capacity.BatCapacity=0;
				}
			}
		else
		{
				if(BMS_capacity.BatCapacity>(Current))
				 {

				 BMS_capacity.BatCapacity += (s16)gCurrentOperation;
					 
				 }
				 else
				 BMS_capacity.BatCapacity=0;
			
		}
	
		}
 
    if(BAT_STATE_Curren==0x01)//充电
		{
				 Voltage[0] =	(Gauge_Data[10]+40) + (u16)(k*30);
			    
				if	(((Voltage[0]<BMS_config.BMS_Read.gVoltMax)&&(5000>BMS_config.BMS_Read.gVoltMax))&&(k<=0.5f))
					{
						Voltage[1] =	(Gauge_Data[0]+30) + (u16)(k*30);
						
						Newsoc = 10000.0/(Voltage[1]-Voltage[0]);
						Newsoc = (Newsoc*(BMS_config.BMS_Read.gVoltMax-Voltage[0]))/1000.0;
						if(Newsoc>10)
						Newsoc = 10;
						Newsoc = 90 +  Newsoc;
						if((BMS_config.BMS_Read.gVoltMax>((u16)((u32)BMS_config.Pack_RW_config.gChgEndVolt*1000)/BMS_config.Pack_RW_config.gBatNumber/10))&&((s16)gCurrentOperation<550))
           {
					  k=50;
					 }//动态估算的SOC已经为100%，直接用5倍速度积分	
						else if(Newsoc!=100)
						{
						 k = (100-BMS_config.Pack_RW_config.gSOC)*100/(100-Newsoc)/100;							
						}
					  else if(BMS_capacity.BatCapacity<BMS_capacity.PackNewCapacity)
           {
					   k=10;
					 }//动态估算的SOC已经为100%，直接用5倍速度积分	
					  else
							{
						    k=0;
						  }
						if( BMS_capacity.BatCapacity<BMS_capacity.PackNewCapacity)
							{
						if(k>=1)
				    {	
				     BMS_capacity.BatCapacity += (s16)gCurrentOperation*(1+3*(k-1));
						}
						else
				    {
				     BMS_capacity.BatCapacity += (s16)gCurrentOperation;
				    }
					}
				 
				}					
				else if( BMS_capacity.BatCapacity<BMS_capacity.PackNewCapacity)
				{			      				
				 BMS_capacity.BatCapacity += (s16)gCurrentOperation;						
				}		
				else
				{
					
				}
				
//////////////////////
			
			if(BMS_capacity.FCC_UPdatat) //记录充入容量
			{
				BMS_capacity.charge += (s16)gCurrentOperation;
			}
    //////////////////////////////////
BMS_capacity.Bat_charge += (s16)gCurrentOperation;
/////////////////////////////////////////////	
            if(BMS_capacity.BatCapacity>=BMS_capacity.PackNewCapacity) //
						 {
									 if(((BMS_config.BMS_Read.gVoltMin>4150)&&((s16)gCurrentOperation<500))||(BMS_config.BMS_Read.gVoltMax>4250))
									 {
									   BMS_capacity.BatCapacity=BMS_capacity.PackNewCapacity;
										   
										 full_falg = 1;
										 /////////////////////////////
                     
										 if(BMS_capacity.FCC_UPdatat)
										 {
										   BMS_capacity.FCC_UPdatat = 0;  //停止更新
											 
											 BMS_capacity.charge= BMS_capacity.charge/(100-BMS_capacity.soc_1000)*100; //更新最新电池容量
											 
											 if(BMS_capacity.charge<(BMS_config.Pack_RW_config.gNewPackCapacity *3600*97))
											 {
											//	 BMS_capacity.charge = BMS_capacity.PackNewCapacity*99/100;
												 BMS_capacity.charge =  BMS_config.Pack_RW_config.gNewPackCapacity *3600*97;
											 }
											 else if(BMS_capacity.charge>(BMS_config.Pack_RW_config.gNewPackCapacity  *3600*103))
											 {
												  BMS_capacity.charge = BMS_config.Pack_RW_config.gNewPackCapacity *3600*103;
											 }
											 else
										  { 
											//;//BMS_capacity.charge =  BMS_config.Pack_RW_config.gNewPackCapacity *3600*100; //更新最新电池容量
											}											
											BMS_capacity.PackNewCapacity=BMS_capacity.charge;
										  BMS_capacity.BatCapacity = BMS_capacity.PackNewCapacity;
											//BMS_capacity.SOH = BMS_capacity.PackNewCapacity*100/Capacity_Max;
											 BMS_capacity.SOH = BMS_capacity.PackNewCapacity*100/((u32)BMS_config.Pack_RW_config.gSetPackCapacity*100*3600);
											 gSOCupdata_falsh = 1;
												
										 }
										 ////////////////////////////										 
									 } 
									 else
									 {
										if((!full_falg)&&(gCurrentOperation>500))
									  BMS_capacity.BatCapacity= BMS_config.Pack_RW_config.gNewPackCapacity *3600*100 - 360000;//BMS_capacity.BatCapacity-(s16)gCurrentOperation;										
									 }								 
			        }     
     if(BMS_capacity.BatCapacity>BMS_capacity.PackNewCapacity) //
       BMS_capacity.BatCapacity=BMS_capacity.PackNewCapacity;		 
		}

}
