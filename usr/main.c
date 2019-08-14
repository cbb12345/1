/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Main.c                                                                      **
** Last modified Date:      2012.7.12                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             主应用模块                                                                  **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              刘飞                                                                        **
** Created date:            2012.7.12                                                                   **
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
#include "netconf.h"
#include "Main.h"
#include "IIC.h"

#include "RTC_DS1302.h"
#include "stm32f10x.h"
#include "common.h"
#include "ParamItem.h"
#include "UDP_module.h"
#include "Local_232_module.h"
#include "Local_485_module.h"
#include "RFIDModule.h"
#include "R2000.h"
#include "WG.h"
#include "stm32flash.h"

/*********************************************************************************************************
   板子相关宏定义
*********************************************************************************************************/
//#define WORK_FLAG     0X5a5a5a22
#define SYSTEMTICK_PERIOD_MS  	10
#define EnableWatchDog 			1


/*********************************************************************************************************
  全局CONSTANTS 常量定义
*********************************************************************************************************/


/*********************************************************************************************************
  全局VARIABLES 变量定义
*********************************************************************************************************/
SDevice g_device;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay, TimingmsDelay;

//[0-1]固件版本 [2-3]软件版本
char Version[4] = {0x03, 0x00, 0x07, 0x03};
unsigned char ReadCardTimeInterver;

#define LEDBASECOUNT 80

unsigned int sys_timer = 0, sys_timer1 = 0;
unsigned char beepFlag = 0;
unsigned char beepCountTimes = 0;


STimer ReadCardTimer;
STimer OutPutTimer;
//STimer EPCATATimer;

unsigned char cmdTmpBuf[BUF_LEN],cmd57ReadTagCount = 0,cmdTmpLen;

int SelfCheckFailure = 0;


unsigned char CardIndex[MAXNUM][TAGIDLENGTH + 6 + 2] = {0,0};

//unsigned char CardTotal = 0;
//unsigned char CardPos = 0;
unsigned int CardPos = 0;
//unsigned int CardPoslast = 0;

//不用建表，数据很有规律 刘飞
//SFREQ curfreq;

//STagFilter tagfilter;
//unsigned char FilterMask[TAGIDLENGTH],FilterMaskLen = 0;

//extern Freq Frequencies;
//SFrequencies Frequencies;

//没啥用extern u8 guiActiveProfile, guiNumFreqs;

extern struct udp_pcb *UdpPcb;

extern SParamTable g_paramTable;
extern SUIP_TRANS_DATA uip;
unsigned int read_count = 0;

//extern void Rf3392_init(void);
//unsigned char FirtReaderTime[6];

extern unsigned char frequency_num;
unsigned char sysReboot = 0,tmpAntenna;
//unsigned char AllowOutput = 0;
extern SLOCAL_TRANS_DATA local;
extern SLOCAL_485_DATA local485;

//extern unsigned char delayFlag0X0F;

extern SR2000_TRANS_DATA R2000;
extern unsigned char AntId;
unsigned char switchAtenna = 0,sendswitchAtenna = 0;

//unsigned char TagSaveCount = 0, TagSaveBuf[SAVEMAXNUM][TAGIDLENGTH + 6 + 2] = {0};
unsigned char TagSaveBuf[SAVEMAXNUM][TAGIDLENGTH + 6 + 2] = {0};
unsigned int TagSaveCount = 0; 

int bakrssi = 0;
const char rssitab[] = {
99,98,97,96,95,94,93,92,91,90,
89,88,87,86,85,84,83,82,81,80,
79,78,77,76,75,74,73,72,71,70,
69,68,67,66,65,64,63,62,61,60,
59,58,57,56,55,54,53,52,51,50,
49,48,47,46,45,44,43,42,41,39,
38,37,36,35,34,33,32,31
};
int rssichk(int rssi)
{
	if(rssi == 0)
	{
		if(bakrssi == 0)
			bakrssi = 98;
		rssi = bakrssi;
	}
	{
		if(rssi > 98)
			rssi = 98;
		else if(rssi < 31)
			rssi = 31;
		bakrssi = rssi;
//		printf("rssi = %d\n", rssi);
//		printf("rssitab[rssi-31] = %d\n", rssitab[rssi-31]);
		return ((int)rssitab[rssi-31]);
	}
}

/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
void SelfCheck(void)
{
/*
-------闪  1  次:参数表分配空间不够溢出
-------闪  2  次:EEPROM 24C16 存储不成功
-------闪  3  次:串口BPS设置超范围
-------闪  4  次:以太网初始化失败
-------闪  5  次:R2000复位启动失败
-------闪  6  次:R2000输出功率失败
-------闪  7  次:R2000内部蜂鸣器关闭失败
-------闪  8  次:R2000频点设置失败
       
*/
	int i;
	for(i = 0;i < SelfCheckFailure; i++)
	{
		//REDLEDOFF;
		//YELLOLEDOFF;
		LEDGreenON;
		BEEPON;
		//Delay2us(45 * 5000);
		DelayMs(450);
		//REDLEDON;
		//YELLOLEDON;
		LEDGreenOFF;
		BEEPOFF;
		//Delay2us(45 * 5000);
		DelayMs(450);
	}
}

void reboot(void)
{
	#if (EnableWatchDog != 0)
	if(1 == sysReboot)
	{
		if(g_paramTable.setFlag == 0)
		{
			LEDYellowON;
			LEDGreenON;
			while(1);
		}
		IWDG_ReloadCounter();
	}
	else
		IWDG_ReloadCounter();
	#endif
}

void Watchdog_init(void)
{
	/* LSI的启动*/
	//RCC_LSICmd(ENABLE);//打开LSI
	//while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);//等待直到LSI稳定

  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  IWDG_SetPrescaler(IWDG_Prescaler_32);

  IWDG_SetReload(1250);	//1s

  IWDG_ReloadCounter();

  IWDG_Enable();
}


#define	R2000_SED_TIMER	200//50 //500MS

 void SelFreq(void)
{
	PCMD_R2000 cmdRstr;
	unsigned long Ftemp=0;
//	unsigned char *PFtemp=NULL;
	unsigned char tep[3];


//没用了，函数
//	switch(g_device.FreqSelectNum)
//	{
//		case 1://FCC
//		case 2://ETSI
//		case 3://CHN
//		default:
//			break;
//	}
	R2000.rec_buf.buf[0] = 0xA0;
	R2000.rec_buf.buf[1] = 0x06;
	R2000.rec_buf.buf[2] = 0xFF;
	R2000.rec_buf.buf[3] = 0x78;
	R2000.rec_buf.buf[4] = g_device.FreqSelect.region; //频率区域范围
	R2000.set_mark = 0;
	if( g_device.FreqSelect.region < 4){
		R2000.rec_buf.buf[5] = g_device.workpara.FrequencyMin; //起始频点
		R2000.rec_buf.buf[6] = g_device.workpara.FrequencyMax; //结束频点
		R2000.rec_buf.buf[7] = SetChecksum(R2000.rec_buf.buf, 7); 	
		UART4Send(R2000.rec_buf.buf, 8);
	}
	else if( g_device.FreqSelect.region == 4)
	{
		
		R2000.rec_buf.buf[1] = 0x09;
		R2000.rec_buf.buf[5] = g_device.FreqSelect.para[0];
		R2000.rec_buf.buf[6] = g_device.workpara.FrequencyMax - g_device.workpara.FrequencyMin;

		tep[0] = g_device.FreqSelect.para[2];
		tep[1] = g_device.FreqSelect.para[3];
		tep[2] = g_device.FreqSelect.para[4];

		//Ftemp = (unsigned long)&g_device.FreqSelect.para[3];
		//Ftemp = (unsigned long)&tep[0];
		memcpy(&Ftemp, &tep[0], 3);
		Ftemp += g_device.workpara.FrequencyMin*g_device.FreqSelect.para[0];
		memcpy( &tep[0],&Ftemp, 3);
		
		//memcpy(&R2000.rec_buf.buf[7], &Ftemp, 3);
	//	PFtemp = (unsigned char *)&Ftemp;
		R2000.rec_buf.buf[7] = tep[2];
		R2000.rec_buf.buf[8] = tep[1];
		R2000.rec_buf.buf[9] = tep[0];
		
		//R2000.rec_buf.buf[7] = g_device.FreqSelect.para[4];
		//R2000.rec_buf.buf[8] = g_device.FreqSelect.para[3];
		//R2000.rec_buf.buf[9] = g_device.FreqSelect.para[2];

		
		R2000.rec_buf.buf[10] = SetChecksum(R2000.rec_buf.buf, 10); 	
		UART4Send(R2000.rec_buf.buf, 11);		
	} 	

	//失败了初始化不成功
	timerStart(&R2000.send_timer,R2000_SED_TIMER);
	while(timerIsOverflow(&R2000.send_timer) == 0)
	{
		if( 0 != R2000.set_mark)
		{
			R2000.set_mark = 0;
			cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
			//if((cmdRstr->cmd == 0x78) && (cmdRstr->para[0] == command_success))
			if(cmdRstr->cmd == 0x78)	
			{
				timerStop(&R2000.send_timer);
				if(cmdRstr->para[0] == command_success)
					return;
				else
					break;
			}
		}
	}
	timerStop(&R2000.send_timer);
	SelfCheckFailure = 8;
	R2000.set_mark = 0;
}

