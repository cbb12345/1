#ifndef __STM32FLASH_H__
#define __STM32FLASH_H__

#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 512 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1        //ʹ��FLASHд��(0->ʧ��;1->ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//�����洢�豸ѡ�� PARA_STORAGE_DEVICE ��ѡ FLASH_STM32 �� FLASH_AT24C02
#define FLASH_AT24C02			1				
#define FLASH_STM32				2				
#define PARA_STORAGE_DEVICE 	FLASH_STM32			//ѡ�񽫲������浽stm32���ڲ�flash

#if (PARA_STORAGE_DEVICE == FLASH_STM32)
#define PARA_STORAGE_ADDR 		(0x8000000+0x3F800)	//�����洢��ַ,��ʾ�洢��STM32��Falsh�ĵ�127ҳ��
#endif

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ

void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);		//��ָ����ַ��ʼ����ָ�����ȵ�����
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
								   
#endif

















