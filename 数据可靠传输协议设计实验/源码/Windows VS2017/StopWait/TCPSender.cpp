#include "stdafx.h"
#include "Global.h"
#include"TCPSender.h"

TCPSender::TCPSender() :base(0), nextseqnum(0), acked(0) {

}

TCPSender::~TCPSender() {

}

bool  TCPSender::getWaitingState() {
	if (nextseqnum == (base + N) % M) {
		return true;
	}
	else return false;
}


bool TCPSender::send(const Message& message) {
	if (getWaitingState() == true)
		return false;
	sndpkt[nextseqnum].acknum = -1;//忽略该字段

	sndpkt[nextseqnum].seqnum = nextseqnum;//当前包的序号即为nextseqnum
	sndpkt[nextseqnum].checksum = 0;//校验和为0
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data));
	//message打包
	sndpkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndpkt[nextseqnum]);
	//计算当前包的校验和
	pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]);
	if ((base % M) == (nextseqnum % M))
	{
		pns->stopTimer(SENDER, 1);
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
	}
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//发送数据
	nextseqnum = (nextseqnum + 1) % M;//后移

	return true;

}



void TCPSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) {//base没确认完
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			if (((base + N - 1) % M > base && (ackPkt.acknum >= base && ackPkt.acknum <= (base + N - 1) % M))
				|| ((base + N - 1) % M < base && ((ackPkt.acknum >= base && ackPkt.acknum <= M - 1) || ackPkt.acknum <= (base + N - 1) % M)))

			{
				// debug 		
				char tmp[22]; 		
				int i = base; 		
				while(i != nextseqnum){		 
					memcpy(tmp, sndpkt[i].payload, sizeof(sndpkt[i].payload)); 	
					tmp[21] = '\0'; 		
					cout << tmp << endl; 		
					i = (i + 1) % M; 		 	
				} 	
				cout << endl;
				base = (ackPkt.acknum + 1) % M;
				// debug 			
				i = base; 		
				while(i != nextseqnum){		 	
					memcpy(tmp, sndpkt[i].payload, sizeof(sndpkt[i].payload)); 		
					tmp[21] = '\0'; 			
					cout << tmp << endl; 		
					i = (i + 1) % M; 		 		
				} 	
				cout << endl;
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				if (base == nextseqnum) {//所有都确认完
					pns->stopTimer(SENDER, 1);
				}
				else {
					pns->stopTimer(SENDER, 1);
					pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
				}
			}
			else {
				acked++;//收到冗余
				if (acked == 3) {//快速重传base
					pUtils->printPacket("发送方收到3个冗余ack，进行快速重传", sndpkt[base]);
					pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);//发送数据
					pns->stopTimer(SENDER, 1);
					pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
					acked = 0;//置0
				}
			}
		}
	}

}


void TCPSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pns->stopTimer(SENDER, 1);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
	//重发base
	//开始重发
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", sndpkt[base]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[base]);//发送数据
	
}