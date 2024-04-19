#ifndef GO_BACK_N_RECEIVER_H
#define GO_BACK_N_RECEIVER_H
#define M 8
#include "RdtReceiver.h"
class GoBackNReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
	Packet lastAckPkt;				//上次发送的确认报文

public:
	GoBackNReceiver();
	virtual ~GoBackNReceiver();

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};

#endif

