
#ifndef DEAL_TCP_DATA_H
#define DEAL_TCP_DATA_H

#include <stdio.h>
#include <stdint.h>

#include "readConfigCsv.h"
#include "dev4chCtrl.h"





////获取通道状态 成功返回 1 失败返回0
////pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1
//uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData);

//解析网络数据包
//返回1正常，返回0失败
uint8_t decodeTcpData(uint8_t *pTcpData, int dataLen);

//编码网络数据包
//devType	设备类型
//pWriteBuf 初始化数据存储地址
//返回初始化数据字节长度 非0正常，返回0失败
int tcpWriteInit(uint8_t devType, uint8_t *pWriteBuf);





#endif