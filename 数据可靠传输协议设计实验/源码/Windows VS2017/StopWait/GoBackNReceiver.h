#ifndef GO_BACK_N_RECEIVER_H
#define GO_BACK_N_RECEIVER_H
#define M 8
#include "RdtReceiver.h"
class GoBackNReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	GoBackNReceiver();
	virtual ~GoBackNReceiver();

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};

#endif

