#include "stm32flash.h"

//brief	:	读取指定地址的半字(16位数据)
//faddr	:	读地址(此地址必须为2的倍数!!)
//返回值:	对应数据.
static u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

//brief		:	从指定地址开始读出指定长度的数据
//ReadAddr	:	起始地址,必须是2的倍数
//pBuffer	:	数据指针
//NumToWrite:	半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

#if STM32_FLASH_WREN	//如果使能了写   

//brief		:	不检查的写入
//WriteAddr	:	起始地址
//pBuffer	:	数据指针
//NumToWrite:	半字(16位)数
static void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);//库函数，写入半字
	    WriteAddr+=2;//地址增加2.
	}
} 

//brief		:	从指定地址开始写入指定长度的数据
//WriteAddr	:	起始地址(此地址必须为2的倍数!!)
//pBuffer	:	数据指针
//NumToWrite:	半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //flash容量小于256KB的,扇区大小就是1024字节
#else 
#define STM_SECTOR_SIZE	2048 //否则就是2048字节
#endif

//最多是2K字节(因为是16位的数组)，它作为临时缓冲区，用于临时保存原来的扇区的数据，
//因为Flash的擦除是按照扇区进行的，如果要写入的地址上的数据不是0xffff
//就需要对整个扇区进行擦除，为了不破坏原有的扇区上的数据
//在擦除操作之前，先将扇区的数据读出来，存储到STMFLASH_BUF
//再对STMFLASH_BUF对应的位置填充数据
//然后将整个STMFLASH_BUF写入到扇区中
static u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];

void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址

	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址

	FLASH_Unlock();							//解锁

	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址,即第几个扇区  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小(2个字节为基本单位.)   

	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围

	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容

		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区，为什么要处以2，是因为这个函数是以半字为单位进行写入的
		}
		else
		{
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		}

		if(NumToWrite==secremain)	
		{
			break;//写入结束了
		}
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减

			if(NumToWrite>(STM_SECTOR_SIZE/2))
			{
				secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			}
			else 
			{
				secremain=NumToWrite;//下一个扇区可以写完了
			}
		}	 
	}	
	FLASH_Lock();//上锁
}
#endif