void registryParams(void)
{
//其他命令
	registryParam(0x01, (void *)&g_device.workpara.BaudRate, Flag_ParamItem_WR, sizeof(g_device.workpara.BaudRate), SetBaudRate);
	registryParam(0x02, (void *)g_device.GetReaderVersion, Flag_ParamItem_RD, sizeof(g_device.GetReaderVersion), NULL);
	registryParam(0x03, (void *)&g_device.RelayStatus, Flag_ParamItem_WR, sizeof(g_device.RelayStatus), SetRelay);
	registryParam(0x04, (void *)&g_device.workpara.OutputPower, Flag_ParamItem_OTHR, sizeof(g_device.workpara.OutputPower), SetOutputPower);
	registryParam(0x05, (void *)&g_device.workpara.FrequencyMin, Flag_ParamItem_OTHR, sizeof(g_device.workpara.FrequencyMin) + sizeof(g_device.workpara.FrequencyMax), SetFrequency);
	registryParam(0x06, (void *)&g_device.workpara, Flag_ParamItem_RD, sizeof(g_device.workpara), ReadParam);
	registryParam(0x07, (void *)&g_device.workpara.RadioDepth, Flag_ParamItem_WR, sizeof(g_device.workpara.RadioDepth), WriteRadioDeep);
	registryParam(0x08, (void *)&g_device.workpara.RadioDepth, Flag_ParamItem_RD, sizeof(g_device.workpara.RadioDepth), ReadRadioDeep);
	registryParam(0x09, (void *)&g_device.workpara, Flag_ParamItem_WR, sizeof(g_device.workpara), WriteParam);
	//registryParam(0x0A, (void *)&g_device.AutoParam.Antenna, Flag_ParamItem_WR, sizeof(g_device.AutoParam.Antenna), SetAntenna);
	registryParam(0x0A, (void *)&tmpAntenna, Flag_ParamItem_OTHR, sizeof(tmpAntenna), SetAntenna);
///////10
	registryParam(0x0B, (void *)&g_device.RelayStatus, Flag_ParamItem_RD, sizeof(g_device.RelayStatus), NULL);
	registryParam(0x0C, (void *)&g_device.Factoryworkpara, Flag_ParamItem_WR, sizeof(g_device.Factoryworkpara), SetReaderPara);
	registryParam(0x0D, (void *)NULL, Flag_ParamItem_WR, sizeof(NULL), ReadReaderPara);
	//registryParam(0x0E, (void *)&g_device.Reboot, Flag_ParamItem_WR, sizeof(g_device.Reboot), Reboot);
	registryParam(0x0E, (void *)&sysReboot, Flag_ParamItem_WR, sizeof(sysReboot), Reboot);
	registryParam(0x0F, (void *)&g_device.workpara.ReaderWorkStatus, Flag_ParamItem_WR, sizeof(g_device.workpara.ReaderWorkStatus), SetReaderWorkType);
	registryParam(0x10, (void *)NULL, Flag_ParamItem_WR, sizeof(NULL), ClearTagListMem);
	registryParam(0x11, (void *)g_device.ReaderTime, Flag_ParamItem_WR, sizeof(g_device.ReaderTime), SetReaderTime);
	registryParam(0x12, (void *)g_device.ReaderTime, Flag_ParamItem_RD, sizeof(g_device.ReaderTime), GetReaderTime);
	registryParam(0x13, (void *)&g_device.AutoParam, Flag_ParamItem_WR, sizeof(g_device.AutoParam), SetAutoPara);
	registryParam(0x14, (void *)&g_device.AutoParam, Flag_ParamItem_RD, sizeof(g_device.AutoParam), GetAutoPara);
////////20
	registryParam(0x15, (void *)&g_device.tagfilter, Flag_ParamItem_WR, sizeof(g_device.tagfilter), SetTagFilter);
	registryParam(0x16, (void *)&g_device.tagfilter, Flag_ParamItem_RD, sizeof(g_device.tagfilter), NULL);
	registryParam(0x17, (void *)&g_device.FactoryAutoParam, Flag_ParamItem_WR, sizeof(g_device.FactoryAutoParam), NULL);
	registryParam(0x18, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), read_r2000vertion);//获取模块版本号
	registryParam(0x30, (void *)&g_device.workpara.ReaderIP1, Flag_ParamItem_WR, 14*sizeof(g_device.workpara.ReaderIP1), SetReaderNetwork);
	registryParam(0x31, (void *)&g_device.workpara.ReaderIP1, Flag_ParamItem_RD, 14*sizeof(g_device.workpara.ReaderIP1), NULL);
	registryParam(0x32, (void *)&g_device.workpara.ReaderMAC1, Flag_ParamItem_WR, 6*sizeof(g_device.workpara.ReaderMAC1), SetReaderMAC);
	registryParam(0x33, (void *)&g_device.workpara.ReaderMAC1, Flag_ParamItem_RD, 6*sizeof(g_device.workpara.ReaderMAC1), NULL);
	registryParam(0x41, (void *)&g_device.ReaderRevStatus, Flag_ParamItem_RD, sizeof(g_device.ReaderRevStatus), GetReaderRevStatus);
	registryParam(0x42, (void *)&g_device.ReaderOutStatus, Flag_ParamItem_WR, sizeof(g_device.ReaderOutStatus), SetReaderOutStatus);
	registryParam(0x43, (void *)&g_device.ReaderOutStatus, Flag_ParamItem_RD, sizeof(g_device.ReaderOutStatus), GetReaderOutStatus);
///////30
	registryParam(0x54, (void *)NULL, Flag_ParamItem_WR, sizeof(NULL), ImmediatelyNotify);
	//registryParam(0x57, (void *)cmdTmpBuf, Flag_ParamItem_RD, sizeof(cmdTmpBuf), GetTagRecord);
	registryParam(0x57, (void *)cmdTmpBuf, Flag_ParamItem_RD, 5, GetTagRecord);
	//registryParam(0x66, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), Responsion);//心跳包 处理
	registryParam(0x85, (void *)g_device.GetReaderVersion, Flag_ParamItem_WR, sizeof(g_device.GetReaderVersion) - 2, NULL);
	//registryParam(0x86, (void *)&g_device.FreqSelectNum, Flag_ParamItem_WR, sizeof(g_device.FreqSelectNum), ModifyReaderFrequenceRange);
	registryParam(0x86, (void *)&g_device.FreqSelect, Flag_ParamItem_WR, sizeof(g_device.FreqSelect), SetFrequency1);// NULL);//SetFrequency1);
	registryParam(0x87, (void *)&g_device.FreqSelect, Flag_ParamItem_RD, sizeof(g_device.FreqSelect), NULL);
	registryParam(0x88, (void *)NULL, Flag_ParamItem_WR, sizeof(NULL), UpdateProgrram);
	registryParam(0x8B, (void *)g_device.ReaderID, Flag_ParamItem_WR, sizeof(g_device.ReaderID), NULL);
	registryParam(0x8C, (void *)g_device.ReaderID, Flag_ParamItem_RD, sizeof(g_device.ReaderID), NULL);
	registryParam(0x8D, (void *)NULL, Flag_ParamItem_RD, sizeof(NULL), NULL);


	//6C
	registryParam(0xDF, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_ListRSSIMaskTag);
	registryParam(0xDE, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_GetRSSIIDList);

	registryParam(0xDD, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_Accessmatch);  //匹配标签命令
	registryParam(0xEF, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_ListTagID);
	registryParam(0xEE, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_ListMaskTagID);
//////////40
	registryParam(0xED, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_GetIDList);
	registryParam(0xEC, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_ReadWordBlock);
	registryParam(0xEB, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_WriteWordBlock);
	registryParam(0xEA, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_WriteSetBlock);
	registryParam(0xE9, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EraseTagData);
	registryParam(0xE8, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), DestroyTag);
	registryParam(0xE7, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_WriteEPC);
	registryParam(0xE6, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), DataLocking);
	registryParam(0xE5, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_changeeas);
	registryParam(0xE4, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_EasAlarm);
//////////50
	registryParam(0xE3, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_ReadProtect);
	registryParam(0xE2, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), EPC1G2_RSTReadProtect);
	registryParam(0xE1, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), SetUserDataProtect);
	registryParam(0xE0, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), ReadEPCAndData);
	//6B
	registryParam(0xFF, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), DetectTagNum);
	registryParam(0xFE, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), ReadLabelID);
	registryParam(0xFD, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), ListIDReport);
	registryParam(0xFB, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), ListSelectedID);
	registryParam(0xF6, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), ReadByteBlock);
	registryParam(0xF5, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), WriteByteBlock);
