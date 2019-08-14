
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Main.h                                                                      **
** Last modified Date:      2012.7.12                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             主应用模块头文件                                                            **
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


#ifndef _MAIN_H

#define _MAIN_H

#include "stm32f10x.h"
#include "stm32f107.h"

#define WORK_ACT      0
#define WORK_CMD      1
#define WORK_FLAG     0X5a5a5a22


#define MAXNUM         500//100//200////45//
#define SAVEMAXNUM		500//	250

/*------------------------------------------------------------------------------------------------------*/
#define UARTO_ENABLE 0X00000001
#define UART1_ENABLE 0X00000002
#define UART2_ENABLE 0X00000004
#define GPIOA_ENABLE 0X00000008
#define GPIOB_ENABLE 0X00000010
#define GPIOC_ENABLE 0X00000020
#define GPIOD_ENABLE 0X00000040
#define GPIOE_ENABLE 0X00000080
#define GPIOF_ENABLE 0X00000100
#define GPIOG_ENABLE 0X00000200
#define IIC0_ENABLE  0X00000400
#define TIME0_ENABLE 0X00000800
#define TIME1_ENABLE 0X00001000

/** Definition of the maximum frequencies for the hopping */
#define MAXFREQ                 53//63//53


#define TAGIDLENGTH     31//21////17////32////12

/*------------------------------------------------------------------------------------------------------*/
#pragma pack(1)
struct frequency
{
  unsigned char Integer_N;
  unsigned int  Fractional_N;
};

typedef struct _TagFilter 
{
	unsigned char TagMaskAddrH;
	unsigned char TagMaskAddrL;
	unsigned char TagMaskLenH;
	unsigned char TagMaskLenL;
	unsigned char TagMaskData[TAGIDLENGTH];
} STagFilter, *PTagFilter;
#if 1
typedef struct _FREQ 
{
    unsigned char count;
	unsigned int step;
	unsigned long freq;
} SFREQ, *PFREQ;
#endif
typedef struct Frequencies_
{
    /** Index of the active frequency in freq. */
    unsigned char activefreq;
    /** List of frequencies which are used for hopping. */
    unsigned long  freq[MAXFREQ];
    /** If rssi measurement is above this threshold the channel is regarded as
        used and the system will hop to the next frequency. Otherwise this frequency is used */
    char rssiThreshold[MAXFREQ];
	/////
	struct frequency frequency_area[MAXFREQ];
	/////
}SFrequencies, *PFrequencies;


