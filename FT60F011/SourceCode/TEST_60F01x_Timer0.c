//********************************************************* 
/*  文件名：TEST_60F01x_Timer0.c
*	功能：  FT60F01x-Timer0功能演示
*   IC:    FT60F011A SOP8
*   晶振：  16M/4T                    
*   说明：  DemoPortOut输出30Hz占空比50%的波形-Timer0实现

*                  FT60F011A  SOP8 
*                 ----------------
*  VDD-----------|1(VDD)    (GND)8|------------GND     
*  NC------------|2(PA2)    (PA4)7|----DemoPortOut 
*  NC------------|3(PA1)    (PA5)6|-------------NC
*  NC------------|4(PA3)    (PA0)5|-------------NC
*			      ----------------
*/
//*********************************************************
#include "SYSCFG.h"

//**********************************************************
//***********************宏定义*****************************
#define  unchar     unsigned char 
#define  unint      unsigned int
#define  unlong     unsigned long

#define  DemoPortOut	RA4   
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
//********************************************************
//**************中断处理**********************************
void interrupt ISR(void)			//PIC_HI-TECH使用
{ 
   
  //定时器0的中断处理**********************
	if(T0IE && T0IF)				//16.384ms翻转一次≈30Hz
	{
		//TMR0 = 100;               //注意:对TMR0重新赋值TMR0在两个周期内不变化
		 
		T0IF = 0;
		DemoPortOut = ~DemoPortOut; //翻转电平
	} 
} 
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{
	 
	OSCCON = 0B01110001;	//WDT 32KHZ IRCF=111=16MHZ/4=4MHZ,0.25US/T
					 		//Bit0=1,系统时钟为内部振荡器(60F01x保留位)
					 		//Bit0=0,时钟源由FOSC<2：0>决定即编译选项时选择
	INTCON = 0;  			//暂禁止所有中断
	PORTA = 0B00000000;		
	TRISA = 0B11101111;		//PA输入输出 0-输出 1-输入
							//PA4->输出
	WPUA = 0B00000100;     	//PA端口上拉控制 1-开上拉 0-关上拉

	OPTION = 0B00001000;	//Bit3=1 WDT MODE,PS=000=1:1 WDT RATE
							//Bit3 预分频器分配位 0-Timer0 1-WDT 
							//Bit7(PAPU)=0 ENABLED PULL UP PA
	MSCKCON = 0B00000000;   //Bit6->0,禁止PA4, PC5稳压输出(60F01x保留位)
					  		//Bit5->0,TIMER2时钟为Fosc(60F01x保留位)
					  		//Bit4->0,禁止LVR(60F01x O版之前)
                            //Bit4->0, LVREN使能时,开启LVR(60F01x O版及O版后)  
	                        //Bit4->1, LVREN使能时,工作时开启LVR, 睡眠时自动关闭LVR(60F01x O版及O版之后)       
}
/*----------------------------------------------------
 *	函数名称：  TIMER0_INITIAL
 *	功能：     初始化设置定时器v
 *	相关寄存器：T0CS T0SE PSA 
 *	设置TMR0定时时长16.384ms=(1/16000000)*4*256*255(16M-4T-PSA 1:256- TMR0=255溢出)	                      
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000111;	
						    //Bit5 T0CS Timer0时钟源选择 
							//1-外部引脚电平变化T0CKI 0-内部时钟(FOSC/2)
							//Bit4 T0CKI引脚触发方式 1-下降沿 0-上升沿
							//Bit3 PSA 预分频器分配位 0-Timer0 1-WDT 
							//Bit2:0 PS2 8个预分频比 111 - 1:256
	T0IF = 0;				//清空T0软件中断
}
/*----------------------------------------------------
 *	函数名称：DelayUs
 *	功能：   短延时函数 --16M-4T--大概快1%左右.
 *	输入参数：Time延时时间长度 延时时长Time*2Us
 *	返回参数：无 
----------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}                  
/*---------------------------------------------------- 
 *	函数名称：DelayMs
 *	功能：   短延时函数
 *	输入参数：Time延时时间长度 延时时长Time ms
 *	返回参数：无 
 ----------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(98); 	//快1%
		}
	}
}
/*----------------------------------------------------
 *	函数名称：DelayS
 *	功能：   短延时函数
 *	输入参数：Time延时时间长度 延时时长Time S
 *	返回参数：无 
 ----------------------------------------------------*/
void DelayS(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<10;b++)
		{
		 	DelayMs(100); 	
		}
	}
}
/*-------------------------------------------------
 *  函数名: main 
 *	功能：  主函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();		//系统初始化
	TIMER0_INITIAL(); 
	GIE = 1; 				//开中断
	T0IE = 1;				//开定时器/计数器0中断
	while(1)
	{
		NOP();
	}
}