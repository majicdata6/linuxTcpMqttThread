
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

	char *strMqttData = NULL;

	//创建josn对象
	json = cJSON_CreateObject();
	//创建数组对象
	jsonData = cJSON_CreateObject();

	//添加设备指令 描述 到对象
	cJSON_AddStringToObject(json, MQTT_DEV_CMD_NAME, MQTT_DEV_CMD_CHSTATUS);

	//添加数据对象到对象
	cJSON_AddItemToObject(json, MQTT_DEV_DATA_NAME, jsonData);
	

	//添加数据到数据数组
	if (chx[0] != NULL)
	{
		cJSON_AddStringToObject(jsonData, MQTT_DEV_CH1_NAME, chx[0]);
	}
	if (chx[1] != NULL)
	{
		cJSON_AddStringToObject(jsonData, MQTT_DEV_CH2_NAME, chx[1]);
	}
	if (chx[2] != NULL)
	{
		cJSON_AddStringToObject(jsonData, MQTT_DEV_CH3_NAME, chx[2]);
	}
	if (chx[3] != NULL)
	{
		cJSON_AddStringToObject(jsonData, MQTT_DEV_CH4_NAME, chx[3]);
	}

	//json数据字符串化
	strMqttData = cJSON_Print(json);
	
	//数据赋值
	strcpy(pWriteBuf, strMqttData);

	//释放json资源
	cJSON_Delete(json);

	//返回数据长度
	return(strlen(pWriteBuf)+1);
}


