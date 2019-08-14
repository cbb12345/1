
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               R2000.c                                                                     **
** Last modified Date:      2012.12.17                                                                  **
** Last Version:            v1.0                                                                        **
** Description:             R2000通信模块                                                               **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              刘飞                                                                        **
** Created date:            2012.12.17                                                                  **
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

#include "R2000.h"
#include "Main.h"
#include <string.h>
#include "stm32f10x.h"

#define	R2000_REV_TIMER	50 //500MS
#define	R2000_SED_TIMER	200//50 //500MS

SR2000_TRANS_DATA R2000;
extern unsigned char beepFlag;
extern unsigned char CardIndex[MAXNUM][TAGIDLENGTH + 6 + 2];
extern unsigned int CardPos;

extern int SelfCheckFailure;

extern SDevice g_device;
extern unsigned char RelayFlag;

extern unsigned short CMDMaskaddr;
extern unsigned char CMDMasklen,CMDMaskData[TAGIDLENGTH];
//extern unsigned short Tag_R2000_CMD91_number;
//extern unsigned short Tag_R2000_CMD80_number;
extern unsigned int Tag_R2000_CMD91_number;
extern unsigned int Tag_R2000_CMD80_number;
extern unsigned char MASKSEL,MASKAddr6B,MASK6B,MASKData6B[8];
extern unsigned char AntId;
extern unsigned char tmpAntenna;