/////////60
	registryParam(0xF4, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), WriteProtect);
	registryParam(0xF3, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), ReadWriteProtect);
	registryParam(0xF2, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), WriteAByte);
	//ATA增加新命令
	//registryParam(0xDE, (void *)NULL, Flag_ParamItem_OTHR, sizeof(NULL), ATA_ListTagID);
	
}

static u32 Relay1Times, Relay2Times;
void relayON(u8 RelayIndex,u32 longtime_ms)
{
	if ( RelayIndex == 1)
	{
		if ( Relay1Times != 0)
		{
			DelayMs(150);
			return;
		}
		RELAYON;
		Relay1Times = longtime_ms;
	}else{
		if ( Relay2Times != 0)
		{
			DelayMs(150);
			return;
		}
		RELAYON1;
		Relay2Times = longtime_ms;
	}
}
void relay_isr_polling(void)
{
	if ( Relay1Times != 0 )
	{
		--Relay1Times;
		if ( Relay1Times == 0 )
			RELAYOFF;
	}

	if ( Relay2Times != 0 )
	{
		--Relay2Times;
		if ( Relay2Times == 0 )
			RELAYOFF1;
	}
}

void initPara(void)
{
	memset(&ReadCardTimer, 0, sizeof(ReadCardTimer));
	memset(&OutPutTimer, 0, sizeof(OutPutTimer));
	memset(&g_device, 0, sizeof(g_device));
	//memset(&g_device.tagfilter, 0, sizeof(g_device.tagfilter));
	//memset(&FilterMask, 0, sizeof(FilterMask));
//	memset(&Frequencies, 0, sizeof(Frequencies));
	memset(CardIndex, 0, sizeof(CardIndex));
	
	ReadPara();

	registryParams();

	if( g_device.AutoParam.ReadTAGType != 1 )
	{
		if(g_device.RelayStatus & 0x01)
			RELAYON;
		else
			RELAYOFF;
		if(g_device.RelayStatus & 0x02)
			RELAYON1;
		else
			RELAYOFF1;
	}else{
		RELAYOFF;
		RELAYOFF1;
	}

	//SWITCH485REV;
	switch(g_device.AutoParam.ReadTagInterval)
	{
	    case 0:
			ReadCardTimeInterver = 1;
			break;
		case 1:
			ReadCardTimeInterver = 2;
			break;
		case 2:
			ReadCardTimeInterver = 3;
			break;
		case 3:
			ReadCardTimeInterver = 5;
			break;
		case 4:
			ReadCardTimeInterver = 10;
			break;
		default:
			ReadCardTimeInterver = 3;
	}
	#if 0
	SetModDepth();
	OutputPower();
	#endif
	//SelFreq();
//	FilterMaskLen = ReportFilter( g_device.tagfilter.TagMaskLenH << 8 +g_device.tagfilter.TagMaskLenL,
//		g_device.tagfilter.TagMaskAddrH << 8 + g_device.tagfilter.TagMaskAddrL ,g_device.tagfilter.TagMaskData, FilterMask);
	//g_device.workpara.ReaderWorkStatus = 0;

	if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
	{
		//cyclic = 1;
		timerStart(&ReadCardTimer, ReadCardTimeInterver);
		if((g_device.AutoParam.CardOutModule == OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule == OUTPUT_Wiegan34))
			timerStart(&OutPutTimer, 100);
		else
			timerStart(&OutPutTimer, g_device.AutoParam.CallPCTimeInterver * 100);
			
		tmpAntenna = g_device.AutoParam.Antenna;
		//AntennaCtrl();
	}
}

#if (PARA_STORAGE_DEVICE == FLASH_AT24C02)
void ReadPara(void)
{
	unsigned char i;
	for(i = 0; i < 5; i++)
	{
		I2C_EE_BufferRead((unsigned char*)(&g_device), 1024, sizeof(g_device));//D1
		if(g_device.Flag == WORK_FLAG)
		{
			break;
		}
	}
	//I2C_EE_BufferRead((unsigned char*)(&g_device), 0, sizeof(g_device));//D1
	//I2C_Read(I2C1,0xA0,0,(unsigned char*)(&g_device),sizeof(g_device));
	if(g_device.Flag != WORK_FLAG)
	{
		defaultPara();
		SaveData();
		g_device.Flag = 0;
		I2C_EE_BufferRead((unsigned char*)&g_device.Flag, 1024, sizeof(g_device.Flag));
		if(g_device.Flag != WORK_FLAG)
			SelfCheckFailure = 2;
	//	I2C_EE_BufferRead((unsigned char*)(&g_device), 0, sizeof(g_device));//D1
	}
	//g_device.GetReaderVersion[0] = Version[0];
	//g_device.GetReaderVersion[1] = Version[1];
	g_device.GetReaderVersion[2] = Version[2];
	g_device.GetReaderVersion[3] = Version[3];
}
#else
void ReadPara(void)
{
	unsigned char i;
	int temp;
	
	for(i = 0; i < 5; i++)
	{
		//I2C_EE_BufferRead((unsigned char*)(&g_device), 1024, sizeof(g_device));//D1
		temp = sizeof(g_device);
		
		STMFLASH_Read(PARA_STORAGE_ADDR,(u16*)(&g_device),sizeof(g_device)/2);

		if(g_device.Flag == WORK_FLAG)
		{
			break;
		}
	}

	if(g_device.Flag != WORK_FLAG)
	{
		defaultPara();
		SaveData();
		g_device.Flag = 0;
		//I2C_EE_BufferRead((unsigned char*)&g_device.Flag, 1024, sizeof(g_device.Flag));
		//因为g_device.Flag是结构体的第一个成员,因此直接读PARA_STORAGE_ADDR地址,读4字节即可
		STMFLASH_Read(PARA_STORAGE_ADDR,(u16*)(&g_device.Flag),sizeof(g_device.Flag)/2);
		if(g_device.Flag != WORK_FLAG)
			SelfCheckFailure = 2;
	}
	//g_device.GetReaderVersion[0] = Version[0];
	//g_device.GetReaderVersion[1] = Version[1];
	g_device.GetReaderVersion[2] = Version[2];
	g_device.GetReaderVersion[3] = Version[3];
}
#endif

