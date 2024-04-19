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
	sndpkt[nextseqnum].acknum = -1;//忽略该字段 

	sndpkt[nextseqnum].seqnum = nextseqnum;//当前包的序号即为nextseqnum  
	sndpkt[nextseqnum].checksum = 0;//校验和为0  
	
	memcpy(sndpkt[nextseqnum].payload, message.data, sizeof(message.data)); 	
	//message打包 
	sndpkt[nextseqnum].checksum= pUtils->calculateCheckSum(sndpkt[nextseqnum]);  	//计算当前包的校验和 
	pUtils->printPacket("发送方发送报文", sndpkt[nextseqnum]); 	 
	//if ((base % M) == (nextseqnum % M)) 
	//{
		pns->stopTimer(SENDER, nextseqnum);
		pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//为第一个元素设置时钟  
	//} 	  
	pns->sendToNetworkLayer(RECEIVER, sndpkt[nextseqnum]);//发送数据 

	nextseqnum = (nextseqnum + 1) % M;	//窗口后移 
	return true;
}

void SRSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) {
		//发送的包没有确认完，继续确认	
		//检查校验和是否正确 		
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {

			if (((base + N - 1) % M > base && (ackPkt.acknum >= base && ackPkt.acknum <= (base + N - 1) % M))
				|| ((base + N - 1) % M < base && ((ackPkt.acknum >= base && ackPkt.acknum <= M - 1) || ackPkt.acknum <= (base + N - 1) % M)))
			{
				sndtag[ackPkt.acknum] = true;//标记为已确认
				pns->stopTimer(SENDER, ackPkt.acknum);
				pUtils->printPacket("发送方正确收到确认", ackPkt);
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
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);//为元素设置时钟 	
	
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", sndpkt[seqNum]);
	pns->sendToNetworkLayer(RECEIVER, sndpkt[seqNum]);//发送数据 
	
}