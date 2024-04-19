#ifndef TCP_SENDER_H 
#define TCP_SENDER_H
#include"RdtSender.h"
#define N 4//窗口大小
#define M 8//序号
class TCPSender :public RdtSender {
private:
	//序号采用3位编码，即0-7，采用模8操作
	int base;//最早未确认分组序号
	int nextseqnum;//下一个待发送分组序号
	Packet sndpkt[M];
	int acked;//冗余ack

public:
	bool send(const Message& message);		//发送应用层下来的Message，由NetworkService调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待确认状态或发送窗口已满而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);	//接受确认Ack，将被NetworkService调用	
	void timeoutHandler(int seqNum);  //Timeout handler，将被NetworkService调用
	bool getWaitingState();	//返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true

public:
	TCPSender();
	~TCPSender();
};

#endif