
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Common.h                                                                      **
** Last modified Date:      2012.3.2                                                                    **
** Last Version:            v1.0                                                                        **
** Description:             公共函数模块头文件                                                            **
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


#ifndef _COMMON_H

#define _COMMON_H

//#include ".h"

/*------------------------------------------------------------------------------------------------------*/
#define BootCode 0x40


#define OUTPUT_RS232    0
#define OUTPUT_RS485    1
#define OUTPUT_RJ45     2
#define OUTPUT_Wiegan26 3
#define OUTPUT_Wiegan34 4


#define STX 0x02
#define ETX 0x03



/*------------------------------------------------------------------------------------------------------*/
#define		BUF_LEN		255
#define		RFBUF_LEN		50

#define		CMDOORCHECKOK		0
#define		ANTCONNECTFAIL		1
#define		NOTAG       		2
#define		IVALIDTAG   		3
#define		POWERLOWER  		4
#define		WRITEPROTECT		5
#define		CHECKSUMWRONG  		6
#define		PARAWRONG   		7
#define		NOEXISTDATA  		8
#define		INVALIDPASSWORD		9
#define		DESTROYOASSNOTZERO	10
#define		INVALIDMASTERCMD	11
#define		PASSWRONGUSER  		12
#define		OUTRFHAPMER   		13
#define		TAGREADPROTECT		14

#define		INVALIDINSTRUCTION	30
#define		UNKNOWINSTRUCTION	31
#define		ORTHERWRONG  		32
#define		ERRORNORETURN  		0xff


#define		command_success		0x10
#define		command_fail		0x11
#define		mcu_reset_error		0x20


//////////////////////////////////////////////
//串口控制命令
#define		CMD_SetBaudRate  	    	0X01
#define		CMD_GetReaderVersion  		0X02
#define		CMD_SetRelay        		0X03
#define		CMD_SetOutputPower  		0X04
#define		CMD_SetFrequency     		0X05
#define		CMD_ReadParam       		0X06
#define		CMD_WriteRadioDeep  		0X07
#define		CMD_ReadRadioDeep   		0X08
#define		CMD_WriteParam      		0X09
#define		CMD_SetAntenna      		0X0A
#define		CMD_ReadRealyStatus 		0X0B
#define		CMD_SetReaderPara    		0X0C
#define		CMD_ReadReaderPara  		0X0D
#define		CMD_Reboot          		       0X0E
#define		CMD_SetReaderWorkType  		0X0F
#define		CMD_ClearTagListMem  		0X10
#define		CMD_SetReaderTime    		0X11
#define		CMD_GetReaderTime   		0X12
#define		CMD_SetAutoPara      		0X13
#define		CMD_GetAutoPara      		0X14
#define		CMD_SetTagFilter      		0X15
#define		CMD_GetTagFilter       		0X16
#define		CMD_SetFactoryAutoPara 		0X17


#if 0
#define		CMD_AddLableID      		0X13
#define		CMD_DelLableID      		0X14
#define		CMD_GetLableID      		0X15
#define		CMD_GetRecord       		0X16
#define		CMD_DelRecord       		0X17
//#define		CMD_SetTagFilter     		0X18
//#define		CMD_GetTagFilter    		0X19
#endif
#define		CMD_SetReaderNetwork  		0X30
#define		CMD_GetReaderNetwork  		0X31
#define		CMD_SetReaderMAC    		0X32
#define		CMD_GetReaderMAC    		0X33

#define		CMD_GetReaderRevStatus 		0X41
#define		CMD_SetReaderOutStatus 		0X42
#define		CMD_GetReaderOutStatus 		0X43

//#define		CMD_SetMasterWorkPara  		0X50
//#define		CMD_GetMasterWorkPara  		0X51
//#define		CMD_SetDefaultPara  		0X52
//#define		CMD_RebackDefaultPara  		0X53
#define		CMD_ImmediatelyNotify  		0X54
//#define		CMD_StartReaderAuto  		0X55
//#define		CMD_StopReaderAuto  		0X56
#define		CMD_GetTagRecord    		0X57