void defaultPara(void)
{
	uint32_t i,j=0;
	memset(&g_device, 0, sizeof(g_device));
	//基本工作参数
	g_device.workpara.BaudRate = 0x08;
	g_device.workpara.OutputPower = 25;
	g_device.workpara.FrequencyMin = 7;//43;//0;
	g_device.workpara.FrequencyMax = 59;//48;//59;
	g_device.workpara.RadioDepth = 0x30;//0x3f;//80;
	g_device.workpara.ReaderWorkStatus = WORK_ACT;
	g_device.workpara.Address = 1;
	g_device.workpara.MaxReadCardNum = 10;//2;
	g_device.workpara.TagType = 4;////1;
	g_device.workpara.TimeOfReadCard = 0;
	g_device.workpara.ReadCardCount = 1;
	g_device.workpara.AlarmRnable = 1;
	g_device.workpara.ReaderIP1 = 192;
	g_device.workpara.ReaderIP2 = 168;
	g_device.workpara.ReaderIP3 = 0;
	g_device.workpara.ReaderIP4 = 100;
	g_device.workpara.ReaderPortHigh = 0x07;//0x13;//5000
	g_device.workpara.ReaderPortLow = 0xb1;//0x88;//5000
	g_device.workpara.ReaderNetMask1 = 255;
	g_device.workpara.ReaderNetMask2 = 255;
	g_device.workpara.ReaderNetMask3 = 255;
	g_device.workpara.ReaderNetMask4 = 0;
	g_device.workpara.ReaderGateway1 = 192;
	g_device.workpara.ReaderGateway2 = 168;
	g_device.workpara.ReaderGateway3 = 0;
	g_device.workpara.ReaderGateway4 = 1;
	g_device.workpara.ReaderMAC1 = 1;
	g_device.workpara.ReaderMAC2 = 2;
	g_device.workpara.ReaderMAC3 = 3;
	g_device.workpara.ReaderMAC4 = 4;
	g_device.workpara.ReaderMAC5 = 5;
	g_device.workpara.ReaderMAC6 = 6;
    //自动工作参数
    g_device.AutoParam.ReadTAGType = 0;
	//标签保留时间不做该功能
    g_device.AutoParam.TagExisTimeH = 1;
    g_device.AutoParam.TagExisTimeL = 1;
	
    g_device.AutoParam.ReadTagInterval = 2;
    g_device.AutoParam.TagHoldNumH = 1;
    g_device.AutoParam.TagHoldNumL = 1;
    g_device.AutoParam.OutputDataType = 0;//3;//0;//208确实能够是3好点，前面的不支持
    g_device.AutoParam.CardOutModule = 3;//0;
    g_device.AutoParam.WidthOfWiegand = 40;
    g_device.AutoParam.InterverOfWiegand = 200;
    g_device.AutoParam.CardStartPos = 0;
    g_device.AutoParam.CardSaveInTagPosition = 0;
    g_device.AutoParam.CallPCTimeInterver = 120;
    g_device.AutoParam.CallPCType = 0;
    g_device.AutoParam.CallPCEnable = 0;
    g_device.AutoParam.Antenna = 1;
    g_device.AutoParam.TriggleType = 0;
    g_device.AutoParam.PCIP1 = 192;
    g_device.AutoParam.PCIP2 = 168;
    g_device.AutoParam.PCIP3 = 0;
    g_device.AutoParam.PCIP4 = 61;
    g_device.AutoParam.PCPortHigh = 0x13;
    g_device.AutoParam.PCPortLow = 0x88;
	g_device.AutoParam.MaxEPCATAtime = 1000;
    g_device.AutoParam.CheckAlarmEnable = 0;
    //g_device.AutoParam.OutputTimeInterver = 120;
    g_device.AutoParam.RelayCTLEnable = 0;
	g_device.AutoParam.EPCATAReadType = 0;
    //默认工作参数
    //g_device.SetReaderWorkType = WORK_CON;
	//g_device.workpara.BaudRate = 0x04;
	g_device.GetReaderVersion[0] = Version[0];
	g_device.GetReaderVersion[1] = Version[1];
	g_device.GetReaderVersion[2] = Version[2];
	g_device.GetReaderVersion[3] = Version[3];
    g_device.RelayStatus = 0;
    //g_device.FrequencyMin = 1;
    //g_device.FrequencyMax = 1;
    //g_device.Reboot = 0;
    //g_device.SetReaderAutoWorkType = 1;
    //g_device.ClearTagListMem = 0;
    //g_device.ReaderTime[] = 0;
    //g_device.TagFilter[] = 0;
    g_device.ReaderRevStatus = 0;
    g_device.ReaderOutStatus= 0;
    g_device.ImmediatelyNotify = 0;
    g_device.GetTagRecord = 0;

	
    g_device.ModifyReaderVersion = 0;
//    g_device.FreqSelect = 0;
	g_device.FreqSelect.region = 1;
    //g_device.BootUpdataFlag = 0;

	//------------------------------------------------------
	//  恢复出厂设置时候,不要恢复读卡器ID号   潘炳阳20170615 
	//--------------------------------------------------------
	for ( i=0 ; i<10 ; i++)
	{
		j += g_device.ReaderID[i]  ;
	}

	if ( j == 0 )
	{
		g_device.ReaderID[0] = '1';
		g_device.ReaderID[1] = '2';
		g_device.ReaderID[2] = '3';
		g_device.ReaderID[3] = '4';
		g_device.ReaderID[4] = '5';
		g_device.ReaderID[5] = '6';
		g_device.ReaderID[6] = '7';
		g_device.ReaderID[7] = '8';
		g_device.ReaderID[8] = '9';
		g_device.ReaderID[9] = '0';
	}
	//-------------------------------------


	//chang jia can shu
	g_device.Factoryworkpara.BaudRate = 0x08;
	g_device.Factoryworkpara.OutputPower = 25;
	g_device.Factoryworkpara.FrequencyMin = 7;//43;
	g_device.Factoryworkpara.FrequencyMax = 59;//48;
	g_device.Factoryworkpara.RadioDepth = 0x30;//0x3f;//80;
	g_device.Factoryworkpara.ReaderWorkStatus = WORK_ACT;
	g_device.Factoryworkpara.Address = 1;
	g_device.Factoryworkpara.MaxReadCardNum = 10;//2;
	g_device.Factoryworkpara.TagType = 4;//2;////1;
	g_device.Factoryworkpara.TimeOfReadCard = 0;
	g_device.Factoryworkpara.ReadCardCount = 1;
	g_device.Factoryworkpara.AlarmRnable = 1;
	g_device.Factoryworkpara.ReaderIP1 = 192;
	g_device.Factoryworkpara.ReaderIP2 = 168;
	g_device.Factoryworkpara.ReaderIP3 = 0;
	g_device.Factoryworkpara.ReaderIP4 = 100;
	g_device.Factoryworkpara.ReaderPortHigh = 0x07;//1969  13;//5000
	g_device.Factoryworkpara.ReaderPortLow = 0xB1;//1969  88;//5000
	g_device.Factoryworkpara.ReaderNetMask1 = 255;
	g_device.Factoryworkpara.ReaderNetMask2 = 255;
	g_device.Factoryworkpara.ReaderNetMask3 = 255;
	g_device.Factoryworkpara.ReaderNetMask4 = 0;
	g_device.Factoryworkpara.ReaderGateway1 = 192;
	g_device.Factoryworkpara.ReaderGateway2 = 168;
	g_device.Factoryworkpara.ReaderGateway3 = 0;
	g_device.Factoryworkpara.ReaderGateway4 = 1;
	g_device.Factoryworkpara.ReaderMAC1 = 1;
	g_device.Factoryworkpara.ReaderMAC2 = 2;
	g_device.Factoryworkpara.ReaderMAC3 = 3;
	g_device.Factoryworkpara.ReaderMAC4 = 4;
	g_device.Factoryworkpara.ReaderMAC5 = 5;
	g_device.Factoryworkpara.ReaderMAC6 = 6;


    g_device.FactoryAutoParam.ReadTAGType = 0;
	//标签保留时间不做该功能
    g_device.FactoryAutoParam.TagExisTimeH = 1;
    g_device.FactoryAutoParam.TagExisTimeL = 1;
	
    g_device.FactoryAutoParam.ReadTagInterval = 2;
    g_device.FactoryAutoParam.TagHoldNumH = 1;
    g_device.FactoryAutoParam.TagHoldNumL = 1;
    g_device.FactoryAutoParam.OutputDataType = 0;//3;//0;//208确实能够是3好点，前面的不支持
    g_device.FactoryAutoParam.CardOutModule = 3;//0;
    g_device.FactoryAutoParam.WidthOfWiegand = 40;
    g_device.FactoryAutoParam.InterverOfWiegand = 200;
    g_device.FactoryAutoParam.CardStartPos = 0;
    g_device.FactoryAutoParam.CardSaveInTagPosition = 0;
    g_device.FactoryAutoParam.CallPCTimeInterver = 120;
    g_device.FactoryAutoParam.CallPCType = 0;
    g_device.FactoryAutoParam.CallPCEnable = 0;
    g_device.FactoryAutoParam.Antenna = 1;
    g_device.FactoryAutoParam.TriggleType = 0;
    g_device.FactoryAutoParam.PCIP1 = 192;
    g_device.FactoryAutoParam.PCIP2 = 168;
    g_device.FactoryAutoParam.PCIP3 = 0;
    g_device.FactoryAutoParam.PCIP4 = 61;
    g_device.FactoryAutoParam.PCPortHigh = 0x13;
    g_device.FactoryAutoParam.PCPortLow = 0x88;
	g_device.FactoryAutoParam.MaxEPCATAtime = 1000;
    g_device.FactoryAutoParam.CheckAlarmEnable = 0;
    //g_device.FactoryAutoParam.OutputTimeInterver = 120;
    g_device.FactoryAutoParam.RelayCTLEnable = 0;
	g_device.FactoryAutoParam.EPCATAReadType = 0;
}

#if (PARA_STORAGE_DEVICE == FLASH_AT24C02)
void SaveData(void)
{
	g_device.Flag = WORK_FLAG;
	I2C_EE_BufferWrite((unsigned char*)(&g_device),1024,sizeof(g_device));
}
#else 
void SaveData(void)
{
  g_device.Flag = WORK_FLAG;
	//I2C_EE_BufferWrite((unsigned char*)(&g_device),1024,sizeof(g_device));
	STMFLASH_Write(PARA_STORAGE_ADDR,(u16*)(&g_device),sizeof(g_device)/2);
}
#endif

void CMDCtrl(void)
{
	if(1 == (g_device.SysConnectParaChange & 0x01))
	{
	    g_device.SysConnectParaChange &= 0xfe;
		//Uart0_init();//ulBautRate);
		//Uart1_init();//ulBautRate);
		RS485_init();
		RS232_init();//ulBautRate);
		
	}
//	if(g_device.FreqSelect)
//	{
//	    g_device.FreqSelect = 0;
//		SelFreq();
//	}
	
	if(sysReboot > 1)
		sysReboot--;
}

unsigned charTohex(unsigned char ch)
{
	if (ch	<= '9')
	{
		return(ch - '0');
	}
	else
	{
		return(ch - 'A' + 10);
	}
}


