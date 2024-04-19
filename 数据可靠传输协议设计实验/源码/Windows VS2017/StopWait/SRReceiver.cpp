#include "stdafx.h"
#include "Global.h"
#include "SRReceiver.h"

SRReceiver::SRReceiver() :base(0)
{
	memset(rcvtag, false, M);

	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1 	
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段 
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

SRReceiver::~SRReceiver()
{

}

void SRReceiver::receive(const Packet& packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);
	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum) {//
		pUtils->printPacket("接收方正确收到发送方的报文", packet);

		if (((base + N - 1) % M > base && (packet.seqnum >= base && packet.seqnum <= (base + N - 1) % M))
			|| ((base + N - 1) % M < base && ((packet.seqnum >= base && packet.seqnum <= M - 1) || packet.seqnum <= (base + N - 1) % M)))
		{
			lastAckPkt.acknum = packet.seqnum;  //确认序号等于收到的报文序号
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("接收方发送确认报文", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
			if (rcvtag[packet.seqnum] == false) {
				rcvpkt[packet.seqnum] = packet;//存入
				rcvtag[packet.seqnum] = true;//已收到
			}
			if (packet.seqnum == base) {
				int i = base;
				for (; i != (base + N) % M;) {
					if (rcvtag[i] == true)//已收到
					{
						//取出Message，向上递交给应用层
						Message msg;
						memcpy(msg.data, rcvpkt[i].payload, sizeof(rcvpkt[i].payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						rcvtag[i] = false;
						i = (i + 1) % M;
					}
					else {
						break;
					}
				}

				// debug
				char tmp[22];
				int j = base;
				while (j != (base + N) % M) {
					printf("{tmp_j}%d\n", j);
					memcpy(tmp, rcvpkt[j].payload, sizeof(rcvpkt[j].payload));
					tmp[21] = '\0';
					cout << tmp << endl;
					j = (j + 1) % M;
				}

				cout << endl;
				base = i;

				// debug
				j = base;
				while (j != (base + N) % M) {
					memcpy(tmp, rcvpkt[j].payload, sizeof(rcvpkt[j].payload));
					tmp[21] = '\0';
					cout << tmp << endl;
					j = (j + 1) % M;
				}
				cout << endl;
			}

		}
		else if (((base - N + M) % M < (base - 1 + M) % M && (packet.seqnum >= (base - N + M) % M && packet.seqnum <= (base - 1 + M) % M))
			|| ((base - N + M) % M > (base - 1 + M) % M && ((packet.seqnum >= (base - N + M) % M && packet.seqnum <= M - 1) || packet.seqnum <= (base - 1 + M) % M)))
		{
			lastAckPkt.acknum = packet.seqnum;  //确认序号等于收到的报文序号
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("接收方发送确认报文", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		}


	}
}

