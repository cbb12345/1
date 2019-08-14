
/****************************************Copyright (c)****************************************************
**                               ��������ȫ���ܼ������޹�˾                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Local_485_module.c                                                          **
** Last modified Date:      2014.8.01                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             ����232ͨ��ģ��                                                             **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              ����                                                                        **
** Created date:            2012.7.12                                                                   **
** Version:                 v1.0                                                                        **
** Descriptions:            The original version ��ʼ�汾                                               **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Modified by:                                                                                         **
** Modified date:                                                                                       **
** Version:                                                                                             **
** Description:                                                                                         **
**                                                                                                      **
*********************************************************************************************************/
/*
   ��Э����Լ򵥣����ݽṹû��ʵ����Խ�ԡ�û��Ҫ�����Ͻ�������Ǵ���Э�飬�ɿ��ǽ�ģ�������ϸ�ֲ㡣
   �˴�����Ǿ�����ģ�黯����׷��ֲ�Ч����
*/

//#include "Common.h"
#include "Local_485_module.h"
#include "stm32f10x.h"
#include "Main.h"
#include "ParamItem.h"
#include <string.h>


/*------------------------------------------------------------------------------------------------------*/
//#define SYSTICKHZ               100
//#define SYSTICKMS               (1000 / SYSTICKHZ)
//#define SYSTICKUS               (1000000 / SYSTICKHZ)
//#define SYSTICKNS               (1000000000 / SYSTICKHZ)


#define	LOCAL_485REV_TIMER	50 //500MS

/*------------------------------------------------------------------------------------------------------*/
SLOCAL_485_DATA local485;
extern SDevice g_device;
extern int SelfCheckFailure;

/*------------------------------------------------------------------------------------------------------*/

void RS485_init(void)//unsigned char bpstype)
{
    unsigned long ulBautRate;// = 0;
	USART_InitTypeDef USART_InitStructure;
	switch(g_device.workpara.BaudRate)//bpstype)
	{
	    case 0x04:
			ulBautRate = 9600;
			break;
		case 0x05:
			ulBautRate = 19200;
			break;
		case 0x06:
			ulBautRate = 38400;
			break;
		case 0x07:
			ulBautRate = 57600;
			break;
		case 0x08:
			ulBautRate = 115200;
			break;
		default:
			ulBautRate = 115200;
			SelfCheckFailure = 3;
	}
    //�ṹ���ʼ��
  memset(&local485, 0, sizeof(local485));

	USART_InitStructure.USART_BaudRate            = ulBautRate  ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);     
}

void RS485Send( unsigned char *pucBuffer, unsigned int ucCount )
{
	Disable485;//Enable485;
    if(ucCount >= BUF_LEN)
		return;
    while(ucCount--)
    {
		while (!(USART1->SR & USART_FLAG_TXE));
   		USART1->DR = (*pucBuffer++ & (uint16_t)0x01FF);
    }
    //��ʵ��Ҫ��Ҫ��ʱ
    DelayMs(20);//(50);
    Enable485;//Disable485;
}

//void RS232IntHandler( void )
void USART1_IRQHandler(void)
{
	//u8 c;
	unsigned char tmpChar;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{ 	
	    //c = USART4->DR;
		tmpChar = (unsigned char )USART_ReceiveData(USART1);
		if(local485.set_mark == 0)
		{
			if((local485.rec_buf.index == 0) && (tmpChar == BootCode))
			{
				local485.rec_buf.buf[local485.rec_buf.index++] = tmpChar;
				timerStart(&local485.local_timer,LOCAL_485REV_TIMER);
			}
			else if(local485.rec_buf.index != 0)
			{
				if(local485.rec_buf.index > BUF_LEN)
				{
					local485.rec_buf.index = 0;
					local485.rec_buf.len = 0;
					timerStop(&local485.local_timer);
				}
				else
				{
					local485.rec_buf.buf[local485.rec_buf.index] = tmpChar;
					if(local485.rec_buf.index > (unsigned int)(local485.rec_buf.buf[1]))
					{
						local485.rec_buf.len = local485.rec_buf.index + 1;
						local485.rec_buf.index = 0;
						local485.set_mark = 1;
						timerStop(&local485.local_timer);
					}
					else
						local485.rec_buf.index++;
				}
			}
		}
	} 
}

void Local_485_module(void)
{
	if(timerIsOverflow(&local485.local_timer)) 
	{
		  memset(&local485, 0, sizeof(local485));
          //timerStop(&local.local_timer);
    }
	if( 0 != local485.set_mark)
	{
	    //�򵥵�Э�鳤���ж� �����𣬳��ȣ�������ݣ�У��
	    if(( local485.rec_buf.len < 4) || ( local485.rec_buf.len > BUF_LEN))
	    {
	        //local.set_mark = 0;
			memset(&local485, 0, sizeof(local485));
			return;
	    }
		//Э�����
		local485.set_mark = 0;
		if(ERRORNORETURN != processParamTable(SourceFrom485, local485.rec_buf.buf, (unsigned int)local485.rec_buf.len))
		{
			local485.rec_buf.len = local485.rec_buf.buf[1] + 2;
			//UART0Send(local.rec_buf.buf ,local.rec_buf.len);
			RS485Send(local485.rec_buf.buf ,local485.rec_buf.len);
		}
	}
}

