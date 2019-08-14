
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               WG.c                                                                        **
** Last modified Date:      2012.7.24                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             WG通信模块                                                                 **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              刘飞                                                                        **
** Created date:            2012.7.24                                                                   **
** Version:                 v1.0                                                                        **
** Descriptions:            The original version 初始版本                                               **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Modified by:                                                                                         **
** Modified date:                                                                                       **
** Version:                                                                                             **
** Description:                                                                                         **
**                                                                                                      **
*********************************************************************************************************/

#include "WG.h"
#include "common.h"
#include "Main.h"
#include "stm32f107.h"

extern SDevice g_device;

//////////////////////////////////////////////////////////////////////////////////////////////////////////


//脉冲时间宽度 单位为10us
//脉冲时间间隔 单位为10us


void packetDataWG26(unsigned char *ch)
{
    int i;
	char even,odd;
	unsigned char one_num = 0,check_temp;//,buf[4];
	long buf = 0;
	//memcpy((unsigned char *)&buf, ch, 4);
	for(i = 0; i< 4; i++)
	{
		buf <<= 8;
		//buf += ch[3 - i];
		buf += ch[i];
	}
    WGDATA0SET;
    WGDATA1SET;
	check_temp = (*ch);
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 1));
	check_temp >>= 4;
	for(i = 0; i< 4; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (even = 0) : (even = 1);
	check_temp = (*(ch + 1));
	one_num = 0;
	for(i = 0; i< 4; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 2));
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (odd = 1) : (odd = 0);
	if(even)
	{
	    WGDATA1CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET;
	}

	else
	{
	    WGDATA0CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET;
	}
	//udelay(g_device.AutoParam.InterverOfWiegand * 10);
	//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
	Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	for(i = 0; i< 24; i++)
	{
	    WGDATA0SET;
	    WGDATA1SET;
		//if((*ch) & 0x80)
		//if(buf & 0x80)
		if(buf & 0x80000000)
		{
			WGDATA1CLR;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA1SET;
		}
		else
		{
			WGDATA0CLR;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA0SET;
		}
		//(*(long*)ch) <<= 1;
		buf <<= 1;
		//buf >>= 1;
		//udelay(g_device.AutoParam.InterverOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	}
	WGDATA0SET;
	WGDATA1SET;
	if(odd)
	{
	    WGDATA1CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET;
	}

	else
	{
	    WGDATA0CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET;
	}
    #if 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER1_BASE, TIMER_A, (SysCtlClockGet()/1000000));	
	IntEnable(INT_TIMER1A); 	
	//关掉其它所有中断	
	UARTIntDisable(UART0_BASE, UART_INT_RX | UART_INT_RT);	
	UARTIntDisable(UART1_BASE, UART_INT_RX | UART_INT_RT);	
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH); /* 使能以太网控制器*/	
	IntDisable(INT_GPIOA);	
	SysTickDisable();
	I2CMasterIntDisable(I2C0_MASTER_BASE);	
	//SysTickIntDisable();	//开启定时器    
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);    
	TimerEnable(TIMER1_BASE, TIMER_A);
	WGTimerEnable = 1;
/////
    //WGTimer
//g_device.workpara.WidthOfWiegand;
//g_device.workpara.InterverOfWiegand;

////
    WGTimerEnable = 0;
    TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);	 
	TimerDisable(TIMER1_BASE, TIMER_A);	
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);	
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);	
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH); /* 使能以太网控制器*/	
	IntEnable(INT_GPIOA);	
	SysTickEnable();
	I2CMasterIntEnable(I2C0_MASTER_BASE);	
	//SysTickIntEnable();
	#endif
}

void packetDataWG34(unsigned char *ch)
{
    int i;
	char even,odd;
	unsigned char one_num = 0,check_temp;
	long buf = 0;
	for(i = 0; i< 4; i++)
	{
		buf <<= 8;
		//buf += ch[3 - i];
		buf += ch[i];
	}
    WGDATA0SET;
    WGDATA1SET;
	check_temp = (*ch);
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 1));
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (even = 0) : (even = 1);
	check_temp = (*(ch + 2));
	one_num = 0;
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 3));
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (odd = 1) : (odd = 0);
	if(even)
	{
	    WGDATA1CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET;
	}

	else
	{
	    WGDATA0CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET;
	}
	//udelay(g_device.AutoParam.InterverOfWiegand * 10);
	//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
	Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	for(i = 0; i< 32; i++)
	{
	    WGDATA0SET;
	    WGDATA1SET;
		//if((*ch) & 0x80)
		if(buf & 0x80000000)
		{
			WGDATA1CLR;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA1SET;
		}
		else
		{
			WGDATA0CLR;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA0SET;
		}
		//(*(long*)ch) <<= 1;
		buf <<= 1;
		//udelay(g_device.AutoParam.InterverOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	}
	WGDATA0SET;
	WGDATA1SET;
	if(odd)
	{
	    WGDATA1CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET;
	}

	else
	{
	    WGDATA0CLR;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET;
	}
}