#define		CMD_ModifyReaderHargVersion	0X85
#define		CMD_ModifyReaderFrequenceRange 0X86
#define		CMD_GetReaderFrequenceRange 0X87
//#define		CMD_OpenRfPower     		0X87
#define		CMD_UpdateProgrram     		0X88
//#define		CMD_ChangeToEP500   		0X88

#define		CMD_ModifyReaderID  		0X8B
#define		CMD_GetReaderID     		0X8C
#define		CMD_GetCPUID     			0X8D

//#define		CMD_ChangeTagType    		0X92



//读写ISO18000-6B命令格式
#define		CMD_DetectTagNum  		0XFF
#define		CMD_ReadLabelID  		0XFE
#define		CMD_ListIDReport  		0XFD

#define		CMD_ListSelectedID 		0XFB

#define		CMD_ReadByteBlock  		0XF6
#define		CMD_WriteByteBlock 		0XF5
#define		CMD_WriteProtect  		0XF4
#define		CMD_ReadWriteProtect	0XF3
#define		CMD_WriteAByte  		0XF2
//#define		CMD_DeleteAll    		0XF1


//读写EPC C1G2命令格式（6C）
#define		CMD_EPC1G2_DetectTagID  	0XEF
#define		CMD_EPC1G2_ListTagID  		0XEE
#define		CMD_EPC1G2_GetIDList  		0XED
#define		CMD_EPC1G2_ReadWordBlock	0XEC
#define		CMD_EPC1G2_WriteWordBlock	0XEB
#define		CMD_EPC1G2_WriteSetLock 	0XEA
#define		CMD_EraseTagData    		0XE9
#define		CMD_DestroyTag      		0XE8
#define		CMD_EPC1G2_WriteEPC  		0XE7
#define		CMD_DataLocking      		0XE6
#define		CMD_EPC1G2_changeeas  		0XE5
#define		CMD_EPC1G2_EasAlarm  		0XE4
#define		CMD_EPC1G2_ReadProtect		0XE3
#define		CMD_EPC1G2_RSTReadProtect   0XE2
#define		CMD_SetUserDataProtect      0XE1
#define		CMD_ReadEPCAndData  		0XE0

#define		CMD_ATA_ListTagID  			0XDE

#define	SourceFrom232 0
#define	SourceFrom485 1
#define	SourceFromUDP 2
#define	SourceFromTCP 3


//串口控制命令数据长度
#define		LEN_SetBaudRate  	    	3////1
#define		LEN_GetReaderVersion  		2///2
#define		LEN_SetRelay        		3///3
#define		LEN_SetOutputPower  		3////4
#define		LEN_SetFrequency     		4////5
#define		LEN_ReadParam       		2////6
#define		LEN_WriteRadioDeep  		3////7
#define		LEN_ReadRadioDeep   		2////8
#define		LEN_WriteParam      		34////9
#define		LEN_SetAntenna      		3////0AH
#define		LEN_ReadRealyStatus 		2////0BH
#define		LEN_SetReaderPara    		34////0CH
#define		LEN_ReadReaderPara  		2////0DH
#define		LEN_Reboot          		2////0EH
#define		LEN_SetReaderWorkType  		3////0FH
#define		LEN_ClearTagListMem  		2////10H
#define		LEN_SetReaderTime    		8////11H
#define		LEN_GetReaderTime   		2////12H


#define		LEN_SetAutoPara	   		    34//13H	  新协议改动了命令设置主动工作参数
#define		LEN_GetAutoPara             2//14H	  新协议改动了命令获取主动工作参数
#define		LEN_SetTagFilter            6//15H	  新协议改动了命令设置标签过滤器
#define		LEN_GetTagFilter            2//16H	  新协议改动了命令获取标签过滤器
#define		LEN_SetFactoryAutoPara      34//17H	  新协议改动了命令设置出厂自动工作参数(内部使用指令)


#if 0
//#define		LEN_AddLableID      		13H
//#define		LEN_DelLableID      		14H
#define		LEN_GetLableID      		5////15H
#define		LEN_GetRecord       		17////16H
#define		LEN_DelRecord       		2////17H
//#define		LEN_SetTagFilter     		18H
#define		LEN_GetTagFilter    		2////19H
#endif
#define		LEN_SetReaderNetwork  		16////30H
#define		LEN_GetReaderNetwork  		2////31H
#define		LEN_SetReaderMAC    		8////32H
#define		LEN_GetReaderMAC    		2////33H