typedef struct _WorkParam 
{
    //  1串口的通信速率,取值：00H~08H，
    unsigned char BaudRate;
	//  2发射功率值, 取值:20-30//30～160
	unsigned char OutputPower;
	//  3微波信号频率的起始点, 取值(缺省值为1): 1~63
	unsigned char FrequencyMin;
	//  4发射微波信号频率的终止点，取值(缺省值为63): 1~63。
	unsigned char FrequencyMax;
	//  5调制深度(208项目有  218项目没有)
	unsigned char RadioDepth;
	//  6读写器工作方式：0-主动，1-命令//1-定时方式，2-触发方式，3-命令方式，4-定时方式2，5-触发方式2
	unsigned char ReaderWorkStatus;
    //  7读写器的RS485地址:0和255为广播地址
    unsigned char Address;
	//  8设定最多读卡数目，缺省值为2。
	unsigned char MaxReadCardNum;
	//  9标签种类:缺省值为01H；标签种类：01H－ISO18000-6B，02H－EPCC1，04H－EPCC1G2，08H－EM4442//TK900。
	unsigned char TagType;
	//  10读卡持续时间：射频发射持续时间，只针对EM标签有效；0－10ms，1－20ms，2－30ms，3－40ms。
	unsigned char TimeOfReadCard;
    //  11读卡次数M：收到上位机读卡命令，读写器执行M次此命令
    unsigned char ReadCardCount;
	//  12  1:使能蜂鸣器0:不使能蜂鸣器
	unsigned char AlarmRnable;
    //  13读写器IP地址1
	unsigned char ReaderIP1;
    //  14读写器IP地址2
	unsigned char ReaderIP2;
    //  15读写器IP地址3
	unsigned char ReaderIP3;
    //  16读写器IP地址4
	unsigned char ReaderIP4;
	//  17读写器I端口1 高位
	unsigned char ReaderPortHigh;
	//  18读写器I端口2 低位
	unsigned char ReaderPortLow;
    //  19读写器掩码1
	unsigned char ReaderNetMask1;
    //  20读写器掩码2
	unsigned char ReaderNetMask2;
    //  21读写器掩码3
	unsigned char ReaderNetMask3;
    //  22读写器掩码4
	unsigned char ReaderNetMask4;
    //  23读写器网关1
	unsigned char ReaderGateway1;
    //  24读写器网关2
	unsigned char ReaderGateway2;
    //  25读写器网关3
	unsigned char ReaderGateway3;
    //  26读写器网关4
	unsigned char ReaderGateway4;
    //  27读写器MAC1
	unsigned char ReaderMAC1;
    //  28读写器MAC2
	unsigned char ReaderMAC2;
    //  29读写器MAC3
	unsigned char ReaderMAC3;
    //  30读写器MAC4
	unsigned char ReaderMAC4;
    //  31读写器MAC5
	unsigned char ReaderMAC5;
    //  32读写器MAC6
	unsigned char ReaderMAC6;
} SWorkParam, *PWorkParam;
typedef struct _AutoWorkParam 
{
    //  1读标签模式：0-定时方式，1-触发方式
    unsigned char ReadTAGType;
    //  2标签保留时间：单位：秒s。缺省值为1。高位
    unsigned char TagExisTimeH;
    //  3标签保留时间：单位：秒s。缺省值为1。低位
    unsigned char TagExisTimeL;
    //  4 0-10ms，1-20ms，2-30ms，3-50ms，4-100ms。缺省值为2。每隔设定时间主动读取一次标签
    unsigned char ReadTagInterval;
    //  5标签保留数目：缺省值为1。已读取的标签ID在读写器内存中保留的数目. 高位
    unsigned char TagHoldNumH;
    //  6标签保留数目：缺省值为1。已读取的标签ID在读写器内存中保留的数目.低位
    unsigned char TagHoldNumL;
    //  7数据输出格式：0-简化格式，1-标准格式，2-XML格式。缺省值为0。3-直接输出 4-标准输出 5-间隔输出
    	//	3,4,5(218项目,三种输出类型只对韦根和RS485有效)
    	//	208项目中没有0,1,2项只有3,4,5三种输出)
    unsigned char OutputDataType;
    //  8输出接口：0－RS232口，1－RS485口，2－RJ45口。缺省值为0。 3- Wiegand26    4- Wiegand34
	unsigned char CardOutModule;
    //  9Wiegand26输出脉冲宽度，缺省值为40
	unsigned char WidthOfWiegand;
    //  10Wiegand34输出脉冲间隔，缺省值为200
	unsigned char InterverOfWiegand;
    //  11设定输出卡号的起始位，取值0～4。缺省值为0。(Wiegand)
	unsigned char CardStartPos;
    //  12设定卡号在电子标签上的存放地址(缺省值为0):0-标签本身ID号  1-用户自定义卡号(Wiegand)
	unsigned char CardSaveInTagPosition;
    //  13通知间隔：单位秒s。缺省值为120。每隔设定时间主动通知上位机一次
	unsigned char CallPCTimeInterver;
    //  14通知条件：缺省值为1。0-被动通知，1-定时通知，2-增加新标签，3-减少标签，4-标签数变化
	unsigned char CallPCType;
    //  15通知输出端,(长时间未读卡,给输出端发送0长度EPC号) 0-不使用 1-使用 时间由("通知间隔"决定)
	unsigned char CallPCEnable;
    //  16天线选择。1-ant1,2-ant2,4-ant4,8-ant8
	unsigned char Antenna;
    //  17设定触发模式(缺省值为0): 0-低电平 1-高电平
	unsigned char TriggleType;
    //  18上位机IP地址1
	unsigned char PCIP1;
    //  19上位机IP地址2
	unsigned char PCIP2;
    //  20上位机IP地址3
	unsigned char PCIP3;
    //  21上位机IP地址4
	unsigned char PCIP4;
    //  22上位机端口 高位
    unsigned char PCPortHigh;
    //  23上位机端口  低位
    unsigned char PCPortLow;
    //  24
    unsigned char Reverse[1];//[4];//Reverse[6];
    //  25
    //  26
    //  27
    unsigned char EPCATAemulate;
    unsigned short MaxEPCATAtime;
    unsigned char EPCATAReadType;
	unsigned char Reverse0[1];
    //  28
    //  29
    //  30 0-不报警，1-报警。在定时和触发方式下是否检测报警
    unsigned char CheckAlarmEnable;
	unsigned char Reverse1;
    //  协议删除了31 标准输出时间间隔，缺省值为120秒，1～255。(218项目韦根和RS485使用)(208项目所有输出都使用)
	//unsigned char OutputTimeInterver;
    //  32 在自动状态是否控制继电器0-不控制  1-控制
    unsigned char RelayCTLEnable;
}SAutoWorkParam, *PAutoWorkParam;

