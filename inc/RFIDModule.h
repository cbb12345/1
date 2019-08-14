
/****************************************Copyright (c)****************************************************
**                               深圳市万全智能技术有限公司                                             **
**                                                                                                      **
**                                 http://www.szeway.cn                                                 **
**                                                                                                      **
**--------------File Info-------------------------------------------------------------------------------**
** File Name:               RFIDModule.h                                                                **
** Last modified Date:      2012.7.24                                                                   **
** Last Version:            v1.0                                                                        **
** Description:             RFID通信模块                                                                **
**                                                                                                      **
**------------------------------------------------------------------------------------------------------**
** Created By:              刘飞                                                                        **
** Created date:            2012.7.24                                                                   **
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
	
#ifndef _RFIDModule_H
		
#define _RFIDModule_H

#include "Common.h"

#define EPCLENGTH              32  /* number of bytes for EPC, standard allows up to 62 bytes */


#pragma pack(1)

typedef struct TagInfo_
{
    /** RN16 number. */
    unsigned char rn16[2];
    /** PC value. */
    unsigned char pc[2];
    /** EPC array. */
    unsigned char epc[EPCLENGTH];
    /** EPC length. */
    unsigned char epclen;  /*length in bytes */
    /** Handle for write and read communication with the Tag. */
    unsigned char handle[2];
    /** rssi which has been measured when reading this Tag. */
    unsigned char rssi;
}STagInfo,*PTagInfo;

#pragma pack()


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void readCard6B(void);
void readCard6C(void);
void readCardATA(void);
void readCardEPCATA(void);

void readCard(void);


#endif


