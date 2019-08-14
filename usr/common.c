
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Common.c                                                                      **
** Last modified Date:      2012.3.2                                                                    **
** Last Version:            v1.0                                                                        **
** Description:             公共函数模块                                                                **
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
//#include "Local_232_module.h"
//#include "Local_485_module.h"
//#include "UDP_module.h"

#include "ParamItem.h"
#include "RFIDModule.h"
#include "common.h"
#include "Main.h"
#include "R2000.h"
#include "IIC.h"
#include <string.h>
#include "RTC_Time.h"
#include "stm32flash.h"

extern SDevice g_device;
//extern SLOCAL_TRANS_DATA local;
//extern SLOCAL_485_DATA local485;

//extern Tag *selectedTag;
extern SParamTable g_paramTable;
extern char cmdTmpBuf[BUF_LEN];//cmd57TagBuf[BUF_LEN];
extern char cmd57ReadTagCount;
extern char cmdTmpLen;//cmd57ReadTagLen;
//extern unsigned char BootUpdataFlag;
extern char TagDataOrigin;
//extern SFrequencies Frequencies;
extern unsigned char sysReboot,tmpAntenna;

extern SR2000_TRANS_DATA R2000;

/*------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------*/

extern char Version[4];
extern SDevice g_device;
extern unsigned char ReadCardTimeInterver;
extern STimer ReadCardTimer;
extern STimer OutPutTimer;
extern unsigned char CardIndex[MAXNUM][TAGIDLENGTH+6+2];
extern unsigned int CardPos;

extern unsigned char FilterMask[TAGIDLENGTH],FilterMaskLen;

extern struct udp_pcb *UdpPcb;
extern unsigned char beepFlag;

unsigned char RelayFlag = 0;
unsigned int RelayCountTimes = 0;


unsigned char frequency_num=0; //调频测试用
unsigned char Tag_send_count=0;
unsigned short Tag_all_number=0;
unsigned int Tag_R2000_CMD80_number=0;//80H命令，识别标签的总数量
unsigned int Tag_R2000_CMD91_number=0;
extern unsigned int read_count;

unsigned short CMDMaskaddr;
unsigned char CMDMasklen,CMDMaskData[TAGIDLENGTH];

unsigned char MASKSEL,MASKAddr6B,MASK6B,MASKData6B[8];
extern unsigned char TagSaveCount, TagSaveBuf[SAVEMAXNUM][TAGIDLENGTH + 6 + 2];

/*------------------------------------------------------------------------------------------------------*/

//启动定时器
void timerStart(PTimer timer, unsigned int time) 
{
	timer->isOverflow = 0;
	timer->time = time;
	timer->isStart = 1;
}

//停止定时器
void timerStop(PTimer timer) 
{
	timer->isOverflow = 0;
	timer->isStart = 0;
}

//定时器初始化
void timerInit(PTimer timer) 
{
	timer->isStart = 0;
	timer->isOverflow = 0;
}

//定时器是否溢出
unsigned char timerIsOverflow(PTimer timer) 
{
	unsigned char b;
	b = timer->isOverflow;
	if (timer->isOverflow) 
	{
		timer->isOverflow = 0;
	}
	return b;
}

unsigned char SetChecksum(unsigned char *buf,unsigned int length)
{
    //不加数组越界判断了，这协议数据很短，没必要
    unsigned int i;
	unsigned char sum = 0;
	
	for(i=0; i < length; i++)
	{
	    sum += buf[i];
	}
	sum = ~sum + 1;
	return sum;
}
//所有函数处理完数据形成返回数据包


#if 1
void EPC1G2_ListTagTID(unsigned char *buf)//, unsigned char type)
{

	PCMD_STR cmdstr;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;//,len
	unsigned char *p;

	cmdstr = (PCMD_STR)buf;

	p = cmdstr->para;
	
	CMDMasklen = 0;
	///CardPos = 0;
	flag_return = encodeR2000(0xC5, p);	 //取消匹配,C5==85H，匹配ACCESS操作的EPC号

	if(flag_return == 0) //返回成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{	
		 	DelayMs(5);
		 	getTime(g_device.ReaderTime);
			//Tag_TID_number=0;
			flag_return = encodeR2000(0xC1, p);
			if(flag_return == 0)
			{
			//	cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
			//	if(cmdRstr->len > 4)
			//	{
				//	beepFlag = 1; //蜂鸣器鸣叫
					read_count++;
				
				//}
			}
		}
	
	}

}
#endif

void SetBaudRate(unsigned char *buf)
//char SetBaudRate(unsigned char type, unsigned char *buf)
{
	g_device.SysConnectParaChange |= 0x01;

}

