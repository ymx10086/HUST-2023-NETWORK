#ifndef TCP_RECEIVER_H 
#define TCP_RECEIVER_H
#include "RdtReceiver.h"
#define N 4//窗口大小
#define M 8//序号

class TCPReceiver :public RdtReceiver {
	int expectSequenceNumberRcvd;//期待收的的序号
	Packet lastAckPkt;//发送的确认packet

public:
	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
	TCPReceiver();
	~TCPReceiver();

};

#endif