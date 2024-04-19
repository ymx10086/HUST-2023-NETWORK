#include "stdafx.h"
#include "Global.h"
#include "SRReceiver.h"

SRReceiver::SRReceiver() :base(0)
{
	memset(rcvtag, false, M);

	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1 	
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ� 
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

SRReceiver::~SRReceiver()
{

}

void SRReceiver::receive(const Packet& packet) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);
	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum) {//
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

		if (((base + N - 1) % M > base && (packet.seqnum >= base && packet.seqnum <= (base + N - 1) % M))
			|| ((base + N - 1) % M < base && ((packet.seqnum >= base && packet.seqnum <= M - 1) || packet.seqnum <= (base + N - 1) % M)))
		{
			lastAckPkt.acknum = packet.seqnum;  //ȷ����ŵ����յ��ı������
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
			if (rcvtag[packet.seqnum] == false) {
				rcvpkt[packet.seqnum] = packet;//����
				rcvtag[packet.seqnum] = true;//���յ�
			}
			if (packet.seqnum == base) {
				int i = base;
				for (; i != (base + N) % M;) {
					if (rcvtag[i] == true)//���յ�
					{
						//ȡ��Message�����ϵݽ���Ӧ�ò�
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
			lastAckPkt.acknum = packet.seqnum;  //ȷ����ŵ����յ��ı������
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		}


	}
}