//函数功能：关闭R2000模块的蜂鸣器
//函数参数：无
//函数返回：无
void close_r2000_beep(void)
{
   unsigned char send_buff[6];
   PCMD_R2000 cmdRstr;

   send_buff[0]=0xA0;
   send_buff[1]=0x04;
   send_buff[2]=0xff;//地址
   send_buff[3]=0x7A;
   send_buff[4]=0x00;
   send_buff[5]=SetChecksum(send_buff,5);
   R2000.set_mark = 0;
   UART4Send(send_buff,6);  
   	//失败了初始化不成功
	timerStart(&R2000.send_timer,R2000_SED_TIMER);
	while(timerIsOverflow(&R2000.send_timer) == 0)
	{
		if( 0 != R2000.set_mark)
		{
			R2000.set_mark = 0;
			cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
			//if((cmdRstr->cmd == 0x7A) && (cmdRstr->para[0] == command_success))
			if(cmdRstr->cmd == 0x7A)
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
	SelfCheckFailure = 7;
	R2000.set_mark = 0;
}


//函数功能：对R2000模块进行复位
//函数参数：无
//函数返回：无
void reset_R2000(void)
{
    unsigned char reset_command[6];

	reset_command[0]=0xA0;  //复位模块
	reset_command[1]=0x03;
	reset_command[2]=0xFF;
	reset_command[3]=0x70;
	reset_command[4]=SetChecksum(reset_command,4);	   
    UART4Send(reset_command,5);
}

//函数功能：对读写器功率进行设置
//函数参数：power_data为要设置的功率值 20-30
//函数返回：无
void set_output_power(void)
{
	unsigned char send_buff[6];
	PCMD_R2000 cmdRstr;
	
	send_buff[0]=0xA0;
	send_buff[1]=0x04;
	send_buff[2]=0xff;//地址
	send_buff[3]=0x76;
	send_buff[4]=g_device.workpara.OutputPower;//20-33
	send_buff[5]=SetChecksum(send_buff,5);
	R2000.set_mark = 0;
	UART4Send(send_buff,6);  
	 //失败了初始化不成功
	 timerStart(&R2000.send_timer,R2000_SED_TIMER);
	 while(timerIsOverflow(&R2000.send_timer) == 0)
	 {
		 if( 0 != R2000.set_mark)
		 {
			 R2000.set_mark = 0;
			 cmdRstr = (PCMD_R2000)R2000.rec_buf.buf;
			 //if((cmdRstr->cmd == 0x76) && (cmdRstr->para[0] == command_success))
			 if(cmdRstr->cmd == 0x76)
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
	 SelfCheckFailure = 6;
	 R2000.set_mark = 0;

}

void R2000_init(void)
{
	USART_InitTypeDef USART_InitStructure;

    memset(&R2000, 0, sizeof(R2000));

	USART_InitStructure.USART_BaudRate            = 115200;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);     
}

void UART4Send( unsigned char *pucBuffer, unsigned long ucCount )
{
    if(ucCount >= BUF_LEN)
		return;
    while(ucCount--)
    {
		while (!(UART4->SR & USART_FLAG_TXE));
   		UART4->DR = (*pucBuffer++ & (uint16_t)0x01FF);
    }
}

void UART4_IRQHandler( void )
{
	unsigned char tmpChar;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
   	{
   	    tmpChar = (unsigned char )USART_ReceiveData(UART4);
		if(R2000.set_mark == 0)
		{
			if((R2000.rec_buf.index == 0) && (tmpChar == 0xA0))
			{
			    R2000.rec_buf.buf[R2000.rec_buf.index++] = tmpChar;
				timerStart(&R2000.local_timer,R2000_REV_TIMER);
			}
			else if(R2000.rec_buf.index != 0)
			{
		                if(R2000.rec_buf.index > BUF_LEN)
		                {
		                    R2000.rec_buf.index = 0;
							R2000.rec_buf.len = 0;
							timerStop(&R2000.local_timer);
		                }
				else
				{
				    R2000.rec_buf.buf[R2000.rec_buf.index] = tmpChar;
					if(R2000.rec_buf.index > (unsigned int)(R2000.rec_buf.buf[1]))
					{
						R2000.rec_buf.len = R2000.rec_buf.index + 1;
					    R2000.rec_buf.index = 0;
						timerStop(&R2000.local_timer);
						//set_mark改作它用，用来处理解析后的数据返回码
						//R2000.set_mark = 1;
						if(( R2000.rec_buf.len < 4) || ( R2000.rec_buf.len > BUF_LEN))
						{
							memset(&R2000, 0, sizeof(R2000));
						}
						else
						{
							R2000.set_mark = 1;
							//如果没几个命令，就不要这个函数
							decodeR2000(R2000.rec_buf.buf, R2000.rec_buf.len);
						}
						
					}
					else
						R2000.rec_buf.index++;
				}
			}
		}
    }
}

unsigned char MaskCheck(unsigned char *buf, unsigned char len)
{
	int i, j, pos, tj, tpos, totallen;
	if(CMDMasklen == 0)
		return 1;
	totallen = CMDMaskaddr + CMDMasklen;
	if(totallen > (len * 8))
		totallen = (len * 8);
	if((len * 8) <= CMDMaskaddr)
		return 0;
	//for(i = 0, j = 0, pos = 0, tj = 0, tpos = 0; i < (CMDMaskaddr + CMDMasklen); i++)
	for(i = 0, j = 0, pos = 0, tj = 0, tpos = 0; i < totallen; i++)
	{
		if(i >= CMDMaskaddr)
		{
			//if((buf[pos] & (1 << j)) != (CMDMaskData[tpos] & (1 << tj)))
			if(((buf[pos] >> (7 - j)) & 0x01) != ((CMDMaskData[tpos] >> (7 - tj)) & 0x01))
				return 0;
			tj++;
			if(tj > 7)
			{
				tpos++;
				tj = 0;
			}
		}
		j++;
		if(j > 7)
		{
			pos++;
			j = 0;
		}
	}
	return 1;
}
unsigned char Mask6BCheck(unsigned char *buf)
{
	//int i, j, pos, tj, tpos;
	int i, tj;
	if(MASK6B == 0)
		return 1;
	//totallen = MASKAddr6B + 64;
	//if(totallen > 64)
	//	totallen = 64;
	for(i = 0, tj = 0; i < 8; i++)
	{
		if(i >= MASKAddr6B)
		{
			if(MASK6B & (1 << (7 - tj)))
			{
				switch(MASKSEL)
				{	//00-相等  01-不等于 02-大于 03-小于
					case 0:
						if(buf[i] != MASKData6B[tj])
							return 0;
						break;
					case 1:
						if(buf[i] != MASKData6B[tj])
							return 1;
						break;
					case 2:
						if(buf[i] < MASKData6B[tj])
							return 0;
						else if(buf[i] > MASKData6B[tj])
							return 1;
						break;
					case 3:
						if(buf[i] > MASKData6B[tj])
							return 0;
						else if(buf[i] < MASKData6B[tj])
							return 1;
						break;
				}
				
			}
			tj++;
			//下面的其实没用，留着吧，保持完整性，方便移植
			if(tj > 7)
			{
				break;
			}
		}
	}
	switch(MASKSEL)
	{
		case 0:
			return 1;
		default:
			return 0;
	}

}

//unsigned char decodeR2000(unsigned char *buffer, unsigned char len)
void decodeR2000(unsigned char *buffer, unsigned char len)
{
	PCMD_R2000 cmdRstr;
//	unsigned char i;
	unsigned char EPC_lenth;
	//int tmplen;
	cmdRstr = (PCMD_R2000)buffer;
	//tmplen = (unsigned int)(g_device.AutoParam.TagHoldNumH << 8);
	//tmplen += (unsigned int)g_device.AutoParam.TagHoldNumL;
	//if(tmplen > MAXNUM)
	//	tmplen = MAXNUM;

	//p = cmdRstr->para;
	//如果是上位机要处理的参数，将其整理下，等待传输
	//如果是非上位机发起的，自己处理
	//如果需要多次操作，此处可以实现。
	//解析
	switch(cmdRstr->cmd)
	{
		//读写器操作命令
		case 0x70://复位读写器 
			SelfCheckFailure = 5;
			R2000.set_mark = 0;
			break;
			//不用处理参数
//		case 0x71://设置串口通讯波特率
//		case 0x72://读取读写器固件版本
//		case 0x73://设置读写器地址
//			break;
			//
//		case 0x74://设置读写器工作天线 		
		
//			break;
//		case 0x75://查询当前天线工作天线
//			break;
//		case 0x76://设置读写器射频输出功率 
//			break;
//		case 0x77://查询读写器当前输出功率
//			break;
//		case 0x78://设置读写器工作频率范围 
//			break;
//		case 0x79://查询读写器工作频率范围 
//			break;
//		case 0x7A://设置蜂鸣器状态 
//			if(cmdRstr->para[0] == command_success)
//				R2000.set_mark = 0;
//			break;
//		case 0x7B://查询当前设备的工作温度 
//			break;
//		case 0x7C://设置DRM状态 
//			break;
//		case 0x7D://查询DRM状态 
//			break;
//		case 0x60:
//			break;
//		case 0x61:
//			break;
//		case 0x62:
//			break;
//		case 0x63:
//			break;
		//EPCC1G2命令
//		case 0x80://标签盘存成功后，就进行读取
		
//			break;
		case 0x81://读标签   
		
			if(cmdRstr->len > 4)
			{
			//beepFlag = 1;
			//判断掩码
			EPC_lenth = cmdRstr->para[cmdRstr->len-6] >> 1;
			if(EPC_lenth == 0x6)
			{
				//存储标签,加上简单的判断
				//if(MaskCheck(&cmdRstr->para[5], EPC_lenth*2) != 0)
				{
					beepFlag = 1;
					CardIndex[CardPos][0] = EPC_lenth;
					memcpy(&CardIndex[CardPos][1], &cmdRstr->para[cmdRstr->len -6 - (EPC_lenth << 1)], (EPC_lenth << 1));
					CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len -5]&0x3) + 1;
					//for(i = 4; i > 0;i --)
						//if(tmpAntenna &(1<<(i-1)))
					 		//CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = i;//天线
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len - 1 - 4])& 0x03 +1;//天线 
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[cmdRstr->len - 0 - 4];//次数
					CardIndex[CardPos][TAGIDLENGTH] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 1] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 2] = g_device.ReaderTime[5];
					CardIndex[CardPos][TAGIDLENGTH + 3] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 4] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 5] = g_device.ReaderTime[5];
					CardPos++;
					if(CardPos > MAXNUM)
					{
						CardPos = MAXNUM;
						Tag_R2000_CMD91_number = Tag_R2000_CMD80_number;
					}
					//自动模式下的联动继电器
					if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
					{
						if(g_device.AutoParam.RelayCTLEnable)
						{
							RelayFlag = 1;
						}
					}
				}
			  }
			}
			 
			break;
