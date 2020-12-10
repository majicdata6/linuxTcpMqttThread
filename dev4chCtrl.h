
#ifndef DEV_4CH_CTRL_H
#define DEV_4CH_CTRL_H


#include <stdint.h>
#include "tcmProData.h"
#include "readConfigCsv.h"







//�ⲿʹ�ú�������

//�ɹ����� 1 ʧ�ܷ���0
//pStorData ָ��ͨ��״̬�洢������ʽch1=0/1,ch2=0/1,ch3=0/1,ch4=0/1  0-�� 1-��
uint8_t get4chCtrlData_chStatus(uint8_t *pTcpData, uint8_t *pStorData);


//pStorData ָ��洢����ȷ�������С�㹻
//pDevId	�豸ID
//ch		���Ƶ�ͨ��	����λ��Ӧͨ��4~1��1��Ч
//status	��Ӧ��ͨ��״̬ ����λ��Ӧͨ��4~1״̬��0-�أ�1-��
//�������ݳ���  �ɹ����ط�0���ݳ��� ʧ�ܷ���0
int set4chCtrlData_chStatus(uint8_t *pStorData, uint8_t *pDevId, uint8_t ch, uint8_t status);

#endif // 
