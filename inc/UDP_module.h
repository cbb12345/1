
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               UDP_module.h                                                                **
** Last modified Date:      2012.3.2                                                                    **
** Last Version:            v1.0                                                                        **
** Description:             UDP通信模块头文件                                                           **
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


#ifndef _UIP_MODULE_H

#define _UIP_MODULE_H

#include "Common.h"
#include "lwip/arch.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"



/*------------------------------------------------------------------------------------------------------*/
#pragma pack(1)

typedef struct _UIP_TRANS_DATA
{
	//unsigned char set_mark;    
	//SBUF_STR  send_buf;
	SBUF_STR  rec_buf;
	STimer	UIP_timer;		
}SUIP_TRANS_DATA,*PUIP_TRANS_DATA;
#pragma pack()

/*------------------------------------------------------------------------------------------------------*/

//void lwip_init(void);
void UDP_init(void);
void UDP_Receive(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 struct ip_addr *addr, u16_t port);
//void uip_app(void);
//void UDP_sendData( unsigned char *pucBuffer, unsigned int ucCount );
void UDP_sendData(struct udp_pcb *UdpPcb, struct ip_addr destAddr, u16_t port,unsigned char *pucBuffer, unsigned int ucCount );
void Udp_BroadCast(void);

#endif

