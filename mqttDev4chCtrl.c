
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "mqttDev4chCtrl.h"
#include "cJSON.h"

#define	MQTT_DEV_CMD_NAME					"mqttDevCmd"
#define MQTT_DEV_DATA_NAME					"data"

#define MQTT_DEV_CH1_NAME					"ch1"
#define MQTT_DEV_CH2_NAME					"ch2"
#define MQTT_DEV_CH3_NAME					"ch3"
#define MQTT_DEV_CH4_NAME					"ch4"

#define MQTT_DEV_CMD_CHSTATUS				"chStatus"
#define MQTT_DEV_CMD_CHSTATUS_SET			"set_chStatus"

#define MQTT_DEV_CH_ON						"ON"
#define MQTT_DEV_CH_OFF						"OFF"


#define DEBUG_EN

//4路控制器发布数据初始化
//pWriteBuf	编码数据存放地址
//devNode	设备节点
//返回编码数据字节长度 非0正常，返回0失败
static int setPub4chCtrlData_chStatus(uint8_t *pWriteBuf, uint8_t ch, uint8_t chStatus)
{
	char *chx[4] = {NULL,NULL,NULL,NULL};
	//数据转换字符串 
	for (uint8_t i = 0; i < 4; i++)
	{
		if (ch & (0x01 << i))
		{
			if (chStatus & (0x01 << i))
			{
				chx[i] = MQTT_DEV_CH_ON;
			}
			else
			{
				chx[i] = MQTT_DEV_CH_OFF;
			}
		}
	}

	//数据 json格式化 
	cJSON *json = NULL;
	cJSON *jsonData = NULL;
	cJSON *jsonCh1 = NULL;
	cJSON *jsonCh2 = NULL;
	cJSON *jsonCh3 = NULL;
	cJSON *jsonCh4 = NULL;

	char *strMqttData = NULL;

	//创建josn对象
	json = cJSON_CreateObject();
	//创建数组对象
	jsonData = cJSON_CreateArray();

	//添加设备指令 描述
	cJSON_AddStringToObject(json, MQTT_DEV_CMD_NAME, MQTT_DEV_CMD_CHSTATUS);

	//添加数据数组到对象
	cJSON_AddItemToObject(json, MQTT_DEV_DATA_NAME, jsonData);

	//添加数据到数据数组
	if (chx[0] != NULL)
	{
		jsonCh1 = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonCh1, MQTT_DEV_CH1_NAME, chx[0]);
		cJSON_AddItemToArray(jsonData, jsonCh1);
	}
	if (chx[1] != NULL)
	{
		jsonCh2 = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonCh2, MQTT_DEV_CH2_NAME, chx[1]);
		cJSON_AddItemToArray(jsonData, jsonCh2);
	}
	if (chx[2] != NULL)
	{
		jsonCh3 = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonCh3, MQTT_DEV_CH3_NAME, chx[2]);
		cJSON_AddItemToArray(jsonData, jsonCh3);
	}
	if (chx[3] != NULL)
	{
		jsonCh4 = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonCh4, MQTT_DEV_CH4_NAME, chx[3]);
		cJSON_AddItemToArray(jsonData, jsonCh4);
	}

	//json数据字符串化
	strMqttData = cJSON_Print(json);
	
	//数据赋值
	strcpy(pWriteBuf, strMqttData);

	//释放json资源
	cJSON_Delete(json);

	//返回数据长度
	return(strlen(pWriteBuf));
}




