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
static u8 USART1_Recv_falg=0;//������ɱ�־
static u8 UART_index=0; //���յ������ݸ���
 
static void Send_string(uint8_t *addr,uint8_t data_long);
/***************************************************************************
*   �� �� ��: RS232_GPIO
*   ����˵��: RS232ͨѶ��Ҫʹ�õ�GPIO�ܽų�ʼ��
*   ��    �Σ���
*   �� �� ֵ: ��
***************************************************************************/
void RS232_GPIO(void)//RS232���ƹܽų�ʼ������
{
  GPIO_InitTypeDef GPIO_InitStructure; //����һ��GPIO_InitTypeDef���͵Ľṹ��
	
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1); //PA9���ܸ���IOʱ��??????
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);//PA10���ܸ���IOʱ��???  
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //ѡ��Ҫ���Ƶ�GPIOA����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����Ҫ���Ƶ�GPIOA����Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //����GPIOA�����ٶ�
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);    //���ÿ⺯������ʼ��GPIOA
	
}

void RS232_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = 9600;//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//��У��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ͽ���ģʽ
	USART_Init(USART1, &USART_InitStructure);//����1��ʼ��
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�ʹ��
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�ʹ��
//	USART_Cmd(USART1, ENABLE);//����ʹ��
}
/*
*********************************************************************************************************
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE *f)
{  
   while((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET));
   return (USART_ReceiveData(USART1));
}

/************************ ��̳��ַ www.zxkjmcu.com ******************************/
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
//��������: USART1_Recv_Control(void)
//�������: void
//�������: void
//����ֵ  : ���յ�����
//��������: USART1_Recv��ʼ��
//ע������:
//=============================================================================
void USART1_Recv_Control(void)
{
	static u8 UART_flame_flg=0;
	
	static u8 UART_head_flg=0;
	static u8 Wate_byte=0;
	u8 ReadUARTData=0;
	
if(sending_flg)  //��������
		{ 
				if(USART_GetITStatus(USART1,USART_IT_TC) != RESET)//��������ж�
				{
						USART_ClearFlag(USART1, USART_IT_TC);//���жϱ�־λ
						if(USART_send_cnt==USART_SEND_LONG)//���ݷ������
						{
								sending_flg=0;
								USART_send_cnt=0;
								USART_ITConfig(USART1, USART_IT_TC, DISABLE);//���ڷ�������ж�
						}
						else//��������û����
						{
								USART_SendData(USART1,rs232_Buf_try[USART_send_cnt++]);
						}
				}
		}
//===============��ȡ����================================================================================	
	//	if((USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)&(USART1_Recv_falg == 0))
		if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
		{
					rs232_Buf_ry[UART_index++] =USART_ReceiveData(USART1);	//������3�մ��ڽ��ջ����ڻ��ÿһ���ַ�								
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
			if(USART1_Recv_falg)//�����ݵ��뻺��
			{
				for(i=0;i<UART_index;i++)
				pRxdata[i] =	rs232_Buf_ry[i];
		rs232_Buf_ry[0]=0;
				len=UART_index; 
				UART_index=0; //�±�����
				USART1_Recv_falg=0;	//�������
			}

		if(((0x55!=pRxdata[0])|(0xaa!=pRxdata[1])|(0x00!=pRxdata[2])|(0x00!=pRxdata[3])
			|(0xaa!=pRxdata[4])|(0x55!=pRxdata[5])))//����Ϊ������ ���ݵĽ�����ͷ��������
		{
			pRxdata[0] = 0;
			return 0;
		}
	   CCITT_CRC16Init(&pRxdata[6],(len-8)); //����Ч��ֵ
		RXCRC1 = (u16)CCITT_CRC16 & 0xffff;
			RxCrc = pRxdata[len-1];
			RxCrc = (RxCrc << 8)  + pRxdata[len-2];//ʵ��Ч��ֵ
		if(	RXCRC1!=RxCrc)//Ч�鲻��ȷ�˳�
		return 0;


		if((len <= 2)||(len >= 244))
		{
			return 0;
		}
//���ݴ�����

		funCode = pRxdata[6];            //��д����
		addr = pRxdata[8];
		addr = (addr << 8) + pRxdata[7]; //�Ĵ�����ַ
		dataLen = pRxdata[9];
		if(funCode==0)//��ָ��
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
		else if (funCode==1)//дָ��
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
		
		CCITT_CRC16Init(&rs232_Buf_try[6],(dataLen+4)); //����Ч��ֵ
		RXCRC1 = (u16)CCITT_CRC16 & 0xffff;
		
		
		rs232_Buf_try[dataLen+10]    = (u8)(RXCRC1);
		rs232_Buf_try[dataLen+11]  = (u8)((RXCRC1)>>8);	
		
		Send_string(rs232_Buf_try,(dataLen+9+3));

  	return 1;
}
////////////////////////////////
/*Ҫ����ȫ�ֱ��USART_SEND_LONG�sending_flg USART_send_cnt */
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