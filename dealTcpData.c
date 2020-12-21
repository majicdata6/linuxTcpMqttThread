
#include "dealTcpData.h"



#define DEBUG_EN


////成功返回 1 失败返回0
////pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1
//uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData)
//{
//	struct devData
//	{
//		uint8_t dorg;					//0x10-手动触发; 0x20-无线开关触发; 0x30-传感器触发;0x40-网关触发;0x50-定时上传
//		uint8_t setDevId[DEV_ID_LEN];	//触发控制器的设备ID
//		uint8_t nodeChStatus;			//高4bit对应控制器地址 0主地址 低4位对应通道状态（通道4-3-2-1） 1开，0关
//		uint8_t devId[DEV_ID_LEN];		//灯光控制器ID
//	}*pDevData;
//
//	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;
//
//	pDevData = (struct devData *)(&pData->commData.privateDate);
//
//	if (0 == (0xf0 & pDevData->nodeChStatus))
//	{
//		uint8_t chStatus = (0x0f & pDevData->nodeChStatus);
//
//		//通道状态转换算法
//		for (uint8_t i = 0; i < 4; i++)
//		{
//			pStorData[i] = (chStatus & 0x01);
//			chStatus >> 1;
//		}
//		return 1;
//	}
//
//	return 0;
//}


//解析网络数据包
//pTcpData	解析数据起始地址
//dataLen	解析数据的长度
//返回解析处理的设备类型，返回0失败
uint8_t decodeTcpData(uint8_t *pTcpData, uint8_t dataLen)
{
	uint8_t dealType = 0;
	uint8_t dealCmd = 0;
	uint8_t devId[DEV_ID_LEN] = { 0 };
	uint8_t devNote = 0;
	tDevTypeNodeTotalTypdef *ptDevNodeInfo = &tDevTypeNodeTotal;

	uint8_t err = 0;
	//获取设备大类型（传感器/控制器）
	dealType = getDevType(pTcpData);

#ifdef DEBUG_EN
	printf("dealType= %02X\n", dealType);
#endif // DEBUG_EN

	//获取设备数据
	switch (dealType)
	{
	case AGREEMENT_CMD_MID_MASTER_4CH:
	{
		//获取设备从指令
		dealCmd = getDevCmd(pTcpData, AGREEMENT_CMD_MID_MASTER_4CH);
#ifdef DEBUG_EN
		printf("dealCmd= %02X\n", dealCmd);
#endif // DEBUG_EN
		switch (dealCmd)
		{
		case AGREEMENT_CMD_SALVE_4L_UP_CH_STATUS:
		{
			//获取设备ID
			if (0 == getDevId(pTcpData, dataLen, devId))
			{
				return err;
			}
#ifdef DEBUG_EN
			printf("devId= %02X %02X %02X %02X\n", devId[0], devId[1], devId[2], devId[3]);
#endif // DEBUG_EN
			
			//查找设备ID数据地址
			devNote = getDev4ChCtlNode(devId);
			if (0 == devNote)
			{
				return err;
			}
#ifdef DEBUG_EN
			printf("devNote= %d\n", devNote);
#endif // DEBUG_EN

			//获取设备数据
			if (0 == get4chCtrlData_chStatus(pTcpData, &ptDevNodeInfo->ptDev4ChCtl[devNote - 1].tTcpReadData.ch, &ptDevNodeInfo->ptDev4ChCtl[devNote - 1].tTcpReadData.chStatus))
			{
				return err;
			}

			//置位标志位
			ptDevNodeInfo->ptDev4ChCtl[devNote - 1].devCmdFlag[DEV_CMD_4CH_ARRAY_READ_CHSTATUS_INDEX][FLAG_BUF_MQTTPUB_COLUMN] = 1;

#ifdef DEBUG_EN
			printf("ch=%02x, chStatus= %02x, mqttPubFlag= %d\n", ptDevNodeInfo->ptDev4ChCtl[devNote - 1].tTcpReadData.ch, ptDevNodeInfo->ptDev4ChCtl[devNote - 1].tTcpReadData.chStatus, ptDevNodeInfo->ptDev4ChCtl[devNote - 1].devCmdFlag[DEV_CMD_4CH_ARRAY_READ_CHSTATUS_INDEX][FLAG_BUF_MQTTPUB_COLUMN]);
#endif // DEBUG_EN

			
			err = AGREEMENT_CMD_MID_MASTER_4CH;
		}
		break;
		default:
			break;
		}
	}
	break;
	case AGREEMENT_CMD_MID_MASTER_TEMP:
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_LED:
	{

	}
	break;
	case AGREEMENT_CMD_MID_MASTER_CURTAIN:
	{

	}
	break;
	case AGREEMENT_CMD_RORG_SENSOR:
	{

	}
	break;
	case AGREEMENT_CMD_RORG_KEY:
	{

	}
	break;
	default:
		break;
	}
	return err;
}

//编码网络数据包
//pWriteBuf	编码数据存放地址
//devNode	设备节点
//cmd		编码指令
//返回编码数据字节长度 非0正常，返回0失败
int encode4chCtrlTcpData(uint8_t *pWriteBuf, int devNode, uint8_t cmd)
{		
	tDev_4channelCtl_Typedef *ptDev4chNode = tDevTypeNodeTotal.ptDev4ChCtl;
	int dataLen = 0;
	if(devNode < 1)
	{
		return 0;
	}

	switch (cmd)
	{
		case AGREEMENT_CMD_SALVE_4L_READ_STATUS:
		{
			//初始化 4通道 查询通道开关指令
			/*uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData);*/
		}
		//break;
		case AGREEMENT_CMD_SALVE_4L_SET_STATUS:
		{
			//初始化 4通道 设置通道开关指令
			dataLen = set4chCtrlData_chStatus(pWriteBuf, (ptDev4chNode + devNode - 1)->devId, (ptDev4chNode + devNode - 1)->tMqttSubData.ch, (ptDev4chNode + devNode - 1)->tMqttSubData.chStatus);
			if (dataLen)
			{
				//复位 tcp 写 标志位
				ptDev4chNode->devCmdFlag[DEV_CMD_4CH_ARRAY_WRITE_CHSTATUS_INDEX][FLAG_BUF_TCPSEND_COLUMN] = 0;
			}
			
			return dataLen;
		}
		//break;
	
		default:
			return 0;
			//break;
	}
}

//编码网络数据包
//devType	设备类型
//pWriteBuf 初始化数据存储地址
//返回初始化数据字节长度 非0正常，返回0失败
int tcpWriteInit(uint8_t devType, uint8_t *pWriteBuf)
{
	uint8_t cmd = 0;
	int devNode = 0;

	switch (devType)
	{
		case AGREEMENT_CMD_MID_MASTER_4CH:
		{
			//获取节点 和指令
			devNode = getTcpSendFlagFromDev4ChCtrl(&cmd);

#ifdef DEBUG_EN
			printf("get tcp send: devnode=%d,cmd=%02X\n",devNode,cmd);
#endif // DEBUG_EN

			if (0 == devNode)
			{
				return 0;
			}

			//初始化数据
			return (encode4chCtrlTcpData(pWriteBuf, devNode, cmd));
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


