#include "pbdata.h"   //引入自定义公共头文件

/****************************************************************************
* 名    称：delay_us(u32 nus)
* 功    能：微秒延时函数
* 入口参数：u32  nus
* 出口参数：无
* 说    明：输入范围(1~1000)ns
* 调用方法：无 
****************************************************************************/ 
void delay_us(uint32_t nus)
{
	volatile uint32_t temp;
	 SysTick->LOAD = 6*nus;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}

/****************************************************************************
* 名    称：delay_ms(u16 nms)
* 功    能：毫秒延时函数
* 入口参数：u16 nms
* 出口参数：无
* 说    明：输入范围(1~1000)ms
* 调用方法：无 
****************************************************************************/ 
void delay_ms(uint16_t nms)
{
	volatile uint32_t temp;
	 SysTick->LOAD = 6000*nms;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}

void Delay10ms( void )
{
	uint32_t i;
	for( i = 0; i < (uint32_t)48100; i++ );
}
/************************ 论坛地址 www.zxkjmcu.com *******************************/