//pDataStr			json字符串数据指针
//storDevNodeNum	解析数据存放地址节点1~n
//返回获取结果 成功1 失败0
uint8_t get4chMqttJsonData(char * pDataStr, int storDevNodeNum, char * pDevName)
{
	//数数据检测
	if ((storDevNodeNum < 1) || (storDevNodeNum > tDevTypeNodeTotal.dev4chCtrlTotal) || (pDevName == NULL))
	{
		//输入信息错误 返回
		return 0;
	}

	tDev_4channelCtl_Typedef *ptDevNode = (tDevTypeNodeTotal.ptDev4ChCtl + storDevNodeNum - 1);
	if (strcmp(ptDevNode->devName, pDevName) != 0)
	{
		//设备名不匹配
		return 0;
	}

	//json数据解析
	cJSON *json_r = cJSON_Parse(pDataStr);
	if (json_r == NULL)
	{
		//失败返回
		return 0;
	}

	//获取 设备指令
	cJSON *json_devCmd = cJSON_GetObjectItemCaseSensitive(json_r, MQTT_DEV_CMD_NAME);
	if (cJSON_IsString(json_devCmd) && (json_devCmd->valuestring != NULL))
	{
#ifdef DEBUG_EN
		printf("devCmd=%s\n", json_devCmd->valuestring);
#endif // DEBUG_EN

	}
	else
	{
		//失败返回
		return 0;
	}

	//指令-获取设置4路控制器通道状态
	if (strcmp(json_devCmd->valuestring, MQTT_DEV_CMD_CHSTATUS_SET) == 0)
	{
		//写使能判断
		if (ptDevNode->writeCmd == 0)
		{
			//次指令没有使能
			return 1;
		}

		//解析数据 它是一个数组
		cJSON *json_data = cJSON_GetObjectItemCaseSensitive(json_r, MQTT_DEV_DATA_NAME);
		cJSON *json_dataValue = NULL;

		//遍历数组
		cJSON_ArrayForEach(json_dataValue, json_data)
		{
			cJSON *json_ch1 = cJSON_GetObjectItemCaseSensitive(json_dataValue, MQTT_DEV_CH1_NAME);
			cJSON *json_ch2 = cJSON_GetObjectItemCaseSensitive(json_dataValue, MQTT_DEV_CH2_NAME);
			cJSON *json_ch3 = cJSON_GetObjectItemCaseSensitive(json_dataValue, MQTT_DEV_CH3_NAME);
			cJSON *json_ch4 = cJSON_GetObjectItemCaseSensitive(json_dataValue, MQTT_DEV_CH4_NAME);

			//设置操作通道 和 通道值

			uint8_t ch = 0;
			uint8_t chStatus = 0;
			if (json_ch1 != NULL)
			{
				ch |= 0x01;
				if (strcmp(json_ch1->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x01;
				}
			}

			if (json_ch2 != NULL)
			{
				ch |= 0x01 << 1;
				if (strcmp(json_ch2->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x01 << 1;
				}
			}

			if (json_ch3 != NULL)
			{
				ch |= 0x01 << 2;
				if (strcmp(json_ch3->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x01 << 2;
				}
			}

			if (json_ch4 != NULL)
			{
				ch |= 0x01 << 3;
				if (strcmp(json_ch4->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x01 << 3;
				}
			}
			//设置4路控制器节点通道值
			if (ptDevNode->devCmdFlag[DEV_CMD_4CH_ARRAY_WRITE_CHSTATUS_INDEX][FLAG_BUF_TCPSEND_COLUMN])
			{
				ptDevNode->tMqttSubData.ch |= ch;
				ptDevNode->tMqttSubData.chStatus |= chStatus;
			}
			else
			{
				ptDevNode->tMqttSubData.ch = ch;
				ptDevNode->tMqttSubData.chStatus = chStatus;
			}

			//tcp写 下发标志位置位
			ptDevNode->devCmdFlag[DEV_CMD_4CH_ARRAY_WRITE_CHSTATUS_INDEX][FLAG_BUF_TCPSEND_COLUMN] = 1;
		}

	}

	//指令-获取
	//指令-获取

	return 1;
}

//编码网络数据包
//pWriteBuf	编码数据存放地址
//devNode	设备节点
//cmd		编码指令
//返回编码数据字节长度 非0正常，返回0失败
int encode4chMqttPubData(uint8_t *pWriteBuf, int devNode, uint8_t cmd)
{
	tDev_4channelCtl_Typedef *ptDev4chNode = tDevTypeNodeTotal.ptDev4ChCtl;
	int dataLen = 0;
	if (devNode < 1)
	{
		return 0;
	}


	switch (cmd)
	{
	case AGREEMENT_CMD_SALVE_4L_READ_STATUS:
	{
		//初始化mqtt pub 4通道数据
		dataLen = setPub4chCtrlData_chStatus(pWriteBuf, (ptDev4chNode + devNode - 1)->tTcpReadData.ch, (ptDev4chNode + devNode - 1)->tTcpReadData.chStatus);

		//复位mqtt发布标志位
		if (dataLen)
		{
			//复位 tcp 写 标志位
			ptDev4chNode->devCmdFlag[DEV_CMD_4CH_ARRAY_READ_CHSTATUS_INDEX][FLAG_BUF_MQTTPUB_COLUMN] = 0;
		}
		return dataLen;

	}
	//break;
	case AGREEMENT_CMD_SALVE_4L_SET_STATUS:
	{
		//初始化mqtt pub 4通道数据
		dataLen = setPub4chCtrlData_chStatus(pWriteBuf, (ptDev4chNode + devNode - 1)->tTcpReadData.ch, (ptDev4chNode + devNode - 1)->tTcpReadData.chStatus);

		//复位mqtt发布标志位
		if (dataLen)
		{
			//复位 tcp 写 标志位
			ptDev4chNode->devCmdFlag[DEV_CMD_4CH_ARRAY_WRITE_CHSTATUS_INDEX][FLAG_BUF_MQTTPUB_COLUMN] = 0;
		}
		return dataLen;
	}
	//break;

	default:
		return 0;
		//break;
	}

}
