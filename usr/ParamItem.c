/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               ParamItem.c                                                                 **
** Last modified Date:      2012.7.12                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             参数表模块                                                                  **
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
//#include "stdlib.h"
//#include <string.h>
//#include <stddef.h>
#include "ParamItem.h"
#include "Main.h"
#include "Common.h"
#include "R2000.h"
#include <string.h>

//#define ParamID_ParamList 0x0009
SParamTable g_paramTable;
extern  SDevice g_device;
extern char cmdTmpBuf[BUF_LEN];
extern char cmd57ReadTagCount;
extern char cmdTmpLen;
extern int SelfCheckFailure;
extern unsigned char beepFlag;
char TagDataOrigin;

//extern  char ready_g_isExit;
//协议简单，不用专门的模块，将协议处理与参数表处理归类到一块处理
char packet(char cmdOrigin, unsigned char returnFlag, unsigned char *buffer, unsigned int size)
{
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	
	cmdstr = (PCMD_STR)buffer;
	cmdstr1 = (PCMD_STR1)buffer;
	if(CMDOORCHECKOK != returnFlag)           
	{
	    	cmdstr->bootCode = 0xF4;
		if(cmdOrigin == SourceFrom485)
		{
		    	cmdstr1->len = 4;
			cmdstr1->para[0] = returnFlag;
			cmdstr1->para[1] = SetChecksum(buffer,cmdstr1->len+1);
		}
		else
		{
		    cmdstr->len = 3;
			cmdstr->para[0] = returnFlag;
		    cmdstr->para[1] = SetChecksum(buffer,cmdstr->len+1);
		}
	}
	else
	{
		cmdstr->bootCode = 0xF0;
		//if(cmdstr->len < 2)
		//	return ERRORNORETURN;
		if(cmdOrigin == SourceFrom485)
		{
			if(cmdstr1->len < 3)
				return ERRORNORETURN;
		    cmdstr1->para[cmdstr1->len - 3] = SetChecksum(buffer,cmdstr1->len+1);
		}
		else
		{
			if(cmdstr->len < 2)
				return ERRORNORETURN;
		    cmdstr->para[cmdstr->len - 2] = SetChecksum(buffer,cmdstr->len+1);
		}
	}
	if((cmdstr->len + 2) >= size)
		returnFlag = ERRORNORETURN;
	return returnFlag;
}
//数据长度鉴权分析
char TagDataLenPurse(char cmdOrigin, unsigned char *buffer, unsigned int length)
{
//	unsigned int i;
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	unsigned char len,cmd,mlen = 0,temp_lenth;
	char returnFlag = CMDOORCHECKOK;
	cmdstr = (PCMD_STR)buffer;
	cmdstr1 = (PCMD_STR1)buffer;
	len = cmdstr->len;
	cmd = cmdstr->cmd;
	switch(cmd)
	{
		case CMD_EPC1G2_DetectTagID:
		//case CMD_ModifyReaderFrequenceRange:
			break;
		case CMD_EPC1G2_ListTagID://有数据体   EE
		case 0XDF:
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[3] + 7)/8;
				if(len != (LEN_EPC1G2_ListMaskTagID + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[3] + 7)/8;;
				if(len != LEN_EPC1G2_ListMaskTagID + mlen)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_GetIDList://ED
		case 0XDE:
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_EPC1G2_GetIDList + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_EPC1G2_GetIDList)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_ReadWordBlock://EC
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_EPC1G2_ReadWordBlock + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_EPC1G2_ReadWordBlock + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_WriteWordBlock://EB
			if(cmdOrigin == SourceFrom485)
			{
				temp_lenth = cmdstr1->para[0] * 2 + 1;
				temp_lenth = cmdstr1->para[temp_lenth+2]*2;
				mlen = (cmdstr1->para[0]) * 2 + temp_lenth;
				if(len != (LEN_EPC1G2_WriteWordBlock + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				temp_lenth = cmdstr->para[0] * 2 + 1;
				temp_lenth = cmdstr->para[temp_lenth+2]*2;
				mlen = (cmdstr->para[0]) * 2 + temp_lenth;
				if(len != (LEN_EPC1G2_WriteWordBlock + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_WriteSetLock://EA
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_EPC1G2_WriteSetBlock + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_EPC1G2_WriteSetBlock + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EraseTagData://E9
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_EPC1G2_WriteSetBlock + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_EPC1G2_WriteSetBlock + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_DestroyTag://E8
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_DestroyTag + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_DestroyTag + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_WriteEPC://E7
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_EPC1G2_WriteEPC + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_EPC1G2_WriteEPC + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_changeeas://E5
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_EPC1G2_changeeas + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_EPC1G2_changeeas + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_EasAlarm://E4
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_EPC1G2_EasAlarm + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != (LEN_EPC1G2_EasAlarm))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_ReadProtect://E3
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_EPC1G2_ReadProtect + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_EPC1G2_ReadProtect + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_EPC1G2_RSTReadProtect://E2
			if(cmdOrigin == SourceFrom485)
			{
				mlen = (cmdstr1->para[0]) * 2;
				if(len != (LEN_EPC1G2_RSTReadProtect + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = (cmdstr->para[0]) * 2;
				if(len != (LEN_EPC1G2_RSTReadProtect + mlen))
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_ReadEPCAndData://E0
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_ReadEPCAndData + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != (LEN_ReadEPCAndData))
					returnFlag = ORTHERWRONG;
			}
			break;
		/////////////////////////////6B命令
		case CMD_DetectTagNum://FF
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_DetectTagNum + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_DetectTagNum)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_ReadLabelID://FE
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_ReadLabelID + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_ReadLabelID)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_ListIDReport://FD
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_ListIDReport + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_ListIDReport)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_ListSelectedID://FB
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_ListSelectedID + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_ListSelectedID)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_ReadByteBlock://F6
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_ReadByteBlock + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_ReadByteBlock)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_WriteByteBlock://F5
			if(cmdOrigin == SourceFrom485)
			{
				mlen = cmdstr1->para[9];
				if(len != (LEN_WriteByteBlock + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = cmdstr->para[9];
				if(len != LEN_WriteByteBlock + mlen)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_WriteProtect://F4
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_WriteProtect + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_WriteProtect)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_ReadWriteProtect://F3
			if(cmdOrigin == SourceFrom485)
			{
				if(len != (LEN_ReadWriteProtect + 1))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != LEN_ReadWriteProtect)
					returnFlag = ORTHERWRONG;
			}
			break;
		case CMD_WriteAByte://F2
			if(cmdOrigin == SourceFrom485)
			{
				mlen = cmdstr1->para[9];
				if(len != (LEN_WriteAByte + 1 + mlen))
					returnFlag = ORTHERWRONG;
			}
			else
			{
				mlen = cmdstr->para[9];
				if(len != LEN_WriteAByte + mlen)
					returnFlag = ORTHERWRONG;
			}
			break;
	//	case CMD_ATA_ListTagID://F2
	//		if(cmdOrigin == SourceFrom485)
	//		{
	//			if(len != (LEN_ATA_ListTagID + 1))
	//				returnFlag = ORTHERWRONG;
	//		}
	//		else
	//		{
	//			if(len != LEN_ATA_ListTagID)
	//				returnFlag = ORTHERWRONG;
	//		}
	//		break;
		case CMD_SetAntenna:
		case CMD_SetOutputPower:
		case CMD_SetFrequency:
			break;
		default:
			//数据体有规律,基本是没法实现或无法做的命令
			//CMD_EPC1G2_DetectTagID    EF
			//LEN_EPC1G2_WriteSetBlock  E9
			//CMD_DataLocking           E6
			//CMD_SetUserDataProtect    E1
			//CMD_WriteProtect  		F4
			//CMD_ReadWriteProtect	    F3
			//CMD_WriteAByte  		    F2
			if(cmdOrigin == SourceFrom485)
			{
				if(len != 3)
					returnFlag = ORTHERWRONG;
			}
			else
			{
				if(len != 2)
					returnFlag = ORTHERWRONG;
			}
			break;
	}
	return returnFlag;
}

