
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Local_485_module.h                                                          **
** Last modified Date:      2012.3.2                                                                    **
** Last Version:            v1.0                                                                        **
** Description:             串口485通信头文件                                                           **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              刘飞                                                                        **
** Created date:            2012.3.2                                                                    **
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


#ifndef _LOCAL_485_MODULE_H

#define _LOCAL_485_MODULE_H

#include "Common.h"

/*------------------------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------------------------*/

#pragma pack(1)
typedef struct _LOCAL_485_DATA
{
 unsigned char set_mark;    
 //unsigned char pcAckMark;   // if pcAckMark = 1;have data  to send from waitpcsendbuf
 //SBUF_STR  send_buf;
 SBUF_STR  rec_buf;
 STimer	local_timer;		//local定时器
 //unsigned char sd_endflag;//发送结束标志
}SLOCAL_485_DATA,*PSLOCAL_485_DATA;

#pragma pack()

/*------------------------------------------------------------------------------------------------------*/

void RS485_init(void);
void RS485Send( unsigned char *pucBuffer, unsigned int ucCount );

void Local_485_module(void);

#endif

