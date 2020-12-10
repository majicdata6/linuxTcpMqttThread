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

//#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <string.h>
// #include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include <stdint.h>	//uint8_t
#include "ccl.h"
#include "cJSON.h"

#include "tcp_mqtt_thread.h"

#define DEBUG_EN



// #define ADDRESS         "172.16.254.211:1883"           //更改此处地址
// #define CLIENTID        "aaabbbccc_pub"                 //更改此处客户端ID
// #define SUB_CLIENTID    "aaabbbccc_sub"                 //更改此处客户端ID
// #define TOPIC           "mytest"                        //更改发送的话题
// // #define PAYLOAD         "Hello Man, Can you see me ?!"  //
// #define QOS             1
// #define TIMEOUT         10000L
// #define USERNAME        "test_user"
// #define PASSWORD	    "jim777"
#define DISCONNECT	    "out"

#define true    1
#define false   0

#if 0
//消息发送同步
int CONNECT = 1;			//
int PUB_FLAG = false;
//char buf[1024] = {0};

int TCP_SEND_FLAG = false;	//tcp下发标志位
char s_buf[1024]={0};		
int s_buf_send_len = 0;		

char *payload=buf;
int payloadlen=0;





//tcp 客户端连接服务器地址和端口
char ip_buf[16]={0};

//char *server_ip_addr = ip_buf;
//int server_ip_port = 10086;     //默认值

//mqtt 客户端连接配置
char mqtt_server_address[25]={0};			//mqtt 服务器地址
char mqtt_pub_client_id[25]={0};			//更改此处客户端ID
char mqtt_sub_client_id[25]={0};			//更改此处客户端ID
int mqtt_pub_qos=1;
int mqtt_sub_qos=1;
int mqtt_timeout=1000;
char mqtt_client_username[25]={0};
char mqtt_client_password[25]={0};

char mqtt_pub_client_topic[256]={0};

//

//全局变量
//
tDev_4channelCtl_Typedef tDev4chCtl;
#endif


//tcp连接服务器信息
tTcpConnectServerInfoTypedef tTcpConnectServerInfo;

//mqtt连接服务器信息
tMqttConnectServerInfoTypedef tMqttConnectServerInfo;



//信号量
sem_t	semTcpSend, semMqttPub;															//Tcp、Mqtt最终发送信号量
sem_t	semTcp4chSend, semTcpLedSend, semTcpTempSend, semTcpCurtainSend;				//Tcp-类x	下发信号量
sem_t	semMqtt4chPub, semMqttLedPub, semMqttTempPub, semMqttCurtainPub;				//Mqtt-类x	发布信号量


//互斥锁
pthread_mutex_t mutexTcpSend = PTHREAD_MUTEX_INITIALIZER;	//tcp发送	互斥锁
pthread_mutex_t mutexMqttPub = PTHREAD_MUTEX_INITIALIZER;	//mqtt发布	互斥锁




volatile MQTTClient_deliveryToken deliveredtoken_4ch;

volatile MQTTClient_deliveryToken deliveredtoken_led;
volatile MQTTClient_deliveryToken deliveredtoken_curtain;
volatile MQTTClient_deliveryToken deliveredtoken_temp;
volatile MQTTClient_deliveryToken deliveredtoken_sensor;

//tcp 连接初始化
void tcpConnectInfoInit(tTcpConnectServerInfoTypedef *ptTcpServerInfo, char *pFileName)
{
	char fileNameBuf[50] = { 0 };

	strcat(fileNameBuf, "./");
	strcat(fileNameBuf, pFileName);

	//tcp客户端 读取配置文件，获取ip/port
	GetProfileString(fileNameBuf, TCP_CONFIG_IP_NAME, ptTcpServerInfo->tcpServerIp);
	GetProfileInt(fileNameBuf, TCP_CONFIG_PORT_NAME, &ptTcpServerInfo->tcpServerPort);
}

//mqttClient连接初始化
void mqttConnectInfoInit(tMqttConnectServerInfoTypedef *ptMqttServerInfo, char *pFileName)
{
	char fileNameBuf[50] = { 0 };

	strcat(fileNameBuf, "./");
	strcat(fileNameBuf, pFileName);

	//mqtt客户端 读取配置文件
	GetProfileString(fileNameBuf, MQTT_CONFIG_ADDRESS_NAME, ptMqttServerInfo->mqttServerAddress);
	GetProfileString(fileNameBuf, MQTT_CONFIG_CLIENT_ID_NAME, ptMqttServerInfo->mqttPubClientId);
	GetProfileString(fileNameBuf, MQTT_CONFIG_SUB_CLIENT_ID_NAME, ptMqttServerInfo->mqttSubClientId);
	GetProfileInt(fileNameBuf, MQTT_CONFIG_TIMEOUT_NAME, &ptMqttServerInfo->mqttTimeOut);
	GetProfileString(fileNameBuf, MQTT_CONFIG_USENAME_NAME, ptMqttServerInfo->mqttClientUsername);
	GetProfileString(fileNameBuf, MQTT_CONFIG_PASSWORT_NAME, ptMqttServerInfo->mqttClientPassword);

	//GetProfileString(fileNameBuf, MQTT_CONFIG_TOPIC_NAME, ptMqttServerInfo->mqttPubClientTopic);
}


