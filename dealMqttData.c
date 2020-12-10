
#include <string.h>
#include <stdlib.h>		//�ַ���ת��ֵ int ������
#include <stdio.h>		//��׼�������

#include "dealMqttData.h"



//OFFSET:	0	/1	/...
//INFO:		����/¥��/����/�豸��/+�����֣�/+���ڵ�ţ�/+������ָ�read��write����
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

//�������л�ȡ�豸�ڵ��
//�����豸�ڵ�� ��Чֵ1~n, ��Чֵ0
static int getIntDevNodeFromMqttTopic(char *pTopic)
{
	char topicBuf[MAX_MQTT_TOPIC_LEN] = { 0 };
	char *pName = NULL, *pTopicStr = topicBuf;
	char delims[] = "/";

	int devNodeNum = 0;

	//��Ч���ж� ʧ��
	if (pTopic == NULL)
	{
		return 0;
	}

	//
	strcpy(topicBuf, pTopic);

	pName = strtok(pTopicStr, delims);

	//������ȡ�� ��ʣ�µ����ַ��� ֱ����ȡ���豸�ڵ��1~n
	for (uint8_t i = 1; (i < MQTT_DEV_NODE_NUM_OFFSET)&& (pName != NULL); i++)
	{
		pName = strtok(pTopicStr, NULL);
	}

	//��ȡ ʧ��
	if (pName == NULL)
	{
		return 0;
	}

	//�ַ���ת int ��Чת�� Ϊ0
	devNodeNum = atoi(pName);

	return devNodeNum;
}

//�������л�ȡ�豸��
//�����ַ����豸�� ��Ч����NULL
static char *getStrDevNameFromMqttTopic(char *pTopic)
{
	char topicBuf[MAX_MQTT_TOPIC_LEN] = { 0 };
	char *pName = NULL, *pTopicStr = topicBuf;
	char delims[] = "/";


	//��Ч���ж� ʧ��
	if (pTopic == NULL)
	{
		return NULL;
	}

	//
	strcpy(topicBuf, pTopic);

	pName = strtok(pTopicStr, delims);

	//������ȡ�� ��ʣ�µ����ַ��� ֱ����ȡ���豸�ڵ��1~n
	for (uint8_t i = 1; (i < MQTT_DEV_NODE_NAME_OFFSET) && (pName != NULL); i++)
	{
		pName = strtok(pTopicStr, NULL);
	}

	//�豸���ַ���ת ��Ч ΪNULL
	return pName;
}

//�������л�ȡ�������Ĳ���
//�����ַ������� ��Ч����NULL
static char *getStrDevOptFromMqttTopic(char *pTopic)
{
	char topicBuf[MAX_MQTT_TOPIC_LEN] = { 0 };
	char *pOperate = NULL, *pTopicStr = topicBuf;
	char delims[] = "/";


	//��Ч���ж� ʧ��
	if (pTopic == NULL)
	{
		return NULL;
	}

	//
	strcpy(topicBuf, pTopic);

	pOperate = strtok(pTopicStr, delims);

	//������ȡ�� ��ʣ�µ����ַ��� ֱ����ȡ���豸�ڵ��1~n
	for (uint8_t i = 1; (i < MQTT_DEV_NODE_OPERATE_OFFSET) && (pOperate != NULL); i++)
	{
		pOperate = strtok(pTopicStr, NULL);
	}

	//�豸���ַ���ת ��Ч ΪNULL
	return pOperate;
}

//devType	�豸����
//devNode	�豸�ڵ�� 1~n
//pTopicStor �豸���������ŵ�ַ
//���� �ɹ�����1��ʧ�ܷ���0
static uint8_t setPubTopic(uint8_t devType, int devNode, uint8_t *pTopicStor)
{
	char *strDevName = NULL;
	char *strDevNodeNum = NULL;

	//��4�� �豸����
	strcat(pTopicStor, "/");
	switch (devType)
	{
	case AGREEMENT_CMD_MID_MASTER_4CH:		//����-4·������
	{
		strcat(pTopicStor, DEV_TYPE_NAME_4CH_CTRL);

		//��5�� �豸��
		strDevName = getDevNodeName(AGREEMENT_CMD_MID_MASTER_4CH, devNode);
		if (strDevName == NULL)
		{
			//��ȡ�豸��ʧ��
			return 0;
		}
		strcat(pTopicStor, "/");
		strcat(pTopicStor, strDevName);

		//��6�� �豸�ڵ��
		strDevNodeNum = getDevStorNodeNum(AGREEMENT_CMD_MID_MASTER_4CH, devNode);

		//�ж�
		if (strDevNodeNum == NULL)
		{
			return 0;
		}

		if (devNode != atoi(strDevNodeNum))
		{
			//�豸�ڵ���洢�ڵ�Ų�ͬ ʧ��
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
	case AGREEMENT_CMD_MID_MASTER_TEMP:		//����-�¿���
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_LED:		//����-led����
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_CURTAIN:	//����-����������
	{

	}
	break;

	default:
		break;
	}
	return 0;
}



//4·��������
//pTopic		����
//pData			���ĵ�����
//dataLen		���ݳ���
//���ض������ݽ������ 1�ɹ� 0ʧ��
uint8_t decodeMqttSub4ch(char *pTopic, char *pData, int dataLen)
{
	int devNodeNum = 0;
	char devDataBuf[MAX_MQTT_DATA_LEN] = { 0 };
	char *pDevName = NULL;

	//���ݿ���
	memcpy(devDataBuf, pData, dataLen);

	//��ȡ�ڵ��
	devNodeNum = getIntDevNodeFromMqttTopic(pTopic);
	if (0 == devNodeNum)
	{
		//��ȡ�ڵ���쳣
		return 0;
	}

	//��ȡ�豸��
	pDevName = getStrDevNameFromMqttTopic(pTopic);
	if (NULL == pDevName)
	{
		//��ȡ�豸��ʧ��
		return 0;
	}

	//��ȡ����
	return (get4chMqttJsonData(devDataBuf, devNodeNum, pDevName));

}



//����mqtt�������ݰ�
//devType	�豸����
//pWriteBuf ��ʼ�����ݴ洢��ַ
//pTopic	��������洢��ַ ��6�� ¥/��/����/�豸��/�豸��/�豸�ڵ�ţ�
//pPublicTopic	�������⹫�����ⲿ�֣�ǰ���� ¥/��/���أ�
//���س�ʼ�������ֽڳ��� ��0����������0ʧ��
int mqttPubInit(uint8_t devType, uint8_t *pWriteBuf, char *pTopic, char *pPublicTopic)
{
	uint8_t cmd = 0;
	int devNode = 0;
	int dataLen = 0;

	switch (devType)
	{
		case AGREEMENT_CMD_MID_MASTER_4CH:
		{
			//��ȡ�ڵ� ��ָ��
			devNode = getDev4ChCtrlCmdFromMqtt(&cmd);
			if (0 == devNode)
			{
				return 0;
			}

			//��ʼ������
			dataLen = encode4chMqttPubData(pWriteBuf, devNode, cmd);
			if (dataLen == 0)
			{
				return 0;
			}

			//���������װ ǰ3��Ϊ�������֣�����/¥��/���أ� 
			if (pPublicTopic != NULL)
			{
				strcpy(pTopic, pPublicTopic);
			}
			//�޸Ĵ˲��ֺ�3������豸��ֵ��/�豸����/�豸��/�豸�ڵ㣩
			setPubTopic(AGREEMENT_CMD_MID_MASTER_4CH, devNode, pTopic);

			//�������ݳ���
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