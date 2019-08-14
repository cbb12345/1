
/****************************************Copyright (c)****************************************************
**                               ��������ȫ���ܼ������޹�˾                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Local_232_module.h                                                          **
** Last modified Date:      2012.3.2                                                                    **
** Last Version:            v1.0                                                                        **
** Description:             ����232ͨ��ͷ�ļ�                                                           **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              ����                                                                        **
** Created date:            2012.3.2                                                                    **
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


#ifndef _LOCAL_232_MODULE_H

#define _LOCAL_232_MODULE_H

#include "Common.h"

/*------------------------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------------------------*/
#pragma pack(1)

typedef struct _LOCAL_TRANS_DATA
{
 unsigned char set_mark;    
 //unsigned char pcAckMark;   // if pcAckMark = 1;have data  to send from waitpcsendbuf
 //SBUF_STR  send_buf;
 SBUF_STR  rec_buf;
 STimer	local_timer;		//local��ʱ��
 //unsigned char sd_endflag;//���ͽ�����־
}SLOCAL_TRANS_DATA,*PSLOCAL_TRANS_DATA;

#pragma pack()
/*------------------------------------------------------------------------------------------------------*/

//void Uart1_init(unsigned long ulBautRate);
//void Uart1_init(void);//unsigned char bpstype);
void RS232_init(void);//unsigned char bpstype);


//void UART1Send( unsigned char *pucBuffer, unsigned int ucCount );
void RS232Send( unsigned char *pucBuffer, unsigned int ucCount );


void Local_232_module(void);

#endif

