// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GoBackNSender.h"
#include "GoBackNReceiver.h"
#include "SRSender.h"
#include "SRReceiver.h"
#include "TCPSender.h"
#include "TCPReceiver.h"


int main(int argc, char* argv[])
{
	//RdtSender *ps = new StopWaitRdtSender();
	//RdtReceiver * pr = new StopWaitRdtReceiver();

	//RdtSender* ps = new	GoBackNSender();
	//RdtReceiver* pr = new GoBackNReceiver();

	RdtSender* ps = new	SRSender();
	RdtReceiver* pr = new SRReceiver();

	//RdtSender* ps = new	TCPSender();
	//RdtReceiver* pr = new TCPReceiver();

	pns->setRunMode(0);  //VERBOSģʽ
//	pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("I:\\Desktop\\RDT\\Windows VS2017\\input.txt");
	pns->setOutputFile("I:\\Desktop\\RDT\\Windows VS2017\\output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

