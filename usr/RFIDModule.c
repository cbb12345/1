
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               RFIDModule.c                                                                **
** Last modified Date:      2012.7.24                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             RFID通信模块                                                                **
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

#include "Common.h"
#include "RFIDModule.h"
#include "Main.h"
#include "ParamItem.h"
#include "R2000.h"
#include "RTC_Time.h" 
#include <string.h>


//STagInfo tags_[MAXNUM];
//unsigned num_of_tags;
extern STimer ReadCardTimer;

extern unsigned char beepFlag;
extern unsigned char RelayFlag;
extern SDevice g_device;

//extern unsigned char CardTotal;

extern unsigned int CardPos;
extern unsigned char frequency_num; 
//#define EPC_buff_lenth  30////150////200
//extern unsigned int EPC_data_buff[EPC_buff_lenth][10];//5];

extern unsigned int read_count;
//extern unsigned char temp_mask_data[30];
//extern unsigned char AllowOutput;
//unsigned char scrollpos= 0;
/////////////////////////////////////////////////////////////////////////////////////////////

//unsigned char Cardtmp[20][TAGIDLENGTH];
extern unsigned short CMDMaskaddr;
extern unsigned char CMDMasklen,CMDMaskData[TAGIDLENGTH];
extern SR2000_TRANS_DATA R2000;
//extern unsigned short Tag_R2000_CMD80_number,Tag_R2000_CMD91_number;
extern unsigned int Tag_R2000_CMD80_number,Tag_R2000_CMD91_number;
extern unsigned char MASKSEL,MASKAddr6B,MASK6B,MASKData6B[8];
unsigned char TagReadFirstFlag = 0;
unsigned char AntId = 0;


//extern unsigned char TagSaveBuf[SAVEMAXNUM][TAGIDLENGTH + 6 + 2] ;
//extern unsigned int TagSaveCount; 

void readCard6B(void)
{
	//PCMD_R2000 cmdRstr;
	//SysCtlDelay(5 * (SysCtlClockGet() / 3000));

	//Tag_R2000_CMD91_number = 0;
	//Tag_R2000_CMD80_number = 0xffff;//0;
	//MASKSEL = p[0];
	//MASKAddr6B = p[1];
	//MASK6B = 0;
	//CardPos = 0;
	//memcpy(MASKData6B, &p[3], 8);
	//Read_timeFrom_Ds1302();
	getTime(g_device.ReaderTime);
	//if(encodeR2000(0xB0, &MASK6B) == 0) //写入成功
	if(encodeR2000(0xB0, &MASK6B) != 0xff) //写入成功
	{
		read_count++;
	}
}
#if 0
void readCard6C(void)
{
	//PCMD_R2000 cmdRstr;
	CardPos = 0;
	Tag_R2000_CMD91_number = 0;
	getTime(g_device.ReaderTime);
	DelayMs(2);
	if(encodeR2000(0x8A, (unsigned char *)&CardPos) == 0) //写入成功
	{
		if(CardPos > 0)
			read_count++;
	}
}
#endif
#if 1
void readCard6C(void)
{
	unsigned char channel;//buf[];
	PCMD_R2000 cmdRstr;
//	SysCtlDelay(10 * (SysCtlClockGet() / 3000));
	//channel = 1;//g_device.workpara.FrequencyMax - g_device.workpara.FrequencyMin + 1;
	if(g_device.workpara.MaxReadCardNum < 20) //通过标签的数量参数 来改变盘存参数
	{
		channel = 255;
	}
	else
	{
		channel=1;
	}

	Tag_R2000_CMD91_number = 0;
	if(encodeR2000(0x80, &channel) == 0) //写入成功
	{
		CardPos = 0;
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->len > 4)
		{
			Tag_R2000_CMD80_number = cmdRstr->para[1];
			Tag_R2000_CMD80_number <<= 8;
			Tag_R2000_CMD80_number |= cmdRstr->para[2];
			if(Tag_R2000_CMD80_number > 0)
			{
//				SysCtlDelay(10 * (SysCtlClockGet() / 3000));	//延时一定时间
				//beepFlag = 1; //蜂鸣器鸣叫
				DelayMs(10);
				CMDMaskaddr = g_device.tagfilter.TagMaskAddrH;
				CMDMaskaddr <<= 8;
				CMDMaskaddr |= g_device.tagfilter.TagMaskAddrL;
				CMDMasklen = g_device.tagfilter.TagMaskLenH;
				CMDMasklen <<= 8;
				CMDMasklen |= g_device.tagfilter.TagMaskLenL;
				memcpy(CMDMaskData, g_device.tagfilter.TagMaskData, CMDMasklen);
				 //CardPos = 0;
				AntId = R2000.rec_buf.buf[4];
				getTime(g_device.ReaderTime);
				//主动模式输出，记得判断是不是0
				encodeR2000(0x91, &channel);
				read_count++;
			}
		}
	}
}
#endif
#define CARD_ISO180006B 1
#define CARD_EPCC1      2
#define CARD_EPCC1G2    4
#define CARD_EM4442     8
#define CARD_ATA       16
#define CARD_EPCATA    32
#define SELF_ID 0
#define USER_ID 1

void readCard(void)
{
	//unsigned char cmdData[10]={0x40, 0x08, 0xEF, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};
	unsigned char cmdData[10]={0x40, 0x08, 0xEF, 0x02, 0x0, 0x06, 0x00, 0x00, 0x00, 0x00};


	//cmdData[4]= g_device.AutoParam.CardStartPos;
	if(AntennaCtrl() != 0)
	{
		//SysCtlDelay(5 * (SysCtlClockGet() / 3000));
		DelayMs(5);
		switch(g_device.workpara.TagType)
		{
			case CARD_ISO180006B:
				readCard6B();
				break;
			case CARD_EPCC1:
			case CARD_EPCC1G2:
				//if( (USER_ID == g_device.AutoParam.CardSaveInTagPosition))
				//if( ((g_device.AutoParam.CardOutModule== OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule== OUTPUT_Wiegan34)) &&(USER_ID == g_device.AutoParam.CardSaveInTagPosition))  //((g_device.AutoParam.CardOutModule== OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule== OUTPUT_Wiegan34)) &&
				if(USER_ID == g_device.AutoParam.CardSaveInTagPosition)
				{ 				
					EPC1G2_ListTagTID(cmdData);					
				}
				else
					readCard6C();
				break;					
		}
	}
	
}

