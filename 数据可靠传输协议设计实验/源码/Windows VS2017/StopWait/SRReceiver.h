#ifndef SELECTIVE_REPEAT_RECEIVER_H
#define SELECTIVE_REPEAT_RECEIVER_H
#define M 8 //序号
#define N 4 //窗口大小
#include "RdtReceiver.h"
class SRReceiver :public RdtReceiver
{
private:
	bool rcvtag[M];//接受窗口是否收到
	Packet rcvpkt[M];//收到的packet
	int base;	// 期待收到的下一个报文序号
	Packet lastAckPkt;				//上次发送的确认报文

public:
	SRReceiver();
	virtual ~SRReceiver();

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};

#endif

