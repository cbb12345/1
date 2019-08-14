
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               UDP_module.c                                                                **
** Last modified Date:      2012.3.9                                                                    **
** Last Version:            v1.0                                                                        **
** Description:             UDP通信模块                                                                 **
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
#include "stm32_eth.h"
#include <string.h>
#include "Main.h"
#include "UDP_module.h"
#include "TCP_module.h"
#include "Common.h"
#include "ParamItem.h"


#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "netconf.h"
#include <stdio.h>

STCP_TRANS_DATA tcp_check;
extern SDevice g_device;
extern int SelfCheckFailure;
struct tcp_pcb *TcpPcb;//创建一个pcb控制块
u8 tcp_client_flag;//0：没有连接服务器 1：连接上服务器了


/*
*创建一个连接
*/
void TCP_init(void)
{
	struct ip_addr ipaddr; //IP 
	
	unsigned short ccc;
	unsigned short remote_port;
	ccc = g_device.workpara.ReaderPortHigh << 8 ;
	ccc += g_device.workpara.ReaderPortLow;
	
	remote_port = g_device.AutoParam.PCPortHigh << 8 ;
	remote_port += g_device.AutoParam.PCPortLow;
		
	
	TcpPcb = tcp_new();//分配一个控制块
	
	//分配失败，后面的可以不执行的，实际没这个必要，暂不管这些
	if (TcpPcb == NULL)
		SelfCheckFailure = 4;
	
	if(TcpPcb!=NULL)
	{
		tcp_bind(TcpPcb,IP_ADDR_ANY,ccc);   /* 绑定本地IP地址 */
		
		IP4_ADDR(&ipaddr,g_device.AutoParam.PCIP1,g_device.AutoParam.PCIP2,g_device.AutoParam.PCIP3,g_device.AutoParam.PCIP4);  //server地址
		//创建连接并注册连接成功呢的回调函数
		tcp_connect(TcpPcb,&ipaddr,remote_port,tcp_client_connected);//注册回调函数
	}
}


//TCP连接建立后调用的回调函数
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	
	if(err==ERR_OK)
	{ 
		tcp_client_flag=1; 				//标记连接到服务器了
		
		tcp_recv(tpcb,tcp_client_recv);  	//初始化LwIP的tcp_recv回调功能
		tcp_poll(tpcb,tcp_client_poll,1); 	//初始化LwIP的tcp_poll回调功能		
		err=ERR_OK;
	}
	else
	{
		tcp_client_connection_close(tpcb);//关闭连接
	}
	
	 return err; 
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	STCP_TRANS_DATA tcp;
	
	LWIP_ASSERT("arg != NULL",arg != NULL);
	
	if(p==NULL)
	{
		tcp_client_flag=0;//需要关闭TCP 连接了 
	}
	else if(err!=ERR_OK)//当接收到一个非空的数据帧,但是err!=ERR_OK
	{
		if(p)
			pbuf_free(p);//释放接收pbuf
	}
	else if(tcp_client_flag==1)	//当处于连接状态时
	{
		if(p!=NULL)
		{
			tcp_recved(tpcb, p->tot_len);
						
			memcpy(&tcp.rec_buf,p->payload,p->len);
			
			tcp.rec_buf.len=p->len;
			
			//简单的协议长度判断 引导吗，长度，命令，数据，校验
			if(( tcp.rec_buf.len < 4) || ( tcp.rec_buf.len > BUF_LEN))
			{
			}	
			//协议解析
			else if(ERRORNORETURN != processParamTable(SourceFromTCP, tcp.rec_buf.buf, (unsigned int)tcp.rec_buf.len))
			{
				tcp.rec_buf.len = tcp.rec_buf.buf[1] + 2;
			
				TCP_sendData(tpcb, tcp.rec_buf.buf, tcp.rec_buf.len);
			}	
			
			pbuf_free(p);
		}	
	}
	
	return ERR_OK;
}

void TCP_sendData(struct tcp_pcb *TcpPcb,unsigned char *pucBuffer, unsigned int ucCount )
{
	tcp_write(TcpPcb, pucBuffer, ucCount, 1);
}


//关闭与服务器的连接
void tcp_client_connection_close(struct tcp_pcb *tpcb)
{
	//移除回调
	tcp_abort(tpcb);//终止连接,删除pcb控制块
	
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);

	tcp_client_flag=0;//标记连接断开了
}

err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	
	if(tcp_client_flag==0)
	{ 
		tcp_client_connection_close(tpcb);//关闭TCP连接
	} 
	ret_err=ERR_OK;

	return ret_err;

}

void Tcp_Client_Check(void)
{

	if(timerIsOverflow(&tcp_check.TCP_timer)) 
	{
		timerStart(&tcp_check.TCP_timer, 300);
   
		if(TcpPcb->state != ESTABLISHED)   //连接没有建立
		{
			
			if(TcpPcb->state != CLOSED)
			{		
				tcp_client_connection_close(TcpPcb);//关闭连接
				
			}
			TCP_init();
			
		}
		else if(tcp_client_flag==0)
		{
			tcp_client_connection_close(TcpPcb);//关闭连接
			TCP_init();
		}

	}
}

