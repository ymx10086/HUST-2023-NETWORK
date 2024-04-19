#ifndef GO_BACK_N_SENDER_H
#define GO_BACK_N_SENDER_H
#define N 4 // ���ڴ�С 
#define M 8 // ���

#include "RdtSender.h"
class GoBackNSender :public RdtSender
{
private:
	//int expectSequenceNumberSend;	// ��һ��������� 
	//bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	//Packet packetWaitingAck;		//�ѷ��Ͳ��ȴ�Ack�����ݰ�
	//��Ų���3λ���룬��0-7������ģ8���� 	
	int base;//����δȷ�Ϸ������ 	
	int nextseqnum;//��һ�������ͷ������ 	
	Packet sndpkt[M];

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	GoBackNSender();
	virtual ~GoBackNSender();
};

#endif

