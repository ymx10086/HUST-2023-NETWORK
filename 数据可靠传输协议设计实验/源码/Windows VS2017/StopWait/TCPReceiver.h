#ifndef TCP_RECEIVER_H 
#define TCP_RECEIVER_H
#include "RdtReceiver.h"
#define N 4//���ڴ�С
#define M 8//���

class TCPReceiver :public RdtReceiver {
	int expectSequenceNumberRcvd;//�ڴ��յĵ����
	Packet lastAckPkt;//���͵�ȷ��packet

public:
	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
	TCPReceiver();
	~TCPReceiver();

};

#endif