
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

#include <string.h>
#include "Main.h"
#include "UDP_module.h"
#include "Common.h"
#include "ParamItem.h"

/*------------------------------------------------------------------------------------------------------*/
extern SDevice g_device;
SUIP_TRANS_DATA uip;
struct udp_pcb *UdpPcb = NULL;
extern int SelfCheckFailure;

void UDP_sendData(struct udp_pcb *UdpPcb, struct ip_addr destAddr, u16_t port,unsigned char *pucBuffer, unsigned int ucCount )
{
	struct pbuf *p; 
	p = pbuf_alloc(PBUF_TRANSPORT,ucCount,PBUF_RAM); 
	if(p != NULL)
	{ 
		memcpy(p->payload, pucBuffer, ucCount); 
		udp_sendto(UdpPcb,p,&destAddr,port);
		pbuf_free(p); 
	}
	//SysCtlDelay(100 * (SysCtlClockGet() / 3000));
}

void UDP_Receive(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                 struct ip_addr *addr, u16_t port)
{
	if(p != NULL)
	{
	    //memcpy(&uip.rec_buf, p, sizeof(uip.rec_buf));
		memcpy(&uip.rec_buf, p->payload, p->len);
		
		uip.rec_buf.len = p->len;
		//�򵥵�Э�鳤���ж� �����𣬳��ȣ�������ݣ�У��
		if(( uip.rec_buf.len < 4) || ( uip.rec_buf.len > BUF_LEN))
		{
//			//uip.set_mark = 0;
//			memset(&uip.rec_buf, 0, sizeof(uip.rec_buf));
//			pbuf_free(p);
//			return;
		}
		//Э�����
		else if(ERRORNORETURN != processParamTable(SourceFromUDP, uip.rec_buf.buf, (unsigned int)uip.rec_buf.len))
		{
			uip.rec_buf.len = uip.rec_buf.buf[1] + 2;
			//memcpy(p->payload ,&uip.rec_buf, uip.rec_buf.len);
			UDP_sendData(upcb, *addr, port, uip.rec_buf.buf,uip.rec_buf.len);
		}

		memset(&uip.rec_buf, 0, sizeof(uip.rec_buf));
		pbuf_free(p);
	}
}

//void uip_app(void)
void UDP_init(void)
{
	unsigned short ccc;
	ccc = g_device.workpara.ReaderPortHigh << 8 ;
	ccc += g_device.workpara.ReaderPortLow;
	UdpPcb = udp_new();

	//����ʧ�ܣ�����Ŀ��Բ�ִ�еģ�ʵ��û�����Ҫ���ݲ�����Щ
	if (UdpPcb == NULL)
		SelfCheckFailure = 4;
	udp_bind(UdpPcb,IP_ADDR_ANY,ccc);   /* �󶨱���IP��ַ */
	// udp_connect(UdpPcb,&ipaddr,1025);	 /* ����Զ������ */
	udp_recv(UdpPcb,UDP_Receive,NULL);   /* �������ݽ���ʱ�Ļص�����*/

}

//��ʱ���ӵĹ��� ��ֱ�ӷų���
void Udp_BroadCast(void)
{
	struct ip_addr addr;
	if(timerIsOverflow(&uip.UIP_timer)) 
	{
		timerStart(&uip.UIP_timer, 300);
		uip.rec_buf.buf[0] = 0xF0;
		uip.rec_buf.buf[1] = 18;
		uip.rec_buf.buf[2] = 0;
		memcpy(&uip.rec_buf.buf[3], &g_device.workpara.ReaderIP1, 6);
		memcpy(&uip.rec_buf.buf[9], &g_device.ReaderID, 10);
		uip.rec_buf.len = 20;
		uip.rec_buf.buf[19] = SetChecksum(uip.rec_buf.buf, 19);
		IP4_ADDR(&addr,0xFF,0xFF,0xFF,0xFF);
		//IP4_ADDR(&addr,192,168,0,61);
		UDP_sendData(UdpPcb, addr, 4444, uip.rec_buf.buf,uip.rec_buf.len);
    }
}

