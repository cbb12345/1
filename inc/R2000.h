	/****************************************Copyright (c)****************************************************
	**								 ��������ȫ���ܼ������޹�˾ 											**
	**																										**
	**								   http://www.szeway.cn 												**
	**																										**
	**--------------File Info-------------------------------------------------------------------------------**
	** File Name:				R2000_module.h	    														**
	** Last modified Date:		2012.12.21																	**
	** Last Version:			v1.0																		**
	** Description: 			����232ͨ��ͷ�ļ�															**
	**																										**
	**------------------------------------------------------------------------------------------------------**
	** Created By:				����																		**
	** Created date:			2012.12.21																	**
	** Version: 				v1.0																		**
	** Descriptions:			The original version ��ʼ�汾												**
	**																										**
	**------------------------------------------------------------------------------------------------------**
	** Modified by: 																						**
	** Modified date:																						**
	** Version: 																							**
	** Description: 																						**
	**																										**
	*********************************************************************************************************/
	
	
#ifndef _R2000_MODULE_H
	
#define _R2000_MODULE_H
	
#include "Common.h"
	
	/*------------------------------------------------------------------------------------------------------*/
	
	
	
	/*------------------------------------------------------------------------------------------------------*/
#pragma pack(1)
	
	typedef struct _R2000_TRANS_DATA
	{
		unsigned char set_mark;	
		SBUF_STR  rec_buf;
		STimer local_timer;		//local��ʱ��
		STimer send_timer;		//local��ʱ��
	}SR2000_TRANS_DATA,*PR2000_TRANS_DATA;
	
#pragma pack()
	/*------------------------------------------------------------------------------------------------------*/
	void R2000_init(void);
	void reset_R2000(void);
	void close_r2000_beep(void);

	void decodeR2000(unsigned char *buffer, unsigned char len);
	unsigned char encodeR2000(unsigned char cmd, unsigned char *buffer);
	void UART4Send( unsigned char *pucBuffer, unsigned long ucCount );
	void set_output_power(void);

#endif
	