char rangeControl(unsigned char cmd, unsigned char value)
{
	char returnFlag = CMDOORCHECKOK;
	switch(cmd)
	{
		case 0x01:
			if((value < 4) || (value > 8))
				returnFlag = PARAWRONG;
			break;
		case 0x04:
			//if((value < 20) || (value > 30))
			//if(value > 63)
			//	returnFlag = PARAWRONG;
			//break;
			if(value < 20)
				returnFlag = PARAWRONG;
			else if(value > 33)
				returnFlag = PARAWRONG;
			break;
		case 0x05:
			if((value < 1) || (value > 63))
				returnFlag = PARAWRONG;
			break;
		case 0x0f:
			if(value > 1)
				returnFlag = PARAWRONG;
			break;
	//	case 0x0a:
	//		if(value < 20)
	//			returnFlag = PARAWRONG;
	//		else if(value > 33)
	//			returnFlag = PARAWRONG;
	//		break;
		case 0x86:
			if(value > 3)
				returnFlag = PARAWRONG;
			else if(value < 1)
				returnFlag = PARAWRONG;
			break;
	}
	return returnFlag;
}

char compare(void * buf1, void * buf2, unsigned char count) 
{
	int i;
	for(i = 0; i < count; i ++) {
		//if (((char *)buf1)[i] != ((char *)buf2)[i]) {
		if (((char *)buf1)[i] != ((char *)buf2)[i]) {
			return 0;
		}
	}
	return 1;
}

