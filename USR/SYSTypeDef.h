//=======================================Copyright(c)==========================================
// 											Auto Code Making Company
//											Auto Code Department
//												CopyRight Private
//-------------------------------------------------FileMessage---------------------------------
//FileName	 SYSTypeDef.h 
//Author	 yys 
//Creation Time
//Description
//-----------------------------------------------��ǰ�汾�޶�----------------------------------
//�޸���	
//��	��	 
//�޸�����	
//Description
	
//=============================================================================================

#ifndef _SYSTYPEDEF_H
#define _SYSTYPEDEF_H

//#include "stm32f10x.h"
#include <stdint.h>
//=============================================================================================
//���������
//=============================================================================================

//ʹ��
#ifndef       ENABLE_YS
#define       ENABLE_YS          								0xaa
#endif
//��ʹ��
#ifndef       DISABLE_YS
#define       DISABLE_YS         								0
#endif

//����
#ifndef       TRUE
#define       TRUE                               				1
#endif

//������
#ifndef       FALSE
#define       FALSE												0
#endif

//������
#ifndef      ERROR_YS
#define       ERROR_YS											1
#endif

//������
#ifndef      RIGHT_YS
#define       RIGHT_YS											0
#endif

//���ݿ�
#ifndef       NULL                                 
#define       NULL                              				0
#endif
//ָ���
#ifndef       PNULL                                 
#define       PNULL                            					(void*(0))
#endif

//ʹ��
#ifndef       BUSY_YS
#define       BUSY_YS          									0xaa
#endif
//��ʹ��
#ifndef       FREE_YS
#define       FREE_YS         									0
#endif

//����
#ifndef       CUT
#define       CUT                               				0
#endif

//������
#ifndef       CONNECT
#define       CONNECT											1
#endif

//============================================================================
//							�꺯������
//============================================================================
#define BITSET(x, y)								((x) |= ((u8)1 << (y)))
#define BITCLR(x, y)								((x) &= ~((u8)1 << (y)))
#define BITGET(x, y)           				 		((u8)(((x) & ((u8)1 << (y))) >> (y)))

#define BITSET16(x, y)								((x) |= ((u16)1 << (y)))
#define BITCLR16(x, y)								((x) &= ~((u16)1 << (y)))
#define BITGET16(x, y)	        					(((x) & (u16)1 << (y)) >> (y))

#define BITSET32(x, y)		    					((x) |= ((u32)1 << (y)))
#define BITCLR32(x, y)		    					((x) &= ~((u32)1 << (y)))
#define BITGET32(x, y)          					((x & ((u32)1 << (y))) >> (y))
#define IsEqual(x, y)           						((x) == (y))
#define IsNotEqual(a,b)		   	 					((a)!=(b))                                  //a������b����1 ���򷵻�0

#define IsInside(x1, x, x2)     					(((x1) <= (x)) && ((x) <= (x2)))
#define ABS(x, y)               					((x) > (y) ? ((x) - (y)) : ((y) - (x)))
//#define _abs_Bety(X)                       ((X)>=0?(X):-(X))

#define SysDelay(Count)     {volatile u16 i = Count; while(i--);}

//#define   EnableInterrupts      INTCONbits.GIE = 1
//#define   DisableInterrupts     INTCONbits.GIE = 0 

//#define   FeedDog()              {_asm clrwdt _endasm} //ι��

//�ź���Ч��
#define ACTIVE       1
#define NO_ACTIVE    0

//============================================================================
//							���Ͷ���
//============================================================================
typedef uint8_t 	  u8;
typedef int8_t 	    s8;
typedef uint16_t 	  u16;
typedef int16_t 		s16;
typedef uint32_t 	  u32;
typedef int32_t 	  s32;
//#define   u8 uint8_t;
//#define   s8 int8_t;
//#define   u16 uint16_t;
//#define   s16  int16_t;
//#define   u32 uint32_t;
//#define   s32 int32_t;

typedef union  
{
    u8   Byte;
    struct 
    {
        u8  B0       :1;                                       
        u8  B1       :1;                                      
        u8  B2       :1;                                    
        u8  B3       :1;                                       
        u8  B4       :1;                                   
        u8  B5       :1;                                     
        u8  B6       :1;                             
        u8  B7       :1;      				
    }Bits;
		
}uBits;

typedef union  
{

    u32   U32Data;
	float  FloatData;
		
}uFloatForm;



//=============================================================================================
//����ȫ�ֱ���
//=============================================================================================

//=============================================================================================
//�����ӿں���
//=============================================================================================

#endif

