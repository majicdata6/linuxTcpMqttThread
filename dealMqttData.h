#ifndef DEAL_MQTT_DATA_H
#define DEAL_MQTT_DATA_H

#include <stdint.h>			//uint8_t
#include "tcmProData.h"
#include "readConfigCsv.h"
#include "cJSON.h"

#include "mqttDev4chCtrl.h"


//����mqtt�������ݰ�
//devType	�豸����
//pWriteBuf ��ʼ�����ݴ洢��ַ
//pTopic	��������洢��ַ ��6�� ¥/��/����/�豸��/�豸��/�豸�ڵ�ţ�
//pPublicTopic	�������⹫�����ⲿ�֣�ǰ���� ¥/��/���أ�
//���س�ʼ�������ֽڳ��� ��0����������0ʧ��
int mqttPubInit(uint8_t devType, uint8_t *pWriteBuf, char *pTopic, char *pPublicTopic);



//4·��������

//pTopic		����
//pData			���ĵ�����
//dataLen		���ݳ���
//���ض������ݽ������ 1�ɹ� 0ʧ��
uint8_t decodeMqttSub4ch(char *pTopic, char *pData, int dataLen);


//led��������

//�¿�����

//������

//���崫����

//��ʪ�ȴ�����

//���Ŵ�����

//���߰�������



#endif // !MQTT_DATA_DEAL_H