//		case 0x82://写标签 返回
///			break;
//		case 0x83://锁定标签
//			break;
//		case 0x84://灭活标签
//			break;
//		case 0x85://匹配或者取消匹配的ACCESS操作
//			break;
//		case 0x86://查询匹配返回
//			break;
//		case 0x89:
//			break;
		//ISO18000-6B命令
//		case 0xB0:
//			break;
//		case 0xB1:
//			break;
//		case 0xB2:
//			break;
//		case 0xB3:
//			break;
//		case 0xB4:
//			break;
		//缓存操作命令
		//case 0x90://提取标签数据并删除缓存
		//	break;
		case 0x8A:
			if(cmdRstr->len == 0x05)
				break;
			EPC_lenth = (cmdRstr->len - 7) >> 1; 
			if(cmdRstr->len == 0x0A)
			{
				Tag_R2000_CMD91_number = (cmdRstr->para[0]);
				Tag_R2000_CMD91_number <<= 8;
				Tag_R2000_CMD91_number |= (cmdRstr->para[1]);
				Tag_R2000_CMD91_number <<= 8;
				Tag_R2000_CMD91_number |= (cmdRstr->para[2]);
				if(Tag_R2000_CMD91_number == 0)
					Tag_R2000_CMD91_number = 0xffffffff;
			}
			else if((cmdRstr->len > 4) && (EPC_lenth <= 15))
			{
				beepFlag++;// = 1;
				CardIndex[CardPos][0] = EPC_lenth;
				memcpy(&CardIndex[CardPos][1], &cmdRstr->para[3], (EPC_lenth << 1));
				CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = ((cmdRstr->para[0])&0x03) + 1;//天线
				CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[(EPC_lenth << 1) + 3];//RSSI
				//时间先不做，没要求
				CardPos++;
				if(CardPos > MAXNUM)
				{
					CardPos = MAXNUM;
				}
			}
			break;
		case 0x91://提取标签数据保留缓存备份 
			if(cmdRstr->len > 4)
			{
			//beepFlag = 1;
			//判断掩码
			EPC_lenth = ((cmdRstr->para[2]) >> 1) - 2;
			if(EPC_lenth <= 15)
			{
				//存储标签,加上简单的判断
				if(MaskCheck(&cmdRstr->para[5], EPC_lenth*2) != 0)
				{
					beepFlag++;// = 1;
					CardIndex[CardPos][0] = EPC_lenth;
					memcpy(&CardIndex[CardPos][1], &cmdRstr->para[5], (EPC_lenth << 1));
					CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = AntId +1;//天线
					//for(i = 4; i > 0;i --)
						//if(tmpAntenna &(1<<(i-1)))
					 	//	CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = i;
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len - 1 - 4])& 0x03 +1;//天线 
					CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[(cmdRstr->para[2]) + 3];//RSSI//cmdRstr->para[cmdRstr->len - 0 - 4];//次数
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[cmdRstr->len - 0 - 4];//次数
					CardIndex[CardPos][TAGIDLENGTH] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 1] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 2] = g_device.ReaderTime[5];
					CardIndex[CardPos][TAGIDLENGTH + 3] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 4] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 5] = g_device.ReaderTime[5];
					CardPos++;
					if(CardPos > MAXNUM)
					{
						CardPos = MAXNUM;
						Tag_R2000_CMD91_number = Tag_R2000_CMD80_number;
					}
					//自动模式下的联动继电器
					if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
					{
						if(g_device.AutoParam.RelayCTLEnable)
						{
							RelayFlag = 1;
						}
					}
				}
			}
			#if 0
			if(MaskCheck(&cmdRstr->para[5]) != 0)
			{
				//存储标签,加上简单的判断
				EPC_lenth = ((cmdRstr->para[2]) >> 1) - 2;
				if(EPC_lenth <= 15)
				{
					CardIndex[CardPos][0] = EPC_lenth;
					memcpy(&CardIndex[CardPos][1], &cmdRstr->para[5], (EPC_lenth << 1));
					CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len - 1 - 4]) & 0x03 +1;//天线 
					CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[cmdRstr->len - 0 - 4];//次数
					CardIndex[CardPos][TAGIDLENGTH] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 1] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 2] = g_device.ReaderTime[5];
					CardIndex[CardPos][TAGIDLENGTH + 3] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 4] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 5] = g_device.ReaderTime[5];
					CardPos++;
					if(CardPos > MAXNUM)
						CardPos = MAXNUM;
					//自动模式下的联动继电器
					if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
					{
						if(g_device.AutoParam.RelayCTLEnable)
						{
							RelayFlag = 1;
						}
					}
				}
			}
			#endif
			Tag_R2000_CMD91_number++;
			}
			break;
		case 0x92://查询缓存中已读标签个数 
			break;
		case 0x93://清空标签数据缓存 
			break;
		case 0x94:
			break;
			#if 0
		case 0xB0:
			if(cmdRstr->len > 5)
			{
			beepFlag = 1;
			CardIndex[CardPos][0] = 4;
			memcpy(&CardIndex[CardPos][1], &cmdRstr->para[1], 8);
			CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = cmdRstr->para[0];//天线 
			CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = 0;//次数
			CardIndex[CardPos][TAGIDLENGTH] = g_device.ReaderTime[3];
			CardIndex[CardPos][TAGIDLENGTH + 1] = g_device.ReaderTime[4];
			CardIndex[CardPos][TAGIDLENGTH + 2] = g_device.ReaderTime[5];
			CardIndex[CardPos][TAGIDLENGTH + 3] = g_device.ReaderTime[3];
			CardIndex[CardPos][TAGIDLENGTH + 4] = g_device.ReaderTime[4];
			CardIndex[CardPos][TAGIDLENGTH + 5] = g_device.ReaderTime[5];
			CardPos++;
			if(CardPos > MAXNUM)
				CardPos = MAXNUM;
			//自动模式下的联动继电器
			if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
			{
				if(g_device.AutoParam.RelayCTLEnable)
				{
					RelayFlag = 1;
				}
			}
			Tag_R2000_CMD91_number++;
			}
			else if(cmdRstr->len == 5)
			{
				Tag_R2000_CMD91_number = cmdRstr->para[1];
			}
			break;
			#endif
		case 0xB0:
			if(cmdRstr->len > 5)
			{
			//beepFlag = 1;
			if(Mask6BCheck(&cmdRstr->para[1]) != 0)
			{
				beepFlag++;// = 1;
			CardIndex[CardPos][0] = 4;
			memcpy(&CardIndex[CardPos][1], &cmdRstr->para[1], 8);
			CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = cmdRstr->para[0] + 1;//天线 
			CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = 0;//次数
			CardIndex[CardPos][TAGIDLENGTH] = g_device.ReaderTime[3];
			CardIndex[CardPos][TAGIDLENGTH + 1] = g_device.ReaderTime[4];
			CardIndex[CardPos][TAGIDLENGTH + 2] = g_device.ReaderTime[5];
			CardIndex[CardPos][TAGIDLENGTH + 3] = g_device.ReaderTime[3];
			CardIndex[CardPos][TAGIDLENGTH + 4] = g_device.ReaderTime[4];
			CardIndex[CardPos][TAGIDLENGTH + 5] = g_device.ReaderTime[5];
			CardPos++;
			if(CardPos > MAXNUM)
			{
				CardPos = MAXNUM;
				Tag_R2000_CMD91_number = Tag_R2000_CMD80_number;
			}
			//自动模式下的联动继电器
			if(g_device.workpara.ReaderWorkStatus == WORK_ACT)
			{
				if(g_device.AutoParam.RelayCTLEnable)
				{
					RelayFlag = 1;
				}
			}
				}
			Tag_R2000_CMD91_number++;
			}
			else if(cmdRstr->len == 5)
			{
				Tag_R2000_CMD80_number = cmdRstr->para[1];
			}
			else if(cmdRstr->len == 4)
				Tag_R2000_CMD80_number = 0;
			break;
	}
}