//unsigned char reduceTagFlag = 0;

unsigned int judgeTag(void)
{
	unsigned int keeptime,t1,t2;
	//unsigned char j,k;
	unsigned int j,k;
	int len, tmplen, tmpNum;
	//unsigned char count, addTag = 0, reduceTag = 0;
	unsigned int count, addTag = 0, reduceTag = 0;
	
	tmpNum = (unsigned int)(g_device.AutoParam.TagHoldNumH << 8);
	tmpNum += (unsigned int)g_device.AutoParam.TagHoldNumL;
	if(tmpNum > SAVEMAXNUM)
		tmpNum = SAVEMAXNUM;
		
	if(g_device.AutoParam.CallPCType == 0)
	{
		TagSaveCount = 0;
		for(j = 0; j < CardPos; j++)
		{
			len = CardIndex[j][0];
			if(len > 15)
				continue;
			else if(len == 0)
				continue;
			memcpy(&TagSaveBuf[j][0], &CardIndex[j][0], TAGIDLENGTH + 6 + 2);
			TagSaveCount++;
		}
		return TagSaveCount;
	}
	else if(g_device.AutoParam.CallPCType == 1)
	{
		if(!timerIsOverflow(&OutPutTimer))
		{
			return 0;
		}
		else{
			TagSaveCount = 0;
			for(j = 0; j < CardPos; j++)
			{
				len = CardIndex[j][0];
				if(len > 15)
					continue;
				else if(len == 0)
					continue;
				memcpy(&TagSaveBuf[j][0], &CardIndex[j][0], TAGIDLENGTH + 6 + 2);
				TagSaveCount++;
			}
			CardPos = 0;
			if((TagSaveCount == 0) && (g_device.AutoParam.CallPCEnable))
			{
				TagSaveCount = 1;
				return 222;
			}
			return TagSaveCount;			
		}
	}

	getTime(g_device.ReaderTime);
	keeptime = g_device.AutoParam.TagExisTimeH;
	keeptime <<= 8;
	keeptime += g_device.AutoParam.TagExisTimeL;
	count = TagSaveCount;
	
	for(k = 0; k < TagSaveCount; k++)
	{
		if(TagSaveBuf[k][0] == 0)
			continue;
		t1 = (TagSaveBuf[k][TAGIDLENGTH] * 3600 + TagSaveBuf[k][TAGIDLENGTH + 1] * 60 + TagSaveBuf[k][TAGIDLENGTH + 2] + keeptime);
		t2 = (g_device.ReaderTime[3] * 3600 + g_device.ReaderTime[4] * 60 + g_device.ReaderTime[5]);	
		if (t1 < t2)
		{
			reduceTag = 1;
			TagSaveBuf[k][0] = 0;			
		}
	}
	for(j = 0; j < CardPos; j++)
	{
		len = CardIndex[j][0];
		if(len > 15)
			continue;
		else if(len == 0)
		{
			continue;
		}
		else
			len <<= 1;
		for(k = 0; k < TagSaveCount; k++)
		{
			tmplen = TagSaveBuf[k][0] << 1;
			if((len != tmplen) && (tmplen != 0))
				continue;

			if(compare((void *) &TagSaveBuf[k][0], (void *) &CardIndex[j][0], len + 1) == 1)
			{
				TagSaveBuf[k][TAGIDLENGTH + 3] = CardIndex[j][TAGIDLENGTH + 3];
				TagSaveBuf[k][TAGIDLENGTH + 4] = CardIndex[j][TAGIDLENGTH + 4];
				TagSaveBuf[k][TAGIDLENGTH + 5] = CardIndex[j][TAGIDLENGTH + 5];
				break;
			}
		}
		if((k == TagSaveCount) || (TagSaveCount == 0))
		{
			tmplen = 0;
			for(k = 0; k < TagSaveCount; k++)
			{
				if(TagSaveBuf[k][0] == 0)
				{
					memcpy(&TagSaveBuf[k][0], &CardIndex[j][0], TAGIDLENGTH + 6 + 2);
					tmplen = 1;
					break;
				}
			}
			if(tmplen == 0)
			{
				memcpy(&TagSaveBuf[count][0], &CardIndex[j][0], TAGIDLENGTH + 6 + 2);
				if(count < tmpNum)//SAVEMAXNUM)
				{
					count++;
				}
				else
				{
					count = 0;
					TagSaveCount = tmpNum;//SAVEMAXNUM;
				}
			}
			addTag = 1;
		    
		}
	
	}
	if(TagSaveCount != tmpNum)//SAVEMAXNUM)
		TagSaveCount = count;  
	if(g_device.AutoParam.CallPCType == 2)
		return addTag;
	else if(g_device.AutoParam.CallPCType == 3)
		return reduceTag;
		//return (reduceTag + addTag);
	else
		return (addTag + reduceTag);
}

unsigned char buf[170] = {0};

