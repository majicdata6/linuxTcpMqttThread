
#ifndef DEV_4CH_CTRL_H
#define DEV_4CH_CTRL_H


#include <stdint.h>

#include "tcmProData.h"
#include "readConfigCsv.h"







//外部使用函数声明

//成功返回 1 失败返回0
//pTcpData		tcp原数据
//pStorCh		操作的通道 低4位 高有效， 
//pStorData		通道状态 低4位 通道4~通道1 对应bit3~0 ，1=开，0=关
uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorCh, uint8_t *pStorData);


//pStorData 指向存储区，确保缓存大小足够
//pDevId	设备ID
//ch		控制的通道	低四位对应通道4~1，1有效
//status	对应的通道状态 低四位对应通道4~1状态，0-关，1-开
//返回数据长度  成功返回非0数据长度 失败返回0
int set4chCtrlData_chStatus(uint8_t *pStorData, uint8_t *pDevId, uint8_t ch, uint8_t status);

#endif // 
