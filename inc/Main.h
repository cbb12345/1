
/****************************************Copyright (c)****************************************************
**                               ��������ȫ���ܼ������޹�˾                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               Main.h                                                                      **
** Last modified Date:      2012.7.12                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             ��Ӧ��ģ��ͷ�ļ�                                                            **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              ����                                                                        **
** Created date:            2012.7.12                                                                   **
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
    //  1���ڵ�ͨ������,ȡֵ��00H~08H��
    unsigned char BaudRate;
	//  2���书��ֵ, ȡֵ:20-30//30��160
	unsigned char OutputPower;
	//  3΢���ź�Ƶ�ʵ���ʼ��, ȡֵ(ȱʡֵΪ1): 1~63
	unsigned char FrequencyMin;
	//  4����΢���ź�Ƶ�ʵ���ֹ�㣬ȡֵ(ȱʡֵΪ63): 1~63��
	unsigned char FrequencyMax;
	//  5�������(208��Ŀ��  218��Ŀû��)
	unsigned char RadioDepth;
	//  6��д��������ʽ��0-������1-����//1-��ʱ��ʽ��2-������ʽ��3-���ʽ��4-��ʱ��ʽ2��5-������ʽ2
	unsigned char ReaderWorkStatus;
    //  7��д����RS485��ַ:0��255Ϊ�㲥��ַ
    unsigned char Address;
	//  8�趨��������Ŀ��ȱʡֵΪ2��
	unsigned char MaxReadCardNum;
	//  9��ǩ����:ȱʡֵΪ01H����ǩ���ࣺ01H��ISO18000-6B��02H��EPCC1��04H��EPCC1G2��08H��EM4442//TK900��
	unsigned char TagType;
	//  10��������ʱ�䣺��Ƶ�������ʱ�䣬ֻ���EM��ǩ��Ч��0��10ms��1��20ms��2��30ms��3��40ms��
	unsigned char TimeOfReadCard;
    //  11��������M���յ���λ�����������д��ִ��M�δ�����
    unsigned char ReadCardCount;
	//  12  1:ʹ�ܷ�����0:��ʹ�ܷ�����
	unsigned char AlarmRnable;
    //  13��д��IP��ַ1
	unsigned char ReaderIP1;
    //  14��д��IP��ַ2
	unsigned char ReaderIP2;
    //  15��д��IP��ַ3
	unsigned char ReaderIP3;
    //  16��д��IP��ַ4
	unsigned char ReaderIP4;
	//  17��д��I�˿�1 ��λ
	unsigned char ReaderPortHigh;
	//  18��д��I�˿�2 ��λ
	unsigned char ReaderPortLow;
    //  19��д������1
	unsigned char ReaderNetMask1;
    //  20��д������2
	unsigned char ReaderNetMask2;
    //  21��д������3
	unsigned char ReaderNetMask3;
    //  22��д������4
	unsigned char ReaderNetMask4;
    //  23��д������1
	unsigned char ReaderGateway1;
    //  24��д������2
	unsigned char ReaderGateway2;
    //  25��д������3
	unsigned char ReaderGateway3;
    //  26��д������4
	unsigned char ReaderGateway4;
    //  27��д��MAC1
	unsigned char ReaderMAC1;
    //  28��д��MAC2
	unsigned char ReaderMAC2;
    //  29��д��MAC3
	unsigned char ReaderMAC3;
    //  30��д��MAC4
	unsigned char ReaderMAC4;
    //  31��д��MAC5
	unsigned char ReaderMAC5;
    //  32��д��MAC6
	unsigned char ReaderMAC6;
} SWorkParam, *PWorkParam;
typedef struct _AutoWorkParam 
{
    //  1����ǩģʽ��0-��ʱ��ʽ��1-������ʽ
    unsigned char ReadTAGType;
    //  2��ǩ����ʱ�䣺��λ����s��ȱʡֵΪ1����λ
    unsigned char TagExisTimeH;
    //  3��ǩ����ʱ�䣺��λ����s��ȱʡֵΪ1����λ
    unsigned char TagExisTimeL;
    //  4 0-10ms��1-20ms��2-30ms��3-50ms��4-100ms��ȱʡֵΪ2��ÿ���趨ʱ��������ȡһ�α�ǩ
    unsigned char ReadTagInterval;
    //  5��ǩ������Ŀ��ȱʡֵΪ1���Ѷ�ȡ�ı�ǩID�ڶ�д���ڴ��б�������Ŀ. ��λ
    unsigned char TagHoldNumH;
    //  6��ǩ������Ŀ��ȱʡֵΪ1���Ѷ�ȡ�ı�ǩID�ڶ�д���ڴ��б�������Ŀ.��λ
    unsigned char TagHoldNumL;
    //  7���������ʽ��0-�򻯸�ʽ��1-��׼��ʽ��2-XML��ʽ��ȱʡֵΪ0��3-ֱ����� 4-��׼��� 5-������
    	//	3,4,5(218��Ŀ,�����������ֻ��Τ����RS485��Ч)
    	//	208��Ŀ��û��0,1,2��ֻ��3,4,5�������)
    unsigned char OutputDataType;
    //  8����ӿڣ�0��RS232�ڣ�1��RS485�ڣ�2��RJ45�ڡ�ȱʡֵΪ0�� 3- Wiegand26    4- Wiegand34
	unsigned char CardOutModule;
    //  9Wiegand26��������ȣ�ȱʡֵΪ40
	unsigned char WidthOfWiegand;
    //  10Wiegand34�����������ȱʡֵΪ200
	unsigned char InterverOfWiegand;
    //  11�趨������ŵ���ʼλ��ȡֵ0��4��ȱʡֵΪ0��(Wiegand)
	unsigned char CardStartPos;
    //  12�趨�����ڵ��ӱ�ǩ�ϵĴ�ŵ�ַ(ȱʡֵΪ0):0-��ǩ����ID��  1-�û��Զ��忨��(Wiegand)
	unsigned char CardSaveInTagPosition;
    //  13֪ͨ�������λ��s��ȱʡֵΪ120��ÿ���趨ʱ������֪ͨ��λ��һ��
	unsigned char CallPCTimeInterver;
    //  14֪ͨ������ȱʡֵΪ1��0-����֪ͨ��1-��ʱ֪ͨ��2-�����±�ǩ��3-���ٱ�ǩ��4-��ǩ���仯
	unsigned char CallPCType;
    //  15֪ͨ�����,(��ʱ��δ����,������˷���0����EPC��) 0-��ʹ�� 1-ʹ�� ʱ����("֪ͨ���"����)
	unsigned char CallPCEnable;
    //  16����ѡ��1-ant1,2-ant2,4-ant4,8-ant8
	unsigned char Antenna;
    //  17�趨����ģʽ(ȱʡֵΪ0): 0-�͵�ƽ 1-�ߵ�ƽ
	unsigned char TriggleType;
    //  18��λ��IP��ַ1
	unsigned char PCIP1;
    //  19��λ��IP��ַ2
	unsigned char PCIP2;
    //  20��λ��IP��ַ3
	unsigned char PCIP3;
    //  21��λ��IP��ַ4
	unsigned char PCIP4;
    //  22��λ���˿� ��λ
    unsigned char PCPortHigh;
    //  23��λ���˿�  ��λ
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
    //  30 0-��������1-�������ڶ�ʱ�ʹ�����ʽ���Ƿ��ⱨ��
    unsigned char CheckAlarmEnable;
	unsigned char Reverse1;
    //  Э��ɾ����31 ��׼���ʱ������ȱʡֵΪ120�룬1��255��(218��ĿΤ����RS485ʹ��)(208��Ŀ���������ʹ��)
	//unsigned char OutputTimeInterver;
    //  32 ���Զ�״̬�Ƿ���Ƽ̵���0-������  1-����
    unsigned char RelayCTLEnable;
}SAutoWorkParam, *PAutoWorkParam;

typedef struct _EPC6CPARA
{
	//��дEPC C1G2�����ʽ��6C��
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
	//��дISO18000-6B�����ʽ
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

//����Ĳ������Ϳ��ܲ�һ�£������ٸ���
//�����������Ҫ�洢�Ĳ���
typedef struct _Device 
{
    unsigned long Flag;
    //  1������
    //unsigned char BaudRate;    ��ǰ���ظ�
	//  2�汾��
    unsigned char GetReaderVersion[4];
    //  3�̵���״̬
    unsigned char RelayStatus;
	//  4��д���������
    //unsigned char OutputPower; ��ǰ���ظ�
	//  5�趨��д�����ⷢ��΢���źŵ�Ƶ��
	//unsigned char CurrentOutPower;
	//��ʼ����Ƶ��
    //unsigned char FrequencyMin;
	//��ֹ����Ƶ��
    //unsigned char FrequencyMax;
	//  6��ȡ��д����������
    //unsigned char ReadParam;
    //  78�������
    //unsigned char RadioDepth;
	//  9���ö�д����������
    //unsigned char WriteParam;
	//  10����
    //unsigned char Antenna;
	//  11��д���̵���״̬
    //unsigned char ReadRealyStatus;
	//  12���ö�д����������
    //unsigned char SetReaderPara;
    //  13�ָ���д���ĳ�������
    //unsigned char ReadReaderPara;
    //  14��λ��д��
    //unsigned char Reboot;
	//  15����/ֹͣ��д�����Զ�����ģʽ
	//****ע�⣬�ò���ֻ��ϵͳ��������ģʽ�������ᱣ��״̬����λ�����Զ�ģʽ
    //unsigned char SetReaderAutoWorkType;
	//  16����ڴ�
    //unsigned char ClearTagListMem;
	//  1718��д��ʱ��
    unsigned char ReaderTime[6];
    //unsigned char GetReaderTime;
    //  1920���û�ȡ������������
    //  2122���û�ȡ��ǩ������
    //unsigned char TagFilter[16];
	STagFilter tagfilter;
	//  23���ó����Զ���������
    //unsigned char SetFactoryPara;;
	//  2425��д�������ַ
    //unsigned char ReaderNetwork[14];
    //unsigned char GetReaderNetwork;
    //  2627��д������MAC
    //unsigned char ReaderMAC[6];
    //unsigned char GetReaderMAC;
	//  28��д������˵�״̬
    //unsigned char GetReaderRevStatus;
    unsigned char ReaderRevStatus;//ָ�������������
	//  2930��д������˵�״̬
    unsigned char ReaderOutStatus;//��֪����
    //unsigned char GetReaderOutStatus;
    //  31����֪ͨ
    unsigned char ImmediatelyNotify;
    //  32��ǩ��¼
    unsigned char GetTagRecord ;
    //  33�޸Ķ�д��Ӳ���汾��(�ڲ�ʹ��)
	unsigned char ModifyReaderVersion;

    //  34�޸ĺͲ�ѯ��д��Ƶ�ʷ�Χ(�ڲ�ʹ��)
 //   unsigned char FreqSelect;
      SFARER  FreqSelect;
	
    //  35�������������־(�ڲ�ʹ��)
    //unsigned char BootUpdataFlag;
    //  3637���ö�д��ID(�ڲ�ʹ��)
	unsigned char ReaderID[10];

	SWorkParam workpara;				//�豸�Ĺ�������
	SWorkParam Factoryworkpara;			//�豸�ĳ�������
	SAutoWorkParam AutoParam;
	SAutoWorkParam FactoryAutoParam;
	//SEPC6CPARA epc6cpara;
    //S6BPARA s6bpara;
	//Bit0 ����,Bit1����,Bit3MAC��ַ
	unsigned char SysConnectParaChange;
	
	SFREQ curfreq;
	
#if (PARA_STORAGE_DEVICE == FLASH_STM32)
	unsigned char UseForMemAlig;		//���UseForMemAlig������ԭ������Ϊ�ڱ�������Ͷ�ȡ������STM32�ڲ���Flashʱ��
										//����16λ���еģ�Ҳ����2���ֽ�Ϊ��λ���У��������һ��u8����
										//��֤g_device�ṹ��ռ�õ��ڴ��СΪ˫�����ò�������ʵ������
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

