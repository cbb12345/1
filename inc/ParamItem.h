/****************************************Copyright (c)****************************************************
**                               ��������ȫ���ܼ������޹�˾                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               ParamItem.h                                                                 **
** Last modified Date:      2012.7.12                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             ������ģ��                                                                  **
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


#ifndef _Para_H

#define _Para_H

#define Count_ParamItem_Max 70

#define Flag_ParamItem_None    0x00
#define Flag_ParamItem_RD      0x01
#define Flag_ParamItem_WR      0x02
#define Flag_ParamItem_OTHR    0x04

//#define Flag_ParamItem_SYS     0x01
//#define Flag_ParamItem_TAG     0x02
//#define Flag_ParamItem_UpLimit 0x04
//#define Flag_ParamItem_DnLimit 0x08
//#define Flag_ParamItem_Signed  0x10


//������Դ:
#define CmdOrigin_Local        0x00
#define CmdOrigin_Local485  0x01
#define CmdOrigin_UDP         0x02

#pragma pack(1)

typedef struct  _ParamItem
{
	char *value;
	//flag����������������ͣ��ڴ˴����岻��ȥ��
	unsigned char flag;
	unsigned char length;
	//Э������ݷ�Χֻ�ھ����������࣬������������������������˴�����ע�ᡣ
	//int upLimit;
	//int dnLimit;
	//����������Ӧ����Ĳ�������˲����ֶ�д����
	//void (*readFunction)(short paramID);
	//void (*writeFunction)(short paramID);
	//void (*Function)(short paramID);
	//void (*Function)(char cmdOrigin, char *buffer);
	//void (*Function)(void);
	void (*Function)(unsigned char *buffer);
	//char (*Function)(char cmdOrigin, char *buffer);

}SParamItem,*PParamItem;

typedef struct _ParamTable
{
	SParamItem table[Count_ParamItem_Max];
	short table_index[Count_ParamItem_Max];
	int setFlag;
}SParamTable,*PParamTable;

#pragma pack()

char processParamTable(char cmdOrigin, unsigned char *buffer, unsigned int length);
//void registryParam(unsigned char id, void *value, char length, void (*Function)(short paramID));
void registryParam(unsigned char id, void *value, unsigned char flag,unsigned char length,void (*Function)(unsigned char *buffer));
char compare(void * buf1, void * buf2, unsigned char count) ;

void unRegistryParam(unsigned char id);
void calculateParamListParam(void);
short getparaindex(unsigned char id);
char packet(char cmdOrigin, unsigned char returnFlag, unsigned char *buffer, unsigned int size);
char TagDataLenPurse(char cmdOrigin, unsigned char *buffer, unsigned int length);


#endif