//pDataStr			json字符串数据指针
//storDevNodeNum	解析数据存放地址节点1~n
//返回获取结果 成功1 失败0
uint8_t get4chMqttJsonData(char * pDataStr, int storDevNodeNum, char * pDevName)
{
	//数数据检测
	if ((storDevNodeNum < 1) || (storDevNodeNum > tDevTypeNodeTotal.dev4chCtrlTotal) || (pDevName == NULL))
	{
#ifdef DEBUG_EN
		printf("input_param-err\n");
#endif // DEBUG_EN
		//输入信息错误 返回
		return 0;
	}

	tDev_4channelCtl_Typedef *ptDevNode = (tDevTypeNodeTotal.ptDev4ChCtl + storDevNodeNum - 1);
	if (strcmp(ptDevNode->devName, pDevName) != 0)
	{
#ifdef DEBUG_EN
		printf("ptDevNode_err=%s\n", ptDevNode->devName);
#endif // DEBUG_EN

		//设备名不匹配
		return 0;
	}

#ifdef DEBUG_EN
	//打印要解析的数据
	printf("json-str= %s\n", pDataStr);
#endif

	//json数据解析
	cJSON *json_r = cJSON_Parse(pDataStr);
	if (json_r == NULL)
	{
#ifdef DEBUG_EN
		printf("json_r_err\n");
#endif // DEBUG_EN

		//失败返回
		return 0;
	}


	//获取 设备指令
	cJSON *json_devCmd = cJSON_GetObjectItemCaseSensitive(json_r, MQTT_DEV_CMD_NAME);
	if (json_devCmd == NULL)
	{

#ifdef DEBUG_EN
		printf("json_devCmd=null\n");
#endif // DEBUG_EN

		cJSON_Delete(json_r);

		return 0;

	}
	else
	{
#ifdef DEBUG_EN
		printf("cmdname= %s\n", json_devCmd->valuestring);
#endif // DEBUG_EN
	}


	if (cJSON_IsString(json_devCmd) && (json_devCmd->valuestring != NULL))
	{
#ifdef DEBUG_EN
		printf("devCmd=%s\n", json_devCmd->valuestring);
#endif // DEBUG_EN

	}
	else
	{

#ifdef DEBUG_EN
		printf("devCmd-err=%s\n", json_devCmd->valuestring);
#endif // DEBUG_EN

		cJSON_Delete(json_r);

		//失败返回
		return 0;
	}



	//指令-获取设置4路控制器通道状态
	if (strcmp(json_devCmd->valuestring, MQTT_DEV_CMD_CHSTATUS_SET) == 0)
	{
		//写使能判断
		if (ptDevNode->writeCmd == 0)
		{
#ifdef DEBUG_EN
			printf("writeCmd-DisEN\n");
#endif // DEBUG_EN
			//次指令没有使能
			return 0;
		}

		//设置操作通道 和 通道值
		uint8_t ch = 0;
		uint8_t chStatus = 0;




		//解析数据 
		cJSON *json_data = cJSON_GetObjectItem(json_r, MQTT_DEV_DATA_NAME);
		if (json_data != NULL)
		{
			cJSON *json_ch1 = cJSON_GetObjectItemCaseSensitive(json_data, MQTT_DEV_CH1_NAME);
			if (json_ch1 != NULL)
			{
#ifdef DEBUG_EN
				printf("ch1v=%s\n", json_ch1->valuestring);
#endif // DEBUG_EN

				ch |= 0x01;
				if (strcmp(json_ch1->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x01;
				}
			}
			cJSON *json_ch2 = cJSON_GetObjectItemCaseSensitive(json_data, MQTT_DEV_CH2_NAME);
			if (json_ch2 != NULL)
			{
#ifdef DEBUG_EN
				printf("ch2v=%s\n", json_ch2->valuestring);
#endif // DEBUG_EN

				ch |= 0x02;
				if (strcmp(json_ch2->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x02;
				}
			}
			cJSON *json_ch3 = cJSON_GetObjectItemCaseSensitive(json_data, MQTT_DEV_CH3_NAME);
			if (json_ch3 != NULL)
			{
#ifdef DEBUG_EN
				printf("ch3v=%s\n", json_ch3->valuestring);
#endif // DEBUG_EN

				ch |= 0x04;
				if (strcmp(json_ch3->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x04;
				}

			}
			cJSON *json_ch4 = cJSON_GetObjectItemCaseSensitive(json_data, MQTT_DEV_CH4_NAME);
			if (json_ch4 != NULL)
			{
#ifdef DEBUG_EN
				printf("ch4v=%s\n", json_ch4->valuestring);
#endif // DEBUG_EN

				ch |= 0x08;
				if (strcmp(json_ch4->valuestring, MQTT_DEV_CH_ON) == 0)
				{
					chStatus |= 0x08;
				}
			}

			//设置4路控制器节点通道值
			if (ptDevNode->devCmdFlag[DEV_CMD_4CH_ARRAY_WRITE_CHSTATUS_INDEX][FLAG_BUF_TCPSEND_COLUMN])
			{
				ptDevNode->tMqttSubData.ch |= ch;
				for (uint8_t i = 0; i < 4; i++)
				{
					if (ch & (0x01 << i))
					{
						if (chStatus & (0x01 << i))
						{
							ptDevNode->tMqttSubData.chStatus |= (0x01 << i);
						}
						else
						{
							ptDevNode->tMqttSubData.chStatus &= (0xff ^ (0x01 << i));
						}
					}
				}
			}
			else
			{
				ptDevNode->tMqttSubData.ch = ch;
				ptDevNode->tMqttSubData.chStatus = chStatus;

				//tcp写 下发标志位置位
				ptDevNode->devCmdFlag[DEV_CMD_4CH_ARRAY_WRITE_CHSTATUS_INDEX][FLAG_BUF_TCPSEND_COLUMN] = 1;
			}

#ifdef DEBUG_EN
			printf("ch=%d,status=%d\n", ptDevNode->tMqttSubData.ch, ptDevNode->tMqttSubData.chStatus);
#endif // DEBUG_EN

		}
	}
	else
	{
		//
#ifdef DEBUG_EN
		printf("json_devCmd != set_chStatus\n");
#endif // DEBUG_EN
	}

	cJSON_Delete(json_r);
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
