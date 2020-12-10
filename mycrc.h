#ifndef MYCRC_H
#define MYCRC_H

#include <stdint.h>


uint8_t VerifyCRC8Sub(uint8_t *pData,uint8_t Len);
void SetCRC8Sub(uint8_t *Data,uint8_t Len);

/**
* @brief  Function implementing crc16 check.
* @param  pData 校验数据起始地址
* @param  Len 校验数据长度
* @retval 0XFF 校验成功，0x00 校验失败
*/
uint8_t VerifyCRC16Sub(uint8_t *pData,uint16_t Len);

/**
* @brief  计算crc16 并存数据到最后两字节
* @param  pData 校验数据起始地址
* @param  Len 校验数据长度
* @retval
*/
void SetCRC16Sub(uint8_t *pData,uint16_t Len);

/*******************************************************************************
*   函数名称: CRC16
*   功能描述: 查表计算CRC16.
*   输入参数: dataIn -- 待校验数据
*             length -- 数据长度
*   返回值: 校验值
*******************************************************************************/
uint16_t MyCRC16(uint8_t* dataIn, uint16_t length);

#endif // MYCRC_H
