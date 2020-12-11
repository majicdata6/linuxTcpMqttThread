
#include "tcmProData.h"

#define DEBUG_EN

//��ȡ�豸ID �ɹ�����1 ʧ�ܷ���0
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

	//�����豸idĩβָ��
	pDevIdAddr = (pDevIdAddr + (dataByteTotal - optDatLen - TCM_ID_LEN - 2));//2= status+crc8d

	//��ȡ�豸ID
	for (uint8_t i = 0; i < TCM_ID_LEN; i++)
	{
		pStorDevId[i] = *(pDevIdAddr + i);
	}

#ifdef DEBUG_EN
	printf("storDevId= %02X %02X %02X %02X\n", pStorDevId[0], pStorDevId[1],pStorDevId[2],pStorDevId[3]);
#endif

	return 1;
}

//��ȡ�豸���� �������� !0 �쳣����0
uint8_t getDevType(uint8_t *pTcpData)
{
	tDevData_Typedef *pData = (tDevData_Typedef *)pTcpData;

	//������
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

		//������ �����Ӧ���ݣ���������Դ���ն�ֵ������0x00������ֵ��
		//		��ʪ�����ݣ��¶�������ʪ��ֵ���¶�ֵ������0x00��
		//		������̼���ݣ��¶�ֵ��������̼Ũ�ȣ�����0x8F��
		//		�������ݣ�����0x00,��ѹֵ���Ŵ�״̬������0X08��
		if (AGREEMENT_CMD_RORG_SENSOR == pData->commData.rorg)
		{
			return AGREEMENT_CMD_RORG_SENSOR;
		}

		// //����-(��ʱ����Ҫ)
		// if(AGREEMENT_CMD_RORG_KEY == pData->commData.rorg)
		// {

		// }

	}
}

//��ȡ�豸ָ�� �����豸����ָ��
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

//�豸ID�Ƚ� ��ȷ���1 ���ȷ���0
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
