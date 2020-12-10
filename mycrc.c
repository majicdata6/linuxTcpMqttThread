#include "mycrc.h"


//CRC查表法 理由计较节省时间
const uint8_t cCRC8Table[256]={
0X00, 0X07, 0X0E, 0X09, 0X1C, 0X1B, 0X12, 0X15,
0X38, 0X3F, 0X36, 0X31, 0X24, 0X23, 0X2A, 0X2D,
0X70, 0X77, 0X7E, 0X79, 0X6C, 0X6B, 0X62, 0X65,
0X48, 0X4F, 0X46, 0X41, 0X54, 0X53, 0X5A, 0X5D,
0XE0, 0XE7, 0XEE, 0XE9, 0XFC, 0XFB, 0XF2, 0XF5,
0XD8, 0XDF, 0XD6, 0XD1, 0XC4, 0XC3, 0XCA, 0XCD,
0X90, 0X97, 0X9E, 0X99, 0X8C, 0X8B, 0X82, 0X85,
0XA8, 0XAF, 0XA6, 0XA1, 0XB4, 0XB3, 0XBA, 0XBD,
0XC7, 0XC0, 0XC9, 0XCE, 0XDB, 0XDC, 0XD5, 0XD2,
0XFF, 0XF8, 0XF1, 0XF6, 0XE3, 0XE4, 0XED, 0XEA,
0XB7, 0XB0, 0XB9, 0XBE, 0XAB, 0XAC, 0XA5, 0XA2,
0X8F, 0X88, 0X81, 0X86, 0X93, 0X94, 0X9D, 0X9A,
0X27, 0X20, 0X29, 0X2E, 0X3B, 0X3C, 0X35, 0X32,
0X1F, 0X18, 0X11, 0X16, 0X03, 0X04, 0X0D, 0X0A,
0X57, 0X50, 0X59, 0X5E, 0X4B, 0X4C, 0X45, 0X42,
0X6F, 0X68, 0X61, 0X66, 0X73, 0X74, 0X7D, 0X7A,
0X89, 0X8E, 0X87, 0X80, 0X95, 0X92, 0X9B, 0X9C,
0XB1, 0XB6, 0XBF, 0XB8, 0XAD, 0XAA, 0XA3, 0XA4,
0XF9, 0XFE, 0XF7, 0XF0, 0XE5, 0XE2, 0XEB, 0XEC,
0XC1, 0XC6, 0XCF, 0XC8, 0XDD, 0XDA, 0XD3, 0XD4,
0X69, 0X6E, 0X67, 0X60, 0X75, 0X72, 0X7B, 0X7C,
0X51, 0X56, 0X5F, 0X58, 0X4D, 0X4A, 0X43, 0X44,
0X19, 0X1E, 0X17, 0X10, 0X05, 0X02, 0X0B, 0X0C,
0X21, 0X26, 0X2F, 0X28, 0X3D, 0X3A, 0X33, 0X34,
0X4E, 0X49, 0X40, 0X47, 0X52, 0X55, 0X5C, 0X5B,
0X76, 0X71, 0X78, 0X7F, 0X6A, 0X6D, 0X64, 0X63,
0X3E, 0X39, 0X30, 0X37, 0X22, 0X25, 0X2C, 0X2B,
0X06, 0X01, 0X08, 0X0F, 0X1A, 0X1D, 0X14, 0X13,
0XAE, 0XA9, 0XA0, 0XA7, 0XB2, 0XB5, 0XBC, 0XBB,
0X96, 0X91, 0X98, 0X9F, 0X8A, 0X8D, 0X84, 0X83,
0XDE, 0XD9, 0XD0, 0XD7, 0XC2, 0XC5, 0XCC, 0XCB,
0XE6, 0XE1, 0XE8, 0XEF, 0XFA, 0XFD, 0XF4, 0XF3
};

const uint8_t cCRC16HiTable[256]={
0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0,
0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41,
0X00, 0XC1, 0X81, 0X40, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0,
0X80, 0X41, 0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1, 0X81, 0X40,
0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1,
0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0, 0X80, 0X41,
0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1,
0X81, 0X40, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41,
0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0,
0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X00, 0XC1, 0X81, 0X40,
0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1,
0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1, 0X81, 0X40,
0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0,
0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X00, 0XC1, 0X81, 0X40,
0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0,
0X80, 0X41, 0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1, 0X81, 0X40,
0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0,
0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41,
0X00, 0XC1, 0X81, 0X40, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0,
0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41,
0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0,
0X80, 0X41, 0X00, 0XC1, 0X81, 0X40, 0X00, 0XC1, 0X81, 0X40,
0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0, 0X80, 0X41, 0X00, 0XC1,
0X81, 0X40, 0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41,
0X00, 0XC1, 0X81, 0X40, 0X01, 0XC0, 0X80, 0X41, 0X01, 0XC0,
0X80, 0X41, 0X00, 0XC1, 0X81, 0X40};