typedef struct _EPC6CPARA
{
	//读写EPC C1G2命令格式（6C）
	unsigned char EPC1G2_DetectTagID;
	unsigned char EPC1G2_ListTagID;
	unsigned char EPC1G2_GetIDList;
	unsigned char EPC1G2_ReadWordBlock;
	unsigned char EPC1G2_WriteWordBlock;
	unsigned char EPC1G2_WriteProtect;
	unsigned char EraseTagData;
	unsigned char DestroyTag ;
	unsigned char EPC1G2_WriteEPC;
	unsigned char DataLocking;
	unsigned char EPC1G2_ChangeEas;
	unsigned char EPC1G2_EasAlarm;
	unsigned char EPC1G2_ReadProtect;
	unsigned char EPC1G2_RSTReadProtect;
	unsigned char SetUserDataProtect;
	unsigned char ReadEPCAndData;

}SEPC6CPARA, *PEPC6CPARA;

typedef struct _6BPARA
{
	//读写ISO18000-6B命令格式
	unsigned char DetectTagNum;
	unsigned char ReadLabelID;
	unsigned char ListIDReport;
	unsigned char ListSelectedID;
	unsigned char ReadByteBlock;
	unsigned char WriteByteBlock;
	unsigned char WriteProtect;
	unsigned char ReadWriteProtect;
	unsigned char WriteAByte;
	//unsigned char DeleteAll;

}S6BPARA, *P6BPARA;



typedef struct _FARER
{
	unsigned char region;
	unsigned char para[5];
} SFARER,*PSFAER;