void sendDataOut(void)
{
	unsigned short port;
	struct ip_addr destAddr;
	//struct udp_pcb *tudp;
	//unsigned char buf[170] = {0},CallPCType;//TAGIDLENGTH*2+5] = {0};//buf[TAGIDLENGTH*2];
	unsigned char CallPCType;
	int chk = 0,i,len,j;
	unsigned int temp_count;
	
#if 1//test
	CallPCType = g_device.AutoParam.CallPCType;
	if((g_device.AutoParam.CardOutModule == OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule == OUTPUT_Wiegan34))
		g_device.AutoParam.CallPCType = 1;
#if 1
	if((CardPos > 0) && (read_count > 0))
		chk = 1;
	if(g_device.AutoParam.CallPCType > 1)
		chk = 1;
	else if(g_device.AutoParam.CallPCEnable)
	{
		if((g_device.AutoParam.CardOutModule == OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule == OUTPUT_Wiegan34))
		{
			chk = 1;
			
		//	CardPoslast = 0;
				//CardPos = 0;
			read_count = 0;
		}
		#if 1
		else		
		if(timerIsOverflow(&OutPutTimer))
		{
			chk = 222;
			 CardPos = 1;
			//CardTotal = 1;
		//	CardPoslast = 0;
			//CardPos = 0;
			read_count = 0;
		}
		#endif
	}
#endif
	if(chk != 0)
	{
		if(chk == 1)
		{
			chk = 0;
			chk = judgeTag();
			CardPos = 0;
			#if 1
			if((chk == 0) && (g_device.AutoParam.CallPCEnable))
			{
				if(timerIsOverflow(&OutPutTimer))
				{
					chk = 222;
					//CardPos = 1;
					//read_count = 0;
					TagSaveCount = 1;
				}
			}
			#endif
		}
		if(chk != 0)
		{	
		
			if((g_device.AutoParam.CardOutModule == OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule == OUTPUT_Wiegan34))
				timerStart(&OutPutTimer, 100);
			else
				timerStart(&OutPutTimer, g_device.AutoParam.CallPCTimeInterver * 100);
			switch(g_device.AutoParam.CardOutModule)
			{
				case OUTPUT_Wiegan26:
				case OUTPUT_Wiegan34:
					for(i = 0; i< TagSaveCount; i++)
					{
						beepFlag = 1; //蜂鸣器鸣叫
						AntId = TagSaveBuf[i][TAGIDLENGTH + 6 + 0];
						if(chk == 222)
						{
							for(j = 0; j < 4; j++)
								buf[j] = 0;
						}
						else
						{
						len = TagSaveBuf[i][0];// << 1;
						if(len > 15)
							continue;
						if(len == 0)
						{
							//if(chk != 222)
							continue;
						}
						else
							len <<= 1;
						if(g_device.AutoParam.CardSaveInTagPosition == 0)
							for(j = 0; j < 4; j++)
							{
								buf[j] = TagSaveBuf[i][g_device.AutoParam.CardStartPos + 1 + j];
							}
						else
							for(j = 0; j < 4; j++)
							{
								buf[j] = TagSaveBuf[i][g_device.AutoParam.CardStartPos + 5 + j];
							}
						}
						

						//buf[0]=0x11;buf[1]=0x22;buf[2]=0x33;buf[3]=0x44;
						if(g_device.AutoParam.CardOutModule == OUTPUT_Wiegan26){
						//RS232Send(buf ,4);
							if(!(AntId & 0x01))
								packetDataWG261(buf);
							else
								packetDataWG26(buf);
						}
						else{
								if(!(AntId & 0x01))
									packetDataWG341(buf);
								else
									packetDataWG34(buf);
							}	
						DelayMs(900);
					}
					break;
					#if 1
				case OUTPUT_RS485:					
					//for(i = 0; i< CardPos; i++)
					for(i = 0; i< TagSaveCount; i++)
					{
						beepFlag = 1; //蜂鸣器鸣叫
					
						if(chk == 222)
							len = 0;
						else
						{
						len = TagSaveBuf[i][0];// << 1;
						if(len > 15)
							continue;
						if(len == 0)
						{
							//if(chk != 222)
							continue;
						}
						else
							len <<= 1;
						}
						//len = TagSaveBuf[i][0] << 1;
/************************************************************************/
/*      将数据包2,3字节改为地址, 天线号加到地址后面     潘炳阳 20170615 */
/************************************************************************/				
						
						for(j = 0; j < len ; j++)
						{
							buf[5 + j * 2] = hextochar(TagSaveBuf[i][j + 1] >> 4);
							buf[5 + j * 2 + 1] = hextochar(TagSaveBuf[i][j + 1] & 0x0f);
						}
						buf[0] = STX;
						
						buf[1] = hextochar(g_device.workpara.Address >> 4);
						buf[2] = hextochar(g_device.workpara.Address & 0x0f);
						buf[3] = hextochar(TagSaveBuf[i][TAGIDLENGTH + 6] >> 4);  // 天线号
						buf[4] = hextochar(TagSaveBuf[i][TAGIDLENGTH + 6] & 0x0f);
						
						buf[5 + len * 2] = ETX;
						buf[len * 2 + 6] = SetChecksum(buf, len * 2 + 6);
						
						//  [7/26/2018 Administrator]
						if ( buf[4] == '1' || buf[4] == '3' )
							relayON(1,2000);
						else if ( buf[4] == '2' || buf[4] == '4' )
							relayON(2,2000);

					
						RS485Send(buf ,len * 2 + 7);
						//DelayMs(15);//(20);
						while(beepFlag != 0);
					}
					break;
					#endif
				case OUTPUT_RS232:
				case OUTPUT_RJ45:
			//	case OUTPUT_RS485:
					temp_count = read_count;
					//if(read_count == 1)
					//	break;
					if(g_device.AutoParam.OutputDataType == 0)//简化数据输出格式
					{
						
						temp_count %= 100000;
						buf[0] = temp_count/10000 + 0x30;////0x30;
						temp_count %= 10000;
						buf[1] = temp_count/1000 + 0x30;////0x30;
						temp_count %= 1000;
						buf[2] = temp_count/100 + 0x30;//次数百位
						temp_count %= 100;
						buf[3] = temp_count/10 + 0x30;//次数十位
						buf[4] = temp_count%10 + 0x30;//次数个位
						
						buf[0] = 0x30;
						buf[5] = ',';
						buf[6] = 0x20;
						//tianxian
						temp_count = TagSaveBuf[i][TAGIDLENGTH + 6 + 0];
						buf[7]= (temp_count/10)+0x30;//hextochar(sysCtl.switchAtenna >> 4);//(antID/10)+0x30; //天线十位
						buf[8]= (temp_count%10)+0x30;//hextochar(sysCtl.switchAtenna & 0x0f);//(antID%10)+0x30; //天线个位
					//	buf[7] = hextochar(g_device.AutoParam.Antenna >> 4);
					//	buf[8] = hextochar(g_device.AutoParam.Antenna & 0x0f);
						buf[9] = ',';
						buf[10] = 0x20;
						//biaoqianzhonglei
						buf[11] = hextochar(g_device.workpara.TagType >> 4);
						buf[12] = hextochar(g_device.workpara.TagType & 0x0f);
						buf[13] = ',';
						buf[14] = 0x20;
						for(i = 0; i< TagSaveCount; i++)
						{
							beepFlag = 1; //蜂鸣器鸣叫
							//if(chk == 222)
							//	len = 0;
							//else
							//len = TagSaveBuf[i][0] << 1;
							if(chk == 222)
								len = 0;
							else
							{
							len = TagSaveBuf[i][0];// << 1;
							if(len > 15)
								continue;
							if(len == 0)
							{
								//if(chk != 222)
								continue;
							}
							else
								len <<= 1;
							}
							for(j = 0; j < len ; j++)
							{
								buf[15 + j * 2] = hextochar(TagSaveBuf[i][j + 1] >> 4);
								buf[15 + j * 2 + 1] = hextochar(TagSaveBuf[i][j + 1] & 0x0f);
							}
							////哎，没人告诉协议，文档也没记录，只好自己写了改改了写。
							//for(j = 0; j < len * 2 + 15; j++)
							//{
							//	buf[len * 2 + 15] += buf[j];
							//}
							////
							for(j = len * 2 + 15; j < 164; j++)
							{
								buf[j] = 0x20;//补空格
							}
							buf[164] = 0x0D;
							buf[165] = 0x0A;
			
							////
							if(g_device.AutoParam.CardOutModule == OUTPUT_RS232)
								RS232Send(buf ,166);//RS232Send(buf ,len * 2 + 16);
							else if(g_device.AutoParam.CardOutModule == OUTPUT_RS485)
								RS485Send(buf ,166);
							else
							{
								//imerDisable(TIMER0_BASE, TIMER_B);
								//imerDisable(TIMER1_BASE, TIMER_A); 
								//IntEnable(INT_ETH);
								IP4_ADDR(&destAddr,g_device.AutoParam.PCIP1,g_device.AutoParam.PCIP2,
									  g_device.AutoParam.PCIP3,g_device.AutoParam.PCIP4);
								//port = (unsigned short)g_device.AutoParam.PCPortHigh << 8 + (unsigned short)g_device.AutoParam.PCPortLow;
								port = g_device.AutoParam.PCPortHigh << 8;
								port += (unsigned short)g_device.AutoParam.PCPortLow;
								//UDP_sendData(UdpPcb, destAddr, port, buf, len * 2 + 16);
								//UDP_sendData(UdpPcb, destAddr, port, buf, 166);
								UDP_sendData(UdpPcb, destAddr, port, buf, 166);
								//IntDisable(INT_ETH);
								//SysCtlDelay(10 * (SysCtlClockGet() / 3000));
								
								//TimerEnable(TIMER0_BASE, TIMER_B);
								//TimerEnable(TIMER1_BASE, TIMER_A); 
							}
							//DelayMs(15);//(20);
							while(beepFlag != 0);
						}
					}
					else if(g_device.AutoParam.OutputDataType == 1)//标准数据输出格式 time_data_buff
					{
						//Read_timeFrom_Ds1302();
					#if 1
					//Disc:2000/04/08 01:09:20, Last:2000/04/08 01:09:49, Count:00005, Ant:01, Type:04, Tag:3005FB63AC1F3841EC880467 
						buf[0]='D';
						buf[1]='i';
						buf[2]='s';
						buf[3]='c';
						buf[4]=':';
						buf[5]='2';
						buf[6]='0';  
						//第一次读取的时间
						//年份全部是20**年
						#if 0
						buf[7]=(FirtReaderTime[0]/10)+0x30;//年十位
						buf[8]=(FirtReaderTime[0]%10)+0x30;//年个位
						buf[9]='/';
						buf[10]=(FirtReaderTime[1]/10)+0x30;//月十位
						buf[11]=(FirtReaderTime[1]%10)+0x30;//月个位
						buf[12]='/';
						buf[13]=(FirtReaderTime[2]/10)+0x30;//日十位
						buf[14]=(FirtReaderTime[2]%10)+0x30;//日个位
						buf[15]=0x20;
						buf[16]=(FirtReaderTime[3]/10)+0x30;//时十位
						buf[17]=(FirtReaderTime[3]%10)+0x30;//时个位
						buf[18]=':';
						buf[19]=(FirtReaderTime[4]/10)+0x30;//分十位
						buf[20]=(FirtReaderTime[4]%10)+0x30;//分个位
						buf[21]=':';
						buf[22]=(FirtReaderTime[5]/10)+0x30;//秒十位
						buf[23]=(FirtReaderTime[5]%10)+0x30;//秒个位
						#endif
						buf[7]=(g_device.ReaderTime[0]/10)+0x30;//年十位
						buf[8]=(g_device.ReaderTime[0]%10)+0x30;//年个位
						buf[9]='/';
						buf[10]=(g_device.ReaderTime[1]/10)+0x30;//月十位
						buf[11]=(g_device.ReaderTime[1]%10)+0x30;//月个位
						buf[12]='/';
						buf[13]=(g_device.ReaderTime[2]/10)+0x30;//日十位
						buf[14]=(g_device.ReaderTime[2]%10)+0x30;//日个位
						buf[15]=0x20;
						buf[16]=(g_device.ReaderTime[3]/10)+0x30;//时十位
						buf[17]=(g_device.ReaderTime[3]%10)+0x30;//时个位
						buf[18]=':';
						buf[19]=(g_device.ReaderTime[4]/10)+0x30;//分十位
						buf[20]=(g_device.ReaderTime[4]%10)+0x30;//分个位
						buf[21]=':';
						buf[22]=(g_device.ReaderTime[5]/10)+0x30;//秒十位
						buf[23]=(g_device.ReaderTime[5]%10)+0x30;//秒个位

						buf[24]=',';
						buf[25]=0x20;
						buf[26]='L';
						buf[27]='a';
						buf[28]='s';
						buf[29]='t';
						buf[30]=':';
						buf[31]='2';
						buf[32]='0';  //最后一次读取的时间    
						buf[33]=(g_device.ReaderTime[0]/10)+0x30;//年十位
						buf[34]=(g_device.ReaderTime[0]%10)+0x30;//年个位
						buf[35]='/';
						buf[36]=(g_device.ReaderTime[1]/10)+0x30;//月十位
						buf[37]=(g_device.ReaderTime[1]%10)+0x30;//月个位
						buf[38]='/';
						buf[39]=(g_device.ReaderTime[2]/10)+0x30;//日十位
						buf[40]=(g_device.ReaderTime[2]%10)+0x30;//日个位
						buf[41]=0x20;
						buf[42]=(g_device.ReaderTime[3]/10)+0x30;//时十位
						buf[43]=(g_device.ReaderTime[3]%10)+0x30;//时个位
						buf[44]=':';
						buf[45]=(g_device.ReaderTime[4]/10)+0x30;//分十位
						buf[46]=(g_device.ReaderTime[4]%10)+0x30;//分个位
						buf[47]=':';
						buf[48]=(g_device.ReaderTime[5]/10)+0x30;//秒十位
						buf[49]=(g_device.ReaderTime[5]%10)+0x30;//秒个位
						buf[50]=',';
						buf[51]=0x20;
						buf[52]='C';
						buf[53]='o';
						buf[54]='u';
						buf[55]='n';
						buf[56]='t';
						buf[57]=':';

					//	temp_count %= 100000;
					//	buf[58]=temp_count/10000 + 0x30;
					//	temp_count %= 10000;
					//	buf[59]=temp_count/1000 + 0x30;
					//	temp_count %= 1000;
					//	buf[60]=(temp_count/100)+0x30; //次数百位
					//	temp_count %= 100;
					//	buf[61]=(temp_count/10)+0x30; //次数十位
					//	buf[62]=(temp_count%10)+0x30; //次数个位

					//20140806			i = sysCtl.CardIndex[sysCtl.CardPos][TAGIDLENGTH + 6 + 1];
								//printf("RSSI = %d\n",temp_count);
								buf[58]=' ';//temp_count/10000 + 0x30;
								//temp_count %= 10000;
								buf[59]=' ';//temp_count/1000 + 0x30;
								//temp_count %= 1000;
								buf[60]='-';//(temp_count/100)+0x30; //次数百位
								//temp_count %= 100;
								//printf("RSSI = %d\n",i);
					//20140806			i = rssichk(i);
					//20140806			autobuf.buf[61]=(i/10)+0x30; //次数十位
					//20140806			autobuf.buf[62]=(i%10)+0x30; //次数个位
								buf[61]=' ';
								buf[62]=' ';


						buf[63]=',';
						buf[64]=0x20;
						buf[65]='A';
						buf[66]='n';
						buf[67]='t';
						buf[68]=':'; //天线号

						buf[71]=',';
						buf[72]=0x20;
						buf[73]='T';
						buf[74]='y';
						buf[75]='p';
						buf[76]='e';
						buf[77]=':';
						buf[78]=hextochar(g_device.workpara.TagType >> 4);
						buf[79]=hextochar(g_device.workpara.TagType & 0x0f);

						buf[80]=',';
						buf[81]=0x20;
						buf[82]='T';
						buf[83]='a';
						buf[84]='g';
						buf[85]=':';
						for(i = 0; i< TagSaveCount; i++)
						{
							beepFlag = 1; //蜂鸣器鸣叫
							
							len = TagSaveBuf[i][TAGIDLENGTH + 6 + 1];
							len = rssichk(len);
							buf[61]=(len/10)+0x30; //次数十位
							buf[62]=(len%10)+0x30; //次数个位
							
							temp_count = TagSaveBuf[i][TAGIDLENGTH + 6 + 0];
							buf[69]= (temp_count/10)+0x30;//hextochar(sysCtl.switchAtenna >> 4);//(antID/10)+0x30; //天线十位
							buf[70]= (temp_count%10)+0x30;//hextochar(sysCtl.switchAtenna & 0x0f);//(antID%10)+0x30; //天线个位

						//	buf[69]= hextochar(TagSaveBuf[i][TAGIDLENGTH+6] >>4);//(antID/10)+0x30; //天线十位
						//	buf[70]= hextochar(TagSaveBuf[i][TAGIDLENGTH+6] & 0x0f);//(antID%10)+0x30; //天线个位
							//if(chk == 222)
							//	len = 0;
							//else
							//len = TagSaveBuf[i][0] << 1;
							if(chk == 222)
								len = 0;
							else
							{
							len = TagSaveBuf[i][0];// << 1;
							if(len > 15)
								continue;
							if(len == 0)
							{
								//if(chk != 222)
								continue;
							}
							else
								len <<= 1;
							}
							for(j = 0; j < len ; j++)
							{
								buf[86 + j * 2] = hextochar(TagSaveBuf[i][j + 1] >> 4);
								buf[86 + j * 2 + 1] = hextochar(TagSaveBuf[i][j + 1] & 0x0f);
							}
							if(len != 0)
							{
							buf[16]=(TagSaveBuf[i][TAGIDLENGTH + 0]/10)+0x30;//时十位
							buf[17]=(TagSaveBuf[i][TAGIDLENGTH + 0]%10)+0x30;//时个位
							//buf[18]=':';
							buf[19]=(TagSaveBuf[i][TAGIDLENGTH + 1]/10)+0x30;//分十位
							buf[20]=(TagSaveBuf[i][TAGIDLENGTH + 1]%10)+0x30;//分个位
							//buf[21]=':';
							buf[22]=(TagSaveBuf[i][TAGIDLENGTH + 2]/10)+0x30;//秒十位
							buf[23]=(TagSaveBuf[i][TAGIDLENGTH + 2]%10)+0x30;//秒个位
							
							buf[42]=(TagSaveBuf[i][TAGIDLENGTH + 3]/10)+0x30;//时十位
							buf[43]=(TagSaveBuf[i][TAGIDLENGTH + 3]%10)+0x30;//时个位
							//buf[44]=':';
							buf[45]=(TagSaveBuf[i][TAGIDLENGTH + 4]/10)+0x30;//分十位
							buf[46]=(TagSaveBuf[i][TAGIDLENGTH + 4]%10)+0x30;//分个位
							//buf[47]=':';
							buf[48]=(TagSaveBuf[i][TAGIDLENGTH + 5]/10)+0x30;//秒十位
							buf[49]=(TagSaveBuf[i][TAGIDLENGTH + 5]%10)+0x30;//秒个位
							}
							//白写了
							//for(j = 0; j < len * 2 + 86; j++)
							//{
							//	buf[len * 2 + 86] += buf[j];
							//}
							////
							for(j = len * 2 + 86; j < 164; j++)
							{
								buf[j] = 0x20;//补空格
							}
							buf[164] = 0x0D;
							buf[165] = 0x0A;

							
							////
							if(g_device.AutoParam.CardOutModule == OUTPUT_RS232)
								RS232Send(buf ,166);//RS232Send(buf ,len * 2 + 87);
							else if(g_device.AutoParam.CardOutModule == OUTPUT_RS485)
								RS485Send(buf ,166);
							else
							{
								//IntEnable(INT_ETH);
								IP4_ADDR(&destAddr,g_device.AutoParam.PCIP1,g_device.AutoParam.PCIP2,
									  g_device.AutoParam.PCIP3,g_device.AutoParam.PCIP4);
								//port = (unsigned short)g_device.AutoParam.PCPortHigh << 8 + (unsigned short)g_device.AutoParam.PCPortLow;
								port = g_device.AutoParam.PCPortHigh << 8;
								port += (unsigned short)g_device.AutoParam.PCPortLow;
								//UDP_sendData(UdpPcb, destAddr, port, buf, len * 2 + 87);
								UDP_sendData(UdpPcb, destAddr, port, buf, 166);
								//IntDisable(INT_ETH);
								//SysCtlDelay(10 * (SysCtlClockGet() / 3000));
							}
							//DelayMs(15);//(20);
							while(beepFlag != 0);
						}
					#endif
				//	Read_timeFrom_Ds1302();
				//	memcpy(FirtReaderTime, g_device.ReaderTime, sizeof(g_device.ReaderTime));
					}
					//read_count = 0;
					
					//Read_timeFrom_Ds1302();
					//memcpy(FirtReaderTime, g_device.ReaderTime, sizeof(g_device.ReaderTime));
					break;
			}
			read_count = 0;
		}
		//timerStart(&OutPutTimer, g_device.AutoParam.CallPCTimeInterver * 100);
	}
	//else
	//	SysCtlDelay(1 * (SysCtlClockGet() / 3000));
	#if 0
	else
	{
		if(g_device.AutoParam.CallPCEnable)
		{
			if(timerIsOverflow(&OutPutTimer))
			{
				timerStart(&OutPutTimer, g_device.AutoParam.CallPCTimeInterver * 100);
				//发0长度EPC号码
				packetZero(g_device.AutoParam.CardOutModule);
			}
			CardPos = 0;
		}
	}
	#endif
//	if(chk == 222)
//		CardPos = 0;
//	CardPoslast = CardPos;
	g_device.AutoParam.CallPCType = CallPCType;

	#endif//test
}

