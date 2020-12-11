#ifndef MQTT_DEV_4CH_CTRL_H
#define MQTT_DEV_4CH_CTRL_H

#include <stdint.h>
#include "readConfigCsv.h"
#include "tcmProData.h"




//�ⲿ��������


//pDataStr			json�ַ�������ָ��
//storDevNodeNum	�������ݴ�ŵ�ַ�ڵ�1~n
//���ػ�ȡ��� �ɹ�1 ʧ��0
uint8_t get4chMqttJsonData(char * pDataStr, int storDevNodeNum, char * pDevName);

//�����������ݰ�
//pWriteBuf	�������ݴ�ŵ�ַ
//devNode	�豸�ڵ�
//cmd		����ָ��
//���ر��������ֽڳ��� ��0����������0ʧ��
int encode4chMqttPubData(uint8_t *pWriteBuf, int devNode, uint8_t cmd);


#endif // !MQTT_DEV_4CH_CTRL_H

