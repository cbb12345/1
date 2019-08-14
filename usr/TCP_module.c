
/****************************************Copyright (c)****************************************************
**                               ��������ȫ���ܼ������޹�˾                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               UDP_module.c                                                                **
** Last modified Date:      2012.3.9                                                                    **
** Last Version:            v1.0                                                                        **
** Description:             UDPͨ��ģ��                                                                 **
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
struct tcp_pcb *TcpPcb;//����һ��pcb���ƿ�
u8 tcp_client_flag;//0��û�����ӷ����� 1�������Ϸ�������


/*
*����һ������
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
		
	
	TcpPcb = tcp_new();//����һ�����ƿ�
	
	//����ʧ�ܣ�����Ŀ��Բ�ִ�еģ�ʵ��û�����Ҫ���ݲ�����Щ
	if (TcpPcb == NULL)
		SelfCheckFailure = 4;
	
	if(TcpPcb!=NULL)
	{
		tcp_bind(TcpPcb,IP_ADDR_ANY,ccc);   /* �󶨱���IP��ַ */
		
		IP4_ADDR(&ipaddr,g_device.AutoParam.PCIP1,g_device.AutoParam.PCIP2,g_device.AutoParam.PCIP3,g_device.AutoParam.PCIP4);  //server��ַ
		//�������Ӳ�ע�����ӳɹ��صĻص�����
		tcp_connect(TcpPcb,&ipaddr,remote_port,tcp_client_connected);//ע��ص�����
	}
}


//TCP���ӽ�������õĻص�����
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	
	if(err==ERR_OK)
	{ 
		tcp_client_flag=1; 				//������ӵ���������
		
		tcp_recv(tpcb,tcp_client_recv);  	//��ʼ��LwIP��tcp_recv�ص�����
		tcp_poll(tpcb,tcp_client_poll,1); 	//��ʼ��LwIP��tcp_poll�ص�����		
		err=ERR_OK;
	}
	else
	{
		tcp_client_connection_close(tpcb);//�ر�����
	}
	
	 return err; 
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	STCP_TRANS_DATA tcp;
	
	LWIP_ASSERT("arg != NULL",arg != NULL);
	
	if(p==NULL)
	{
		tcp_client_flag=0;//��Ҫ�ر�TCP ������ 
	}
	else if(err!=ERR_OK)//�����յ�һ���ǿյ�����֡,����err!=ERR_OK
	{
		if(p)
			pbuf_free(p);//�ͷŽ���pbuf
	}
	else if(tcp_client_flag==1)	//����������״̬ʱ
	{
		if(p!=NULL)
		{
			tcp_recved(tpcb, p->tot_len);
						
			memcpy(&tcp.rec_buf,p->payload,p->len);
			
			tcp.rec_buf.len=p->len;
			
			//�򵥵�Э�鳤���ж� �����𣬳��ȣ�������ݣ�У��
			if(( tcp.rec_buf.len < 4) || ( tcp.rec_buf.len > BUF_LEN))
			{
			}	
			//Э�����
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


//�ر��������������
void tcp_client_connection_close(struct tcp_pcb *tpcb)
{
	//�Ƴ��ص�
	tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
	
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);

	tcp_client_flag=0;//������ӶϿ���
}

err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	
	if(tcp_client_flag==0)
	{ 
		tcp_client_connection_close(tpcb);//�ر�TCP����
	} 
	ret_err=ERR_OK;

	return ret_err;

}

void Tcp_Client_Check(void)
{

	if(timerIsOverflow(&tcp_check.TCP_timer)) 
	{
		timerStart(&tcp_check.TCP_timer, 300);
   
		if(TcpPcb->state != ESTABLISHED)   //����û�н���
		{
			
			if(TcpPcb->state != CLOSED)
			{		
				tcp_client_connection_close(TcpPcb);//�ر�����
				
			}
			TCP_init();
			
		}
		else if(tcp_client_flag==0)
		{
			tcp_client_connection_close(TcpPcb);//�ر�����
			TCP_init();
		}

	}
}

