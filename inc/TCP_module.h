
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


#ifndef _TCP_MODULE_H

#define _TCP_MODULE_H

#include "lwip/err.h"
#include "Common.h"
#include "lwip/arch.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"


#pragma pack(1)

typedef struct _TCP_TRANS_DATA
{
	SBUF_STR  rec_buf;
	STimer	TCP_timer;		
}STCP_TRANS_DATA,*PTCP_TRANS_DATA;
#pragma pack()



void TCP_init(void);
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void Tcp_Client_Check(void);
void TCP_sendData(struct tcp_pcb *TcpPcb,unsigned char *pucBuffer, unsigned int ucCount );
void tcp_client_connection_close(struct tcp_pcb *tpcb);
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);

#endif

