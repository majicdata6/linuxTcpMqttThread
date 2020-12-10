
#include <string.h>
#include "dev4chCtrl.h"
#include "mycrc.h"



//成功返回 1 失败返回0
//pStorData 指向通道状态存储区，格式ch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1 0-关 1-开
uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData)
{
	struct devData
	{
		uint8_t dorg;					//0x10-手动触发; 0x20-无线开关触发; 0x30-传感器触发;0x40-网关触发;0x50-定时上传
		uint8_t setDevId[DEV_ID_LEN];	//触发控制器的设备ID
		uint8_t nodeChStatus;			//高4bit对应控制器地址 0主地址 低4位对应通道状态（通道4-3-2-1） 1开，0关
		uint8_t devId[DEV_ID_LEN];		//灯光控制器ID
	}*pDevData;

	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;

	pDevData = (struct devData *)(&pData->commData.privateDate);

	if (0 == (0xf0 & pDevData->nodeChStatus))
	{
		uint8_t chStatus = (0x0f & pDevData->nodeChStatus);

		//通道状态转换算法
		for (uint8_t i = 0; i < 4; i++)
		{
			pStorData[i] = (chStatus & 0x01);
			chStatus >> 1;
		}
		return 1;
	}

	return 0;
}

//
//pStorData 指向存储区，确保缓存大小足够
//pDevId	设备ID
//ch		控制的通道	低四位对应通道4~1，1有效
//status	对应的通道状态 低四位对应通道4~1状态，0-关，1-开
//返回数据长度  成功返回非0数据长度 失败返回0
int set4chCtrlData_chStatus(uint8_t *pStorData, uint8_t *pDevId, uint8_t ch, uint8_t status)
{
	//数据初始化
	struct tDevSetStatus
	{
		//head data
		uint8_t head;						//= AGREEMENT_CMD_HEAD;
		uint8_t dataLenH;					//= 0;
		uint8_t dataLenL;					//= 0;
		uint8_t optionalLen;				//= AGREEMENT_CMD_OPTIONAL_LEN;
		uint8_t packType;					//= AGREEMENT_CMD_PACK_TYPE_PADIO;
		uint8_t CRC8H;						//= 0;           //

		//data
		uint8_t rorg;						//= AGREEMENT_CMD_RORG_CTRL;
		uint8_t manufacturerId1;			//= AGREEMENT_CMD_MANUFACTURER_ID;
		uint8_t manuId2_MasterCmdId;		//= AGREEMENT_CMD_MID_MASTER_4CH;      //
		uint8_t salveCmd;					//= AGREEMENT_CMD_SALVE_4L_SET_STATUS;

		uint8_t controlId[TCM_ID_LEN];		//= { 0 };	//
		uint8_t nodeNumChNum;				//= 0x0f;		//4路通道同时设置 低字节 对应相应通道 1有效 0无效
		uint8_t chStatus;					//= 0;		//低字节对应相应的通道 1开 0关
		uint8_t gatewayId[TCM_ID_LEN];		//= { 0x88,0x88,0x88,0x88 };  //必须有
		uint8_t status;						//= 0;		//必须有 通常设置0x00

		//constant data
		uint8_t bufConsData[AGREEMENT_CMD_OPTIONAL_LEN];// = { AGREEMENT_CMD_OPTIONA_DATA_TIMES_OF_SUB_MES,0XFF,0XFF,0XFF,0XFF,0XFF };// {0x03,0XFF,0XFF,0XFF,0XFF,0XFF}0x03固定 为子报文次数
		uint8_t CRC8D;						// = 0;

	}*ptDevSetStatus;	//tDevSetStatus , *ptDevSetStatus;

	//有效参数检测
	if ((pStorData == NULL) || (pDevId == NULL) || (ch == 0))
	{
		return 0;
	}


	//初始化缓存
	memset(pStorData, 0x00, sizeof(struct tDevSetStatus));

	ptDevSetStatus = (struct tDevSetStatus *)pStorData;

	//head data
	ptDevSetStatus->head = AGREEMENT_CMD_HEAD;
	ptDevSetStatus->optionalLen = AGREEMENT_CMD_OPTIONAL_LEN;
	ptDevSetStatus->packType = AGREEMENT_CMD_PACK_TYPE_PADIO;

	//data
	ptDevSetStatus->rorg = AGREEMENT_CMD_RORG_CTRL;					
	ptDevSetStatus->manufacturerId1 = AGREEMENT_CMD_MANUFACTURER_ID;		//
	ptDevSetStatus->manuId2_MasterCmdId = AGREEMENT_CMD_MID_MASTER_4CH;      //主地址
	ptDevSetStatus->salveCmd = AGREEMENT_CMD_SALVE_4L_SET_STATUS;			//从地址

	//私有数据
	//获取tcmID
	memcpy(ptDevSetStatus->controlId, pDevId, TCM_ID_LEN);

	//获取操作通道
	ptDevSetStatus->nodeNumChNum &= (0x0f & ch);

	//获取通道状态
	ptDevSetStatus->chStatus = (0x0f & status);

	
	//
	memset(ptDevSetStatus->gatewayId, 0x88, TCM_ID_LEN);//必须有
	memset(ptDevSetStatus->bufConsData, 0xFF, AGREEMENT_CMD_OPTIONAL_LEN);
	ptDevSetStatus->bufConsData[0] = AGREEMENT_CMD_OPTIONA_DATA_TIMES_OF_SUB_MES;

	//计算数据长度
	ptDevSetStatus->dataLenL = sizeof(struct tDevSetStatus) - AGREEMENT_CMD_OPTIONAL_LEN - AGREEMENT_CMD_HEAD_LEN -  2;

	//计算CRC8H
	SetCRC8Sub(&ptDevSetStatus->dataLenH, AGREEMENT_CMD_CRC8H_CHECK_BYTE);

	//计算CRC8D
	SetCRC8Sub(&ptDevSetStatus->rorg, ptDevSetStatus->dataLenL + ptDevSetStatus->optionalLen);

	////复制数据到发送
	//memcpy(pStorData, (uint8_t *)ptDevSetStatus, sizeof(struct tDevSetStatus));

	return (sizeof(struct tDevSetStatus));
}