#define		LEN_GetReaderRevStatus 		2////41H
#define		LEN_SetReaderOutStatus 		3////42H
#define		LEN_GetReaderOutStatus 		2////43H

//#define		LEN_SetMasterWorkPara  		50H
//#define		LEN_GetMasterWorkPara  		51H
//#define		LEN_SetDefaultPara  		52H
//#define		LEN_RebackDefaultPara  		53H
#define		LEN_ImmediatelyNotify  		2////54H
#define		LEN_StartReaderAuto  		3////55H
#define		LEN_StopReaderAuto  		12////56H
#define		LEN_GetTagRecord    		5////57H

#define		LEN_ModifyReaderHargVersion	4//85H
#define		LEN_ModifyReaderFrequenceRange 3//86H
#define		LEN_GetReaderFrequenceRange 2//86H
//#define		LEN_OpenRfPower     		87H
#define		LEN_UpdateProgram   		2//87H
//#define		LEN_ChangeToEP500   		88H

#define		LEN_ModifyReaderID  		12////8BH
#define		LEN_GetReaderID     		2////8CH

//#define		LEN_ChangeTagType    		92H



//读写ISO18000-6B命令格式
#define		LEN_DetectTagNum  		2////0xFFH
#define		LEN_ReadLabelID  		2////0xFEH
#define		LEN_ListIDReport  		4////FDH

#define		LEN_ListSelectedID 		13////FBH

#define		LEN_ReadByteBlock  		12////F6H
#define		LEN_WriteByteBlock 		12////F5H
#define		LEN_WriteProtect  		11//F4H
#define		LEN_ReadWriteProtect	11////F3H
#define		LEN_WriteAByte  		12////F2H
#define		LEN_DeleteAll    		2////F1H


//读写EPC C1G2命令格式（6C）
#define		LEN_EPC1G2_ListTagID  		2////EFH
#define		LEN_EPC1G2_ListMaskTagID  	6////EEH
#define		LEN_EPC1G2_GetIDList  		4////EDH
#define		LEN_EPC1G2_ReadWordBlock	10////ECH
#define		LEN_EPC1G2_WriteWordBlock	10////EBH
#define		LEN_EPC1G2_WriteSetBlock 	9////EAH
#define		LEN_EraseTagData    		6////E9H
#define		LEN_DestroyTag      		7////E8H
#define		LEN_EPC1G2_WriteEPC  		7////E7H
//#define		LEN_DataLocking      		E6H
#define		LEN_EPC1G2_changeeas  		8////E5H
#define		LEN_EPC1G2_EasAlarm  		2////E4H
#define		LEN_EPC1G2_ReadProtect		7////E3H
#define		LEN_EPC1G2_RSTReadProtect   6////E2H
//#define		LEN_SetUserDataProtect      E1H
#define		LEN_ReadEPCAndData  		5////E0H

#define		LEN_ATA_ListTagID  			2////DEH

/*------------------------------------------------------------------------------------------------------*/
#pragma pack(1)

//定时器
typedef struct _Timer {
	unsigned char isStart;			//是否启动 1：启动 0：关闭
	unsigned char isOverflow;		//是否溢出 1：溢出 0：未溢出
	unsigned int time;				//定时时间: 等于(time * 100)(毫秒)
} STimer, *PTimer;

typedef	struct  BUF_STR
{
	unsigned char buf[BUF_LEN];	//缓冲区指针
	unsigned int index;			//数据发送的序号
	unsigned int len;				//数据的总长度
} SBUF_STR,*PBUF_STR;

typedef	struct  _CMD_STR
{
	unsigned char bootCode;
	unsigned char len;
	unsigned char cmd;	
	unsigned char para[RFBUF_LEN];	
	//unsigned char chkSum;
} SCMD_STR,*PCMD_STR;

typedef	struct  _CMD_STR1
{
	unsigned char bootCode;
	unsigned char len;
	unsigned char cmd;	
	unsigned char addr;	
	unsigned char para[RFBUF_LEN];	
	//unsigned char chkSum;
} SCMD_STR1,*PCMD_STR1;