//发送成功回调
void delivered_4ch(void *context, MQTTClient_deliveryToken dt)
{
	#ifdef DEBUG_EN
		printf("Message_4ch with token value %d delivery confirmed\n", dt);
	#endif // DEBUG

	deliveredtoken_4ch = dt;
}

void delivered_led(void *context, MQTTClient_deliveryToken dt)
{
#ifdef DEBUG_EN
	printf("Message_led with token value %d delivery confirmed\n", dt);
#endif // DEBUG

	deliveredtoken_led = dt;
}

void delivered_curtain(void *context, MQTTClient_deliveryToken dt)
{
#ifdef DEBUG_EN
	printf("Message_curtain with token value %d delivery confirmed\n", dt);
#endif // DEBUG

	deliveredtoken_curtain = dt;
}

void delivered_temp(void *context, MQTTClient_deliveryToken dt)
{
#ifdef DEBUG_EN
	printf("Message_temp with token value %d delivery confirmed\n", dt);
#endif // DEBUG

	deliveredtoken_temp = dt;
}

void delivered_sensor(void *context, MQTTClient_deliveryToken dt)
{
#ifdef DEBUG_EN
	printf("Message_sensor with token value %d delivery confirmed\n", dt);
#endif // DEBUG

	deliveredtoken_sensor = dt;
}

