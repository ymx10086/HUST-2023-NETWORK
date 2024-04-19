#include "stdafx.h"
#include "Global.h"
#include "SRSender.h"

SRSender::SRSender() :nextseqnum(0), base(0)
{
	memset(sndtag, false, M);
}


SRSender::~SRSender()
{

}

bool SRSender::getWaitingState() {
	if (nextseqnum == (base + N) % M) 
	{ 
		return true; 
	}
	else 
		return false;
}

bool SRSender::send(const Message& message) {
	if (getWaitingState() == true) 		
		return false; 	 
	
	sndtag[nextseqnum] = false;
	sndpkt[nextseqnum].acknum = -1;//���Ը��ֶ� 

	sndpkt[nextseqnum].seqnum = nextseqnum;//��ǰ������ż�Ϊnextseqnum  
	sndpkt[nextseqnum].checksum = 0;//У���Ϊ0  
	
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data)); 	
	//message��� 
	sndpkt[nextseqnum].checksum= pUtils->calculateCheckSum(sndpkt[nextseqnum]);  	//���㵱ǰ����У��� 
	pUtils->printPacket("���ͷ����ͱ���", sndpkt[nextseqnum]); 	 
	//if ((base % M) == (nextseqnum % M)) 
	//{
		pns->stopTimer(SENDER, nextseqnum);
		pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//Ϊ��һ��Ԫ������ʱ��  
	//} 	  
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//�������� 

	nextseqnum = (nextseqnum + 1) % M;	//���ں��� 
	return true;
}

void SRSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) {
		//���͵İ�û��ȷ���꣬����ȷ��	
		//���У����Ƿ���ȷ 		
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {

			if (((base + N - 1) % M > base && (ackPkt.acknum >= base && ackPkt.acknum <= (base + N - 1) % M))
				|| ((base + N - 1) % M < base && ((ackPkt.acknum >= base && ackPkt.acknum <= M - 1) || ackPkt.acknum <= (base + N - 1) % M)))
			{
				sndtag[ackPkt.acknum] = true;//���Ϊ��ȷ��
				pns->stopTimer(SENDER, ackPkt.acknum);
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				if (base == ackPkt.acknum) {
					int i = base;
					for (; i != nextseqnum;)
					{
						if (sndtag[i] == false) break;
						i = (i + 1) % M;
					}
					base = i;
				}
			}
		}
	}
}

void SRSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, seqNum); 
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);//ΪԪ������ʱ�� 	
	
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", sndpkt[seqNum]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[seqNum]);//�������� 
	
}