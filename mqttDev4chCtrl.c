
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

//4·�������������ݳ�ʼ��
//pWriteBuf	�������ݴ�ŵ�ַ
//devNode	�豸�ڵ�
//���ر��������ֽڳ��� ��0����������0ʧ��
static int setPub4chCtrlData_chStatus(uint8_t *pWriteBuf, uint8_t ch, uint8_t chStatus)
{
	char *chx[4] = {NULL,NULL,NULL,NULL};
	//����ת���ַ��� 
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

	//���� json��ʽ�� 
	cJSON *json = NULL;
	cJSON *jsonData = NULL;

	char *strMqttData = NULL;

	//����josn����
	json = cJSON_CreateObject();
	//�����������
	jsonData = cJSON_CreateObject();

	//����豸ָ�� ���� ������
	cJSON_AddStringToObject(json, MQTT_DEV_CMD_NAME, MQTT_DEV_CMD_CHSTATUS);

	//������ݶ��󵽶���
	cJSON_AddItemToObject(json, MQTT_DEV_DATA_NAME, jsonData);
	

	//������ݵ���������
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

	//json�����ַ�����
	strMqttData = cJSON_Print(json);
	
	//���ݸ�ֵ
	strcpy(pWriteBuf, strMqttData);

	//�ͷ�json��Դ
	cJSON_Delete(json);

	//�������ݳ���
	return(strlen(pWriteBuf)+1);
}


//pDataStr			json�ַ�������ָ��
//storDevNodeNum	�������ݴ�ŵ�ַ�ڵ�1~n
//���ػ�ȡ��� �ɹ�1 ʧ��0
uint8_t get4chMqttJsonData(char * pDataStr, int storDevNodeNum, char * pDevName)
{
	//�����ݼ��
	if ((storDevNodeNum < 1) || (storDevNodeNum > tDevTypeNodeTotal.dev4chCtrlTotal) || (pDevName == NULL))
	{
#ifdef DEBUG_EN
		printf("input_param-err\n");
#endif // DEBUG_EN
		//������Ϣ���� ����
		return 0;
	}

	tDev_4channelCtl_Typedef *ptDevNode = (tDevTypeNodeTotal.ptDev4ChCtl + storDevNodeNum - 1);
	if (strcmp(ptDevNode->devName, pDevName) != 0)
	{
#ifdef DEBUG_EN
		printf("ptDevNode_err=%s\n", ptDevNode->devName);
#endif // DEBUG_EN

		//�豸����ƥ��
		return 0;
	}

#ifdef DEBUG_EN
	//��ӡҪ����������
	printf("json-str= %s\n", pDataStr);
#endif

	//json���ݽ���
	cJSON *json_r = cJSON_Parse(pDataStr);
	if (json_r == NULL)
	{
#ifdef DEBUG_EN
		printf("json_r_err\n");
#endif // DEBUG_EN

		//ʧ�ܷ���
		return 0;
	}


	//��ȡ �豸ָ��
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

		//ʧ�ܷ���
		return 0;
	}



	//ָ��-��ȡ����4·������ͨ��״̬
	if (strcmp(json_devCmd->valuestring, MQTT_DEV_CMD_CHSTATUS_SET) == 0)
	{
		//дʹ���ж�
		if (ptDevNode->writeCmd == 0)
		{
#ifdef DEBUG_EN
			printf("writeCmd-DisEN\n");
#endif // DEBUG_EN
			//��ָ��û��ʹ��
			return 0;
		}

		//���ò���ͨ�� �� ͨ��ֵ
		uint8_t ch = 0;
		uint8_t chStatus = 0;




		//�������� 
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

			//����4·�������ڵ�ͨ��ֵ
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

				//tcpд �·���־λ��λ
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

//�����������ݰ�
//pWriteBuf	�������ݴ�ŵ�ַ
//devNode	�豸�ڵ�
//cmd		����ָ��
//���ر��������ֽڳ��� ��0����������0ʧ��
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
		//��ʼ��mqtt pub 4ͨ������
		dataLen = setPub4chCtrlData_chStatus(pWriteBuf, (ptDev4chNode + devNode - 1)->tTcpReadData.ch, (ptDev4chNode + devNode - 1)->tTcpReadData.chStatus);

		//��λmqtt������־λ
		if (dataLen)
		{
			//��λ tcp д ��־λ
			ptDev4chNode->devCmdFlag[DEV_CMD_4CH_ARRAY_READ_CHSTATUS_INDEX][FLAG_BUF_MQTTPUB_COLUMN] = 0;
		}
		return dataLen;

	}
	//break;
	case AGREEMENT_CMD_SALVE_4L_SET_STATUS:
	{
		//��ʼ��mqtt pub 4ͨ������
		dataLen = setPub4chCtrlData_chStatus(pWriteBuf, (ptDev4chNode + devNode - 1)->tTcpReadData.ch, (ptDev4chNode + devNode - 1)->tTcpReadData.chStatus);

		//��λmqtt������־λ
		if (dataLen)
		{
			//��λ tcp д ��־λ
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
