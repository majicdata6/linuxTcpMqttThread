#ifndef TCP_MQTT_THREAD_H
#define TCP_MQTT_THREAD_H

#include <pthread.h>	//线程
#include <stdio.h>		//标准输入输出
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		//sleep延时函数
#include <semaphore.h>	//信号量
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

////tcp 客户端连接服务器地址和端口
//char ip_buf[16] = { 0 };
//
//char *server_ip_addr = ip_buf;
//int server_ip_port = 10086;     //默认值
//
//								//mqtt 客户端连接配置
//char mqtt_server_address[25] = { 0 };			//mqtt 服务器地址
//char mqtt_pub_client_id[25] = { 0 };			//更改此处客户端ID
//char mqtt_sub_client_id[25] = { 0 };			//更改此处客户端ID
//int mqtt_pub_qos = 1;
//int mqtt_sub_qos = 1;
//int mqtt_timeout = 1000;
//char mqtt_client_username[25] = { 0 };
//char mqtt_client_password[25] = { 0 };
//
//char mqtt_pub_client_topic[256] = { 0 };

typedef struct
{
	char tcpServerIp[16];						//tcp 服务器IP地址 =ip
	int	tcpServerPort;							//TCP 服务器端口号 =port
	char rbuf[TCP_RECIVE_BUF_LEN];				//TCP 接收存储区
	int	readLen;								//TCP 接收实际数据长度
	char wbuf[TCP_WRITE_BUF_LEN];				//TCP 发送存储区
	int	writeLen;								//TCP 发送实际数据长度

}tTcpConnectServerInfoTypedef;

typedef struct
{
	char mqttPubClientTopic[MQTT_PUB_TOPIC_LEN];//发布主题缓存
	char mqttPubBuf[MQTT_PUB_BUF_LEN];			//发布数据存储
	int	mqttPubLen;								//发布数据长度
}tMqttPubInfoTypedef;

typedef struct
{
	char mqttServerAddress[MQTT_SERVER_ADDS_LEN];				//mqtt 服务器地址 =ip:port
	char mqttPubClientId[MQTT_CLIENT_ID_LEN];					//更改此处客户端ID
	char mqttSubClientId[MQTT_CLIENT_ID_LEN];					//更改此处客户端ID

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


//线程入口函数指针
typedef void *(*funThread)(void *arg);

//线程控制块
//将每个线程抽象成由线程号、线程入口函数、监视错误码组成的结构体
typedef struct {
	pthread_t		tid;        //线程号
	funThread		pfun;       //入口函数
	int				watch_err;  //监视错误码
}threadsCBT;
















#endif