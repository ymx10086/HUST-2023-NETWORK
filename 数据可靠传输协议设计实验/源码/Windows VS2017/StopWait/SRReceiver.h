#ifndef SELECTIVE_REPEAT_RECEIVER_H
#define SELECTIVE_REPEAT_RECEIVER_H
#define M 8 //���
#define N 4 //���ڴ�С
#include "RdtReceiver.h"
class SRReceiver :public RdtReceiver
{
private:
	bool rcvtag[M];//���ܴ����Ƿ��յ�
	Packet rcvpkt[M];//�յ���packet
	int base;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	SRReceiver();
	virtual ~SRReceiver();

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};

#endif

