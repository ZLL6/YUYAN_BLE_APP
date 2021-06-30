#include "pbdata.h"
#include "data.h"
///////////////////////////////////////////////
#define           CRC_CCITT_INIT               0xFFFF
#define         CRC_CCITT_POLY            0x1021U
#define rs232_Buf_Max 250

uint16_t CCITT_CRC16=0;
uint8_t rs232_Buf=0;
uint8_t rs232_Buf_count=0;
uint8_t rs232_Buf_try[rs232_Buf_Max];
uint8_t rs232_Buf_ry[250];
//PC_typedef PC_dat;
static u8 sending_flg=0;
static u8 USART_send_cnt=0;
static u8 USART_SEND_LONG;
static u8 USART1_Recv_falg=0;//接收完成标志
static u8 UART_index=0; //接收到的数据个数
 
static void Send_string(uint8_t *addr,uint8_t data_long);
/***************************************************************************
*   函 数 名: RS232_GPIO
*   功能说明: RS232通讯需要使用的GPIO管脚初始化
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void RS232_GPIO(void)//RS232控制管脚初始化函数
{
  GPIO_InitTypeDef GPIO_InitStructure; //定义一个GPIO_InitTypeDef类型的结构体
	
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1); //PA9功能复用IO时钟??????
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);//PA10功能复用IO时钟???  
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //选择要控制的GPIOA引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //设置要控制的GPIOA引脚为输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //设置GPIOA引脚速度
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);    //调用库函数，初始化GPIOA
	
}

void RS232_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = 9600;//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//发送接收模式
	USART_Init(USART1, &USART_InitStructure);//串口1初始化
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//串口中断使能
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//串口中断使能
//	USART_Cmd(USART1, ENABLE);//串口使能
}
/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(uint8_t)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
	return ch;
}
/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{  
   while((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET));
   return (USART_ReceiveData(USART1));
}

/************************ 论坛地址 www.zxkjmcu.com ******************************/
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
//函数名称: USART1_Recv_Control(void)
//输入参数: void
//输出参数: void
//返回值  : 接收到数据
//功能描述: USART1_Recv初始化
//注意事项:
//=============================================================================
void USART1_Recv_Control(void)
{
	static u8 UART_flame_flg=0;
	
	static u8 UART_head_flg=0;
	static u8 Wate_byte=0;
	u8 ReadUARTData=0;
	
if(sending_flg)  //发送数据
		{ 
				if(USART_GetITStatus(USART1,USART_IT_TC) != RESET)//发送完成中断
				{
						USART_ClearFlag(USART1, USART_IT_TC);//清中断标志位
						if(USART_send_cnt==USART_SEND_LONG)//数据发送完成
						{
								sending_flg=0;
								USART_send_cnt=0;
								USART_ITConfig(USART1, USART_IT_TC, DISABLE);//串口发送完成中断
						}
						else//还有数据没发完
						{
								USART_SendData(USART1,rs232_Buf_try[USART_send_cnt++]);
						}
				}
		}
//===============读取数据================================================================================	
	//	if((USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)&(USART1_Recv_falg == 0))
		if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
		{
					rs232_Buf_ry[UART_index++] =USART_ReceiveData(USART1);	//缓冲器3收串口接收缓存内获得每一个字符								
			}
		if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
		{
		//	UART_index=0;
			 USART_ClearITPendingBit(USART1,USART_IT_IDLE); 
			USART1_Recv_falg=1;
		}
		
	 }
	
//=============================================================================

//=============================================================================
u8 Uart1_Task(void)
{
		u16 crc = 0;
		u16 RxCrc = 0;
	  u16 RXCRC1 =0;
		u16  addr = 0;
	   u8 ModBusAddr=0;
		u8  funCode = 0;
	  u8  command_data=0;
		u16  command = 0;
		u8  dataLen = 0;
		u16 temp = 0;
		u16 ModBusDataLen=0;
	  u8 pRxdata[244];
	  u8 len=0;
		u8   i = 0;
			if(USART1_Recv_falg)//把数据倒入缓存
			{
				for(i=0;i<UART_index;i++)
				pRxdata[i] =	rs232_Buf_ry[i];
		rs232_Buf_ry[0]=0;
				len=UART_index; 
				UART_index=0; //下标清零
				USART1_Recv_falg=0;	//允许接收
			}

		if(((0x55!=pRxdata[0])|(0xaa!=pRxdata[1])|(0x00!=pRxdata[2])|(0x00!=pRxdata[3])
			|(0xaa!=pRxdata[4])|(0x55!=pRxdata[5])))//数据为空跳出 数据的接收码头不对跳出
		{
			pRxdata[0] = 0;
			return 0;
		}
	   CCITT_CRC16Init(&pRxdata[6],(len-8)); //计算效验值
		RXCRC1 = (u16)CCITT_CRC16 & 0xffff;
			RxCrc = pRxdata[len-1];
			RxCrc = (RxCrc << 8)  + pRxdata[len-2];//实际效验值
		if(	RXCRC1!=RxCrc)//效验不正确退出
		return 0;


		if((len <= 2)||(len >= 244))
		{
			return 0;
		}
//数据处理部分

		funCode = pRxdata[6];            //读写命令
		addr = pRxdata[8];
		addr = (addr << 8) + pRxdata[7]; //寄存器地址
		dataLen = pRxdata[9];
		if(funCode==0)//读指令
		{
			if((addr >= 0x2000) && ((addr + dataLen) <=0x4000))
			{
				temp = (u16)((u16*)&BMS_config.BMS_Read + (addr - 0x2000));
				if(temp)
				{
					for(i = 0;i < (dataLen);i++)
					{
						rs232_Buf_try[i+10] = *(((u8*)(&temp))+ i );
          //  rs232_Buf_try[i+10] = 0x05;
					}
				}
			}
			
		}
		else if (funCode==1)//写指令
		{
			
			
		}
		else
		{
			
		}
		rs232_Buf_try[0] = 0x55;
		rs232_Buf_try[1] = 0xaa;
		rs232_Buf_try[2] = 0x00;
		rs232_Buf_try[3] = 0x00;
		rs232_Buf_try[4] = 0xaa;
		rs232_Buf_try[5] = 0x55; 
		rs232_Buf_try[6] = 0x01; 	
		rs232_Buf_try[7] = pRxdata[7];
    rs232_Buf_try[8] = pRxdata[8];		
    rs232_Buf_try[9] = dataLen;	
		
		CCITT_CRC16Init(&rs232_Buf_try[6],(dataLen+4)); //计算效验值
		RXCRC1 = (u16)CCITT_CRC16 & 0xffff;
		
		
		rs232_Buf_try[dataLen+10]    = (u8)(RXCRC1);
		rs232_Buf_try[dataLen+11]  = (u8)((RXCRC1)>>8);	
		
		Send_string(rs232_Buf_try,(dataLen+9+3));

  	return 1;
}
////////////////////////////////
/*要定义全局变罸SART_SEND_LONGending_flg USART_send_cnt */
///////////////////////////////////////////////////////
static void Send_string(uint8_t *addr,uint8_t data_long)
{
  uint8_t tem_i;
	while(sending_flg);
	USART_SEND_LONG=data_long;
  USART_ClearFlag(USART1, USART_IT_TC);
  USART_SendData(USART1,addr[USART_send_cnt++]);
  sending_flg =1;
  USART_ITConfig(USART1, USART_IT_TC, ENABLE);
}	