const uint8_t cCRC16LoTable[256]={
0X00, 0XC0, 0XC1, 0X01, 0XC3, 0X03, 0X02, 0XC2, 0XC6, 0X06,
0X07, 0XC7, 0X05, 0XC5, 0XC4, 0X04, 0XCC, 0X0C, 0X0D, 0XCD,
0X0F, 0XCF, 0XCE, 0X0E, 0X0A, 0XCA, 0XCB, 0X0B, 0XC9, 0X09,
0X08, 0XC8, 0XD8, 0X18, 0X19, 0XD9, 0X1B, 0XDB, 0XDA, 0X1A,
0X1E, 0XDE, 0XDF, 0X1F, 0XDD, 0X1D, 0X1C, 0XDC, 0X14, 0XD4,
0XD5, 0X15, 0XD7, 0X17, 0X16, 0XD6, 0XD2, 0X12, 0X13, 0XD3,
0X11, 0XD1, 0XD0, 0X10, 0XF0, 0X30, 0X31, 0XF1, 0X33, 0XF3,
0XF2, 0X32, 0X36, 0XF6, 0XF7, 0X37, 0XF5, 0X35, 0X34, 0XF4,
0X3C, 0XFC, 0XFD, 0X3D, 0XFF, 0X3F, 0X3E, 0XFE, 0XFA, 0X3A,
0X3B, 0XFB, 0X39, 0XF9, 0XF8, 0X38, 0X28, 0XE8, 0XE9, 0X29,
0XEB, 0X2B, 0X2A, 0XEA, 0XEE, 0X2E, 0X2F, 0XEF, 0X2D, 0XED,
0XEC, 0X2C, 0XE4, 0X24, 0X25, 0XE5, 0X27, 0XE7, 0XE6, 0X26,
0X22, 0XE2, 0XE3, 0X23, 0XE1, 0X21, 0X20, 0XE0, 0XA0, 0X60,
0X61, 0XA1, 0X63, 0XA3, 0XA2, 0X62, 0X66, 0XA6, 0XA7, 0X67,
0XA5, 0X65, 0X64, 0XA4, 0X6C, 0XAC, 0XAD, 0X6D, 0XAF, 0X6F,
0X6E, 0XAE, 0XAA, 0X6A, 0X6B, 0XAB, 0X69, 0XA9, 0XA8, 0X68,
0X78, 0XB8, 0XB9, 0X79, 0XBB, 0X7B, 0X7A, 0XBA, 0XBE, 0X7E,
0X7F, 0XBF, 0X7D, 0XBD, 0XBC, 0X7C, 0XB4, 0X74, 0X75, 0XB5,
0X77, 0XB7, 0XB6, 0X76, 0X72, 0XB2, 0XB3, 0X73, 0XB1, 0X71,
0X70, 0XB0, 0X50, 0X90, 0X91, 0X51, 0X93, 0X53, 0X52, 0X92,
0X96, 0X56, 0X57, 0X97, 0X55, 0X95, 0X94, 0X54, 0X9C, 0X5C,
0X5D, 0X9D, 0X5F, 0X9F, 0X9E, 0X5E, 0X5A, 0X9A, 0X9B, 0X5B,
0X99, 0X59, 0X58, 0X98, 0X88, 0X48, 0X49, 0X89, 0X4B, 0X8B,
0X8A, 0X4A, 0X4E, 0X8E, 0X8F, 0X4F, 0X8D, 0X4D, 0X4C, 0X8C,
0X44, 0X84, 0X85, 0X45, 0X87, 0X47, 0X46, 0X86, 0X82, 0X42,
0X43, 0X83, 0X41, 0X81, 0X80, 0X40 };

//********************************************************************************************************/
/**
* @brief  Function implementing crc8check.
* @param  pData 校验数据起始地址
* @param  Len 校验数据长度
* @retval 返回0XFF 校验成功，返回0x00 校验失败
*/
uint8_t VerifyCRC8Sub(uint8_t *pData, uint8_t Len)
{
  uint8_t  CRC_Data;
  uint8_t  TempData;
  uint8_t  Loop;

  CRC_Data=0X00;

  for(Loop=0X00;Loop<Len;Loop++)
  {

    TempData=CRC_Data^(*pData);
    CRC_Data=cCRC8Table[TempData];
    pData++;
  }

  if(CRC_Data!=*pData)
  {
    return 0X00;
  }
  else
  {
    return 0XFF;
  }
}