void workACT(void)
{
	unsigned char i,j,k,mm = 0;
	
	if(switchAtenna != 0)//轮流循环结束没?
	{
		mm = tmpAntenna;
		i = 1;
		while(switchAtenna != 0)
		{
			tmpAntenna = switchAtenna & i;
			sendswitchAtenna = tmpAntenna;
			if(tmpAntenna != 0)
			{
				readCard();
				switchAtenna &= (~i);
				break;
			}
			i <<= 1;
		}
		tmpAntenna = mm;
	}
	else
	{
		if(0 == g_device.AutoParam.ReadTAGType)
		{
			if(timerIsOverflow(&ReadCardTimer))
			{
				timerStart(&ReadCardTimer, ReadCardTimeInterver);
				switchAtenna = tmpAntenna;
			}
		}
		else
		{
			g_device.AutoParam.TriggleType == 0 ? (i = 0):(i = 1);
			GetPinTriggle() == 0 ? (j = 1):(j = 0);
			GetPinTriggle1() == 0 ? (k = 1):(k = 0);
			mm = 0;
			if(i == j)
			{
				mm |= 0x05;
			}
			if(i == k)
			{
				mm |= 0x0a;
			}
			if(mm != 0)
			{
				switchAtenna = mm & tmpAntenna;
			}
		}
		mm = tmpAntenna;
		i = 1;
		while(switchAtenna != 0)
		{
			tmpAntenna = switchAtenna & i;
			sendswitchAtenna = tmpAntenna;
			if(tmpAntenna != 0)
			{
				readCard();
				switchAtenna &= (~i);
				break;
			}
			i <<= 1;
		}
		tmpAntenna = mm;
	}
}

