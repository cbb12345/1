
/****************************************Copyright (c)****************************************************
**                               ��������ȫ���ܼ������޹�˾                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               R2000.c                                                                     **
** Last modified Date:      2012.12.17                                                                  **
** Last Version:            v1.0                                                                        **
** Description:             R2000ͨ��ģ��                                                               **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              ����                                                                        **
** Created date:            2012.12.17                                                                  **
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

//�������ܣ��ر�R2000ģ��ķ�����
//������������
//�������أ���
void close_r2000_beep(void)
{
   unsigned char send_buff[6];
   PCMD_R2000 cmdRstr;

   send_buff[0]=0xA0;
   send_buff[1]=0x04;
   send_buff[2]=0xff;//��ַ
   send_buff[3]=0x7A;
   send_buff[4]=0x00;
   send_buff[5]=SetChecksum(send_buff,5);
   R2000.set_mark = 0;
   UART4Send(send_buff,6);  
   	//ʧ���˳�ʼ�����ɹ�
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


//�������ܣ���R2000ģ����и�λ
//������������
//�������أ���
void reset_R2000(void)
{
    unsigned char reset_command[6];

	reset_command[0]=0xA0;  //��λģ��
	reset_command[1]=0x03;
	reset_command[2]=0xFF;
	reset_command[3]=0x70;
	reset_command[4]=SetChecksum(reset_command,4);	   
    UART4Send(reset_command,5);
}

//�������ܣ��Զ�д�����ʽ�������
//����������power_dataΪҪ���õĹ���ֵ 20-30
//�������أ���
void set_output_power(void)
{
	unsigned char send_buff[6];
	PCMD_R2000 cmdRstr;
	
	send_buff[0]=0xA0;
	send_buff[1]=0x04;
	send_buff[2]=0xff;//��ַ
	send_buff[3]=0x76;
	send_buff[4]=g_device.workpara.OutputPower;//20-33
	send_buff[5]=SetChecksum(send_buff,5);
	R2000.set_mark = 0;
	UART4Send(send_buff,6);  
	 //ʧ���˳�ʼ�����ɹ�
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
						//set_mark�������ã������������������ݷ�����
						//R2000.set_mark = 1;
						if(( R2000.rec_buf.len < 4) || ( R2000.rec_buf.len > BUF_LEN))
						{
							memset(&R2000, 0, sizeof(R2000));
						}
						else
						{
							R2000.set_mark = 1;
							//���û��������Ͳ�Ҫ�������
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
				{	//00-���  01-������ 02-���� 03-С��
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
			//�������ʵû�ã����Űɣ����������ԣ�������ֲ
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
	//�������λ��Ҫ����Ĳ��������������£��ȴ�����
	//����Ƿ���λ������ģ��Լ�����
	//�����Ҫ��β������˴�����ʵ�֡�
	//����
	switch(cmdRstr->cmd)
	{
		//��д����������
		case 0x70://��λ��д�� 
			SelfCheckFailure = 5;
			R2000.set_mark = 0;
			break;
			//���ô������
//		case 0x71://���ô���ͨѶ������
//		case 0x72://��ȡ��д���̼��汾
//		case 0x73://���ö�д����ַ
//			break;
			//
//		case 0x74://���ö�д���������� 		
		
//			break;
//		case 0x75://��ѯ��ǰ���߹�������
//			break;
//		case 0x76://���ö�д����Ƶ������� 
//			break;
//		case 0x77://��ѯ��д����ǰ�������
//			break;
//		case 0x78://���ö�д������Ƶ�ʷ�Χ 
//			break;
//		case 0x79://��ѯ��д������Ƶ�ʷ�Χ 
//			break;
//		case 0x7A://���÷�����״̬ 
//			if(cmdRstr->para[0] == command_success)
//				R2000.set_mark = 0;
//			break;
//		case 0x7B://��ѯ��ǰ�豸�Ĺ����¶� 
//			break;
//		case 0x7C://����DRM״̬ 
//			break;
//		case 0x7D://��ѯDRM״̬ 
//			break;
//		case 0x60:
//			break;
//		case 0x61:
//			break;
//		case 0x62:
//			break;
//		case 0x63:
//			break;
		//EPCC1G2����
//		case 0x80://��ǩ�̴�ɹ��󣬾ͽ��ж�ȡ
		
//			break;
		case 0x81://����ǩ   
		
			if(cmdRstr->len > 4)
			{
			//beepFlag = 1;
			//�ж�����
			EPC_lenth = cmdRstr->para[cmdRstr->len-6] >> 1;
			if(EPC_lenth == 0x6)
			{
				//�洢��ǩ,���ϼ򵥵��ж�
				//if(MaskCheck(&cmdRstr->para[5], EPC_lenth*2) != 0)
				{
					beepFlag = 1;
					CardIndex[CardPos][0] = EPC_lenth;
					memcpy(&CardIndex[CardPos][1], &cmdRstr->para[cmdRstr->len -6 - (EPC_lenth << 1)], (EPC_lenth << 1));
					CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len -5]&0x3) + 1;
					//for(i = 4; i > 0;i --)
						//if(tmpAntenna &(1<<(i-1)))
					 		//CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = i;//����
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len - 1 - 4])& 0x03 +1;//���� 
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[cmdRstr->len - 0 - 4];//����
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
					//�Զ�ģʽ�µ������̵���
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
//		case 0x82://д��ǩ ����
///			break;
//		case 0x83://������ǩ
//			break;
//		case 0x84://����ǩ
//			break;
//		case 0x85://ƥ�����ȡ��ƥ���ACCESS����
//			break;
//		case 0x86://��ѯƥ�䷵��
//			break;
//		case 0x89:
//			break;
		//ISO18000-6B����
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
		//�����������
		//case 0x90://��ȡ��ǩ���ݲ�ɾ������
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
				CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = ((cmdRstr->para[0])&0x03) + 1;//����
				CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[(EPC_lenth << 1) + 3];//RSSI
				//ʱ���Ȳ�����ûҪ��
				CardPos++;
				if(CardPos > MAXNUM)
				{
					CardPos = MAXNUM;
				}
			}
			break;
		case 0x91://��ȡ��ǩ���ݱ������汸�� 
			if(cmdRstr->len > 4)
			{
			//beepFlag = 1;
			//�ж�����
			EPC_lenth = ((cmdRstr->para[2]) >> 1) - 2;
			if(EPC_lenth <= 15)
			{
				//�洢��ǩ,���ϼ򵥵��ж�
				if(MaskCheck(&cmdRstr->para[5], EPC_lenth*2) != 0)
				{
					beepFlag++;// = 1;
					CardIndex[CardPos][0] = EPC_lenth;
					memcpy(&CardIndex[CardPos][1], &cmdRstr->para[5], (EPC_lenth << 1));
					CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = AntId +1;//����
					//for(i = 4; i > 0;i --)
						//if(tmpAntenna &(1<<(i-1)))
					 	//	CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = i;
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len - 1 - 4])& 0x03 +1;//���� 
					CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[(cmdRstr->para[2]) + 3];//RSSI//cmdRstr->para[cmdRstr->len - 0 - 4];//����
					//CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[cmdRstr->len - 0 - 4];//����
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
					//�Զ�ģʽ�µ������̵���
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
				//�洢��ǩ,���ϼ򵥵��ж�
				EPC_lenth = ((cmdRstr->para[2]) >> 1) - 2;
				if(EPC_lenth <= 15)
				{
					CardIndex[CardPos][0] = EPC_lenth;
					memcpy(&CardIndex[CardPos][1], &cmdRstr->para[5], (EPC_lenth << 1));
					CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = (cmdRstr->para[cmdRstr->len - 1 - 4]) & 0x03 +1;//���� 
					CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = cmdRstr->para[cmdRstr->len - 0 - 4];//����
					CardIndex[CardPos][TAGIDLENGTH] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 1] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 2] = g_device.ReaderTime[5];
					CardIndex[CardPos][TAGIDLENGTH + 3] = g_device.ReaderTime[3];
					CardIndex[CardPos][TAGIDLENGTH + 4] = g_device.ReaderTime[4];
					CardIndex[CardPos][TAGIDLENGTH + 5] = g_device.ReaderTime[5];
					CardPos++;
					if(CardPos > MAXNUM)
						CardPos = MAXNUM;
					//�Զ�ģʽ�µ������̵���
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
		case 0x92://��ѯ�������Ѷ���ǩ���� 
			break;
		case 0x93://��ձ�ǩ���ݻ��� 
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
			CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = cmdRstr->para[0];//���� 
			CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = 0;//����
			CardIndex[CardPos][TAGIDLENGTH] = g_device.ReaderTime[3];
			CardIndex[CardPos][TAGIDLENGTH + 1] = g_device.ReaderTime[4];
			CardIndex[CardPos][TAGIDLENGTH + 2] = g_device.ReaderTime[5];
			CardIndex[CardPos][TAGIDLENGTH + 3] = g_device.ReaderTime[3];
			CardIndex[CardPos][TAGIDLENGTH + 4] = g_device.ReaderTime[4];
			CardIndex[CardPos][TAGIDLENGTH + 5] = g_device.ReaderTime[5];
			CardPos++;
			if(CardPos > MAXNUM)
				CardPos = MAXNUM;
			//�Զ�ģʽ�µ������̵���
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
			CardIndex[CardPos][TAGIDLENGTH + 6 + 0] = cmdRstr->para[0] + 1;//���� 
			CardIndex[CardPos][TAGIDLENGTH + 6 + 1] = 0;//����
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
			//�Զ�ģʽ�µ������̵���
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
	//�ȸ����������
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
			R2000.rec_buf.buf[len++] = 0; // ��������ʱ����
			R2000.rec_buf.buf[len] = 1;//3;//�ݶ������̵�Ч�����Ƿ��ȫ
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
			//R2000.rec_buf.buf[5] = buffer[0]; //��ʼƵ��
			//R2000.rec_buf.buf[6] = buffer[1]; //����Ƶ��
			g_device.FreqSelect.region = buffer[0];
			
			switch(g_device.FreqSelect.region)

			{
				case 0x01:
				case 0x02:
				case 0x03:
					R2000.rec_buf.buf[1] = 0x06;
					R2000.rec_buf.buf[4] = buffer[0]; //����Ƶ�����
					R2000.rec_buf.buf[5] = buffer[1]; //��ʼƵ��
					R2000.rec_buf.buf[6] = buffer[2]; //����Ƶ��
					break;
				case 0x04:
					R2000.rec_buf.buf[1] = 0x09;
					R2000.rec_buf.buf[4] = buffer[0]; //����Ƶ�����
					R2000.rec_buf.buf[5] = buffer[1]; //Ƶ����
					R2000.rec_buf.buf[6] = buffer[2]; //Ƶ������
					R2000.rec_buf.buf[7] = buffer[3]; //Ƶ��ֵ��λ
					R2000.rec_buf.buf[8] = buffer[4]; //Ƶ��ֵ�м�λ
					R2000.rec_buf.buf[9] = buffer[5]; //Ƶ��ֵ��λ					
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

			R2000.rec_buf.buf[7] = buffer[len + 3];	//����
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
			if(R2000.rec_buf.buf[8] == 0x01)//дEPC
			{
				R2000.rec_buf.buf[1] += 2;
				R2000.rec_buf.buf[9] = 1;
				R2000.rec_buf.buf[10] = buffer[len] + 1;
				R2000.rec_buf.buf[11] = buffer[len] << 3;
				R2000.rec_buf.buf[12] = 0;
				memcpy(&R2000.rec_buf.buf[13], &buffer[len + 1], buffer[len] * 2);
				#if 0
				send_buff[1]=10+lenth*2+2;
				
				send_buff[9]=0x01; //д���ַ��PC��ʼ��Ҫ��дPC,�޸�EPC�ĳ���
				send_buff[10]=lenth+1;	//��λΪ���� �����ֽ�Ϊ��λ �������ֽڵ�PC
				send_buff[11]=lenth;   //EPC�ĳ���
				
				send_buff[11]=send_buff[11]<<3;   //PC��һ���ֽ�
				send_buff[12]=0x00; 			  //PC�ڶ����ֽ�
				
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
			if(R2000.rec_buf.buf[9] == 0)			//��д
				R2000.rec_buf.buf[9] = 0;//����
			else if(R2000.rec_buf.buf[9] == 1)		//���ÿ�д
				R2000.rec_buf.buf[9] = 2;//���ÿ���
			else if(R2000.rec_buf.buf[9] == 2)		//������д
				R2000.rec_buf.buf[9] = 1;//����
			else if(R2000.rec_buf.buf[9] == 3)		//������д
				R2000.rec_buf.buf[9] = 3;//��������
			else if(R2000.rec_buf.buf[9] == 4)		//�ɶ�д 
				R2000.rec_buf.buf[9] = 0;//����
			else if(R2000.rec_buf.buf[9] == 5)		//���ÿɶ�д
				R2000.rec_buf.buf[9] = 2;//���ÿ���
			else if(R2000.rec_buf.buf[9] == 6)		//�������д 
				R2000.rec_buf.buf[9] = 1;//����
			else if(R2000.rec_buf.buf[9] == 7)		//�����ɶ�д
				R2000.rec_buf.buf[9] = 3;//��������
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
		////	���������
		case 0xC5:
		//	len = (buffer[0] << 1);
		//	R2000.rec_buf.buf[1] = 5 + len;
		//	R2000.rec_buf.buf[4] = 1;
		//	R2000.rec_buf.buf[5] = len;
		//	memcpy(&R2000.rec_buf.buf[6], &buffer[1], len);

			R2000.rec_buf.buf[1] = 4;
			R2000.rec_buf.buf[4] = 1;   //ȡ��ƥ��
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

			//����Э������������д���������Ҫʵ��
			R2000.rec_buf.buf[7] = 0;//buffer[3];	//����
			R2000.rec_buf.buf[8] = 0;//buffer[4];
			R2000.rec_buf.buf[9] = 0;//buffer[5];
			R2000.rec_buf.buf[10] = 0;//buffer[6];
			break;
		////
		case 0x91:
			R2000.rec_buf.buf[1] = 0x03;
			break;
		//���������⣬���ݲ��ԡ�
		case 0xB0:
			R2000.rec_buf.buf[1] = 0x03; //4;��Э�飬��3��4������ͨ��
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
 				//��80�����ݶ��Ϻ�
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