//********************************************************************************************************/
/**
* @brief  Function implementing crc8 set.
* @param  pData 校验数据起始地址
* @param  Len 校验数据长度
* @retval none 计算结果直接填在数据末位
*/
void SetCRC8Sub(uint8_t *Data, uint8_t Len)
{
  uint8_t  CRC_Data;
  uint8_t  TempData;
  uint8_t  Loop;

  CRC_Data=0X00;

  for(Loop=0X00;Loop<Len;Loop++)
  {

    TempData=CRC_Data^(*Data);
    CRC_Data=cCRC8Table[TempData];
    Data++;
  }

  *Data=CRC_Data;
//  qDebug("crcd=%02X",CRC_Data);
}



///****************************************************************************************/


  /**
  * @brief  Function implementing crc16 check.
  * @param  pData 校验数据起始地址
  * @param  Len 校验数据长度
  * @retval 返回0XFF 校验成功，返回0x00 校验失败
  */
uint8_t VerifyCRC16Sub(uint8_t *pData, uint16_t Len)
{
  uint8_t  CRCHi_Data;
  uint8_t  CRCLo_Data;
  uint8_t  Index;

  CRCHi_Data=0XFF;
  CRCLo_Data=0XFF;
  while(Len--)
  {
      Index = CRCLo_Data ^ *( pData++ );
      CRCLo_Data = (uint8_t)( CRCHi_Data ^ cCRC16HiTable[Index]);
      CRCHi_Data = cCRC16LoTable[Index];
  }

  if(CRCHi_Data!=*pData)  //根据数据存放格式要求，高字节在前，低字节在后
  {
    return 0X00;
  }
  pData++;
  if(CRCLo_Data!=*pData)
  {
    return 0X00;
  }
  else
  {
    return 0XFF;
  }
}

  /**
  * @brief  计算crc16 并存数据到最后两字节
  * @param  pData 校验数据起始地址
  * @param  Len 校验数据长度
  * @retval
  */
///****************************************************************************************/
void SetCRC16Sub(uint8_t *pData, uint16_t Len)
{
  uint8_t  CRCHi_Data;
  uint8_t  CRCLo_Data;
  uint8_t  Index;

  CRCHi_Data=0XFF;
  CRCLo_Data=0XFF;

  while(Len--)
  {
      Index = CRCLo_Data ^ *( pData++ );
      CRCLo_Data = (uint8_t)( CRCHi_Data ^ cCRC16HiTable[Index]);
      CRCHi_Data = cCRC16LoTable[Index];
  }

  *pData=CRCHi_Data;//根据数据存放格式要求，高字节在前，低字节在后
  pData++;
  *pData=CRCLo_Data;//;
  //    return ( USHORT )( ucCRCHi << 8 | ucCRCLo );//低字节在前
}


#if 1 //查表法

////////////////////////////////////////////////////////////////////////////////
//
//  CRC16码表 本文采用CRC-16/IBM: X16+X15+X2+1 多项式8005 初始值ffff
//
////////////////////////////////////////////////////////////////////////////////
static uint16_t const CRC16Table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};


/*******************************************************************************
*   函数名称: CRC16
*   功能描述: 查表计算CRC16.
*   输入参数: dataIn -- 待校验数据
*             length -- 数据长度
*   返回值: 校验值
*******************************************************************************/
uint16_t MyCRC16(uint8_t *dataIn, uint16_t length)
{
    uint16_t result  = 0xffff;  //初始值0xffff
    uint16_t tableNo = 0;

    for(int i = 0; i < length; i++)
    {
        tableNo = ((result & 0xff) ^ (dataIn[i] & 0xff));
        result  = ((result >> 8) & 0xff) ^ CRC16Table[tableNo];
    }

    return result;
}
#endif

#if 0   //0x8005
//crc16-多项式8005 algorithm
uint16_t CalCRC16(uint8_t Data, uint16_t CRCSeed)
{
    uint16_t i;
    uint16_t Check = 0;
    CRCSeed = CRCSeed ^ Data;
    for ( i = 0; i < 8; i++) {
        Check   = CRCSeed & 1;
        CRCSeed = CRCSeed >> 1;
        CRCSeed = CRCSeed & 0x7fff;

        if (Check == 1) {
            CRCSeed = CRCSeed ^ 0xa001;
        }
    }
    return CRCSeed;
}

uint16_t MyCRC16(uint8_t *pchMsg, uint16_t wDataLen)
{
    uint16_t seed=0xffff;
    for(uint16_t i=0;i<wDataLen;i++){
        seed=CalCRC16(pchMsg[i],seed);
    }
        return (seed);
}

#endif


#if 0 ////crc16-ccitt 多项式1021 初始值0000 查表法
static uint16_t const CRC16Table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
//crc16-ccitt 多项式1021 初始值0000
uint16_t MyCRC16(uint8_t *ptr,uint16_t len)
{
   unsigned short int crc;
   unsigned char da;
    crc=0;
   while(len--!=0)
    {
       da=(unsigned short)crc>>8;
        crc<<=8;
        crc^=CRC16Table[da^*ptr];
        ptr++;
      }
   return(crc);
}
#endif
