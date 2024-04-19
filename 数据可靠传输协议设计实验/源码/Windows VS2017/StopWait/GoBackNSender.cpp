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

	sndpkt[nextseqnum].acknum = -1;//忽略该字段  	
	sndpkt[nextseqnum].seqnum = nextseqnum;//当前包的序号即为nextseqnum 
	sndpkt[nextseqnum].checksum = 0;//校验和为0 

	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data)); 	
	
	//message打包 	
	sndpkt[nextseqnum].checksum= pUtils->calculateCheckSum(sndpkt[nextseqnum]); 
	
	//计算当前包的校验和 
	pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]); 	
	if ((base % M) == (nextseqnum % M)) 	{ 
		pns->stopTimer(SENDER, 1); 	
		pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟 
	} 	

	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//发送数据 	
	//窗口后移
	nextseqnum = (nextseqnum + 1) % M;	
	return true;
}

void GoBackNSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) {
		//发送的包没有确认完，继续确认	
		//检查校验和是否正确 		
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
			
			pUtils->printPacket("发送方正确收到确认", ackPkt); 	
			if (base == nextseqnum) {
				// 等待所有的都确定完	
				pns->stopTimer(SENDER, 1); 	
			} 			
			else { 			
				pns->stopTimer(SENDER, 1); 		
				pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟
			} 	
		} 
	}
}

void GoBackNSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum 	
	pns->stopTimer(SENDER, 1); 	
	pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//为第一个元素设置时钟 
	for (int i = base; i != nextseqnum; ) { 	    
		//开始重发 		
		pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", sndpkt[i]);
		pns->sendToNetworkLayer(RECEIVER, sndpkt[i]);//发送数据 		
		i = (i + 1) % M;
	}
}