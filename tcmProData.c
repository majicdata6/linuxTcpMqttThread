
#include "tcmProData.h"

#define DEBUG_EN

//获取设备ID 成功返回1 失败返回0
uint8_t getDevId(uint8_t *pTcpData, uint8_t tcpDataLen, uint8_t *pStorDevId)
{
	uint8_t *pDevIdAddr = pTcpData;

	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;
	uint8_t datLen = pData->dataLenL;
	uint8_t optDatLen = pData->optionalLen;
	uint8_t dataByteTotal = datLen + optDatLen + AGREEMENT_CMD_HEAD_LEN + 2;

	if (tcpDataLen != dataByteTotal)
	{
#ifdef DEBUG_EN
		printf("dataLenErr_dataReadLen=%d - dataTotalLen=%d\n", tcpDataLen, dataByteTotal);
#endif
		return 0;
	}

	//计算设备id末尾指针
	pDevIdAddr = (pDevIdAddr + (dataByteTotal - optDatLen - TCM_ID_LEN - 2));//2= status+crc8d

	//获取设备ID
	for (uint8_t i = 0; i < TCM_ID_LEN; i++)
	{
		pStorDevId[i] = *(pDevIdAddr + i);
	}

#ifdef DEBUG_EN
	printf("storDevId= %02X %02X %02X %02X\n", pStorDevId[0], pStorDevId[1],pStorDevId[2],pStorDevId[3]);
#endif

	return 1;
}

//获取设备类型 正常返回 !0 异常返回0
uint8_t getDevType(uint8_t *pTcpData)
{
	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;

	//控制器
	if (AGREEMENT_CMD_RORG_CTRL == pData->commData.rorg)
	{
		switch (pData->commData.manuId2_MasterCmdId)
		{
		case AGREEMENT_CMD_MID_MASTER_4CH:
		{
			return AGREEMENT_CMD_MID_MASTER_4CH;
		}
		break;

		case AGREEMENT_CMD_MID_MASTER_LED:
		{
			return AGREEMENT_CMD_MID_MASTER_LED;
		}
		break;

		case AGREEMENT_CMD_MID_MASTER_CURTAIN:
		{
			return AGREEMENT_CMD_MID_MASTER_CURTAIN;
		}
		break;

		case AGREEMENT_CMD_MID_MASTER_TEMP:
		{
			return AGREEMENT_CMD_MID_MASTER_TEMP;
		}
		break;


		}

		//传感器 人体感应数据（传感器电源，照度值，保留0x00，人体值）
		//		温湿度数据（温度正负，湿度值，温度值，保留0x00）
		//		二氧化碳数据（温度值，二氧化碳浓度，保留0x8F）
		//		窗磁数据（保留0x00,电压值，门窗状态，保留0X08）
		if (AGREEMENT_CMD_RORG_SENSOR == pData->commData.rorg)
		{
			return AGREEMENT_CMD_RORG_SENSOR;
		}

		// //按键-(暂时不需要)
		// if(AGREEMENT_CMD_RORG_KEY == pData->commData.rorg)
		// {

		// }

	}
}

//获取设备指令 返回设备操作指令
uint8_t getDevCmd(uint8_t *pTcpData, uint8_t devType)
{
	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;

	switch (devType)
	{
	case AGREEMENT_CMD_MID_MASTER_4CH:
	{
		return(pData->commData.salveCmd);
	}
	break;

	case AGREEMENT_CMD_MID_MASTER_LED:
	{
		return(pData->commData.salveCmd);
	}
	break;

	case AGREEMENT_CMD_MID_MASTER_CURTAIN:
	{
		return(pData->commData.salveCmd);
	}
	break;

	case AGREEMENT_CMD_MID_MASTER_TEMP:
	{
		return(pData->commData.salveCmd);
	}
	break;

	}

}

//设备ID比较 相等返回1 不等返回0
uint8_t compDevId(uint8_t *pDevId1, uint8_t *pDevId2)
{
	for (uint8_t i = 0; i < TCM_ID_LEN; i++)
	{
		if (*(pDevId1 + i) != *(pDevId2 + i))
		{
			return 0;
		}
	}
	return 1;
}
