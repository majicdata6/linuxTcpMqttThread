
#include <string.h>
#include <stdlib.h>		//字符串转数值 int 。。。
#include <stdio.h>		//标准输入输出

#include "dealMqttData.h"



//OFFSET:	0	/1	/...
//INFO:		建筑/楼层/网关/设备类/+（名字）/+（节点号）/+（操作指令（read、write））
#define MQTT_DEV_NODE_NAME_OFFSET			4
#define MQTT_DEV_NODE_NUM_OFFSET			5
#define MQTT_DEV_NODE_OPERATE_OFFSET		6

#define MAX_MQTT_TOPIC_LEN					256
#define MAX_MQTT_DATA_LEN					512

//#define MQTT_DEV_CMD_NAME					"mqttDevCmd"
//#define MQTT_DEV_DATA_NAME					"data"

//#define MQTT_4CH_DEV_CMD_NAME_PUB_CHSTATUS	"chStatus"
//#define MQTT_4CH_DEV_CMD_NAME_SET_CHSTATUS	"set_chStatus"
//#define MQTT_4CH_DEV_CH1_NAME				"ch1"
//#define MQTT_4CH_DEV_CH2_NAME				"ch2"
//#define MQTT_4CH_DEV_CH3_NAME				"ch3"
//#define MQTT_4CH_DEV_CH4_NAME				"ch4"
//#define MQTT_4CH_STATUS_VALUE_ON			"ON"
//#define MQTT_4CH_STATUS_VALUE_OFF			"OFF"



#define DEBUG_EN

//从主题中获取设备节点号
//返回设备节点号 有效值1~n, 无效值0
static int getIntDevNodeFromMqttTopic(char *pTopic)
{
	char topicBuf[MAX_MQTT_TOPIC_LEN] = { 0 };
	char *pName = NULL, *pTopicStr = topicBuf;
	char delims[] = "/";

	int devNodeNum = 0;

	//有效性判断 失败
	if (pTopic == NULL)
	{
		return 0;
	}

	//
	strcpy(topicBuf, pTopic);

	pName = strtok(pTopicStr, delims);

	//继续读取行 里剩下的列字符串 直到获取到设备节点号1~n
	for (uint8_t i = 1; (i < MQTT_DEV_NODE_NUM_OFFSET)&& (pName != NULL); i++)
	{
		pName = strtok(pTopicStr, NULL);
	}

	//获取 失败
	if (pName == NULL)
	{
		return 0;
	}

	//字符串转 int 无效转换 为0
	devNodeNum = atoi(pName);

	return devNodeNum;
}

//从主题中获取设备名
//返回字符串设备名 无效返回NULL
static char *getStrDevNameFromMqttTopic(char *pTopic)
{
	char topicBuf[MAX_MQTT_TOPIC_LEN] = { 0 };
	char *pName = NULL, *pTopicStr = topicBuf;
	char delims[] = "/";


	//有效性判断 失败
	if (pTopic == NULL)
	{
		return NULL;
	}

	//
	strcpy(topicBuf, pTopic);

	pName = strtok(pTopicStr, delims);

	//继续读取行 里剩下的列字符串 直到获取到设备节点号1~n
	for (uint8_t i = 1; (i < MQTT_DEV_NODE_NAME_OFFSET) && (pName != NULL); i++)
	{
		pName = strtok(pTopicStr, NULL);
	}

	//设备名字符串转 无效 为NULL
	return pName;
}

//从主题中获取操作订阅操作
//返回字符串操作 无效返回NULL
static char *getStrDevOptFromMqttTopic(char *pTopic)
{
	char topicBuf[MAX_MQTT_TOPIC_LEN] = { 0 };
	char *pOperate = NULL, *pTopicStr = topicBuf;
	char delims[] = "/";


	//有效性判断 失败
	if (pTopic == NULL)
	{
		return NULL;
	}

	//
	strcpy(topicBuf, pTopic);

	pOperate = strtok(pTopicStr, delims);

	//继续读取行 里剩下的列字符串 直到获取到设备节点号1~n
	for (uint8_t i = 1; (i < MQTT_DEV_NODE_OPERATE_OFFSET) && (pOperate != NULL); i++)
	{
		pOperate = strtok(pTopicStr, NULL);
	}

	//设备名字符串转 无效 为NULL
	return pOperate;
}