//里面的参数类型可能不一致，编码再改正
//后边再整理需要存储的参数
typedef struct _Device 
{
    unsigned long Flag;
    //  1波特率
    //unsigned char BaudRate;    与前面重复
	//  2版本号
    unsigned char GetReaderVersion[4];
    //  3继电器状态
    unsigned char RelayStatus;
	//  4读写器输出功率
    //unsigned char OutputPower; 与前面重复
	//  5设定读写器向外发射微波信号的频率
	//unsigned char CurrentOutPower;
	//起始工作频率
    //unsigned char FrequencyMin;
	//终止工作频率
    //unsigned char FrequencyMax;
	//  6获取读写器工作参数
    //unsigned char ReadParam;
    //  78调制深度
    //unsigned char RadioDepth;
	//  9设置读写器工作参数
    //unsigned char WriteParam;
	//  10天线
    //unsigned char Antenna;
	//  11读写器继电器状态
    //unsigned char ReadRealyStatus;
	//  12设置读写器出厂参数
    //unsigned char SetReaderPara;
    //  13恢复读写器的出厂参数
    //unsigned char ReadReaderPara;
    //  14复位读写器
    //unsigned char Reboot;
	//  15启动/停止读写器的自动工作模式
	//****注意，该参数只让系统进入命令模式，但不会保存状态，复位后还是自动模式
    //unsigned char SetReaderAutoWorkType;
	//  16清除内存
    //unsigned char ClearTagListMem;
	//  1718读写器时间
    unsigned char ReaderTime[6];
    //unsigned char GetReaderTime;
    //  1920设置获取主动工作参数
    //  2122设置获取标签过滤器
    //unsigned char TagFilter[16];
	STagFilter tagfilter;
	//  23设置出厂自动工作参数
    //unsigned char SetFactoryPara;;
	//  2425读写器网络地址
    //unsigned char ReaderNetwork[14];
    //unsigned char GetReaderNetwork;
    //  2627读写器网络MAC
    //unsigned char ReaderMAC[6];
    //unsigned char GetReaderMAC;
	//  28读写器输入端的状态
    //unsigned char GetReaderRevStatus;
    unsigned char ReaderRevStatus;//指出触发的输入脚
	//  2930读写器输出端的状态
    unsigned char ReaderOutStatus;//不知何意
    //unsigned char GetReaderOutStatus;
    //  31立即通知
    unsigned char ImmediatelyNotify;
    //  32标签记录
    unsigned char GetTagRecord ;
    //  33修改读写器硬件版本号(内部使用)
	unsigned char ModifyReaderVersion;

    //  34修改和查询读写器频率范围(内部使用)
 //   unsigned char FreqSelect;
      SFARER  FreqSelect;
	
    //  35设置软件升级标志(内部使用)
    //unsigned char BootUpdataFlag;
    //  3637设置读写器ID(内部使用)
	unsigned char ReaderID[10];

	SWorkParam workpara;				//设备的工作参数
	SWorkParam Factoryworkpara;			//设备的出厂设置
	SAutoWorkParam AutoParam;
	SAutoWorkParam FactoryAutoParam;
	//SEPC6CPARA epc6cpara;
    //S6BPARA s6bpara;
	//Bit0 串口,Bit1网口,Bit3MAC地址
	unsigned char SysConnectParaChange;
	
	SFREQ curfreq;
	
#if (PARA_STORAGE_DEVICE == FLASH_STM32)
	unsigned char UseForMemAlig;		//添加UseForMemAlig变量的原因是因为在保存参数和读取参数到STM32内部的Flash时，
										//是以16位进行的，也就是2个字节为单位进行，这里添加一个u8类型
										//保证g_device结构体占用的内存大小为双数，该参数并无实际意义
#endif
	
	//unsigned char SaveFlag;
} SDevice, *PDevice;
#pragma pack()

/*------------------------------------------------------------------------------------------------------*/

//void SysTickIntHandler(void);
void Watchdog_init(void);
void ReadPara(void);
void initPara(void);
void Systick_init(void);
void SysGpio_init( void );
void initSYS(void);
void defaultPara(void);
void SaveData(void);
//void triggleDetect(void);
void CMDCtrl(void);
//void WorkContinous(void);
void workACT(void);
void SelFreq(void);

int main( void );
void OutputPower(void);
void SetFreq(void);
void SetModDepth(void);
void updateCardNum(unsigned char *buf,unsigned char len);
void sendDataOut(void);

//void ReportFilter(void);
unsigned char ReportFilter(unsigned char len, unsigned char addr,unsigned char *mask, unsigned char *buf);
void reboot(void);
void wdogFeed(void);
void initSPI(void);
void Delay2us(unsigned long Count_us);
void Time_Update(void);
void DelayMs(unsigned long nTime);
int rssichk(int rssi);


#endif