void ReadID(unsigned char *buf)
{
//test 40 02 8D 31
	//u32 Device_Serial0, Device_Serial1, Device_Serial2; 	 
	u32 Device_Serial[3];
	Device_Serial[0] = *(vu32*)(0x1FFFF7E8); 
	Device_Serial[1] = *(vu32*)(0x1FFFF7EC); 
	Device_Serial[2] = *(vu32*)(0x1FFFF7F0);
	memcpy(buf, Device_Serial, 12);
	//memcpy(&buf[0], &Device_Serial0, 4);
	//memcpy(&buf[4], &Device_Serial1, 4);
	//memcpy(&buf[8], &Device_Serial2, 4);
}

char processParamTable(char cmdOrigin, unsigned char *buffer, unsigned int length)
{
	unsigned int i;
	PCMD_STR cmdstr;
	PCMD_STR1 cmdstr1;
	unsigned char len,cmd,addr,sum = 0;
	char returnFlag = CMDOORCHECKOK;
	short idindex;
	unsigned char ReaderWorkStatus;
	cmdstr1 = (PCMD_STR1)buffer;
	cmdstr = (PCMD_STR)buffer;
	
	len = cmdstr->len;//+2;
	cmd = cmdstr->cmd;
	addr = cmdstr1->addr;

	//beepFlag = 1;
	idindex = getparaindex(cmd);
	for(i=0; i < (len + 1);i++)
	{
	    sum += (unsigned char) buffer[i];
	}
	sum =~ sum +1;
	//参数编号太大
	if (idindex >= Count_ParamItem_Max)
	{
		returnFlag = INVALIDINSTRUCTION;
	}
	else
	if(cmd == 0)
	{	// 可以要也可以不要
		returnFlag = INVALIDINSTRUCTION;
	}
	else //长度错误
	if ((len + 2) > length)
	///if ((len + 2) != length)
	{
		returnFlag = ORTHERWRONG;
	}
	else //CRC错
	if(sum != buffer[i])
	{
	    returnFlag = CHECKSUMWRONG;
	}
	else
	if(cmdOrigin == SourceFrom485)
	{
		if(addr != g_device.workpara.Address)
	    {
	    	returnFlag = ERRORNORETURN;
	    }
	}
	if(CMDOORCHECKOK == returnFlag)
	{
		sum = 1;
		if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
		{
			switch(cmd)
			{
				case CMD_GetReaderVersion://02
				case CMD_SetReaderWorkType://0f
				case CMD_ClearTagListMem://10
				case CMD_ImmediatelyNotify://54
				case CMD_GetTagRecord://57
				case CMD_Reboot:
				case CMD_UpdateProgrram:
				case CMD_WriteParam:
				case CMD_SetAutoPara:
					break;
				default:
					sum = 0;
			}
		}
		if(sum != 0)
		{
			if (g_paramTable.table[idindex].flag & Flag_ParamItem_WR)
			{
				switch(cmd)
				{
					case CMD_ReadReaderPara:
					case CMD_Reboot:
					case CMD_ClearTagListMem:
					case CMD_ImmediatelyNotify:
					case CMD_UpdateProgrram:
						if(cmdOrigin == SourceFrom485)
						{
							if(len != 3)
								returnFlag = ORTHERWRONG;
						}
						else
						{
							if(len != 2)
								returnFlag = ORTHERWRONG;
						}
						if(cmd == CMD_ReadReaderPara)
							g_paramTable.setFlag = 1;
						if(cmd == CMD_UpdateProgrram)
							TagDataOrigin = cmdOrigin;
						break;
					default:
						if(cmdOrigin == SourceFrom485)
						{
							if(cmd == CMD_SetTagFilter)
							{
								if(len == 7)
								{
									//memset(&g_device.tagfilter, 0, sizeof(g_device.tagfilter));
									memcpy(g_paramTable.table[idindex].value, cmdstr->para, len - 3);
									g_paramTable.table[idindex].length = 5;
								}
								else if(len < 7)
								{
									returnFlag = PARAWRONG;
								}
								else
								{
									g_paramTable.setFlag = 1;
									memcpy(g_paramTable.table[idindex].value, cmdstr1->para, len - 3);
								}
								g_paramTable.table[idindex].length = cmdstr1->len - 3;
							}
							else if(len != (g_paramTable.table[idindex].length + 3))
								returnFlag = ORTHERWRONG;
							else
							{
								if (g_paramTable.table[idindex].value != NULL)
								{
									returnFlag = rangeControl(cmd, cmdstr1->para[0]);
									if(CMDOORCHECKOK == returnFlag)
									{
										if(cmd == CMD_SetReaderWorkType)
										{
											g_device.workpara.ReaderWorkStatus = cmdstr1->para[0];
										}
										else if(cmd == CMD_WriteParam)
										{
											g_paramTable.setFlag = 1;
											memcpy(g_paramTable.table[idindex].value, cmdstr1->para, g_paramTable.table[idindex].length);
										}
										else if(cmd == CMD_SetAutoPara)
										{
											g_paramTable.setFlag = 1;
											memcpy(g_paramTable.table[idindex].value, cmdstr1->para, g_paramTable.table[idindex].length);
										}
										else if(compare((void *) g_paramTable.table[idindex].value, (void *) cmdstr1->para, g_paramTable.table[idindex].length) == 0)
										{
											memcpy(g_paramTable.table[idindex].value, cmdstr1->para, g_paramTable.table[idindex].length);
											//if((cmd != CMD_SetReaderWorkType) && (cmd != CMD_SetAntenna))
											if(cmd != CMD_SetAntenna)
											g_paramTable.setFlag = 1;
										}
									}
								}
							}
						}
						else
						{
							if(cmd == CMD_SetTagFilter)
							{
								if(len == 6)
								{
									//memset(&g_device.tagfilter, 0, sizeof(g_device.tagfilter));
									memcpy(g_paramTable.table[idindex].value, cmdstr->para, len - 2);
									g_paramTable.table[idindex].length = 4;
								}
								else if(len < 6)
								{
									returnFlag = PARAWRONG;
								}
								else
								{
									g_paramTable.setFlag = 1;
									g_paramTable.table[idindex].length = 0;
									memcpy(g_paramTable.table[idindex].value, cmdstr->para, len - 2);
								}
								g_paramTable.table[idindex].length = cmdstr->len - 2;
							}
							else if(len != (g_paramTable.table[idindex].length + 2))
								returnFlag = ORTHERWRONG;
							else
							{
								if (g_paramTable.table[idindex].value != NULL)
								{
									//if(CMDOORCHECKOK == rangeControl(cmd, cmdstr->para[0]))
									returnFlag = rangeControl(cmd, cmdstr->para[0]);
									if(CMDOORCHECKOK == returnFlag)
									{
										if(cmd == CMD_SetReaderWorkType)
										{
											g_device.workpara.ReaderWorkStatus = cmdstr->para[0];
										}
										else if(cmd == CMD_WriteParam)
										{
											g_paramTable.setFlag = 1;
											memcpy(g_paramTable.table[idindex].value, cmdstr->para, g_paramTable.table[idindex].length);
										}
										else if(cmd == CMD_SetAutoPara)
										{
											g_paramTable.setFlag = 1;
											memcpy(g_paramTable.table[idindex].value, cmdstr->para, g_paramTable.table[idindex].length);
										}
										else if(compare((void *) g_paramTable.table[idindex].value, (void *) cmdstr->para, g_paramTable.table[idindex].length) == 0)
										{
											memcpy(g_paramTable.table[idindex].value, cmdstr->para, g_paramTable.table[idindex].length);
											//if((cmd != CMD_SetReaderWorkType) && (cmd != CMD_SetAntenna))
											if(cmd != CMD_SetAntenna)
											g_paramTable.setFlag = 1;
										}
									}
								}
							}
						}
						cmdstr->len -= g_paramTable.table[idindex].length;
						break;
				}
				if (returnFlag == CMDOORCHECKOK) 
				{
					if (g_paramTable.table[idindex].Function != NULL)
						g_paramTable.table[idindex].Function(buffer);//cmdOrigin, buffer);
				}
			}
			else if (g_paramTable.table[idindex].flag & Flag_ParamItem_RD)
			{
				if(cmd == CMD_GetTagRecord)
					len -= 3;
				if(cmdOrigin == SourceFrom485)
				{
					if(len != 3)
						returnFlag = ORTHERWRONG;
				}
				else
				{
					if(len != 2)
						returnFlag = ORTHERWRONG;
				}
				if (returnFlag == CMDOORCHECKOK) 
				{
					if(cmd == CMD_ReadParam)
						ReaderWorkStatus = g_device.workpara.ReaderWorkStatus;
					if (g_paramTable.table[idindex].Function != NULL)
						g_paramTable.table[idindex].Function(buffer);//cmdOrigin, buffer);
					if (g_paramTable.table[idindex].value != NULL)
					{
						if(cmdOrigin == SourceFrom485)
						{
							if(cmd == CMD_GetTagRecord)
								memcpy(cmdstr1->para, g_paramTable.table[idindex].value, cmdTmpLen);
							
							else if(cmd == CMD_GetTagFilter)
							{
								cmdstr1->para[0] = g_device.tagfilter.TagMaskAddrH;
								cmdstr1->para[1] = g_device.tagfilter.TagMaskAddrL;
								cmdstr1->para[2] = g_device.tagfilter.TagMaskLenH;
								cmdstr1->para[3] = g_device.tagfilter.TagMaskLenL;
								
								g_paramTable.table[idindex].length = g_device.tagfilter.TagMaskLenH << 8;
								g_paramTable.table[idindex].length |= g_device.tagfilter.TagMaskLenL;
								if(g_paramTable.table[idindex].length ==0)
									g_paramTable.table[idindex].length = 4;
								else
								{
									g_paramTable.table[idindex].length = (g_paramTable.table[idindex].length + 7)/8;
									memcpy(&(cmdstr1->para[4]), g_device.tagfilter.TagMaskData, g_paramTable.table[idindex].length);
									g_paramTable.table[idindex].length += 4;
								}
							}
							else
							memcpy(cmdstr1->para, g_paramTable.table[idindex].value, g_paramTable.table[idindex].length);
						}
						else
						{
							if(cmd == CMD_GetTagRecord)
								memcpy(cmdstr->para, g_paramTable.table[idindex].value, cmdTmpLen);
							else if(cmd == CMD_GetTagFilter)
							{
								cmdstr->para[0] = g_device.tagfilter.TagMaskAddrH;
								cmdstr->para[1] = g_device.tagfilter.TagMaskAddrL;
								cmdstr->para[2] = g_device.tagfilter.TagMaskLenH;
								cmdstr->para[3] = g_device.tagfilter.TagMaskLenL;
								
								g_paramTable.table[idindex].length = g_device.tagfilter.TagMaskLenH << 8;
								g_paramTable.table[idindex].length |= g_device.tagfilter.TagMaskLenL;
								if(g_paramTable.table[idindex].length ==0)
									g_paramTable.table[idindex].length = 4;
								else
								{
									g_paramTable.table[idindex].length = (g_paramTable.table[idindex].length + 7)/8;
									memcpy(&(cmdstr->para[4]), g_device.tagfilter.TagMaskData, g_paramTable.table[idindex].length);
									g_paramTable.table[idindex].length += 4;
								}
							}
							else
								memcpy(cmdstr->para, g_paramTable.table[idindex].value, g_paramTable.table[idindex].length);
						}
						if(cmd == CMD_GetTagRecord)
						{
							cmdstr->len -= 3;
							//if(cmdTmpLen == 0)
							//	222;
							//else
							cmdstr->len += cmdTmpLen;//(17 + 1 + CardIndex[i][0]);
						}
						else
						cmdstr->len += g_paramTable.table[idindex].length;
					}
					else if(cmd == CMD_GetCPUID)
					{
						if(cmdOrigin == SourceFrom485)
						{
							ReadID(cmdstr1->para);
						}
						else
						{
							ReadID(cmdstr->para);
						}
						cmdstr->len += 12;
					}
					if(cmd == CMD_ReadParam)
						g_device.workpara.ReaderWorkStatus = ReaderWorkStatus;
				}
			}
			else if (g_paramTable.table[idindex].flag & Flag_ParamItem_OTHR)
			{
				returnFlag = TagDataLenPurse(cmdOrigin, buffer, length);
				if (returnFlag == CMDOORCHECKOK) 
				{
					//beepFlag = 1;
					//如果函数体是空，则原样返回的
					//这部分也回调函数实现得了
					if (g_paramTable.table[idindex].Function != NULL)
					{
						TagDataOrigin = cmdOrigin;
						g_paramTable.table[idindex].Function(buffer);
						if(cmdTmpLen >= BUF_LEN)    ////sizeof(cmdTmpBuf))
						{
							returnFlag = NOTAG;
							//beepFlag = 0;
						}
						else
						{
							//直接在buffer中填充数据
							if(cmdOrigin == SourceFrom485)
							{
								//memcpy(g_paramTable.table[idindex].value, cmdstr1->para, cmdTmpLen);
								cmdstr1->len = cmdTmpLen + 3;
								
							}
							else
							{
								//memcpy(g_paramTable.table[idindex].value, cmdstr->para, cmdTmpLen);
								cmdstr->len = cmdTmpLen + 2;
							}
						}
					}
					else
					{	//beepFlag = 0;
						if(cmdOrigin == SourceFrom485)
						{
							cmdstr1->len = 3;
						}
						else
						{
							cmdstr->len = 2;
						}
					}
				}
			}
			#if 0
			if (returnFlag == CMDOORCHECKOK) 
			{
				g_device.workpara.ReaderWorkStatus = WORK_CMD;
			}
			#endif
		}
		else
		{
			returnFlag = INVALIDMASTERCMD;
		}
	}
	returnFlag = packet(cmdOrigin, returnFlag, buffer, BUF_LEN);
	//returnFlag = packet(cmdOrigin, returnFlag, buffer, BUF_LEN);
	return returnFlag;
}
//获取参数存放的实际编号位置
short getparaindex(unsigned char id)
{
	short i;
	for(i = 0; i < Count_ParamItem_Max; i ++)
	{
		if(g_paramTable.table_index[i]==id)
		{
			break;
		}
	}
	return i;
}
// 如果做自检，可以将函数变成返回函数，注册表空间不够返回错误，供LED指示。
void registryParam(unsigned char id, void *value, unsigned char flag,unsigned char length,void (*Function)(unsigned char *buffer))
{
	int i;
	for(i = 0; i < Count_ParamItem_Max; i ++)
	{
		//if(g_paramTable.table[i].value == NULL)
		if(g_paramTable.table_index[i] == NULL)
		{
			g_paramTable.table[i].value = value;
			g_paramTable.table[i].flag = flag;
			g_paramTable.table[i].length = length;
			g_paramTable.table[i].Function = Function;
			g_paramTable.table_index[i]=id;
			break;
		}
	}
	if(i >= Count_ParamItem_Max)
		SelfCheckFailure = 1;
}

void unRegistryParam(unsigned char id)
{
	int i;
	for(i = 0; i < Count_ParamItem_Max; i ++)
	{
		if (g_paramTable.table_index[i] == id)
		{
			memset((unsigned char *)&g_paramTable.table[i], 0, sizeof(SParamItem));
			memset((unsigned char *)&g_paramTable.table_index[i], 0, sizeof(short));
			break;
		}
	}
}

