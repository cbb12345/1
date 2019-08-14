/**
  ******************************************************************************
  * @file    stm32f107.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   This file contains all the functions prototypes for the STM32F107 
  *          file.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F107_H
#define __STM32F107_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
//#include "stm32_eval.h"
//#include "stm3210c_eval_lcd.h"
//#include "stm3210c_eval_ioe.h"

#define Enable485 		GPIO_ResetBits(GPIOD,GPIO_Pin_4)
#define Disable485	 	GPIO_SetBits(GPIOD,GPIO_Pin_4)

#define BEEPON  GPIO_SetBits(GPIOE, GPIO_Pin_12)
#define BEEPOFF GPIO_ResetBits(GPIOE, GPIO_Pin_12)

#define LEDYellowOFF   GPIO_SetBits(GPIOE, GPIO_Pin_3)
#define LEDYellowON  GPIO_ResetBits(GPIOE, GPIO_Pin_3)
	 
#define LEDGreenOFF   GPIO_SetBits(GPIOE, GPIO_Pin_2)	 
#define LEDGreenON  GPIO_ResetBits(GPIOE, GPIO_Pin_2)

#define PHY_RESETHigh   GPIO_SetBits(GPIOB, GPIO_Pin_14)	 
#define PHY_RESETLow  GPIO_ResetBits(GPIOB, GPIO_Pin_14)	 
	 
#define WGDATA0CLR	GPIO_SetBits(GPIOE, GPIO_Pin_13)
#define WGDATA0SET	GPIO_ResetBits(GPIOE, GPIO_Pin_13)
#define WGDATA1CLR	GPIO_SetBits(GPIOE, GPIO_Pin_14)
#define WGDATA1SET	GPIO_ResetBits(GPIOE, GPIO_Pin_14)

#define WGDATA0CLR1	GPIO_SetBits(GPIOE, GPIO_Pin_11)
#define WGDATA0SET1	GPIO_ResetBits(GPIOE, GPIO_Pin_11)
#define WGDATA1CLR1	GPIO_SetBits(GPIOE, GPIO_Pin_10)
#define WGDATA1SET1	GPIO_ResetBits(GPIOE, GPIO_Pin_10)

#define RELAYOFF   GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define RELAYON  GPIO_SetBits(GPIOB, GPIO_Pin_10)

#define RELAYOFF1   GPIO_ResetBits(GPIOE, GPIO_Pin_8)
#define RELAYON1  GPIO_SetBits(GPIOE, GPIO_Pin_8)

#define GetPinTriggle()	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)
#define GetPinTriggle1()	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)

void System_Setup(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10F107_H */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
