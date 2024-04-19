#include "stdafx.h"
#include "Global.h"
#include "GoBackNSender.h"

GoBackNSender::GoBackNSender() :base(0), nextseqnum(0)
{

}


GoBackNSender::~GoBackNSender()
{

}

bool GoBackNSender::getWaitingState() {
	if (nextseqnum == (base + N) % M) 
	{ 
		return true;
	}
	else 
		return false;
}

bool GoBackNSender::send(const Message& message) {
	if (getWaitingState() == true) 		
		return false; 	

	sndpkt[nextseqnum].acknum = -1;//���Ը��ֶ�  	
	sndpkt[nextseqnum].seqnum = nextseqnum;//��ǰ������ż�Ϊnextseqnum 
	sndpkt[nextseqnum].checksum = 0;//У���Ϊ0 

	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data)); 	
	
	//message��� 	
	sndpkt[nextseqnum].checksum= pUtils->calculateCheckSum(sndpkt[nextseqnum]); 
	
	//���㵱ǰ����У��� 
	pUtils->printPacket("���ͷ����ͱ���", sndpkt[nextseqnum]); 	
	if ((base % M) == (nextseqnum % M)) 	{ 
		pns->stopTimer(SENDER, 1); 	
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//Ϊ��һ��Ԫ������ʱ�� 
	} 	

	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//�������� 	
	//���ں���
	nextseqnum = (nextseqnum + 1) % M;	
	return true;
}

void GoBackNSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) {
		//���͵İ�û��ȷ���꣬����ȷ��	
		//���У����Ƿ���ȷ 		
		int checkSum = pUtils->calculateCheckSum(ackPkt); 		
		if (checkSum == ackPkt.checksum) { 		

			// debug
			char tmp[22];
			int i = base;
			while(i != nextseqnum){		
				memcpy(tmp, sndpkt[i].payload, sizeof(sndpkt[i].payload));
				tmp[21] = '\0';
				cout << tmp << endl;
				i = (i + 1) % M; 		
			} 		

			base = (ackPkt.acknum + 1) % M; 	

			// debug
			i = base;
			while (i != nextseqnum) {		
				memcpy(tmp, sndpkt[i].payload, sizeof(sndpkt[i].payload)); 		
				tmp[21] = '\0'; 			
				cout << tmp << endl;
				i = (i + 1) % M; 		
			} 		
			// cout << endl; 		
			
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt); 	
			if (base == nextseqnum) {
				// �ȴ����еĶ�ȷ����	
				pns->stopTimer(SENDER, 1); 	
			} 			
			else { 			
				pns->stopTimer(SENDER, 1); 		
				pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//Ϊ��һ��Ԫ������ʱ��
			} 	
		} 
	}
}

void GoBackNSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum 	
	pns->stopTimer(SENDER, 1); 	
	pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//Ϊ��һ��Ԫ������ʱ�� 
	for (int i = base; i != nextseqnum; ) { 	    
		//��ʼ�ط� 		
		pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", sndpkt[i]);
		pns->sendToNetworkLayer(RECEIVER, sndpkt[i]);//�������� 		
		i = (i + 1) % M;
	}
}