void Delay2us(unsigned long Count_us)
{
	//名字不改了，意思变成1us
	TIM_Cmd(TIM3,ENABLE);
	timingdelay = Count_us; 
	while(timingdelay != 0);
	TIM_Cmd(TIM3,DISABLE);
}

void TIM3_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
		//TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		if (timingdelay != 0x00)
		{ 
			timingdelay--;
		}
		#if 0
		if(tag.rev_overtimes > 0)
		{
			tag.rev_overtimes++;
			if(tag.rev_overtimes > 50000)
			{
				tag.flag_Rev_OK = TAG_REV_OVERT;
				tag.rev_overtimes = 0;
				flag_time_20ms = 0;
			}
		}
		#endif
	}
}

void time3init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructure.TIM_Period=1;
	TIM_TimeBaseStructure.TIM_Prescaler=71;//144 - 1;//2us
	//下面的数字都对
	//TIM_TimeBaseStructure.TIM_Period=3;//1;
	//TIM_TimeBaseStructure.TIM_Prescaler= 71;//144 - 1;//2us
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); 
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3,DISABLE);
	//TIM_Cmd(TIM3,ENABLE);
}


#define CARD_ISO180006B 1
#define CARD_EPCC1      2
#define CARD_EPCC1G2    4
#define CARD_EM4442     8
#define CARD_ATA       16
#define CARD_EPCATA    32

int main(void )
{	
	System_Setup();  
//	IIC_init();
	initPara(); 
	
	LwIP_Init();
	UDP_init();  
	timerStart(&uip.UIP_timer, 300);
	time3init();
	RS232_init();
	RS485_init();
	R2000_init();
	RTC_init();
	BEEPOFF;
	LEDGreenOFF;
	
//	reset_R2000();
	//检查下有无信息返回，没有是对的，可以做个小函数，用来自检R2000
//	DelayMs(2000); //因为上面复位R2000
//	close_r2000_beep();//关闭R2000蜂鸣器
//	DelayMs(500);
	SelFreq();
	set_output_power();
	SelfCheck();
#if (EnableWatchDog != 0)
	Watchdog_init();
#endif

	while(1)
	{
		//以太网广播 
		Udp_BroadCast();
		if(g_paramTable.setFlag)
		{
		    SaveData();
			g_paramTable.setFlag = 0;
		}
		Local_232_module();
		Local_485_module();
		#if 0
		//switch(g_device.workpara.ReaderWorkStatus)
		if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
		{
			//case WORK_ACT:
				//if((g_device.workpara.TagType == CARD_ISO180006B) || 
				//	(((g_device.AutoParam.CardOutModule== OUTPUT_Wiegan26) || (g_device.AutoParam.CardOutModule== OUTPUT_Wiegan34)) &&(USER_ID == g_device.AutoParam.CardSaveInTagPosition)))
				if((g_device.workpara.TagType == CARD_ISO180006B) || 
					(1 == g_device.AutoParam.CardSaveInTagPosition))
					workACT();
				else// if((g_device.workpara.TagType ==CARD_EPCC1 ) ||(g_device.workpara.TagType ==CARD_EPCC1G2))
					readCard6C();
				sendDataOut();
			//	break;
		}
		#endif
		#if 1
		switch(g_device.workpara.ReaderWorkStatus)
		{
			case WORK_ACT:
				workACT();
				sendDataOut();
				break;
		}
		#endif
		CMDCtrl();
	}
}

void DelayMs(unsigned long nTime)
{ 
	TimingmsDelay = (nTime << 1);
	while(TimingmsDelay != 0);
}
extern  u32 timess;
void Time_Update(void)
{
	relay_isr_polling();

	timess++;
	LocalTime += SYSTEMTICK_PERIOD_MS;
	if (TimingmsDelay != 0x00)
	{ 
		TimingmsDelay--;
	}
	LwIP_Periodic_Handle(LocalTime);
	sys_timer1++;
	if(sys_timer1 > 9)//19)
	{
		reboot();
		RelayControl();
		sys_timer++;
		sys_timer1 = 0;
		#if 1
		if(beepFlag != 0)
		{
			beepCountTimes++;
			if(beepCountTimes < 2)
			{
				if(g_device.workpara.AlarmRnable)
					BEEPON;
				LEDGreenON;
			}
			else if(beepCountTimes > 2)
			{
				//beepFlag = 0;
				beepFlag--;
				beepCountTimes = 0;
			}
			else
			{
				BEEPOFF;
				LEDGreenOFF;
			}
		}
		#endif
		//else
		if(sys_timer < LEDBASECOUNT)
		{
			LEDYellowOFF;
		}
		else if(sys_timer < (LEDBASECOUNT + 3))
		{
			LEDYellowON;
		}
		else if(sys_timer < (LEDBASECOUNT + 3 + 15))
		{
			LEDYellowOFF;
		}
		else if(sys_timer < (LEDBASECOUNT + 3 + 15 + 3))
		{
			LEDYellowON;
		}
		else
		{
			sys_timer = 0;
		}
		if (local.local_timer.isStart && (0 == local.local_timer.isOverflow))
		{
			if (local.local_timer.time == 1) 
				local.local_timer.isOverflow = 1;
			else 
				local.local_timer.time--;
		}
		if (local485.local_timer.isStart && (0 == local485.local_timer.isOverflow))
		{
			if (local485.local_timer.time == 1) 
				local485.local_timer.isOverflow = 1;
			else 
				local485.local_timer.time--;
		}
		if (uip.UIP_timer.isStart && (0 == uip.UIP_timer.isOverflow))
		{
			if (uip.UIP_timer.time == 1) 
				uip.UIP_timer.isOverflow = 1;
			else 
				uip.UIP_timer.time--;
		}
		if (ReadCardTimer.isStart && (0 == ReadCardTimer.isOverflow))
		{
			if (ReadCardTimer.time == 1) 
				ReadCardTimer.isOverflow = 1;
			else 
				ReadCardTimer.time--;
		}
		if (OutPutTimer.isStart && (0 == OutPutTimer.isOverflow))
		{
			if (OutPutTimer.time == 1) 
				OutPutTimer.isOverflow = 1;
			else 
				OutPutTimer.time--;
		}
		if (R2000.local_timer.isStart && (0 == R2000.local_timer.isOverflow))
		{
			if (R2000.local_timer.time == 1) 
				R2000.local_timer.isOverflow = 1;
			else 
				R2000.local_timer.time--;
		}
 		if ((0 == R2000.send_timer.isOverflow))
		{
			if (R2000.send_timer.time == 1) 
				R2000.send_timer.isOverflow = 1;
			else 
				R2000.send_timer.time--;
		}
	}
}


