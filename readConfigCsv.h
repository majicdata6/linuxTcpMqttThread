#ifndef READ_CONFIG_CSV_H
#define READ_CONFIG_CSV_H

#include "tcmProData.h"	//

// #include "tcp_mqtt_thread.h"

#define DEV_CONFIG_CSV_FILE_NAME 		"./devconfig.CSV"

#define DEV_TYPE_NAME_4CH_CTRL			"4channelCtrl"
#define DEV_TYPE_NAME_LED_CTRL			"3ledCtrl"
#define DEV_TYPE_NAME_CURTAIN_CTRL 		"curtainCtrl"
#define DEV_TYPE_NAME_TEMP_CTRL			"tempCtrl"

#define DEV_TYPE_NAME_BODY_SENSOR		"bodySensor"
#define DEV_TYPE_NAME_HUMTEMP_SENSOR	"humTempSensor"
#define DEV_TYPE_NAME_CURTAIN_SENSOR	"curtainSensor"

#define DEV_TYPE_NAME_KEY_SWITCH		"keySwitch"

#define CMD_EN							"YES"
#define CMD_DISEN						"NO"

#define DEV_ID_LEN 						TCM_ID_LEN
#define DEV_ID_STR_LEN					(2*DEV_ID_LEN)

#define DEV_NUM_LEN						12
#define DEV_NAME_LEN 					32
#define MQTT_TOPIC_NAME_LEN				64

#define FLAG_BUF_CMD_COLUMN				0
#define FLAG_BUF_TCPSEND_COLUMN			1
#define FLAG_BUF_MQTTPUB_COLUMN			2

#define DEV_CMD_4CH_ARRAY_READ_CHSTATUS_INDEX		0	//
#define DEV_CMD_4CH_ARRAY_WRITE_CHSTATUS_INDEX		1	//





//结构体宏

typedef struct tDev_4channelCtlData
{
	uint8_t ch;			//bit3~bit0 = 通道4~通道1 高有效
	uint8_t chStatus;	//bit3~bit0 = 通道4~通道1 高有效
}tDev_4channelCtlData_Typdef;

typedef struct tDev_4channelCtl
{
	char 	devNum[DEV_NUM_LEN];				//设备号1~n
	char 	devName[DEV_NAME_LEN];				//设备名 (默认 设备类型_设备号)
	char 	mqttTopicName[MQTT_TOPIC_NAME_LEN];	//发布主题
	
	uint8_t devId[DEV_ID_LEN];					//设备ID = TCM_ID
	
	uint8_t	readCmd;							//使能可读
	uint8_t writeCmd;							//使能可写
	
	uint8_t devCmdFlag[DEV_4L_CMD_TOTAL][3];	//指令总数 cmd tcpSendFlag mqttSendFlag
	
	tDev_4channelCtlData_Typdef tTcpReadData;	//tcp接收到的数据
	tDev_4channelCtlData_Typdef tMqttSubData;	//mqtt订阅到的数据

}tDev_4channelCtl_Typedef;


typedef struct tDevTypeNodeTotal
{
	int dev4chCtrlTotal;
	int dev4chCtrlCurrentNode;
	int dev4chCtrlCurrentMqttNode;

	tDev_4channelCtl_Typedef *ptDev4ChCtl;

	int dev3ledCtrlTotal;
	int devCurtainCtrlTotal;
	int devTempCtrlTotal;
	int devBodySensorTotal;
	int devHumTempSensorTotal;
	int devCurtainSensorTotal;
	int devKeySwitchTotal;
}tDevTypeNodeTotalTypdef;

//外部变量
extern tDevTypeNodeTotalTypdef tDevTypeNodeTotal;



//函数
//获取配置文件中配置设备数 
//profile 文件路径名
//s 设备名字符串
int getNameCount(char *profile, char *s);

//字符串转大写 str必须是字符串
char *strupr(char *str);

//字符串转小写 str必须是字符串
char *strlowr(char *str);

//字符串转HEX
/*************************************************
Function: 		string2hex
Description: 	字符串转换成hex,要求str只能是大写字母ABCDEF和数字
Input: 			str:要转换的字符串
Output: 		hex:转换后的hex字符数组
Return: 		0 成功
                1 不符合规则，失败
*************************************************/
int string2hex(char* str,char* hex);

//返回1-成功，返回0-失败，返回2-配置文件设备超出设备buff节点数
uint8_t get4lCtrlConfig(char *profile, tDev_4channelCtl_Typedef *ptDevBuf, int getDevNum);


//设备配置初始化
//返回1成功 返回0失败 返回2-配置文件设备超出设备buff节点数
uint8_t initDevConfig(char *fileName, char *pDevTypeName);

//根据设备ID	获取设备4路控制器节点号（1~n）0没找到
int getDev4ChCtlNode(uint8_t *pDevId);

///根据tcp下发标志位 获取设备4路控制器节点号（1~n）0没找到
//cmdStor 有效标志为职位，获取置位标志位指令到此地址存储
int getDev4ChCtNodeFromTcpFlag(uint8_t *cmdStor);

//根据mqtt发布标志位 获取设备4路控制器节点号（1~n）0没找到
//cmdStor 有效标志为职位，获取置位标志位指令到此地址存储
int getDev4ChCtrlCmdFromMqtt(uint8_t *cmdStor);

//从设备类型节点处获取设备名
//devType	设备类型
//devNode	设备节点
//返回设备名地址，异常返回NULL
char *getDevNodeName(uint8_t devType, int devNode);

//从设备类型节点处获取设备字符串节点号
//devType	设备类型
//devNode	设备节点
//返回设备节点的设备字符串节点号 异常返回NULL
char *getDevStorNodeNum(uint8_t devType, int devNode);
#endif