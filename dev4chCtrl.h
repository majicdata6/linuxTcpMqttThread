
#ifndef DEV_4CH_CTRL_H
#define DEV_4CH_CTRL_H


#include <stdint.h>
#include "tcmProData.h"
#include "readConfigCsv.h"







//外部使用函数声明

//成功返回 1 失败返回0
//pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1  0-关 1-开
uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData);


//pStorData 指向存储区，确保缓存大小足够
//pDevId	设备ID
//ch		控制的通道	低四位对应通道4~1，1有效
//status	对应的通道状态 低四位对应通道4~1状态，0-关，1-开
//返回数据长度  成功返回非0数据长度 失败返回0
int set4chCtrlData_chStatus(uint8_t *pStorData, uint8_t *pDevId, uint8_t ch, uint8_t status);

#endif // 