//订阅回调
//4路控制器 mqtt订阅回调
int msgarrvd_4ch(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int i;
	char* payloadptr;

	//数据解析、存放
	if (0 == decodeMqttSub4ch(topicName, message->payload, message->payloadlen))
	{
		//解析错误
		MQTTClient_freeMessage(&message);
		MQTTClient_free(topicName);
		return 1;
	}

	//根据下发需求 发送tcp-4ch发送信号量
	sem_post(&semTcp4chSend);

#if 0
	//
	memset(s_buf,'\0',sizeof(s_buf));
	strcpy(s_buf,topicName);
	int str_len = 0;
	str_len = strlen(s_buf);
	s_buf_send_len = str_len;

	strcpy(&s_buf[str_len],message->payload);
	s_buf_send_len += message->payloadlen;

	// //json数据解析
	// cJSON *rjson=cJSON_Parse(s_buf);
	// cJSON *json_item=cJSON_GetObjectItem(rjson,"name");
	// printf("%s\n", json_item->valuestring);


	TCP_SEND_FLAG = true;

	#ifdef DEBUG_EN
		printf("Message arrived\n");
		printf("     topic: %s\n", topicName);
		printf("   message: ");
	#endif

	payloadptr = message->payload;
	if(strcmp(payloadptr, DISCONNECT) == 0)
	{
		printf(" \n out!!");
		CONNECT = 0;
	}


	
	for(i=0; i<message->payloadlen; i++)
	{
		putchar(*payloadptr++);
	}
	printf("\n");
#endif

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

//led调光控制器 mqtt订阅回调
int msgarrvd_led(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
#if 0
	int i;
	char* payloadptr;

	//
	memset(s_buf, '\0', sizeof(s_buf));
	strcpy(s_buf, topicName);
	int str_len = 0;
	str_len = strlen(s_buf);
	s_buf_send_len = str_len;

	strcpy(&s_buf[str_len], message->payload);
	s_buf_send_len += message->payloadlen;

	// //json数据解析
	// cJSON *rjson=cJSON_Parse(s_buf);
	// cJSON *json_item=cJSON_GetObjectItem(rjson,"name");
	// printf("%s\n", json_item->valuestring);


	TCP_SEND_FLAG = true;

#ifdef DEBUG_EN
	printf("Message arrived\n");
	printf("topic=: %s\n", topicName);
	printf("message: ");
#endif

	payloadptr = message->payload;
	if (strcmp(payloadptr, DISCONNECT) == 0)
	{
		printf(" \n out!!");
		CONNECT = 0;
	}



	for (i = 0; i<message->payloadlen; i++)
	{
		putchar(*payloadptr++);
	}
	printf("\n");

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
#endif
	return 1;
}

//窗帘控制器 mqtt订阅回调
int msgarrvd_curtain(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
//	int i;
//	char* payloadptr;
//
//	//
//	memset(s_buf, '\0', sizeof(s_buf));
//	strcpy(s_buf, topicName);
//	int str_len = 0;
//	str_len = strlen(s_buf);
//	s_buf_send_len = str_len;
//
//	strcpy(&s_buf[str_len], message->payload);
//	s_buf_send_len += message->payloadlen;
//
//	// //json数据解析
//	// cJSON *rjson=cJSON_Parse(s_buf);
//	// cJSON *json_item=cJSON_GetObjectItem(rjson,"name");
//	// printf("%s\n", json_item->valuestring);
//
//
//	TCP_SEND_FLAG = true;
//
//#ifdef DEBUG_EN
//	printf("Message arrived\n");
//	printf("     topic: %s\n", topicName);
//	printf("   message: ");
//#endif
//
//	payloadptr = message->payload;
//	if (strcmp(payloadptr, DISCONNECT) == 0)
//	{
//		printf(" \n out!!");
//		CONNECT = 0;
//	}
//
//
//
//	for (i = 0; i<message->payloadlen; i++)
//	{
//		putchar(*payloadptr++);
//	}
//	printf("\n");

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

//温控器控制器 mqtt订阅回调
int msgarrvd_temp(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
//	int i;
//	char* payloadptr;
//
//	//
//	memset(s_buf, '\0', sizeof(s_buf));
//	strcpy(s_buf, topicName);
//	int str_len = 0;
//	str_len = strlen(s_buf);
//	s_buf_send_len = str_len;
//
//	strcpy(&s_buf[str_len], message->payload);
//	s_buf_send_len += message->payloadlen;
//
//	// //json数据解析
//	// cJSON *rjson=cJSON_Parse(s_buf);
//	// cJSON *json_item=cJSON_GetObjectItem(rjson,"name");
//	// printf("%s\n", json_item->valuestring);
//
//
//	TCP_SEND_FLAG = true;
//
//#ifdef DEBUG_EN
//	printf("Message arrived\n");
//	printf("     topic: %s\n", topicName);
//	printf("   message: ");
//#endif
//
//	payloadptr = message->payload;
//	if (strcmp(payloadptr, DISCONNECT) == 0)
//	{
//		printf(" \n out!!");
//		CONNECT = 0;
//	}
//
//
//
//	for (i = 0; i<message->payloadlen; i++)
//	{
//		putchar(*payloadptr++);
//	}
//	printf("\n");

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

//传感器控制器 mqtt订阅回调
int msgarrvd_sensor(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
//	int i;
//	char* payloadptr;
//
//	//
//	memset(s_buf, '\0', sizeof(s_buf));
//	strcpy(s_buf, topicName);
//	int str_len = 0;
//	str_len = strlen(s_buf);
//	s_buf_send_len = str_len;
//
//	strcpy(&s_buf[str_len], message->payload);
//	s_buf_send_len += message->payloadlen;
//
//	// //json数据解析
//	// cJSON *rjson=cJSON_Parse(s_buf);
//	// cJSON *json_item=cJSON_GetObjectItem(rjson,"name");
//	// printf("%s\n", json_item->valuestring);
//
//
//	TCP_SEND_FLAG = true;
//
//#ifdef DEBUG_EN
//	printf("Message arrived\n");
//	printf("     topic: %s\n", topicName);
//	printf("   message: ");
//#endif
//
//	payloadptr = message->payload;
//	if (strcmp(payloadptr, DISCONNECT) == 0)
//	{
//		printf(" \n out!!");
//		CONNECT = 0;
//	}
//
//
//
//	for (i = 0; i<message->payloadlen; i++)
//	{
//		putchar(*payloadptr++);
//	}
//	printf("\n");
//
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

//连接异常回调
//4路控制器连接异常
void connlost_4ch(void *context, char *cause)
{
	printf("\nConnection 4ch lost\n");
	printf("     cause: %s\n", cause);
}

//led调光控制器连接异常
void connlost_led(void *context, char *cause)
{
	printf("\nConnection led lost\n");
	printf("     cause: %s\n", cause);
}

//窗帘控制器连接异常
void connlost_curtain(void *context, char *cause)
{
	printf("\nConnection curtain lost\n");
	printf("     cause: %s\n", cause);
}

//温控器连接异常
void connlost_temp(void *context, char *cause)
{
	printf("\nConnection temp lost\n");
	printf("     cause: %s\n", cause);
}

//传感器连接异常
void connlost_sensor(void *context, char *cause)
{
	printf("\nConnection sensor lost\n");
	printf("     cause: %s\n", cause);
}


//4路控制器订阅线程
void *subClient_4ch(void *tMqttInfo)
{
	tMqttConnectServerInfoTypedef *pInfo = (tMqttConnectServerInfoTypedef *)tMqttInfo;


	printf("thread subClient_4ch start!\n");
   
	while (1)
	{

		MQTTClient client;
		MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
		char subTopic[MQTT_PUB_TOPIC_LEN] = { 0 }; //
		int rc;
		int ch;

		//创建
		MQTTClient_create(&client, pInfo->mqttServerAddress, pInfo->mqttSubClientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);

		conn_opts.keepAliveInterval = 20;
		conn_opts.cleansession = 1;
		conn_opts.username = pInfo->mqttClientUsername;
		conn_opts.password = pInfo->mqttClientPassword;

		//注册回调
		MQTTClient_setCallbacks(client, NULL, connlost_4ch, msgarrvd_4ch, delivered_4ch);

		//连接 失败后间隔30s继续连接
		//if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
		//{
		//	printf("Failed to connect, return code %d\n", rc);
		//	exit(EXIT_FAILURE);
		//}
		while ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
		{
			printf("Failed to connect, return code %d\n", rc);
			/*	exit(EXIT_FAILURE);*/
			sleep(30);
		}

		//封装订阅主题 7层（过滤后三层）
		strcpy(subTopic, pInfo->mqttSubClientTopicHead);	//主题头部3层 建筑/楼层/网关
		strcat(subTopic, DEV_TYPE_NAME_4CH_CTRL);			//4层 设备类型
		strcat(subTopic, "/+");								//5层 设备名 -统配
		strcat(subTopic, "/+");								//6层 设备节点 -统配
		strcat(subTopic, "/+");								//7层 设备指令（读、写）-统配

		printf(" Subscribing to topic = %s\n for clientid = %s \n using QoS = %d\n\n"
			"Press Q<Enter> to quit\n\n", subTopic, pInfo->mqttSubClientId, pInfo->mqttSubQos);
		//订阅主题
		MQTTClient_subscribe(client, subTopic, pInfo->mqttSubQos);

		do
		{
			ch = getchar();
		} while (ch != 'Q' && ch != 'q');

		MQTTClient_unsubscribe(client, subTopic);
		MQTTClient_disconnect(client, 10000);
		MQTTClient_destroy(&client);
		
		printf("subClient_4ch disconnect next connect  after 300s!\n");
		sleep(300);
	}
   
   pthread_exit(NULL);
}

//mqtt发布线程
void *pubClient(void *tMqttInfo)
{
	tMqttConnectServerInfoTypedef *pInfo = (tMqttConnectServerInfoTypedef *)tMqttInfo;
	
	//声明一个MQTTClient
	MQTTClient client;
	//初始化MQTT Client选项
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	//#define MQTTClient_message_initializer { {'M', 'Q', 'T', 'M'}, 0, 0, NULL, 0, 0, 0, 0 }
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	//声明消息token
	MQTTClient_deliveryToken token;
	
	int rc;
	
	//使用参数创建一个client，并将其赋值给之前声明的client
	MQTTClient_create(&client, pInfo->mqttServerAddress, pInfo->mqttPubClientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = pInfo->mqttClientUsername;
	conn_opts.password = pInfo->mqttClientPassword;

	 //使用MQTTClient_connect将client连接到服务器，使用指定的连接选项。成功则返回MQTTCLIENT_SUCCESS
	//连接失败 循环间隔连接
	//if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	//{
	//	printf("Failed to connect, return code %d\n", rc);
	//	/*exit(EXIT_FAILURE);*/
	//}

	while ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		/*exit(EXIT_FAILURE);*/

		sleep(30);
	}

	//循环发布
	while(1)
	{
		//等待发送信号
		sem_wait(&semMqttPub);

		//发布初始化
		pubmsg.payload = pInfo->tmqttPubInfo.mqttPubBuf;			//
		pubmsg.payloadlen = pInfo->tmqttPubInfo.mqttPubLen;			//
		pubmsg.qos = pInfo->mqttPubQos;								//
		pubmsg.retained = 0;

		//发布
		MQTTClient_publishMessage(client, pInfo->tmqttPubInfo.mqttPubClientTopic, &pubmsg, &token);

		rc = MQTTClient_waitForCompletion(client, token, pInfo->mqttTimeOut);

		//解锁 互斥信号
		pthread_mutex_unlock(&mutexMqttPub);

	}
	
	
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
}

//4路控制器发布初始化
void *mqttPubInit_4ch(void *tMqttInfo)
{
	tMqttConnectServerInfoTypedef *ptMqttServerInfo = (tMqttConnectServerInfoTypedef *)tMqttInfo;
	tMqttPubInfoTypedef *pInfo = &ptMqttServerInfo->tmqttPubInfo;
	while (1)
	{
		//等待发送信号
		sem_wait(&semMqtt4chPub);

		//上锁 mutexMqttPub 互斥锁
		pthread_mutex_lock(&mutexMqttPub);

		//发送数据初始化
		memset(pInfo->mqttPubBuf, 0, MQTT_PUB_BUF_LEN);
		memset(pInfo->mqttPubClientTopic, 0, MQTT_PUB_TOPIC_LEN);

		//初始化MQTT PUB数据
		pInfo->mqttPubLen = mqttPubInit(AGREEMENT_CMD_MID_MASTER_4CH, pInfo->mqttPubBuf, pInfo->mqttPubClientTopic, ptMqttServerInfo->mqttSubClientTopicHead);
		
		if (pInfo->mqttPubLen)
		{
			//根据下发需求 发送tcp发送信号量
			sem_post(&semMqttPub);

		}
		else
		{
			//解锁 mutexMqttPub 互斥锁
			pthread_mutex_unlock(&mutexMqttPub);
		}
	}
}

void *tcpClient_r(void *tTcpInfo)
{
	tTcpConnectServerInfoTypedef *pInfo = (tTcpConnectServerInfoTypedef *)tTcpInfo;

	int rec_len = 0;
	int socket_fd = 0;				//声明socket描述符
	struct sockaddr_in t_sockaddr;	//连接服务器地址结构体声明

	uint8_t dealType = 0;

	while (1)
	{
		//socket描述符
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd < 0)
		{
			fprintf(stderr, "socket error %s errno: %d\n", strerror(errno), errno);
		}

		memset(&t_sockaddr, 0, sizeof(struct sockaddr_in));

		t_sockaddr.sin_family = AF_INET;
		t_sockaddr.sin_port = htons(pInfo->tcpServerPort);
		inet_pton(AF_INET, pInfo->tcpServerIp, &t_sockaddr.sin_addr);

		//connect连接 -失败后30s重连
		//if((connect(socket_fd, (struct sockaddr*)&t_sockaddr, sizeof(struct sockaddr))) < 0 ) 
		//{
		//	fprintf(stderr, "connect error %s errno: %d\n", strerror(errno), errno);
		//	// return 0;
		//	exit(1);  
		//}

		while ((connect(socket_fd, (struct sockaddr*)&t_sockaddr, sizeof(struct sockaddr))) < 0)
		{

			fprintf(stderr, "connect error %s errno: %d\n", strerror(errno), errno);
			/*	exit(1);*/
			sleep(30);
		}

		//循环接收
		while (1)
		{
			//
			if ((rec_len = recv(socket_fd, pInfo->rbuf, TCP_RECIVE_BUF_LEN, 0)) == -1)
			{
				perror("recv error\n");
				/*exit(1);*/
				break;
			}

			pInfo->readLen = rec_len;

			//tcp数据接收解析
			dealType = decodeTcpData(pInfo->rbuf, pInfo->readLen);
			switch (dealType)
			{
			case AGREEMENT_CMD_MID_MASTER_4CH:
			{
				dealType = 0;
				//发送信号
				sem_post(&semMqtt4chPub);
			}
			break;
			case AGREEMENT_CMD_MID_MASTER_TEMP:
			{
				dealType = 0;
				//发送信号
				sem_post(&semMqttTempPub);
			}
			break;
			case AGREEMENT_CMD_MID_MASTER_LED:
			{
				dealType = 0;
				//发送信号
				sem_post(&semMqttLedPub);
			}
			break;
			case AGREEMENT_CMD_MID_MASTER_CURTAIN:
			{
				dealType = 0;
				//发送信号
				sem_post(&semMqttCurtainPub);
			}
			break;

			default:
				dealType = 0;
				break;
			}
			

#ifdef DEBUG_EN
			printf("%s\n", pInfo->rbuf);
#endif // DEBUG_EN
	
		}

		close(socket_fd);
		socket_fd = -1;

		//异常退出或断开后间隔连接
		perror("err_tcpRead_disconnect try to connect after 300s\n");
		sleep(300);//5分钟
	}
}

void *tcpClient_w(void *tTcpInfo)
{
	tTcpConnectServerInfoTypedef *pInfo = (tTcpConnectServerInfoTypedef *)tTcpInfo;


	int socket_fd = 0;				//socket 声明
	struct sockaddr_in t_sockaddr;	//服务器地址声明

	char *send_message = "hello";
	
	int rec_len = 0;

	while (1)
	{
		//socket描述符建立
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd < 0)
		{
			fprintf(stderr, "socket error %s errno: %d\n", strerror(errno), errno);
		}


		memset(&t_sockaddr, 0, sizeof(struct sockaddr_in));

		t_sockaddr.sin_family = AF_INET;
		t_sockaddr.sin_port = htons(pInfo->tcpServerPort);
		inet_pton(AF_INET, pInfo->tcpServerIp, &t_sockaddr.sin_addr);

		//建立连接 -失败后延时循环连接
		//if((connect(socket_fd, (struct sockaddr*)&t_sockaddr, sizeof(struct sockaddr))) < 0 ) 
		//{
		//	fprintf(stderr, "connect error %s errno: %d\n", strerror(errno), errno);
		//	// return 0;
		//	exit(1);  
		//}
		while ((connect(socket_fd, (struct sockaddr*)&t_sockaddr, sizeof(struct sockaddr))) < 0)
		{
			fprintf(stderr, "connect error %s errno: %d\n", strerror(errno), errno);
			// return 0;
			/*exit(1);  */

			sleep(30);
		}

		//循环发送
		while (1)
		{
			//等待等待发送信号量
			sem_wait(&semTcpSend);

			//发送数据
			rec_len = send(socket_fd, pInfo->wbuf, pInfo->writeLen, 0);

#ifdef DEBUG_EN
			printf("send_tcp_data_len=%d\n", rec_len);
#endif // DEBUG_EN

			//解锁 互斥信号
			pthread_mutex_unlock(&mutexTcpSend);
		}

		close(socket_fd);
		socket_fd = -1;

		//异常退出或断开后间隔连接
		perror("err_tcpWrite_disconnect try to connect after 300s\n");
		sleep(300);//5分钟
	}
}

void *tcpWriteInit_4ch(void *tTcpInfo)
{
	tTcpConnectServerInfoTypedef *pInfo = (tTcpConnectServerInfoTypedef *)tTcpInfo;

	while (1)
	{
		//等待 semTcp4chSend 信号量
		sem_wait(&semTcp4chSend);

		//上锁 mutexTcpSend 互斥锁
		pthread_mutex_lock(&mutexTcpSend);

		//发送数据初始化
		memset(pInfo->wbuf, 0, TCP_WRITE_BUF_LEN);	//可省略
		pInfo->writeLen = 0;

		pInfo->writeLen = tcpWriteInit(AGREEMENT_CMD_MID_MASTER_4CH, pInfo->wbuf);
		if (pInfo->writeLen)
		{
			//发送 semTcpSend 信号量
			sem_post(&semTcpSend);
		}
		else
		{
			//初始化失败
			//解锁 mutexTcpSend 互斥锁
			pthread_mutex_unlock(&mutexTcpSend);
		}
	}
}

int main(int argc, char* argv[])
{
	pthread_t threads[NUM_THREADS];
	
	//读取参数判断
	if (argc < 4)
	{
		printf("Please enter 3 parameters：'TCP Profile name (**.conf)' 'MQTT Profile name (**.conf)'  'DEV Profile name (**.CSV)'\n");
		return 0;
	}

#ifdef DEBUG_EN
	printf("tcpFileName=%s\n", argv[1]);
	printf("mqttFileName=%s\n", argv[2]);
	printf("devFileName=%s\n", argv[3]);

#endif // DEBUG_EN


	//初始化tcp配置信息
	memset(&tTcpConnectServerInfo, 0, sizeof(tTcpConnectServerInfo));
	tcpConnectInfoInit(&tTcpConnectServerInfo, argv[1]);

	//初始化mqtt配置信息
	memset(&tMqttConnectServerInfo, 0, sizeof(tMqttConnectServerInfo));
	mqttConnectInfoInit(&tMqttConnectServerInfo, argv[2]);

	//设备初始化 
	//4ch
	if (0 == initDevConfig(argv[3], DEV_TYPE_NAME_4CH_CTRL))
	{
		printf("Dev_%s config fail", DEV_TYPE_NAME_4CH_CTRL);
	}
	//led
	//temp
	//curtain

#if 0
	//led
	if (0 == initDevConfig(argv[3], DEV_TYPE_NAME_LED_CTRL))
	{
		printf("Dev_%s config fail", DEV_TYPE_NAME_LED_CTRL);
	}
	//temp
	if (0 == initDevConfig(argv[3], DEV_TYPE_NAME_CURTAIN_CTRL))
	{
		printf("Dev_%s config fail", DEV_TYPE_NAME_CURTAIN_CTRL);
	}
	//curtain
	if (0 == initDevConfig(argv[3], DEV_TYPE_NAME_TEMP_CTRL))
	{
		printf("Dev_%s config fail", DEV_TYPE_NAME_TEMP_CTRL);
	}
	//sensor
#endif

	//信号量初始化 tcp mqtt发送信号量
	sem_init(&semTcpSend, 0, 0);		//初始化信号量为0
	sem_init(&semMqttPub, 0, 0);		//初始化信号量为0

	//信号量初始化 tcp下发初始化信号量
	sem_init(&semTcp4chSend, 0, 0);	//
	sem_init(&semTcpLedSend, 0, 0);	//
	sem_init(&semTcpTempSend, 0, 0);	//
	sem_init(&semTcpCurtainSend, 0, 0);	//


	//信号量初始化 mqtt发布初始化信号量
	sem_init(&semMqtt4chPub, 0, 0);	//
	sem_init(&semMqttLedPub, 0, 0);	//
	sem_init(&semMqttTempPub, 0, 0);	//
	sem_init(&semMqttCurtainPub, 0, 0);	//



	
#if 0
	//tcp客户端 读取配置文件，获取ip/port
	GetProfileString("./tcpclient.conf", "tcpclient_ip", ip_buf);
	GetProfileInt("./tcpclient.conf", "tcpclient_port", &server_ip_port);

	//mqtt客户端 读取配置文件
	GetProfileString("./mqttclient.conf", "mqtt_address", mqtt_server_address);
	GetProfileString("./mqttclient.conf", "mqtt_clientid", mqtt_pub_client_id);
	GetProfileString("./mqttclient.conf", "mqtt_sub_clientid", mqtt_sub_client_id);
	GetProfileInt("./mqttclient.conf", "mqtt_timeout", &mqtt_timeout);
	GetProfileString("./mqttclient.conf", "mqtt_username", mqtt_client_username);
	GetProfileString("./mqttclient.conf", "mqtt_password", mqtt_client_password);

	GetProfileString("./mqttclient.conf", "mqtt_topic", mqtt_pub_client_topic);

	char testbuf[100]={0};
	int testtimeout=0;

	printf("%s\n", mqtt_server_address);
	printf("%s\n", mqtt_pub_client_id);
	printf("%s\n", mqtt_sub_client_id);
	printf("%d\n", mqtt_timeout);
	printf("%s\n", mqtt_client_username);
	printf("%s\n", mqtt_client_password);
	printf("%s\n", mqtt_pub_client_topic);
	


	// strcpy(mqtt_server_address,"172.16.254.211:1883");
	// strcpy(mqtt_pub_client_id,"aaabbbccc_pub");
	// strcpy(mqtt_sub_client_id,"aaabbbccc_sub");
	// strcpy(mqtt_client_username,"test_user");
	// strcpy(mqtt_client_password,"jim777");
	// strcpy(mqtt_pub_client_topic,"mytest");
	
	// mqtt_timeout = 1000;

	//全局变量初始化
	tDev4chCtl.devNum = 1;
	strcpy(tDev4chCtl.devName,"4chCtrl_1");
	tDev4chCtl.statusCh1 = 0;
	tDev4chCtl.statusCh2 = 1;
	tDev4chCtl.statusCh3 = 0;
	tDev4chCtl.statusCh4 = 0;

	strcpy(tDev4chCtl.mqttTopicName,"4chennelCtrl/devNum1/4chCtrl_1");//"ah/15/gw1/4chennelCtrl/devNum1/4chCtrl_1"
	
	//json数据格式化

	//创建json对象
	cJSON *json=cJSON_CreateObject();

	//在json对象上，添加数组
	cJSON *array=NULL;
	cJSON_AddItemToObject(json,"chStatus",array=cJSON_CreateArray());

	//在array数组上,添加对象
	cJSON *obj=NULL;
	cJSON_AddItemToArray(array,obj=cJSON_CreateObject());
	cJSON_AddItemToObject(obj,"ch1",cJSON_CreateString("off"));
	cJSON_AddItemToObject(obj,"ch2",cJSON_CreateString("on"));
	cJSON_AddStringToObject(obj,"ch3","off");
	cJSON_AddStringToObject(obj,"ch4","off");
	
	//json数据 转化字符串
	char *s=cJSON_Print(json);
	strcpy(tDev4chCtl.mqttData,s);
	printf("%s\n",s);
	tDev4chCtl.mqttDataLen = strlen(tDev4chCtl.mqttData);

	//json数据解析
	cJSON *rjson=cJSON_Parse(s);
	cJSON *json_arr_item=cJSON_GetObjectItem(rjson,"chStatus");
	cJSON *object = cJSON_GetArrayItem(json_arr_item,0);   //因为这个对象是个数组获取，且只有一个元素所以写下标为0获取

	/*下面就是可以重复使用cJSON_GetObjectItem来获取每个成员的值了*/
	cJSON *item = cJSON_GetObjectItem(object,"ch1");  //
	printf("ch1=%s\n",item->valuestring);

	item = cJSON_GetObjectItem(object,"ch2");  //
	printf("ch2=%s\n",item->valuestring);

	item = cJSON_GetObjectItem(object,"ch3");  //
	printf("ch3=%s\n",item->valuestring);

	item = cJSON_GetObjectItem(object,"ch4");  //
	printf("ch4=%s\n",item->valuestring);
	
	
	//csv文件读取

	// tDev_4channelCtl_Typedef *ptDev4chCtl = NULL;

	// printf("csv_4ch_num=%d\n", getNameCount(DEV_CONFIG_CSV_FILE_NAME, DEV_TYPE_NAME_4CH_CTRL));
	// tDevTypeNodeTotal.dev4chCtrlTotal = getNameCount(DEV_CONFIG_CSV_FILE_NAME, DEV_TYPE_NAME_4CH_CTRL);
	
	// tDevTypeNodeTotal.ptDev4ChCtl = (tDev_4channelCtl_Typedef*)calloc(tDevTypeNodeTotal.dev4chCtrlTotal, sizeof(tDev_4channelCtl_Typedef));

	// printf("csv_4ch_num=%d\n", tDevTypeNodeTotal.dev4chCtrlTotal);
	// printf("getConfigErr=%d\n", get4lCtrlConfig(DEV_CONFIG_CSV_FILE_NAME, tDevTypeNodeTotal.ptDev4ChCtl, tDevTypeNodeTotal.dev4chCtrlTotal));
	
	//初始化4路控制器的配置
	initDevConfig(DEV_TYPE_NAME_4CH_CTRL);

	tDev_4channelCtl_Typedef *tDev4chCtl = tDevTypeNodeTotal.ptDev4ChCtl;
	
	//打印配置结构体数据
	for(int num=0; num<tDevTypeNodeTotal.dev4chCtrlTotal; num++)
	{
		printf("dev%dNum=%d\n",num, (tDev4chCtl + num)->devNum);
		printf("dev%dName=%s\n",num, (tDev4chCtl + num)->devName);
		printf("dev%dId=%02x %02x %02x %02x\n",num, (tDev4chCtl + num)->devId[0],(tDev4chCtl + num)->devId[1],(tDev4chCtl + num)->devId[2],(tDev4chCtl + num)->devId[3]);
		printf("read%dCmd=%s\n",num, (tDev4chCtl + num)->readCmd);
		printf("write%dCmd=%s\n",num, (tDev4chCtl + num)->writeCmd);
		printf("\r\n");
	}
	
#endif

	//应用线程创建

	//tcp接收
	pthread_create(&threads[0], NULL, tcpClient_r, (void *)&tTcpConnectServerInfo);
	
	//tcp发送
	pthread_create(&threads[1], NULL, tcpClient_w, (void *)&tTcpConnectServerInfo);
	//检测 发送4路控制器
	pthread_create(&threads[2], NULL, tcpWriteInit_4ch, (void *)&tTcpConnectServerInfo);

	////检测 写led控制器
	//pthread_create(&threads[3], NULL, tcpWriteInit_4ch, (void *)&tTcpConnectServerInfo);
	////检测 写temp控制器
	//pthread_create(&threads[4], NULL, tcpWriteInit_4ch, (void *)&tTcpConnectServerInfo);
	////检测 写curtain控制器
	//pthread_create(&threads[5], NULL, tcpWriteInit_4ch, (void *)&tTcpConnectServerInfo);


	//mqtt 订阅控制-4路控制器
	pthread_create(&threads[3], NULL, subClient_4ch, (void *)&tMqttConnectServerInfo);


	//mqtt 发布
	pthread_create(&threads[4], NULL, pubClient, (void *)&tMqttConnectServerInfo);
	//监测mqtt-4路控制器发布 
	pthread_create(&threads[5], NULL, mqttPubInit_4ch, (void *)&tMqttConnectServerInfo);


	//线程销毁
	//用来等待一个线程的结束 方法一（一般用在主线程中）
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	pthread_join(threads[3], NULL);
	pthread_join(threads[4], NULL);
	pthread_join(threads[5], NULL);


	//用来等待一个线程的结束，方法二（一般用在线程内）
	//pthread_exit(NULL);

	//信号量销毁
	sem_destroy(&semTcpSend);
	sem_destroy(&semMqttPub);

	sem_destroy(&semTcp4chSend);
	sem_destroy(&semTcpLedSend);
	sem_destroy(&semTcpTempSend);
	sem_destroy(&semTcpCurtainSend);

	sem_destroy(&semMqtt4chPub);
	sem_destroy(&semMqttLedPub);
	sem_destroy(&semMqttTempPub);
	sem_destroy(&semMqttCurtainPub);


	return 0;
}