///
void packetDataWG261(unsigned char *ch)
{
    int i;
	char even,odd;
	unsigned char one_num = 0,check_temp;//,buf[4];
	long buf = 0;
	//memcpy((unsigned char *)&buf, ch, 4);
	for(i = 0; i< 4; i++)
	{
		buf <<= 8;
		//buf += ch[3 - i];
		buf += ch[i];
	}
    WGDATA0SET1;
    WGDATA1SET1;
	check_temp = (*ch);
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 1));
	check_temp >>= 4;
	for(i = 0; i< 4; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (even = 0) : (even = 1);
	check_temp = (*(ch + 1));
	one_num = 0;
	for(i = 0; i< 4; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 2));
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (odd = 1) : (odd = 0);
	if(even)
	{
	    WGDATA1CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET1;
	}

	else
	{
	    WGDATA0CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET1;
	}
	//udelay(g_device.AutoParam.InterverOfWiegand * 10);
	//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
	Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	for(i = 0; i< 24; i++)
	{
	    WGDATA0SET1;
	    WGDATA1SET1;
		//if((*ch) & 0x80)
		//if(buf & 0x80)
		if(buf & 0x80000000)
		{
			WGDATA1CLR1;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA1SET1;
		}
		else
		{
			WGDATA0CLR1;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA0SET1;
		}
		//(*(long*)ch) <<= 1;
		buf <<= 1;
		//buf >>= 1;
		//udelay(g_device.AutoParam.InterverOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	}
	WGDATA0SET1;
	WGDATA1SET1;
	if(odd)
	{
	    WGDATA1CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET1;
	}

	else
	{
	    WGDATA0CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET1;
	}
    #if 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER1_BASE, TIMER_A, (SysCtlClockGet()/1000000));	
	IntEnable(INT_TIMER1A); 	
	//关掉其它所有中断	
	UARTIntDisable(UART0_BASE, UART_INT_RX | UART_INT_RT);	
	UARTIntDisable(UART1_BASE, UART_INT_RX | UART_INT_RT);	
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH); /* 使能以太网控制器*/	
	IntDisable(INT_GPIOA);	
	SysTickDisable();
	I2CMasterIntDisable(I2C0_MASTER_BASE);	
	//SysTickIntDisable();	//开启定时器    
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);    
	TimerEnable(TIMER1_BASE, TIMER_A);
	WGTimerEnable = 1;
/////
    //WGTimer
//g_device.workpara.WidthOfWiegand;
//g_device.workpara.InterverOfWiegand;

////
    WGTimerEnable = 0;
    TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);	 
	TimerDisable(TIMER1_BASE, TIMER_A);	
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);	
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);	
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH); /* 使能以太网控制器*/	
	IntEnable(INT_GPIOA);	
	SysTickEnable();
	I2CMasterIntEnable(I2C0_MASTER_BASE);	
	//SysTickIntEnable();
	#endif
}

void packetDataWG341(unsigned char *ch)
{
    int i;
	char even,odd;
	unsigned char one_num = 0,check_temp;
	long buf = 0;
	for(i = 0; i< 4; i++)
	{
		buf <<= 8;
		//buf += ch[3 - i];
		buf += ch[i];
	}
    WGDATA0SET1;
    WGDATA1SET1;
	check_temp = (*ch);
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 1));
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (even = 0) : (even = 1);
	check_temp = (*(ch + 2));
	one_num = 0;
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	check_temp = (*(ch + 3));
	for(i = 0; i< 8; i++)
	{
	    if(check_temp & 0x01)
	    {
	        one_num++;
	    }
		check_temp >>= 1;
	}
	one_num % 2 == 0 ? (odd = 1) : (odd = 0);
	if(even)
	{
	    WGDATA1CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET1;
	}

	else
	{
	    WGDATA0CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET1;
	}
	//udelay(g_device.AutoParam.InterverOfWiegand * 10);
	//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
	Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	for(i = 0; i< 32; i++)
	{
	    WGDATA0SET1;
	    WGDATA1SET1;
		//if((*ch) & 0x80)
		if(buf & 0x80000000)
		{
			WGDATA1CLR1;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA1SET1;
		}
		else
		{
			WGDATA0CLR1;
			//udelay(g_device.AutoParam.WidthOfWiegand * 10);
			//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
			Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
			WGDATA0SET1;
		}
		//(*(long*)ch) <<= 1;
		buf <<= 1;
		//udelay(g_device.AutoParam.InterverOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.InterverOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.InterverOfWiegand * 5);
	}
	WGDATA0SET1;
	WGDATA1SET1;
	if(odd)
	{
	    WGDATA1CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA1SET1;
	}

	else
	{
	    WGDATA0CLR1;
		//udelay(g_device.AutoParam.WidthOfWiegand * 10);
		//SysCtlDelay(g_device.AutoParam.WidthOfWiegand * 10 * (SysCtlClockGet()/3000000));
		Delay2us(g_device.AutoParam.WidthOfWiegand * 5);
		WGDATA0SET1;
	}
}