//devType	设备类型
//devNode	设备节点号 1~n
//pTopicStor 设备发布主题存放地址
//返回 成功返回1，失败返回0
static uint8_t setPubTopic(uint8_t devType, int devNode, uint8_t *pTopicStor)
{
	char *strDevName = NULL;
	char *strDevNodeNum = NULL;

	//第4层 设备类型
	strcat(pTopicStor, "/");
	switch (devType)
	{
	case AGREEMENT_CMD_MID_MASTER_4CH:		//类型-4路控制器
	{
		strcat(pTopicStor, DEV_TYPE_NAME_4CH_CTRL);

		//第5层 设备名
		strDevName = getDevNodeName(AGREEMENT_CMD_MID_MASTER_4CH, devNode);
		if (strDevName == NULL)
		{
			//获取设备名失败
			return 0;
		}
		strcat(pTopicStor, "/");
		strcat(pTopicStor, strDevName);

		//第6层 设备节点号
		strDevNodeNum = getDevStorNodeNum(AGREEMENT_CMD_MID_MASTER_4CH, devNode);

		//判断
		if (strDevNodeNum == NULL)
		{
			return 0;
		}

		if (devNode != atoi(strDevNodeNum))
		{
			//设备节点与存储节点号不同 失败
#ifdef DEBUG_EN
			printf("devNode!=strDevNode");
#endif // DEBUG_EN
			return 0;
		}

		strcat(pTopicStor, "/");
		strcat(pTopicStor, strDevNodeNum);

		return 1;
	}
	//break;
	case AGREEMENT_CMD_MID_MASTER_TEMP:		//类型-温控器
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_LED:		//类型-led调光
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_CURTAIN:	//类型-窗帘控制器
	{

	}
	break;

	default:
		break;
	}
	return 0;
}



//4路控制器类
//pTopic		主题
//pData			订阅的数据
//dataLen		数据长度
//返回订阅数据解析结果 1成功 0失败
uint8_t decodeMqttSub4ch(char *pTopic, char *pData, int dataLen)
{
	int devNodeNum = 0;
	char devDataBuf[MAX_MQTT_DATA_LEN] = { 0 };
	char *pDevName = NULL;

	//数据拷贝
	memcpy(devDataBuf, pData, dataLen);

	//获取节点号
	devNodeNum = getIntDevNodeFromMqttTopic(pTopic);
	if (0 == devNodeNum)
	{
		//获取节点号异常
		return 0;
	}

	//获取设备名
	pDevName = getStrDevNameFromMqttTopic(pTopic);
	if (NULL == pDevName)
	{
		//获取设备名失败
		return 0;
	}

	//获取数据
	return (get4chMqttJsonData(devDataBuf, devNodeNum, pDevName));

}



//编码mqtt网络数据包
//devType	设备类型
//pWriteBuf 初始化数据存储地址
//pTopic	发布主题存储地址 （6层 楼/层/网关/设备类/设备名/设备节点号）
//pPublicTopic	发布主题公共主题部分（前三层 楼/层/网关）
//返回初始化数据字节长度 非0正常，返回0失败
int mqttPubInit(uint8_t devType, uint8_t *pWriteBuf, char *pTopic, char *pPublicTopic)
{
	uint8_t cmd = 0;
	int devNode = 0;
	int dataLen = 0;

	switch (devType)
	{
		case AGREEMENT_CMD_MID_MASTER_4CH:
		{
			//获取节点 和指令
			devNode = getDev4ChCtrlCmdFromMqtt(&cmd);
			if (0 == devNode)
			{
				return 0;
			}

			//初始化数据
			dataLen = encode4chMqttPubData(pWriteBuf, devNode, cmd);
			if (dataLen == 0)
			{
				return 0;
			}

			//发布主题封装 前3层为公共部分（建筑/楼层/网关） 
			if (pPublicTopic != NULL)
			{
				strcpy(pTopic, pPublicTopic);
			}
			//修改此部分后3层根据设备赋值（/设备类型/设备名/设备节点）
			setPubTopic(AGREEMENT_CMD_MID_MASTER_4CH, devNode, pTopic);

			//反回数据长度
			return dataLen;
		}
		//break;
		case AGREEMENT_CMD_MID_MASTER_TEMP:
		{

		}
		//break;
		case AGREEMENT_CMD_MID_MASTER_LED:
		{

		}
		//break;
		case AGREEMENT_CMD_MID_MASTER_CURTAIN:
		{

		}
		//break;
		case AGREEMENT_CMD_RORG_SENSOR:
		{

		}
		//break;
		case AGREEMENT_CMD_RORG_KEY:
		{

		}
		//break;
		default:
			return 0;
			//break;
	}
}