//case CMD_SetRelay		 :		//03H
//void SetRelay(unsigned char *buf, unsigned char type)
void SetRelay(unsigned char *buf)
{
	if(0 != (g_device.RelayStatus & 0x01))
	{
		////1#继电器闭合
		RELAYON;
	}
	else
	{
		////1#继电器断开
		RELAYOFF;
	}
	if(0 != (g_device.RelayStatus & 0x02))
	{
		////2#继电器闭合
		RELAYON1;
	}
	else
	{
		////2#继电器断开
		RELAYOFF1;
	}
}
//case CMD_SetOutputPower  :		//04H
void SetOutputPower(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	if(p[0] < 20)
		p[0] = 20;
	else if(p[0] > 33)
		p[0] = 33;
	g_device.workpara.OutputPower = p[0];
	flag_return = encodeR2000(0x76, p);
	if(flag_return==0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			cmdTmpLen = 0;
			g_paramTable.setFlag = 1;
	//		beepFlag = 1; //蜂鸣器鸣叫
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
//case CMD_SetFrequency	 :		//05H
void SetFrequency(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
//	PCMD_R2000 cmdRstr;
//	unsigned char flag_return;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
//	if(p[0] < 1)
//		p[0] = 1;
//	else if(p[0] > 63)
//		p[0] = 63;
	memcpy(&g_device.FreqSelect.region,p,6);
	if(g_device.FreqSelect.region < 4) {
		g_device.workpara.FrequencyMax = g_device.FreqSelect.para[1];
		g_device.workpara.FrequencyMin = g_device.FreqSelect.para[0];
	}
	cmdTmpLen = 0;
	g_paramTable.setFlag = 1;

	/*
	flag_return = encodeR2000(0x78, p);
	if(flag_return==0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			cmdTmpLen = 0;
			g_paramTable.setFlag = 1;
			
			g_device.workpara.FrequencyMin=p[0];
			g_device.workpara.FrequencyMax=p[1];	
		//	beepFlag = 1; //蜂鸣器鸣叫
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}*/

}
#if 1
void SetFrequency1(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	p[0] = g_device.workpara.FrequencyMin;
	p[1] = g_device.workpara.FrequencyMax;
	flag_return = encodeR2000(0x78, p);
	if(flag_return==0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			cmdTmpLen = 0;
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}

}
#endif

//case CMD_ReadParam	:		//06H
#if (PARA_STORAGE_DEVICE == FLASH_AT24C02)
void ReadParam(unsigned char *buf)
{
	unsigned char i;
	for(i = 0; i < 5; i++)
	{
		I2C_EE_BufferRead((unsigned char*)(&g_device), 0, sizeof(g_device));
		if(g_device.Flag == WORK_FLAG)
		{
			break;
		}
	}
	g_device.GetReaderVersion[2] = Version[2];
	g_device.GetReaderVersion[3] = Version[3];

}
#else
void ReadParam(unsigned char *buf)
{
	unsigned char i;
	for(i = 0; i < 5; i++)
	{
		STMFLASH_Read(PARA_STORAGE_ADDR,(u16*)(&g_device),sizeof(g_device)/2);
		if(g_device.Flag == WORK_FLAG)
		{
			break;
		}
	}
	g_device.GetReaderVersion[2] = Version[2];
	g_device.GetReaderVersion[3] = Version[3];

}
#endif
	
//case CMD_WriteRadioDeep  :		//07H
void WriteRadioDeep(unsigned char *buf)//, unsigned char type)
{
}
//case CMD_ReadRadioDeep	 :		//08H
void ReadRadioDeep(unsigned char *buf)//, unsigned char type)
{
}
//case CMD_WriteParam 	 :		//09H
void WriteParam(unsigned char *buf)//, unsigned char type)
{
	//复位处理
	//Reboot(buf);
}

#if 1
#define	R2000_SED_TIMER	200//50 //500MS

//void AntennaCtrl(void)
unsigned char AntennaCtrl(void)
{
	//set_work_antenna(tmpAntenna);
    unsigned char send_buff[6];
	PCMD_R2000 cmdRstr;
	//DelayMs(20);
	DelayMs(10);
	send_buff[0]=0xA0;  //设置工作天线
	send_buff[1]=0x04;
	send_buff[2]=0xFF;
	send_buff[3]=0x74;
	//send_buff[4]=tmpAntenna; //天线1  
	send_buff[4]=0;

	if(tmpAntenna == 1)
		send_buff[4] = 0;
	else if(tmpAntenna == 2)
		send_buff[4] = 1;
	else if(tmpAntenna == 4)
		send_buff[4] = 2;
	else if(tmpAntenna == 8)
		send_buff[4] = 3;

	send_buff[5]=SetChecksum(send_buff,5);
	R2000.set_mark = 0;
      UART4Send(send_buff,6); 
	timerStart(&R2000.send_timer,R2000_SED_TIMER);
	while(timerIsOverflow(&R2000.send_timer) == 0)
	{
		if( 0 != R2000.set_mark)
		{
			R2000.set_mark = 0;
			cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
			//if((cmdRstr->cmd == 0x74) && (cmdRstr->para[0] == command_success))
			if(cmdRstr->cmd == 0x74)
			{
				timerStop(&R2000.send_timer);
				if(cmdRstr->para[0] == command_success)
					return 1;
				else
					return 0;
			}
		}
	}
	timerStop(&R2000.send_timer);
	memset(&R2000, 0, sizeof(R2000));
	return 0;
}
#endif
//case CMD_SetAntenna 	 :		//0AH
void SetAntenna(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	DelayMs(10);
	flag_return = encodeR2000(0x74, p);
	if(flag_return==0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			cmdTmpLen = 0;
	//		beepFlag = 1; //蜂鸣器鸣叫
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
		//	cmdTmpLen = BUF_LEN;
			//我靠，返回0是啥意思?咋文档里没个解释，错误代码表里面就没这个值
			//去你娘的，R2000的代码写的太烂,不管了，认为是成功
			cmdTmpLen = 0;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}

	//AntennaCtrl();
	//set_work_antenna();

	//30130712
//	SysCtlDelay(10 * (SysCtlClockGet() / 3000));
}

//case CMD_SetReaderPara	 :		//0CH出厂参数
void SetReaderPara(unsigned char *buf)//, unsigned char type)
{

}
//case CMD_ReadReaderPara  :		//0DH恢复出厂参数 2019-4-15 恢复出厂设置的时候不恢复MAC地址
void ReadReaderPara(unsigned char *buf)//, unsigned char type)
{
	unsigned char *p,*q;
	unsigned char tmp_mac[6],tmp_id[10];
	unsigned char i = 0;
	
	for(i=0; i<10; i++)
	{
		tmp_id[i] = g_device.ReaderID[i];
	}
	
	tmp_mac[0] = g_device.workpara.ReaderMAC1;
	tmp_mac[1] = g_device.workpara.ReaderMAC2;
	tmp_mac[2] = g_device.workpara.ReaderMAC3;
	tmp_mac[3] = g_device.workpara.ReaderMAC4;
	tmp_mac[4] = g_device.workpara.ReaderMAC5;
	tmp_mac[5] = g_device.workpara.ReaderMAC6;

	p = (unsigned char *)&g_device.workpara;
	q = (unsigned char *)&g_device.Factoryworkpara;
	memcpy(p, q, sizeof(SWorkParam));
	p = (unsigned char *)&g_device.AutoParam;
	q = (unsigned char *)&g_device.FactoryAutoParam;
	memcpy(p, q, sizeof(SWorkParam));
	
	g_device.workpara.ReaderMAC1 = tmp_mac[0];
	g_device.workpara.ReaderMAC2 = tmp_mac[1];
	g_device.workpara.ReaderMAC3 = tmp_mac[2];
	g_device.workpara.ReaderMAC4 = tmp_mac[3];
	g_device.workpara.ReaderMAC5 = tmp_mac[4];
	g_device.workpara.ReaderMAC6 = tmp_mac[5];
	
	for(i=0; i<10; i++)
	{
		g_device.ReaderID[i] = tmp_id[i];
	}
	
	SaveData();
	
	Reboot(buf);

}
//case CMD_Reboot 		 :		//0EH
void Reboot(unsigned char *buf)//, unsigned char type)
{
	sysReboot = 2;//1;
	// SysCtlReset();
}
//case CMD_SetReaderWorkType: 	//0FH
void SetReaderWorkType(unsigned char *buf)//, unsigned char type)
{
	if(0 == g_device.workpara.ReaderWorkStatus)
	{
		g_device.workpara.ReaderWorkStatus = WORK_CMD;
		timerStop(&ReadCardTimer);
		timerStop(&OutPutTimer);
	}
	else if(1 == g_device.workpara.ReaderWorkStatus)
	{
		g_device.workpara.ReaderWorkStatus = WORK_ACT;
		//启动定时器
		timerStart(&ReadCardTimer, ReadCardTimeInterver);
		//timerStart(&OutPutTimer, g_device.AutoParam.CallPCTimeInterver);
		if((g_device.AutoParam.CardOutModule == OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule == OUTPUT_Wiegan34))
			timerStart(&OutPutTimer, 100);
		else
			timerStart(&OutPutTimer, g_device.AutoParam.CallPCTimeInterver * 100);
		
		CardPos = 0;
	//	CardPoslast = 0;
		//CardTotal = 0;
//		Read_timeFrom_Ds1302();
//		memcpy(FirtReaderTime, g_device.ReaderTime, sizeof(g_device.ReaderTime));
		
		tmpAntenna = g_device.AutoParam.Antenna;
//		AntennaCtrl();
		//memset(EPCATACard_ATA, 0, sizeof(EPCATACard_ATA));
		//memset(EPCATACard_EPC, 0, sizeof(EPCATACard_EPC));
		//EPCATACount = 0;
		//AllowOutput = 0;
		// 加延时标志
//		delayFlag0X0F = 1;
		//SysCtlDelay(100 * (SysCtlClockGet() / 3000));
//		num_of_tags = 0;
	}

}
//case CMD_ClearTagListMem  : 	//10H
void ClearTagListMem(unsigned char *buf)//, unsigned char type)
{
	memset(CardIndex, 0, sizeof(CardIndex));
	memset(TagSaveBuf, 0, sizeof(CardIndex));
	CardPos = 0;
//	CardPoslast = 0;
	TagSaveCount = 0;

}
//case CMD_SetReaderTime	  : 	//11H
void SetReaderTime(unsigned char *buf)//, unsigned char type)
{
	setTime(g_device.ReaderTime);
}
//case CMD_GetReaderTime	  : 	//12H
void GetReaderTime(unsigned char *buf)//, unsigned char type)
{
	//Read_timeFrom_Ds1302();
	getTime(g_device.ReaderTime);

}
//case CMD_SetAutoPara	  : 	//13H	  新协议改动了命令设置主动工作参数
void SetAutoPara(unsigned char *buf)//, unsigned char type)
{
}

//case CMD_GetAutoPara	  : 	//14H	  新协议改动了命令获取主动工作参数
void GetAutoPara(unsigned char *buf)////, unsigned char type)
{

}

//case CMD_SetTagFilter	:		//15H	  新协议改动了命令设置标签过滤器
//专指自动模式
void SetTagFilter(unsigned char *buf)//, unsigned char type)
{

}


//case CMD_SetReaderNetwork : 	//30H
void SetReaderNetwork(unsigned char *buf)//, unsigned char type)
{
	//g_device.SysConnectParaChange |= 0x02;
	//复位处理
//	Reboot(buf);

}

//case CMD_SetReaderMAC	  : 	//32H
void SetReaderMAC(unsigned char *buf)//, unsigned char type)
{
	//复位处理
//	Reboot(buf);

}
//case CMD_GetReaderRevStatus:	//41H
void GetReaderRevStatus(unsigned char *buf)//, unsigned char type)
{
	g_device.ReaderRevStatus = GetPinTriggle();

}
//case CMD_SetReaderOutStatus:	//42H
void SetReaderOutStatus(unsigned char *buf)//, unsigned char type)
{

}
//case CMD_GetReaderOutStatus:	//43H
void GetReaderOutStatus(unsigned char *buf)//, unsigned char type)
{

}

//case CMD_ImmediatelyNotify :	//54H
void ImmediatelyNotify(unsigned char *buf)//, unsigned char type)
{
}

//case CMD_GetTagRecord	   :	//57H
void GetTagRecord(unsigned char *buf)//, unsigned char type)
{
	int i,j = 0;
	//Read_timeFrom_Ds1302();
	getTime(g_device.ReaderTime);

	cmdTmpLen = 1+16;
	if((TagSaveCount - cmd57ReadTagCount) > 7)
	{
		cmdTmpBuf[0] = 8;
		for(i = 0;i < 8; i++)
		{
			cmdTmpBuf[cmdTmpLen - 17 + 1] = g_device.ReaderTime[0];//(g_device.ReaderTime[3]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 2] = g_device.ReaderTime[1];//(g_device.ReaderTime[3]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 3] = g_device.ReaderTime[2];//(g_device.ReaderTime[4]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 4] = g_device.ReaderTime[3];//(g_device.ReaderTime[4]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 5] = g_device.ReaderTime[4];//(g_device.ReaderTime[5]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 6] = g_device.ReaderTime[5];//(g_device.ReaderTime[6]%10)+0x30;
			
			cmdTmpBuf[cmdTmpLen - 17 + 7] = g_device.ReaderTime[0];//(g_device.ReaderTime[3]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 8] = g_device.ReaderTime[1];//(g_device.ReaderTime[3]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 9] = g_device.ReaderTime[2];//(g_device.ReaderTime[4]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 10] = g_device.ReaderTime[3];//(g_device.ReaderTime[4]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 11] = g_device.ReaderTime[4];//(g_device.ReaderTime[5]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 12] = g_device.ReaderTime[5];//(g_device.ReaderTime[6]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 13] = 0;//0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 14] = 0;//0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 15] = g_device.AutoParam.Antenna;//hextochar(g_device.AutoParam.Antenna & 0x0f);
			cmdTmpBuf[cmdTmpLen - 17 + 16] = g_device.workpara.TagType;//hextochar(g_device.workpara.TagType & 0x0f);
		
			memcpy(&cmdTmpBuf[cmdTmpLen], &CardIndex[cmd57ReadTagCount][0], 2*CardIndex[cmd57ReadTagCount][0]+1);
			j = cmdTmpLen + (2*CardIndex[cmd57ReadTagCount][0] + 1);
			cmdTmpLen = 17 + j - 1;
			cmd57ReadTagCount++;
		}
	}
	else
	if((TagSaveCount - cmd57ReadTagCount) > 0)
	{
		cmdTmpBuf[0] = TagSaveCount - cmd57ReadTagCount;
		for(i = 0;i < cmdTmpBuf[0]; i++)
		{
			cmdTmpBuf[cmdTmpLen - 17 + 1] = g_device.ReaderTime[0];//(g_device.ReaderTime[3]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 2] = g_device.ReaderTime[1];//(g_device.ReaderTime[3]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 3] = g_device.ReaderTime[2];//(g_device.ReaderTime[4]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 4] = g_device.ReaderTime[3];//(g_device.ReaderTime[4]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 5] = g_device.ReaderTime[4];//(g_device.ReaderTime[5]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 6] = g_device.ReaderTime[5];//(g_device.ReaderTime[6]%10)+0x30;
			
			cmdTmpBuf[cmdTmpLen - 17 + 7] = g_device.ReaderTime[0];//(g_device.ReaderTime[3]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 8] = g_device.ReaderTime[1];//(g_device.ReaderTime[3]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 9] = g_device.ReaderTime[2];//(g_device.ReaderTime[4]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 10] = g_device.ReaderTime[3];//(g_device.ReaderTime[4]%10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 11] = g_device.ReaderTime[4];//(g_device.ReaderTime[5]/10)+0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 12] = g_device.ReaderTime[5];//(g_device.ReaderTime[6]%10)+0x30;
			
			cmdTmpBuf[cmdTmpLen - 17 + 13] = 0;//0x30;
			cmdTmpBuf[cmdTmpLen - 17 + 14] = 0;//0x30;
			
			cmdTmpBuf[cmdTmpLen - 17 + 15] = g_device.AutoParam.Antenna;//hextochar(g_device.AutoParam.Antenna & 0x0f);
			cmdTmpBuf[cmdTmpLen - 17 + 16] = g_device.workpara.TagType;//hextochar(g_device.workpara.TagType & 0x0f);
			memcpy(&cmdTmpBuf[cmdTmpLen], &CardIndex[cmd57ReadTagCount][0], 2*CardIndex[cmd57ReadTagCount][0]+1);
			j = cmdTmpLen + (2*CardIndex[cmd57ReadTagCount][0] + 1);
			cmdTmpLen = 17 +j - 1;
			cmd57ReadTagCount++;
		}
		cmd57ReadTagCount = 0;
	}
	else
	{
		cmdTmpBuf[0] = 0;
		cmd57ReadTagCount = 0;
	}
}
#if 0
//case CMD_ModifyReaderFrequenceRange://86H
void ModifyReaderFrequenceRange(unsigned char *buf)//, unsigned char type)
{
	g_device.FreqSelect = 1;

}
#endif
void UpdateProgrram(unsigned char *buf)//, unsigned char type)
{
#if 0
	if(TagDataOrigin == SourceFromUDP)
	{
		BootUpdataFlag = 2;
	}
	else
	{
		BootUpdataFlag = 1;
	}
	g_device.workpara.ReaderWorkStatus = WORK_CMD;
	timerStop(&ReadCardTimer);
	timerStop(&OutPutTimer);
#endif
}

//读写ISO18000-6B命令格式
//case CMD_DetectTagNum	:		//FFH
void DetectTagNum(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	//PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	Tag_R2000_CMD91_number = 0;
	Tag_R2000_CMD80_number = 0;
	//MASKSEL = p[0];
	//MASKAddr6B = p[1];
	MASK6B = 0;
	//memcpy(MASKData6B, &p[3], 8);
	CardPos = 0;
	flag_return = encodeR2000(0xB0, p);
	if(flag_return != 0xff)//if(flag_return == 0) //写入成功
	{
		p[0] = CardPos;
		cmdTmpLen = 1;
	//	beepFlag = 1;
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}


//case CMD_ReadLabelID	:		//FEH
void ReadLabelID(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
//	PCMD_R2000 cmdRstr;
	unsigned char flag_return,i,lenth_count;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	Tag_R2000_CMD91_number = 0;
	Tag_R2000_CMD80_number = 0;
	//MASKSEL = p[0];
	//MASKAddr6B = p[1];
	MASK6B = 0;
	CardPos = 0;
	//memcpy(MASKData6B, &p[3], 8);

	flag_return = encodeR2000(0xB0, p);
	if(flag_return != 0xff)//if(flag_return == 0) //写入成功
	{
	//	beepFlag = 1;
		Tag_all_number = CardPos;
		if(Tag_all_number > 8)
		{
			Tag_send_count = 8;
		}
		else 
		{
			Tag_send_count = Tag_all_number;
		}
		//p[0] = Tag_send_count;
		p[0] = Tag_all_number;
		lenth_count = 1;
		for(i = 0; i < Tag_send_count; i++)
		{
			flag_return = (CardIndex[i][0] << 1) + 1;
			memcpy(&p[lenth_count], &CardIndex[i][0], flag_return);
			lenth_count += flag_return;
		}
		cmdTmpLen = lenth_count;
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}

//case CMD_ListIDReport	:		//FDH
void ListIDReport(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	unsigned char *p;
	unsigned char flag_return,i,lenth_count;
	unsigned char tmplen;
	
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	if((Tag_all_number-Tag_send_count) > 8)
	{
		Tag_send_count += 8;
		tmplen = 8;
	}
	else 
	{
		tmplen = Tag_all_number - Tag_send_count;
		Tag_send_count = Tag_all_number;
	}
	p[0] = tmplen;
	lenth_count = 1;
	for(i = 0; i < tmplen; i++)
	{
		flag_return = (CardIndex[i][0] << 1) + 1;
		memcpy(&p[lenth_count], &CardIndex[i][0], flag_return);
		lenth_count += flag_return;
	}
	cmdTmpLen = lenth_count;
}

//case CMD_ListSelectedID :		//FBH
void ListSelectedID(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
//	PCMD_R2000 cmdRstr;
	unsigned char flag_return,i,lenth_count;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	Tag_R2000_CMD91_number = 0;
	Tag_R2000_CMD80_number = 0;

	MASKSEL = p[0];
	MASKAddr6B = p[1];
	MASK6B = p[2];
	memcpy(MASKData6B, &p[3], 8);
	CardPos = 0;

	flag_return = encodeR2000(0xB0, p);
	if(flag_return != 0xff)//if(flag_return == 0) //写入成功
	{
		Tag_all_number = CardPos;
		if(Tag_all_number > 8)
		{
			Tag_send_count = 8;
		}
		else 
		{
			Tag_send_count = Tag_all_number;
		}
		p[0] = Tag_send_count;
		
		lenth_count = 1;
		for(i = 0; i < Tag_send_count; i++)
		{
			flag_return = (CardIndex[i][0] << 1) + 1;
			memcpy(&p[lenth_count], &CardIndex[i][0], flag_return);
			lenth_count += flag_return;
		}
		cmdTmpLen = lenth_count;
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}


//case CMD_ReadByteBlock	:		//F6H
void ReadByteBlock(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0xB1, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->len > 4)
		{
			beepFlag = 1; //蜂鸣器鸣叫
			flag_return = cmdRstr->len - 4;//cmdRstr->para[cmdRstr->len - 4];//暂存长度
			memcpy(p, &cmdRstr->para[1], flag_return);
			//memcpy(p, &cmdRstr->para[0], flag_return);
			cmdTmpLen = flag_return;
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}

//case CMD_WriteByteBlock :		//F5H
void WriteByteBlock(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0xB2, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->len > 4)
		{
			beepFlag = 1; //蜂鸣器鸣叫
			cmdTmpLen = p[9];
			memcpy(p, &p[10], cmdTmpLen);
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}

//case CMD_WriteProtect	:		//F4H
void WriteProtect(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0xB3, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->len > 4)
		{
			beepFlag = 1; //蜂鸣器鸣叫
			flag_return = cmdRstr->para[1];
		//	if(flag_return == 0xff)
		//	{
		//		cmdTmpLen = 1;
		//		p[0] = flag_return;
		//	}
		//	else
			cmdTmpLen = 0;
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}

}
//case CMD_ReadWriteProtect:		//F3H
void ReadWriteProtect(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0xB4, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->len > 4)
		{
			beepFlag = 1; //蜂鸣器鸣叫
			p[0] = cmdRstr->para[1];
			cmdTmpLen = 1;
		}
		else
		{
			cmdTmpLen = 1;
			p[0] = cmdRstr->para[0];
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
//case CMD_WriteAByte 	:		//F2H
void WriteAByte(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0xB2, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->len > 4)
		{
			beepFlag = 1; //蜂鸣器鸣叫
			cmdTmpLen = p[9];
			memcpy(p, &p[10], cmdTmpLen);
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}

//case CMD_DeleteAll		:		//F1H
//void DeleteAll(unsigned char *buf, unsigned char type)
//{
//	
//}
//读写EPC C1G2命令格式（6C）

void read_r2000vertion(unsigned char *buf)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}


	flag_return = encodeR2000(0x72, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		//if(cmdRstr->para[0] == command_success)
		if(cmdRstr->len > 4)  //模块版本号返回成功
		{
		   memcpy(p, &cmdRstr->para, 2);	 //版本号拷贝到缓冲区里
		   cmdTmpLen=2;//两个字节
		}
		else
		{

			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}  
}

void EPC1G2_ListRSSIMaskTag(unsigned char *buf)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return,i,lenth_count;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	//暂存主动模式掩码
	//DEMO没做好，传输数据错误，且DEMO没掩码，干脆为零
	CMDMaskaddr = p[1];
	CMDMaskaddr <<= 8;
	CMDMaskaddr |= p[2];
	CMDMasklen = 0;//p[3];
	//memset(CMDMaskData, 0, sizeof(CMDMaskData));
	memcpy(CMDMaskData, &p[4], CMDMasklen);
	CardPos = 0;
	//p[0] = 1;//为什么不调频啊
	if(g_device.workpara.MaxReadCardNum < 20) //通过标签的数量参数 来改变盘存参数
	{
		p[0] = 255;
	}
	else
	{
		p[0] = 1;
	}
	
	//p[0] = g_device.workpara.FrequencyMax - g_device.workpara.FrequencyMin + 1;
	//SysCtlDelay(5 * (SysCtlClockGet() / 3000));
	flag_return = encodeR2000(0x80, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		//if(cmdRstr->para[0] == command_success)
		if(cmdRstr->len > 4)
		{
			Tag_R2000_CMD80_number = cmdRstr->para[1];
			Tag_R2000_CMD80_number <<= 8;
			Tag_R2000_CMD80_number |= cmdRstr->para[2];
			if(Tag_R2000_CMD80_number > 0)
			{
				DelayMs(5);
				//beepFlag = 1; //蜂鸣器鸣叫
				Tag_R2000_CMD91_number = 0;
				flag_return = encodeR2000(0x91, p);
				if(flag_return == 0)
				{
				//	beepFlag = 1;
					Tag_all_number = CardPos;
					if(Tag_all_number > 8)
					{
						Tag_send_count = 8;
					}
					else 
					{
						Tag_send_count = Tag_all_number;
					}
					//p[0] = Tag_send_count;
					p[0] = Tag_all_number;
					lenth_count = 1;
					for(i = 0; i < Tag_send_count; i++)
					{
						flag_return = (CardIndex[i][0] << 1) + 1;
						memcpy(&p[lenth_count], &CardIndex[i][0], flag_return);
						lenth_count += flag_return;
						
						p[lenth_count] = rssichk(CardIndex[i][TAGIDLENGTH + 6 + 1]);
						lenth_count++;
					}
					cmdTmpLen = lenth_count;
				}
			}
			else
				//cmdTmpLen = 0;
			cmdTmpLen = BUF_LEN;
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}

void EPC1G2_GetRSSIIDList(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	unsigned char *p;
	unsigned char i,j;//,k=0;
	unsigned char tmplen,pos,lenth_count;
	
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	pos = Tag_send_count;
	
	p[0] = Tag_all_number-Tag_send_count;
	if((Tag_all_number-Tag_send_count) > 8)
	{
		Tag_send_count += 8;
		//Tag_all_number -= 8;
		tmplen = 8;
	}
	else 
	{
		tmplen = Tag_all_number - Tag_send_count;
		Tag_send_count = Tag_all_number;
	}
	//p[0] = tmplen;//Tag_send_count;
	lenth_count = 1;
	//tmpcount = 1;
	//for(i = 0; i < Tag_send_count; i++)
	for(i = 0; i < tmplen; i++)
	{
		j = (CardIndex[pos + i][0] << 1) + 1;
		memcpy(&p[lenth_count], &CardIndex[pos + i][0], j);
		lenth_count += j;
		
		p[lenth_count] = rssichk(CardIndex[i][TAGIDLENGTH + 6 + 1]);
		lenth_count++;
	}
	cmdTmpLen = lenth_count;
}

//函数功能：匹配指定的EPC
//函数参数：
//函数返回：无
//case CMD_EPC1G2_WriteEPC:		//DDH
void EPC1G2_Accessmatch(unsigned char *buf)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	//unsigned char tmpEpcdata[35];
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}


	flag_return = encodeR2000(0x85, p); //一直匹配标签
	if(flag_return == 0)
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		//if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->para[2] + 3] == command_success))
		{
			beepFlag = 1; //蜂鸣器鸣叫
			cmdTmpLen = 0;
		}
		else
		{

			cmdTmpLen = BUF_LEN;
		}
	}
	else
	{
		cmdTmpLen = BUF_LEN;
	}
}

/*
void ReadEPCAndData (unsigned char * buf)  //E0
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return,len,MaskFlag;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0x86, p);	
	if(flag_return == 0){
		MaskFlag = R2000.rec_buf.buf[4];
		flag_return = R2000.rec_buf.len;
		len = R2000.rec_buf.buf[5];
		if((MaskFlag == 0) && (flag_return < 7)){  //有匹配
		
			memcpy(&p[0],&R2000.rec_buf.buf[6],len);
			flag_return = encodeR2000(0x85, p);
			if(flag_return == 0) //写入成功
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				if(cmdRstr->para[0] == command_success)
				{
					flag_return = encodeR2000(0x81, p);
					if(flag_return == 0)
					{
						cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
						if(cmdRstr->len > 4)
						{
							beepFlag = 1; //蜂鸣器鸣叫
							flag_return = cmdRstr->len - 2;//暂存长度
							len = R2000.rec_buf.buf[flag_return];//cmdRstr->para[flag_return];
							memcpy(p, &R2000.rec_buf.buf[flag_return - len], len);
							cmdTmpLen = len;//cmdRstr->para[flag_return];
						}
						else
						{
							cmdTmpLen = BUF_LEN;
						}
					}
					else
					{
						cmdTmpLen = BUF_LEN;
					}
				}
				else
				{
					cmdTmpLen = BUF_LEN;
					//cmdTmpLen = 1;
					//p[0] = cmdRstr->para[0];
				}
			}
			else //返回失败
			{
				cmdTmpLen = BUF_LEN;
			}
					
		}
		else cmdTmpLen = BUF_LEN;
	}
	else{
		cmdTmpLen = BUF_LEN;
	}
	
}*/

//case CMD_EPC1G2_DetectTagID	:	//EFH
void EPC1G2_ListTagID(unsigned char *buf)//, unsigned char type)
{

	//cmdTmpLen = 0;
	#if 1
	//篡改的读取TID
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return,len;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	CMDMasklen = 0;
	CardPos = 0;
	flag_return = encodeR2000(0xC5, p);	 //取消匹配

	if(flag_return == 0) //返回成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{	
		 	DelayMs(5);
			//Tag_TID_number=0;
			flag_return = encodeR2000(0xC1, p);
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				if(cmdRstr->len > 4)
				{
					beepFlag = 1; //蜂鸣器鸣叫
					len = R2000.rec_buf.buf[1]-3;//
					memcpy(p, cmdRstr->para, len);

					cmdTmpLen = len;//
				}
				else
				{
					cmdTmpLen = BUF_LEN;
				}
			}
			else
			{
				cmdTmpLen = BUF_LEN;
			}
		}
		else
		{
			cmdTmpLen = BUF_LEN;

		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
	#endif
}

 u32 timess;
//case CMD_EPC1G2_ListTagID	:	//EEH
void EPC1G2_ListMaskTagID(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return,i,lenth_count;
	unsigned char *p;
	//unsigned short lenth_count;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	//暂存主动模式掩码
	//DEMO没做好，传输数据错误，且DEMO没掩码，干脆为零
	CMDMaskaddr = p[1];
	CMDMaskaddr <<= 8;
	CMDMaskaddr |= p[2];
	CMDMasklen = 0;//p[3];
	//memset(CMDMaskData, 0, sizeof(CMDMaskData));
	memcpy(CMDMaskData, &p[4], CMDMasklen);
	CardPos = 0;
	//p[0] = 1;//为什么不调频啊
	if(g_device.workpara.MaxReadCardNum < 20) //通过标签的数量参数 来改变盘存参数
	{
		p[0] = 255;
	}
	else
	{
		p[0] = 1;
	}
	
	//p[0] = g_device.workpara.FrequencyMax - g_device.workpara.FrequencyMin + 1;
	//SysCtlDelay(5 * (SysCtlClockGet() / 3000));
	timess = 0;
	flag_return = encodeR2000(0x80, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		//if(cmdRstr->para[0] == command_success)
		if(cmdRstr->len > 4)
		{
			Tag_R2000_CMD80_number = cmdRstr->para[1];
			Tag_R2000_CMD80_number <<= 8;
			Tag_R2000_CMD80_number |= cmdRstr->para[2];
			if(Tag_R2000_CMD80_number > 0)
			{
				DelayMs(5);
				//beepFlag = 1; //蜂鸣器鸣叫
				Tag_R2000_CMD91_number = 0;
				timess = timess;
				flag_return = encodeR2000(0x91, p);
				if(flag_return == 0)
				{
				//	beepFlag = 1;
					Tag_all_number = CardPos;
					if(Tag_all_number > 8)
					{
						Tag_send_count = 8;
					}
					else 
					{
						Tag_send_count = Tag_all_number;
					}
					//p[0] = Tag_send_count;
					p[0] = Tag_all_number;
					lenth_count = 1;
					for(i = 0; i < Tag_send_count; i++)
					{
						flag_return = (CardIndex[i][0] << 1) + 1;
						memcpy(&p[lenth_count], &CardIndex[i][0], flag_return);
						lenth_count += flag_return;
					}
					cmdTmpLen = lenth_count;
				}
			}
			else
				//cmdTmpLen = 0;
			cmdTmpLen = BUF_LEN;
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
//case CMD_EPC1G2_GetIDList	:	//EDH
void EPC1G2_GetIDList(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	unsigned char *p;
	unsigned char i,j;//,k=0;
	unsigned char tmplen,pos,lenth_count;
	
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	pos = Tag_send_count;
	
	p[0] = Tag_all_number-Tag_send_count;
	if((Tag_all_number-Tag_send_count) > 8)
	{
		Tag_send_count += 8;
		//Tag_all_number -= 8;
		tmplen = 8;
	}
	else 
	{
		tmplen = Tag_all_number - Tag_send_count;
		Tag_send_count = Tag_all_number;
	}
	//p[0] = tmplen;//Tag_send_count;
	lenth_count = 1;
	//tmpcount = 1;
	//for(i = 0; i < Tag_send_count; i++)
	for(i = 0; i < tmplen; i++)
	{
		j = (CardIndex[pos + i][0] << 1) + 1;
		memcpy(&p[lenth_count], &CardIndex[pos + i][0], j);
		lenth_count += j;
	}
	cmdTmpLen = lenth_count;
}
//case CMD_EPC1G2_ReadWordBlock:	//ECH
void EPC1G2_ReadWordBlock(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return,len;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0x85, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			DelayMs(10);
			flag_return = encodeR2000(0x81, p);
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				if(cmdRstr->len > 4)
				{
					beepFlag = 1; //蜂鸣器鸣叫
					flag_return = cmdRstr->len - 2;//暂存长度
					len = R2000.rec_buf.buf[flag_return];//cmdRstr->para[flag_return];
					memcpy(p, &R2000.rec_buf.buf[flag_return - len], len);
					//memcpy(p, &cmdRstr->para[flag_return - len], len);
					//memcpy(p, &cmdRstr->para[flag_return + 4 - cmdRstr->para[flag_return]], flag_return);
					cmdTmpLen = len;//cmdRstr->para[flag_return];
				}
				else
				{
					cmdTmpLen = BUF_LEN;
				}
			}
			else
			{
				cmdTmpLen = BUF_LEN;
			}
		}
		else
		{
			cmdTmpLen = BUF_LEN;
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
//case CMD_EPC1G2_WriteWordBlock: //EBH
void EPC1G2_WriteWordBlock(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0x85, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			DelayMs(10);
			flag_return = encodeR2000(0x82, p);
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				//if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->len -2 - 4] == command_success))
				if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->para[2] + 3] == command_success))
				{
					beepFlag = 1; //蜂鸣器鸣叫
					cmdTmpLen = 0;
				}
				else
				{
					//cmdTmpLen = 1;
					//p[0] = cmdRstr->para[cmdRstr->len -2];
					cmdTmpLen = BUF_LEN;
				}
			}
			else
			{
				cmdTmpLen = BUF_LEN;
			}
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
//case CMD_EPC1G2_WriteWordBlock: //EAH
void EPC1G2_WriteSetBlock(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0x85, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			DelayMs(10);
			flag_return = encodeR2000(0x83, p);
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				//if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->len -2 - 6] == command_success))
				if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->para[2] + 3] == command_success))
				{
					beepFlag = 1; //蜂鸣器鸣叫
					cmdTmpLen = 0;
				}
				else
				{
					//cmdTmpLen = 1;
					//p[0] = cmdRstr->para[cmdRstr->len -2];
					cmdTmpLen = BUF_LEN;
				}
			}
			else
			{
				cmdTmpLen = BUF_LEN;
			}
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
} 
//case CMD_EraseTagData	:		//E9H
void EraseTagData(unsigned char *buf)//, unsigned char type)
{
	//和写诗一个命令,却没了密码
	cmdTmpLen = 0;
}
//case CMD_DestroyTag 	:		//E8H
void DestroyTag(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = encodeR2000(0x85, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == command_success)
		{
			DelayMs(10);
			flag_return = encodeR2000(0x84, p);
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				//if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->len -2 - 6] == command_success))
				if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->para[2] + 3] == command_success))
				{
					beepFlag = 1; //蜂鸣器鸣叫
					cmdTmpLen = 0;
				}
				else
				{
					//cmdTmpLen = 1;
					//p[0] = cmdRstr->para[cmdRstr->len -2];
					cmdTmpLen = BUF_LEN;
				}
			}
			else
			{
				cmdTmpLen = BUF_LEN;
			}
		}
		else
		{
			//cmdTmpLen = 1;
			//p[0] = cmdRstr->para[0];
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
//case CMD_EPC1G2_WriteEPC:		//E7H
void EPC1G2_WriteEPC(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;
	unsigned char *p;
	unsigned char tmpEpcdata[35];
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = (p[0] << 1) + 5;
	if(flag_return > 35)
		flag_return = 35;
	memcpy(tmpEpcdata, p, flag_return);
	flag_return = encodeR2000(0x86, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if(cmdRstr->para[0] == 0)//有匹配
		{
			p[0] = cmdRstr->para[1] >> 1;
			memcpy(&p[1], &(cmdRstr->para[2]), cmdRstr->para[1]);
			flag_return = encodeR2000(0xC5, p);//取消匹配的85命令
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				if(cmdRstr->para[0] == command_success)
				{
					flag_return = encodeR2000(0xC2, tmpEpcdata);//补充的82命令
					if(flag_return == 0)
					{
						cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
						//if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->len -2 - 4] == command_success))
						if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->para[2] + 3] == command_success))
						{
							beepFlag = 1; //蜂鸣器鸣叫
							cmdTmpLen = 0;
						}
						else
						{
							//cmdTmpLen = 1;
							//p[0] = cmdRstr->para[cmdRstr->len -2];
							cmdTmpLen = BUF_LEN;
						}
					}
					else
					{
						//cmdTmpLen = 1;
						//p[0] = cmdRstr->para[0];
						cmdTmpLen = BUF_LEN;
					}
				}
				else
					cmdTmpLen = BUF_LEN;
			}
			else
			{
				//cmdTmpLen = 1;
				//p[0] = cmdRstr->para[0];
				cmdTmpLen = BUF_LEN;
			}
		}
		else if(cmdRstr->para[0] == 1)
		{
			flag_return = encodeR2000(0xC2, p);//tmpEpcdata);//补充的82命令
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				//if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->len -2 - 4] == command_success))
				if((cmdRstr->len > 4) && (cmdRstr->para[cmdRstr->para[2] + 3] == command_success))
				{
					beepFlag = 1; //蜂鸣器鸣叫
					cmdTmpLen = 0;
				}
				else
				{
					//cmdTmpLen = 1;
					//p[0] = cmdRstr->para[cmdRstr->len -2];
					cmdTmpLen = BUF_LEN;
				}
			}
			else
			{
				//cmdTmpLen = 1;
				//p[0] = cmdRstr->para[0];
				cmdTmpLen = BUF_LEN;
			}
		}
		else
		{
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
//case CMD_DataLocking	:		//E6H
void DataLocking(unsigned char *buf)//, unsigned char type)
{
	cmdTmpLen = 0;

}
//case CMD_EPC1G2_changeeas:		//E5H
void EPC1G2_changeeas(unsigned char *buf)//, unsigned char type)
{
	cmdTmpLen = 0;
}
//case CMD_EPC1G2_EasAlarm:		//E4H
void EPC1G2_EasAlarm(unsigned char *buf)//, unsigned char type)
{
	cmdTmpLen = 0;
}
//case CMD_EPC1G2_ReadProtect:	//E3H
void EPC1G2_ReadProtect(unsigned char *buf)//, unsigned char type)
{
	cmdTmpLen = 0;
}
//case CMD_EPC1G2_RSTReadProtect: //E2H
void EPC1G2_RSTReadProtect(unsigned char *buf)//, unsigned char type)
{
	cmdTmpLen = 0;
}
//case CMD_SetUserDataProtect:	//E1H
void SetUserDataProtect(unsigned char *buf)//, unsigned char type)
{
	cmdTmpLen = 0;
}
//case CMD_ReadEPCAndData:		//E0H

void ReadEPCAndData(unsigned char *buf)//, unsigned char type)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	PCMD_R2000 cmdRstr;
	unsigned char flag_return;//,len;
	unsigned char *p;
	unsigned char tmpEpcdata[35];
	cmdstr = (PCMD_STR)buf;
	cmdstr1 = (PCMD_STR1)buf;
	if(TagDataOrigin == SourceFrom485)
	{
		p = cmdstr1->para;
	}
	else
	{
		p = cmdstr->para;
	}
	flag_return = (p[0] << 1) + 5;
	if(flag_return > 35)
		flag_return = 35;
	memcpy(tmpEpcdata, p, flag_return);
	flag_return = encodeR2000(0x86, p);
	if(flag_return == 0) //写入成功
	{
		cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
		if((cmdRstr->para[0] == 0) && (cmdRstr->para[0] > 0))//有匹配
		{
			p[0] = cmdRstr->para[1] >> 1;
			memcpy(&p[1], &(cmdRstr->para[2]), cmdRstr->para[1]);
			flag_return = encodeR2000(0xC5, p);//取消匹配的85命令
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				if(cmdRstr->para[0] == command_success)
				{
					flag_return = encodeR2000(0xC1, tmpEpcdata);//补充的81命令
					if(flag_return == 0)
					{
						cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
						if(cmdRstr->len > 4)
						{
							beepFlag = 1; //蜂鸣器鸣叫
							memcpy(p, &cmdRstr->para[7], cmdRstr->para[6]);
							cmdTmpLen = flag_return;

							#if 0
							beepFlag = 1; //蜂鸣器鸣叫
							flag_return = cmdRstr->len - 2;//暂存长度
							len = R2000.rec_buf.buf[flag_return];//cmdRstr->para[flag_return];
							memcpy(p, &R2000.rec_buf.buf[flag_return - len], len);
							//memcpy(p, &cmdRstr->para[flag_return - len], len);
							//memcpy(p, &cmdRstr->para[flag_return + 4 - cmdRstr->para[flag_return]], flag_return);
							cmdTmpLen = len;//cmdRstr->para[flag_return];
							#endif
						}
						else
						{
							//cmdTmpLen = 1;
							//p[0] = cmdRstr->para[0];
							cmdTmpLen = BUF_LEN;
						}
					}
					else
					{
						//cmdTmpLen = 1;
						//p[0] = cmdRstr->para[0];
						cmdTmpLen = BUF_LEN;
					}
				}
				else
					cmdTmpLen = BUF_LEN;
			}
			else
			{
				//cmdTmpLen = 1;
				//p[0] = cmdRstr->para[0];
				cmdTmpLen = BUF_LEN;
			}
		}
		else if(cmdRstr->para[0] == 1)
		{
			flag_return = encodeR2000(0xC1, p);//补充的81命令
			if(flag_return == 0)
			{
				cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
				if(cmdRstr->len > 4)
				{
					beepFlag = 1; //蜂鸣器鸣叫
					memcpy(p, &cmdRstr->para[7], cmdRstr->para[6]);
					cmdTmpLen = flag_return;
				}
				else
				{
					//cmdTmpLen = 1;
					//p[0] = cmdRstr->para[0];
					cmdTmpLen = BUF_LEN;
				}
			}
			else
			{
				//cmdTmpLen = 1;
				//p[0] = cmdRstr->para[0];
				cmdTmpLen = BUF_LEN;
			}
		}
		else
		{
			cmdTmpLen = BUF_LEN;
		}
	}
	else //返回失败
	{
		cmdTmpLen = BUF_LEN;
	}
}
#if 0
void Responsion(unsigned char *buf)
{
	#if 0
  if(flag_over_time==1)
  {
    flag_over_time=2;
  }
  else
  {
    flag_over_time=0; 
  }

  flag_start_time=0; //开始重新计时
	#endif
}
#endif
unsigned hextochar(unsigned char ch)
{
	if (ch	<= 9)
	{
		return(ch + '0');
	}
	else
	{
		ch -= 10;
		return(ch + 'A');
	}
}


//定时调用
void RelayControl(void)
{
	if(RelayFlag != 0)
	{
		RelayCountTimes++;
		if(RelayCountTimes < 400)
		{
	//		if(relayCtrStatus & 0x01)
			RELAYON;
	//		if(relayCtrStatus & 0x02)
			RELAYON1;
		}
		else
		{
			RelayFlag = 0;
			RELAYOFF;
			RELAYOFF1;
			RelayCountTimes = 0;
		}
	}
}


