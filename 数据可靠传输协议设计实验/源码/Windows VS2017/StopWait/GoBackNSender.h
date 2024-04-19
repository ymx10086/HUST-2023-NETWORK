#ifndef GO_BACK_N_SENDER_H
#define GO_BACK_N_SENDER_H
#define N 4 // 窗口大小 
#define M 8 // 序号

#include "RdtSender.h"
class GoBackNSender :public RdtSender
{
private:
	//int expectSequenceNumberSend;	// 下一个发送序号 
	//bool waitingState;				// 是否处于等待Ack的状态
	//Packet packetWaitingAck;		//已发送并等待Ack的数据包
	//序号采用3位编码，即0-7，采用模8操作 	
	int base;//最早未确认分组序号 	
	int nextseqnum;//下一个待发送分组序号 	
	Packet sndpkt[M];

public:

	bool getWaitingState();
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用

public:
	GoBackNSender();
	virtual ~GoBackNSender();
};

#endif