typedef	struct  _CMD_R2000
{
	unsigned char bootCode;
	unsigned char len;
	unsigned char addr;	
	unsigned char cmd;	
	unsigned char para[RFBUF_LEN];	
	//unsigned char chkSum;
} SCMD_R2000,*PCMD_R2000;

#pragma pack()

/*------------------------------------------------------------------------------------------------------*/
/*extern tBoolean WatchdogRunning(unsigned long ulBase);
extern void WatchdogEnable(unsigned long ulBase);
extern void WatchdogResetEnable(unsigned long ulBase);
extern void WatchdogResetDisable(unsigned long ulBase);
extern void WatchdogLock(unsigned long ulBase);
extern void WatchdogUnlock(unsigned long ulBase);
extern tBoolean WatchdogLockState(unsigned long ulBase);
extern void WatchdogReloadSet(unsigned long ulBase, unsigned long ulLoadVal);
extern unsigned long WatchdogReloadGet(unsigned long ulBase);
extern unsigned long WatchdogValueGet(unsigned long ulBase);
extern void WatchdogIntRegister(unsigned long ulBase, void(*pfnHandler)(void));
extern void WatchdogIntUnregister(unsigned long ulBase);
extern void WatchdogIntEnable(unsigned long ulBase);
extern unsigned long WatchdogIntStatus(unsigned long ulBase, tBoolean bMasked);
extern void WatchdogIntClear(unsigned long ulBase);
extern void WatchdogStallEnable(unsigned long ulBase);
extern void WatchdogStallDisable(unsigned long ulBase);

 */
//void WatchdogIntHandler(void);
/*------------------------------------------------------------------------------------------------------*/

//启动定时器
void timerStart(PTimer timer, unsigned int time);
//停止定时器
void timerStop(PTimer timer); 
//定时器初始化
void timerInit(PTimer timer); 
//定时器是否溢出
unsigned char timerIsOverflow(PTimer timer); 
void WatchdogIntHandler(void);
unsigned char SetChecksum(unsigned char *buf,unsigned int length);
//所有函数处理完数据形成返回数据包
//void SetBaudRate(unsigned char *buf, unsigned char type);
void SetBaudRate(unsigned char *buf);

//case CMD_GetReaderVersion:		//02H
void GetReaderVersion(unsigned char *buf, unsigned char type);
//case CMD_SetRelay		 :		//03H
void SetRelay(unsigned char *buf);//, unsigned char type);
//case CMD_SetOutputPower  :		//04H
void SetOutputPower(unsigned char *buf);//, unsigned char type);
//case CMD_SetFrequency	 :		//05H
void SetFrequency(unsigned char *buf);//, unsigned char type);
//case CMD_ReadParam		 :		//06H
void ReadParam(unsigned char *buf);//, unsigned char type);
//case CMD_WriteRadioDeep  :		//07H
void WriteRadioDeep(unsigned char *buf);//, unsigned char type);
//case CMD_ReadRadioDeep	 :		//08H
void ReadRadioDeep(unsigned char *buf);//, unsigned char type);
//case CMD_WriteParam 	 :		//09H
void WriteParam(unsigned char *buf);//, unsigned char type);

//void 
unsigned char AntennaCtrl(void);

//case CMD_SetAntenna 	 :		//0AH
void SetAntenna(unsigned char *buf);//, unsigned char type);
//case CMD_ReadRealyStatus :		//0BH
void ReadRealyStatus(unsigned char *buf);//, unsigned char type);
//case CMD_SetReaderPara	 :		//0CH
void SetReaderPara(unsigned char *buf);//, unsigned char type);
//case CMD_ReadReaderPara  :		//0DH
void ReadReaderPara(unsigned char *buf);//, unsigned char type);
//case CMD_Reboot 		 :		//0EH
void Reboot(unsigned char *buf);//, unsigned char type);
//case CMD_SetReaderWorkType: 	//0FH
void SetReaderWorkType(unsigned char *buf);//, unsigned char type);
//case CMD_ClearTagListMem  : 	//10H
void ClearTagListMem(unsigned char *buf);//, unsigned char type);
//case CMD_SetReaderTime	  : 	//11H
void SetReaderTime(unsigned char *buf);//, unsigned char type);
//case CMD_GetReaderTime	  : 	//12H
void GetReaderTime(unsigned char *buf);//, unsigned char type);
void SetAutoPara(unsigned char *buf);//, unsigned char type);
void GetAutoPara(unsigned char *buf);//, unsigned char type);
void SetTagFilter(unsigned char *buf);//, unsigned char type);
void GetTagFilter(unsigned char *buf);//, unsigned char type);
void SetFactoryAutoPara(unsigned char *buf);//, unsigned char type);

