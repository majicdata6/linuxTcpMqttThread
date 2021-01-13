#ifndef TCP_MQTT_THREAD_H
#define TCP_MQTT_THREAD_H

#include <pthread.h>	//�߳�
#include <stdio.h>		//��׼�������
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		//sleep��ʱ����
#include <semaphore.h>	//�ź���
#include "MQTTClient.h"
#if !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "readConfigCsv.h"

#include "dealTcpData.h"
#include "dealMqttData.h"



//macro definition 

#define NUM_THREADS	    				6
#define TCP_RECIVE_BUF_LEN				1024
#define TCP_WRITE_BUF_LEN				1024
#define MQTT_PUB_BUF_LEN				128
#define MQTT_PUB_TOPIC_LEN				128
#define MQTT_SERVER_ADDS_LEN			25
#define MQTT_CLIENT_ID_LEN				25
#define MQTT_CLIENT_USER_NAME_LEN		25
#define MQTT_CLIENT_PASSWORD_LEN		25
#define MQTT_CLIENT_SUB_TOPIC_HEAD_LEN	64
 

#define TCP_CONFIG_IP_NAME				"tcpclient_ip"
#define TCP_CONFIG_PORT_NAME			"tcpclient_port"

#define MQTT_CONFIG_ADDRESS_NAME		"mqtt_address"
#define MQTT_CONFIG_CLIENT_ID_NAME		"mqtt_clientid"
#define MQTT_CONFIG_SUB_CLIENT_ID_NAME	"mqtt_sub_clientid"
#define MQTT_CONFIG_TIMEOUT_NAME		"mqtt_timeout"
#define MQTT_CONFIG_USENAME_NAME		"mqtt_username"
#define MQTT_CONFIG_PASSWORT_NAME		"mqtt_password"
#define MQTT_CONFIG_TOPIC_NAME			"mqtt_topic"



//The macro structure

typedef struct 
{
	int tcp_send_flag ;

	char *pData;
	int dataLen;

}tTcpMessageDealTypedef;

typedef struct 
{
	int mqtt_push_flag;
	int mqtt_read_flag;
	int mqtt_writ_flag;

	char *pMqttTopic;
	int mqttTopicLen;
	char *pMqttMessgae;
	int mqttMessageLen;

}tMqttMessageDealTypedef;

////tcp �ͻ������ӷ�������ַ�Ͷ˿�
//char ip_buf[16] = { 0 };
//
//char *server_ip_addr = ip_buf;
//int server_ip_port = 10086;     //Ĭ��ֵ
//
//								//mqtt �ͻ�����������
//char mqtt_server_address[25] = { 0 };			//mqtt ��������ַ
//char mqtt_pub_client_id[25] = { 0 };			//���Ĵ˴��ͻ���ID
//char mqtt_sub_client_id[25] = { 0 };			//���Ĵ˴��ͻ���ID
//int mqtt_pub_qos = 1;
//int mqtt_sub_qos = 1;
//int mqtt_timeout = 1000;
//char mqtt_client_username[25] = { 0 };
//char mqtt_client_password[25] = { 0 };
//
//char mqtt_pub_client_topic[256] = { 0 };

typedef struct
{
	char tcpServerIp[16];						//tcp ������IP��ַ =ip
	int	tcpServerPort;							//TCP �������˿ں� =port
	char rbuf[TCP_RECIVE_BUF_LEN];				//TCP ���մ洢��
	int	readLen;								//TCP ����ʵ�����ݳ���
	char wbuf[TCP_WRITE_BUF_LEN];				//TCP ���ʹ洢��
	int	writeLen;								//TCP ����ʵ�����ݳ���

}tTcpConnectServerInfoTypedef;

typedef struct
{
	char mqttPubClientTopic[MQTT_PUB_TOPIC_LEN];//�������⻺��
	char mqttPubBuf[MQTT_PUB_BUF_LEN];			//�������ݴ洢
	int	mqttPubLen;								//�������ݳ���
}tMqttPubInfoTypedef;

typedef struct
{
	char mqttServerAddress[MQTT_SERVER_ADDS_LEN];				//mqtt ��������ַ =ip:port
	char mqttPubClientId[MQTT_CLIENT_ID_LEN];					//���Ĵ˴��ͻ���ID
	char mqttSubClientId[MQTT_CLIENT_ID_LEN];					//���Ĵ˴��ͻ���ID

	char mqttClientUsername[MQTT_CLIENT_USER_NAME_LEN];
	char mqttClientPassword[MQTT_CLIENT_PASSWORD_LEN];
	char mqttSubClientTopicHead[MQTT_CLIENT_SUB_TOPIC_HEAD_LEN];

	int mqttPubQos;
	int mqttSubQos;
	int mqttTimeOut;

	tMqttPubInfoTypedef tmqttPubInfo;

}tMqttConnectServerInfoTypedef;


/*
typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_CurtainCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_LedCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_TempCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_PWSwitchCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_DoorMagneticCtl_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_BodySensor_Typedef;

typedef struct 
{
	int8_t 	devName[DEV_NAME_LEN];
	uint8_t devId[TCM_ID_LEN];

}tDev_HumTempSensor_Typedef;


*/


//�߳���ں���ָ��
typedef void *(*funThread)(void *arg);

//�߳̿��ƿ�
//��ÿ���̳߳�������̺߳š��߳���ں��������Ӵ�������ɵĽṹ��
typedef struct {
	pthread_t		tid;        //�̺߳�
	funThread		pfun;       //��ں���
	int				watch_err;  //���Ӵ�����
}threadsCBT;
















#endif