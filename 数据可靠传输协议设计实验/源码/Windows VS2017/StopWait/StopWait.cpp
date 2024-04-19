// StopWait.cpp : 定义控制台应用程序的入口点。
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

	pns->setRunMode(0);  //VERBOS模式
//	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("I:\\Desktop\\RDT\\Windows VS2017\\input.txt");
	pns->setOutputFile("I:\\Desktop\\RDT\\Windows VS2017\\output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