#if 0
//case CMD_AddLableID 	  : 	//13H
void AddLableID(unsigned char *buf, unsigned char type);
//case CMD_DelLableID 	  : 	//14H
void DelLableID(unsigned char *buf, unsigned char type);
//case CMD_GetLableID 	  : 	//15H
void GetLableID(unsigned char *buf, unsigned char type);
//case CMD_GetRecord		  : 	//16H
void GetRecord(unsigned char *buf, unsigned char type);
//case CMD_DelRecord		  : 	//17H
void DelRecord(unsigned char *buf, unsigned char type);
//case CMD_SetTagFilter	  : 	//18H
void SetTagFilter(unsigned char *buf, unsigned char type);
//case CMD_GetTagFilter	  : 	//19H
void GetTagFilter(unsigned char *buf, unsigned char type);
#endif

//case CMD_SetReaderNetwork : 	//30H
void SetReaderNetwork(unsigned char *buf);//, unsigned char type);
//case CMD_GetReaderNetwork : 	//31H
void GetReaderNetwork(unsigned char *buf);//, unsigned char type);
//case CMD_SetReaderMAC	  : 	//32H
void SetReaderMAC(unsigned char *buf);//, unsigned char type);
//case CMD_GetReaderMAC	  : 	//33H
void GetReaderMAC(unsigned char *buf);//, unsigned char type);
//case CMD_GetReaderRevStatus:	//41H
void GetReaderRevStatus(unsigned char *buf);//, unsigned char type);
//case CMD_SetReaderOutStatus:	//42H
void SetReaderOutStatus(unsigned char *buf);//, unsigned char type);
//case CMD_GetReaderOutStatus:	//43H
void GetReaderOutStatus(unsigned char *buf);//, unsigned char type);
//case CMD_SetMasterWorkPara :	//50H
//void SetMasterWorkPara(unsigned char *buf, unsigned char type);
//case CMD_GetMasterWorkPara :	//51H
//void GetMasterWorkPara(unsigned char *buf, unsigned char type);
//case CMD_SetDefaultPara    :	//52H
//void SetDefaultPara(unsigned char *buf, unsigned char type);
//case CMD_RebackDefaultPara :	//53H
//void RebackDefaultPara(unsigned char *buf, unsigned char type);
//case CMD_ImmediatelyNotify :	//54H
void ImmediatelyNotify(unsigned char *buf);//, unsigned char type);
//case CMD_StartReaderAuto   :	//55H
//void StartReaderAuto(unsigned char *buf, unsigned char type);
//case CMD_StopReaderAuto    :	//56H
//void StopReaderAuto(unsigned char *buf, unsigned char type);
//case CMD_GetTagRecord	   :	//57H
void GetTagRecord(unsigned char *buf);//, unsigned char type);
//case CMD_ModifyReaderHargVersion   ://85H
//void ModifyReaderHargVersion(unsigned char *buf);//, unsigned char type);
//case CMD_ModifyReaderFrequenceRange://86H
void ModifyReaderFrequenceRange(unsigned char *buf);//, unsigned char type);

//void GetReaderFrequenceRange(unsigned char *buf);//, unsigned char type);

//case CMD_OpenRfPower	 :		//87H
//void OpenRfPower(unsigned char *buf, unsigned char type);
void UpdateProgrram(unsigned char *buf);//, unsigned char type);

