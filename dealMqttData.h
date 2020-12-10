#ifndef DEAL_MQTT_DATA_H
#define DEAL_MQTT_DATA_H

#include <stdint.h>			//uint8_t
#include "tcmProData.h"
#include "readConfigCsv.h"
#include "cJSON.h"

#include "mqttDev4chCtrl.h"


//编码mqtt网络数据包
//devType	设备类型
//pWriteBuf 初始化数据存储地址
//pTopic	发布主题存储地址 （6层 楼/层/网关/设备类/设备名/设备节点号）
//pPublicTopic	发布主题公共主题部分（前三层 楼/层/网关）
//返回初始化数据字节长度 非0正常，返回0失败
int mqttPubInit(uint8_t devType, uint8_t *pWriteBuf, char *pTopic, char *pPublicTopic);



//4路控制器类

//pTopic		主题
//pData			订阅的数据
//dataLen		数据长度
//返回订阅数据解析结果 1成功 0失败
uint8_t decodeMqttSub4ch(char *pTopic, char *pData, int dataLen);


//led控制器类

//温控器类

//窗帘类

//人体传感器

//温湿度传感器

//窗磁传感器

//无线按键开关



#endif // !MQTT_DATA_DEAL_H

