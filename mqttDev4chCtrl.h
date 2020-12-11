#ifndef MQTT_DEV_4CH_CTRL_H
#define MQTT_DEV_4CH_CTRL_H

#include <stdint.h>
#include "readConfigCsv.h"
#include "tcmProData.h"




//外部函数声明


//pDataStr			json字符串数据指针
//storDevNodeNum	解析数据存放地址节点1~n
//返回获取结果 成功1 失败0
uint8_t get4chMqttJsonData(char * pDataStr, int storDevNodeNum, char * pDevName);

//编码网络数据包
//pWriteBuf	编码数据存放地址
//devNode	设备节点
//cmd		编码指令
//返回编码数据字节长度 非0正常，返回0失败
int encode4chMqttPubData(uint8_t *pWriteBuf, int devNode, uint8_t cmd);


#endif // !MQTT_DEV_4CH_CTRL_H