//case CMD_ChangeToEP500	 :		//88H
//void ChangeToEP500(unsigned char *buf, unsigned char type);
//case CMD_ModifyReaderID  :		//8BH
void ModifyReaderID(unsigned char *buf);//, unsigned char type);
//case CMD_GetReaderID	 :		//8CH
void GetReaderID(unsigned char *buf);//, unsigned char type);
//case CMD_ChangeTagType	:		//92H
//void ChangeTagType(unsigned char *buf, unsigned char type);
//读写ISO18000-6B命令格式
//case CMD_DetectTagNum	:		//FFH
void DetectTagNum(unsigned char *buf);//, unsigned char type);
//case CMD_ReadLabelID	:		//FEH
void ReadLabelID(unsigned char *buf);//, unsigned char type);
//case CMD_ListIDReport	:		//FDH
void ListIDReport(unsigned char *buf);//, unsigned char type);
//case CMD_ListSelectedID :		//FBH
void ListSelectedID(unsigned char *buf);//, unsigned char type);
//case CMD_ReadByteBlock	:		//F6H
void ReadByteBlock(unsigned char *buf);//, unsigned char type);
//case CMD_WriteByteBlock :		//F5H
void WriteByteBlock(unsigned char *buf);//, unsigned char type);
//case CMD_WriteProtect	:		//F4H
void WriteProtect(unsigned char *buf);//, unsigned char type);
//case CMD_ReadWriteProtect:		//F3H
void ReadWriteProtect(unsigned char *buf);//, unsigned char type);
//case CMD_WriteAByte 	:		//F2H
void WriteAByte(unsigned char *buf);//, unsigned char type);
//case CMD_DeleteAll		:		//F1H
//void DeleteAll(unsigned char *buf, unsigned char type);
//读写EPC C1G2命令格式（6C）
//case CMD_EPC1G2_ListTagID	:	//EFH
void EPC1G2_ListTagID(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_ListTagID	:	//EEH
void EPC1G2_ListMaskTagID(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_GetIDList	:	//EDH
void EPC1G2_GetIDList(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_ReadWordBlock:	//ECH
void EPC1G2_ReadWordBlock(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_WriteWordBlock: //EBH
void EPC1G2_WriteWordBlock(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_WriteWordBlock: //EAH
void EPC1G2_WriteSetBlock(unsigned char *buf);//, unsigned char type)

//case CMD_EraseTagData	:		//E9H
void EraseTagData(unsigned char *buf);//, unsigned char type);
//case CMD_DestroyTag 	:		//E8H
void DestroyTag(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_WriteEPC:		//E7H
void EPC1G2_WriteEPC(unsigned char *buf);//, unsigned char type);
//case CMD_DataLocking	:		//E6H
void DataLocking(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_changeeas:		//E5H
void EPC1G2_changeeas(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_EasAlarm:		//E4H
void EPC1G2_EasAlarm(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_ReadProtect:	//E3H
void EPC1G2_ReadProtect(unsigned char *buf);//, unsigned char type);
//case CMD_EPC1G2_RSTReadProtect: //E2H
void EPC1G2_RSTReadProtect(unsigned char *buf);//, unsigned char type);
//case CMD_SetUserDataProtect:	//E1H
void SetUserDataProtect(unsigned char *buf);//, unsigned char type);
//case CMD_ReadEPCAndData:		//E0H
void ReadEPCAndData(unsigned char *buf);//, unsigned char type);
void defaultCMDpara(unsigned char *buf);//, unsigned char type);
void ATA_ListTagID(unsigned char *buf);//, unsigned char type)

//unsigned char checkCmd(unsigned char *buf)
//void checkCmd(unsigned char *buf, unsigned char type);
//unsigned char getChecksum(unsigned char *buf,unsigned int length, unsigned char type)
//void getChecksum(unsigned char *buf,unsigned int length, unsigned char type);
//void packedData(unsigned char *buf,unsigned char returnValue);
//void UART0IntHandler( void );
//void UART1IntHandler( void );
//void packetData(unsigned char type,long cardID);
void packetDataWG26(unsigned char *ch);
void packetDataWG34(unsigned char *ch);
void packetDatatotal(unsigned char *ch, unsigned char type);
unsigned hextochar(unsigned char ch);
void packetZero(unsigned char type);
void BeepLedControl(void);
void RelayControl(void);
void read_r2000vertion(unsigned char *buf);
//void Responsion(unsigned char *buf);
void EPC1G2_Accessmatch(unsigned char *buf);
void SetFrequency1(unsigned char *buf);
void EPC1G2_ListTagTID(unsigned char *buf);//, unsigned char type);
void EPC1G2_ListRSSIMaskTag(unsigned char *buf);
void EPC1G2_GetRSSIIDList(unsigned char *buf);


#endif

