
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               IIC.c                                                                       **
** Last modified Date:      2013.4.15                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             IIC通信模块                                                                 **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              刘飞                                                                        **
** Created date:            2013.4.15                                                                   **
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


//#include "Common.h"
#include "stm32f10x.h"
#include "IIC.h"

/*****24Cxx Seriel EEPROM*************************/
#define EEPROM 16
/********
01 -> 24C01;   02 -> 24C02;  04 -> 24C04;   08 -> 24C08;
16 -> 24C16;   32 -> 24C32;  64 -> 24C64;   128 -> 24C128;
256-> 24C256;  512 -> 24C512;  
*/

#if EEPROM==1
   #define I2C_PageSize           8
   #define EE_SIZE             0x007F
#elif EEPROM==2
   #define I2C_PageSize           16 
   #define EE_SIZE             0x00FF
#elif EEPROM==4
   #define I2C_PageSize           16
   #define EE_SIZE             0x01FF
#elif EEPROM==8
   #define I2C_PageSize           16
   #define EE_SIZE             0x03FF
#elif EEPROM==16
   #define I2C_PageSize           16
   #define EE_SIZE             0x07FF
#elif EEPROM==32
   #define I2C_PageSize           32
   #define EE_SIZE             0x0FFF
#elif EEPROM==64
   #define I2C_PageSize           32
   #define EE_SIZE             0x1FFF
#elif EEPROM==128
   #define I2C_PageSize           64
   #define EE_SIZE             0x3FFF
#elif EEPROM==256
   #define I2C_PageSize           64
   #define EE_SIZE             0x7FFF
#elif EEPROM==512
   #define I2C_PageSize           128
   #define EE_SIZE             0xFFFF
#endif 

#define I2C_DELAYMAXCOUNT 10000

#define SCLH         GPIOB->BSRR = GPIO_Pin_6
#define SCLL         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDAH         GPIOB->BSRR = GPIO_Pin_7
#define SDAL         GPIOB->BRR  = GPIO_Pin_7

#define SCLread      GPIOB->IDR  & GPIO_Pin_6
#define SDAread      GPIOB->IDR  & GPIO_Pin_7

/*------------------------------------------------------------------------------------------------------*/

#define I2C_Speed              400000
#define I2C2_SLAVE_ADDRESS7    0xA0
//#define I2C_PageSize           16

#define EEPROM_Block_ADDRESS   0xA0

/*------------------------------------------------------------------------------------------------------*/
u16 EEPROM_ADDRESS;

/*------------------------------------------------------------------------------------------------------*/
void IIC_init( void )
{
	#if 1
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C2_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  
  /* I2C Peripheral Enable */
  I2C_Cmd(I2C1, ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C1, &I2C_InitStructure);
  EEPROM_ADDRESS = EEPROM_Block_ADDRESS;
//  I2C_AcknowledgeConfig(I2C1, ENABLE);
  #endif
  #if 0
  I2C_InitTypeDef  I2C_InitStructure;
	   
  I2C_DeInit(I2C1);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x30;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;
   
  I2C_Cmd(I2C1, ENABLE);
  I2C_Init(I2C1, &I2C_InitStructure);
  
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  #endif
}

#if 0

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_delay(uint16_t cnt)
{
	while(cnt--);
}

/*******************************************************************************
* Function Name  : I2C_AcknowledgePolling
* Description    : 
* Input          : I2C_TypeDef * , uint8_t
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2Cx, ENABLE);

    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);

#ifdef AT24C01A

	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
#else

	I2C_Send7bitAddress(I2Cx, 0, I2C_Direction_Transmitter);
#endif

  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2Cx, ENABLE);
}


/*******************************************************************************
* Function Name  : I2C_Read
* Description    : 
* Input          : 
* Output         : 
* Return         : 
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
    if(num==0)
	return 1;
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
		
	I2C_AcknowledgeConfig(I2Cx, ENABLE);


    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

#ifdef AT24C01A	
    I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2Cx, DISABLE);
    		I2C_GenerateSTOP(I2Cx, ENABLE);
		}
	    
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
	    *buf = I2C_ReceiveData(I2Cx);
	    buf++;
	    /* Decrement the read bytes counter */
	    num--;
    }

	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 0;
}	

/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : 
* Input          : 
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value)
{
  	I2C_GenerateSTART(I2Cx, ENABLE);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef AT24C01A
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  	I2C_SendData(I2Cx, addr);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

  	I2C_SendData(I2Cx, value); 
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	I2C_GenerateSTOP(I2Cx, ENABLE);
  
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);

	I2C_delay(1000);

	return 0;
}


/*******************************************************************************
* Function Name  : I2C_Write
* Description    : 
* Input          : 
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
	uint8_t err=0;
	
	while(num--)
	{
		if(I2C_WriteOneByte(I2Cx, I2C_Addr,addr++,*buf++))
		{
			err++;
		}
	}
	if(err)
		return 1;
	else 
		return 0;	
}

#endif
#if 1
/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
		if(NumOfPage == 4)
			NumOfPage = 4;
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}

/*******************************************************************************
* Function Name  : I2C_EE_ByteWrite
* Description    : Writes one byte to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr)
{
	unsigned long loopCount;
  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (loopCount-- > 0));  

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (loopCount-- > 0));
      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, WriteAddr);
  
  /* Test on EV8 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (loopCount-- > 0));

  /* Send the byte to be written */
  I2C_SendData(I2C1, *pBuffer); 
   
  /* Test on EV8 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (loopCount-- > 0));
  
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle. The number of byte can't exceed the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
	unsigned long loopCount;
  /* While the bus is busy */
  loopCount = I2C_DELAYMAXCOUNT;
  while((I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) && (loopCount-- > 0));
  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (loopCount-- > 0)); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (loopCount-- > 0));  

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(I2C1, WriteAddr);  

  /* Test on EV8 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (loopCount-- > 0));

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(I2C1, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
	loopCount = I2C_DELAYMAXCOUNT;
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (loopCount-- > 0));
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
	unsigned long loopCount;

    /* While the bus is busy */
	loopCount = I2C_DELAYMAXCOUNT;
  while((I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) && (loopCount-- > 0));
  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (loopCount-- > 0));
   
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (loopCount-- > 0));
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, ReadAddr);  

  /* Test on EV8 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (loopCount-- > 0));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (loopCount-- > 0));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  loopCount = I2C_DELAYMAXCOUNT;
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (loopCount-- > 0));
  
  /* While there is data to be read */
  loopCount = I2C_DELAYMAXCOUNT;
  //while((NumByteToRead) && (loopCount-- > 0))  
  while(NumByteToRead)
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2C1);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;    
	  loopCount = I2C_DELAYMAXCOUNT;
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_WaitEepromStandbyState
* Description    : Wait for EEPROM Standby state
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WaitEepromStandbyState(void)      
{
	unsigned long loopCount;

  vu16 SR1_Tmp = 0;

  loopCount = I2C_DELAYMAXCOUNT;
  do
  {
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Read I2C2 SR1 register */
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	
  }while((!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002)) && (loopCount-- > 0));
  
  /* Clear AF flag */
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
  
  /* STOP condition */    
  I2C_GenerateSTOP(I2C1, ENABLE);  
}
#endif