unsigned char encodeR2000(unsigned char cmd, unsigned char *buffer)
{
	unsigned char len, i;
	//先根据命令组包
	R2000.rec_buf.buf[0] = 0xA0;
	R2000.rec_buf.buf[2] = 0xFF;
	if(cmd == 0xC5)
		R2000.rec_buf.buf[3] = 0x85;
	else if(cmd == 0xC2)
		R2000.rec_buf.buf[3] = 0x82;
	else if(cmd == 0xC1)
		R2000.rec_buf.buf[3] = 0x81;
	else
	R2000.rec_buf.buf[3] = cmd;
	switch(cmd)
	{
		case 0x8A:
			R2000.rec_buf.buf[1] = 0x0D;
			len = 4;
			for(i = 0; i < 4; i++)
			{
				if(tmpAntenna & (1 << i))
					R2000.rec_buf.buf[len++] = i;
				else
					R2000.rec_buf.buf[len++] = 100;
				R2000.rec_buf.buf[len++] = 1;
			}
			R2000.rec_buf.buf[len++] = 0; // 天线休眠时间间隔
			R2000.rec_buf.buf[len] = 1;//3;//暂定，看盘点效果，是否读全
			break;
		case 0x72:
			R2000.rec_buf.buf[1] = 0x03;
			break;
		case 0x74:
			R2000.rec_buf.buf[1] = 0x04;
			len = buffer[0];
			R2000.rec_buf.buf[4] = 0;
			if(len == 1)
				R2000.rec_buf.buf[4] = 0;
			else if(len == 2)
				R2000.rec_buf.buf[4] = 1;
			else if(len == 4)
				R2000.rec_buf.buf[4] = 2;
			else if(len == 8)
				R2000.rec_buf.buf[4] = 3;
			else
				R2000.rec_buf.buf[4] = 0;
			//R2000.rec_buf.buf[4] = buffer[0];
			break;
		case 0x76:
			R2000.rec_buf.buf[1] = 0x04;
			R2000.rec_buf.buf[4] = buffer[0];
			break;
		case 0x78:
			//R2000.rec_buf.buf[1] = 0x06;
			//R2000.rec_buf.buf[4] = g_device.FreqSelectNum; //
			//R2000.rec_buf.buf[5] = buffer[0]; //起始频点
			//R2000.rec_buf.buf[6] = buffer[1]; //结束频点
			g_device.FreqSelect.region = buffer[0];
			
			switch(g_device.FreqSelect.region)

			{
				case 0x01:
				case 0x02:
				case 0x03:
					R2000.rec_buf.buf[1] = 0x06;
					R2000.rec_buf.buf[4] = buffer[0]; //区域频点代号
					R2000.rec_buf.buf[5] = buffer[1]; //起始频点
					R2000.rec_buf.buf[6] = buffer[2]; //结束频点
					break;
				case 0x04:
					R2000.rec_buf.buf[1] = 0x09;
					R2000.rec_buf.buf[4] = buffer[0]; //区域频点代号
					R2000.rec_buf.buf[5] = buffer[1]; //频点间隔
					R2000.rec_buf.buf[6] = buffer[2]; //频点数量
					R2000.rec_buf.buf[7] = buffer[3]; //频率值低位
					R2000.rec_buf.buf[8] = buffer[4]; //频率值中间位
					R2000.rec_buf.buf[9] = buffer[5]; //频率值高位					
					break;		
			}			
			break;
		case 0x80:
			R2000.rec_buf.buf[1] = 0x04;
			R2000.rec_buf.buf[4] = buffer[0];
			break;
		case 0x81:
			R2000.rec_buf.buf[1] = 0x06 + 4;
			len = buffer[0] * 2 + 1;
			R2000.rec_buf.buf[4] = buffer[len];
			//R2000.rec_buf.buf[5] = buffer[len + 1] + 2;
			R2000.rec_buf.buf[5] = buffer[len + 1] ;
			R2000.rec_buf.buf[6] = buffer[len + 2];

			R2000.rec_buf.buf[7] = buffer[len + 3];	//密码
			R2000.rec_buf.buf[8] = buffer[len + 4];
			R2000.rec_buf.buf[9] = buffer[len + 5];
			R2000.rec_buf.buf[10] = buffer[len + 6];
			break;
		case 0x82:
			len = buffer[0] * 2 + 3;
			R2000.rec_buf.buf[1] = 10 + (buffer[len] << 1);
			
			R2000.rec_buf.buf[4] = buffer[len + buffer[len] * 2 + 1];
			R2000.rec_buf.buf[5] = buffer[len + buffer[len] * 2 + 2];
			R2000.rec_buf.buf[6] = buffer[len + buffer[len] * 2 + 3];
			R2000.rec_buf.buf[7] = buffer[len + buffer[len] * 2 + 4];
			
			R2000.rec_buf.buf[8] = buffer[len - 2];
			if(R2000.rec_buf.buf[8] == 0x01)//写EPC
			{
				R2000.rec_buf.buf[1] += 2;
				R2000.rec_buf.buf[9] = 1;
				R2000.rec_buf.buf[10] = buffer[len] + 1;
				R2000.rec_buf.buf[11] = buffer[len] << 3;
				R2000.rec_buf.buf[12] = 0;
				memcpy(&R2000.rec_buf.buf[13], &buffer[len + 1], buffer[len] * 2);
				#if 0
				send_buff[1]=10+lenth*2+2;
				
				send_buff[9]=0x01; //写入地址从PC开始，要改写PC,修改EPC的长度
				send_buff[10]=lenth+1;	//单位为半字 两个字节为单位 加两个字节的PC
				send_buff[11]=lenth;   //EPC的长度
				
				send_buff[11]=send_buff[11]<<3;   //PC第一个字节
				send_buff[12]=0x00; 			  //PC第二个字节
				
				for(i=0;i<(lenth*2);i++)
				{
				  send_buff[i+13]=data_buff[i];
				}
				
				send_buff[i+13]=CheckSum(send_buff,i+13);
				#endif
			}
			else
			{
				R2000.rec_buf.buf[9] = buffer[len - 1];
				R2000.rec_buf.buf[10] = buffer[len];
				memcpy(&R2000.rec_buf.buf[11], &buffer[len + 1], buffer[len] * 2);
			}
			break;
		case 0x83:
			len = buffer[0] * 2 + 1;//mem
			R2000.rec_buf.buf[1] = 9;
			R2000.rec_buf.buf[4] = buffer[len + 2];
			R2000.rec_buf.buf[5] = buffer[len + 3];
			R2000.rec_buf.buf[6] = buffer[len + 4];
			R2000.rec_buf.buf[7] = buffer[len + 5];
		//	if(buffer[len] == 0)
		//		R2000.rec_buf.buf[8] = 5;
		//	if(buffer[len] == 1)
		//		R2000.rec_buf.buf[8] = 4;
		//	if(buffer[len] == 2)
		//		R2000.rec_buf.buf[8] = 3;
		//	if(buffer[len] == 3)
		//		R2000.rec_buf.buf[8] = 2;
		//	if(buffer[len] == 4)
		//		R2000.rec_buf.buf[8] = 1;
			R2000.rec_buf.buf[8] = 5 - buffer[len];
			R2000.rec_buf.buf[9] = buffer[len + 1];
			if(R2000.rec_buf.buf[9] == 0)			//可写
				R2000.rec_buf.buf[9] = 0;//开放
			else if(R2000.rec_buf.buf[9] == 1)		//永久可写
				R2000.rec_buf.buf[9] = 2;//永久开放
			else if(R2000.rec_buf.buf[9] == 2)		//带密码写
				R2000.rec_buf.buf[9] = 1;//锁定
			else if(R2000.rec_buf.buf[9] == 3)		//永不可写
				R2000.rec_buf.buf[9] = 3;//永久锁定
			else if(R2000.rec_buf.buf[9] == 4)		//可读写 
				R2000.rec_buf.buf[9] = 0;//开放
			else if(R2000.rec_buf.buf[9] == 5)		//永久可读写
				R2000.rec_buf.buf[9] = 2;//永久开放
			else if(R2000.rec_buf.buf[9] == 6)		//带密码读写 
				R2000.rec_buf.buf[9] = 1;//锁定
			else if(R2000.rec_buf.buf[9] == 7)		//永不可读写
				R2000.rec_buf.buf[9] = 3;//永久锁定
			break;
		case 0x84:
			R2000.rec_buf.buf[1] = 0x07;
			len = buffer[0] * 2 + 1;
			R2000.rec_buf.buf[4] = buffer[len];
			R2000.rec_buf.buf[5] = buffer[len + 1];
			R2000.rec_buf.buf[6] = buffer[len + 2];
			R2000.rec_buf.buf[7] = buffer[len + 3];
			break;
		case 0x85:
			len = (buffer[0] << 1);
			R2000.rec_buf.buf[1] = 5 + len;
			R2000.rec_buf.buf[4] = 0;
			R2000.rec_buf.buf[5] = len;
			memcpy(&R2000.rec_buf.buf[6], &buffer[1], len);
			break;
		case 0x86:
			R2000.rec_buf.buf[1] = 3;
			break;
		////	假造的命令
		case 0xC5:
		//	len = (buffer[0] << 1);
		//	R2000.rec_buf.buf[1] = 5 + len;
		//	R2000.rec_buf.buf[4] = 1;
		//	R2000.rec_buf.buf[5] = len;
		//	memcpy(&R2000.rec_buf.buf[6], &buffer[1], len);

			R2000.rec_buf.buf[1] = 4;
			R2000.rec_buf.buf[4] = 1;   //取消匹配
			break;
		case 0xC2:
			#if 0
			len = buffer[0] * 2;// + 3;;
			R2000.rec_buf.buf[1] = 10 + buffer[len];
			
			R2000.rec_buf.buf[4] = buffer[buffer[len] * 2 + 1];
			R2000.rec_buf.buf[5] = buffer[buffer[len] * 2 + 2];
			R2000.rec_buf.buf[6] = buffer[buffer[len] * 2 + 3];
			R2000.rec_buf.buf[7] = buffer[buffer[len] * 2 + 4];
			
			R2000.rec_buf.buf[8] = 1;
			R2000.rec_buf.buf[9] = 0;
			R2000.rec_buf.buf[10] = buffer[0];
			memcpy(&R2000.rec_buf.buf[11], &buffer[1], buffer[0] * 2);
			#endif
			len = buffer[0] * 2;// + 3;;
			R2000.rec_buf.buf[1] = 10 + len + 2;//buffer[len];
			
			R2000.rec_buf.buf[4] = buffer[len + 1];//buffer[buffer[len] * 2 + 1];
			R2000.rec_buf.buf[5] = buffer[len + 2];//buffer[buffer[len] * 2 + 2];
			R2000.rec_buf.buf[6] = buffer[len + 3];//buffer[buffer[len] * 2 + 3];
			R2000.rec_buf.buf[7] = buffer[len + 4];//buffer[buffer[len] * 2 + 4];
			
			R2000.rec_buf.buf[8] = 1;
			R2000.rec_buf.buf[9] = 1;
			R2000.rec_buf.buf[10] = buffer[0] + 1;
			R2000.rec_buf.buf[11] = buffer[0] << 3;//buffer[len] << 3;
			R2000.rec_buf.buf[12] = 0;
			memcpy(&R2000.rec_buf.buf[13], &buffer[1], len);//buffer[0] * 2);
			break;
		case 0xC1:
			R2000.rec_buf.buf[1] = 0x06 + 4;
			R2000.rec_buf.buf[4] = buffer[0];
			R2000.rec_buf.buf[5] = buffer[1];
			R2000.rec_buf.buf[6] = buffer[2];

			//这里协议的升级，还有传不传密码要实验
			R2000.rec_buf.buf[7] = 0;//buffer[3];	//密码
			R2000.rec_buf.buf[8] = 0;//buffer[4];
			R2000.rec_buf.buf[9] = 0;//buffer[5];
			R2000.rec_buf.buf[10] = 0;//buffer[6];
			break;
		////
		case 0x91:
			R2000.rec_buf.buf[1] = 0x03;
			break;
		//资料有问题，数据不对。
		case 0xB0:
			R2000.rec_buf.buf[1] = 0x03; //4;破协议，用3和4都可以通信
			break;
		case 0xB1:
			R2000.rec_buf.buf[1] = 0x0D;
			memcpy(&R2000.rec_buf.buf[4], &buffer[0], 8);
			R2000.rec_buf.buf[12] = buffer[8];
			R2000.rec_buf.buf[13] = buffer[9];
			break;
		case 0xB2:
			R2000.rec_buf.buf[1] = 0x0D + buffer[9];
			memcpy(&R2000.rec_buf.buf[4], &buffer[0], 8);
			R2000.rec_buf.buf[12] = buffer[8];
			R2000.rec_buf.buf[13] = buffer[9];
			memcpy(&R2000.rec_buf.buf[14], &buffer[10], buffer[9]);
			break;
		case 0xB3:
			R2000.rec_buf.buf[1] = 0x0C;
			memcpy(&R2000.rec_buf.buf[4], &buffer[0], 8);
			R2000.rec_buf.buf[12] = buffer[8];
			break;
		case 0xB4:
			R2000.rec_buf.buf[1] = 0x0C;
			memcpy(&R2000.rec_buf.buf[4], &buffer[0], 8);
			R2000.rec_buf.buf[12] = buffer[8];
			break;
		case 0xB5:
			R2000.rec_buf.buf[1] = 0x0C;
			memcpy(&R2000.rec_buf.buf[4], &buffer[0], 8);
			R2000.rec_buf.buf[12] = buffer[8];
			break;
		default:
			return 0xFF;
	}
	len = R2000.rec_buf.buf[1] + 1;
	R2000.rec_buf.buf[len] = SetChecksum(R2000.rec_buf.buf, len);
	R2000.set_mark = 0;
	UART4Send(R2000.rec_buf.buf, len + 1);
	if((cmd == 0x91) || (cmd == 0xB0) ||(cmd == 0x80))// || (cmd == 0x74))
		timerStart(&R2000.send_timer,R2000_SED_TIMER+800);
	else
		timerStart(&R2000.send_timer,R2000_SED_TIMER);
	while(timerIsOverflow(&R2000.send_timer) == 0)
	{
		if( 0 != R2000.set_mark)
		{
			R2000.set_mark = 0;
			if(cmd == 0x91)
			{
 				//和80的数据对上号
				if(Tag_R2000_CMD91_number >= Tag_R2000_CMD80_number)
				{
					timerStop(&R2000.send_timer);
					return 0;
				}
				else
					timerStart(&R2000.send_timer,R2000_SED_TIMER+800);
			}
			else if(cmd == 0xB0)
			{
				//if(CardPos >= Tag_R2000_CMD91_number)
				//if(Tag_R2000_CMD80_number >= Tag_R2000_CMD91_number)
				//if(Tag_R2000_CMD91_number >= Tag_R2000_CMD80_number)
				if((Tag_R2000_CMD91_number >= Tag_R2000_CMD80_number) || (Tag_R2000_CMD80_number == 0))
				{
					timerStop(&R2000.send_timer);
					if(Tag_R2000_CMD80_number == 0)
						return 1;
					else
						return 0;
				}
				else
					timerStart(&R2000.send_timer,R2000_SED_TIMER+800);
			}
			else if(cmd == 0x8A)
			{
				if(Tag_R2000_CMD91_number != 0)
				{
					timerStop(&R2000.send_timer);
					return 0;
				}
				else
				{
					timerStart(&R2000.send_timer,R2000_SED_TIMER);
				}
			}
			else
			{
				timerStop(&R2000.send_timer);
				return 0;
			}
		}
		
	}
	timerStop(&R2000.send_timer);
//	if(cmd == 0x74)
//		return 0;
	if((cmd == 0x91) && (CardPos > 0))
		return 0;
	if((cmd == 0xB0) && (CardPos > 0))
		return 0;
	//if((cmd == 0x8A) && (CardPos > 0))
	if(cmd == 0x8A)
	{
		return 0;
	}
	memset(&R2000, 0, sizeof(R2000));
	return 0xFF;
}


