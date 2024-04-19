#ifndef SELECTIVE_REPEAT_SENDER_H
#define SELECTIVE_REPEAT_SENDER_H
#define M 8
#define N 4
#include "RdtSender.h"
class SRSender :public RdtSender
{
private:
	//int expectSequenceNumberSend;	// ��һ���������  
	//bool waitingState;			
	// �Ƿ��ڵȴ�Ack��״̬ 
	//Packet packetWaitingAck;	

	//�ѷ��Ͳ��ȴ�Ack�����ݰ� 
	//��Ų���3λ���룬��0-7������ģ8���� 
	int base;//����δȷ�Ϸ������ 	 
	int nextseqnum;//��һ�������ͷ������ 
	Packet sndpkt[M];
	bool sndtag[M];

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	SRSender();
	virtual ~SRSender();
};

#endif

