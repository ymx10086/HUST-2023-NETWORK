#pragma once
#include "winsock2.h"
#include <stdio.h>
#include <iostream>
#include <regex>
#include <fstream>
#include<sstream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

void main() {
	WSADATA wsaData;
	fd_set rfds;				//用于检查socket是否有数据到来的的文件描述符，用于socket非阻塞模式下等待网络事件通知（有数据到来）
	fd_set wfds;				//用于检查socket是否可以发送的文件描述符，用于socket非阻塞模式下等待网络事件通知（可以发送数据）
	bool first_connetion = true;
	string fileposition;
	int nRc = WSAStartup(0x0202, &wsaData);

	string str_config;
	string str_ip;
	short port;
	char str[40];
	memset(str, '\0', 40);
	ifstream config("D:/net/config.txt", ios::in);
	if (!config) {
		cout << "open config failed" << endl;
		return;
	}
	else {
		cout << "import config success\n" << endl;

		config.getline(str, 40);
		str_config = str;

		regex s0("(D.*)+ (.*) +([0-9]{4,5})");
		smatch m0;
		int result = regex_search(str_config, m0, s0);
		if (result == 0) {
			cout << "config format error\n" << endl;
			return;
		}
		else {

			fileposition = m0[1];
			str_ip = m0[2];
			port = stoi(m0[3]);
		}
	}



	if (nRc) {
		printf("Winsock  startup failed with error!\n");
	}

	if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
	}

	printf("Winsock  startup Ok!\n");


	SOCKET srvSocket;
	sockaddr_in addr, clientAddr;
	SOCKET sessionSocket;
	int addrLen;
	//create socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");
	//set port and ip
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	cout << "\nset server port success :" << port << endl;
	cout << "set server ip success :" << str_ip << endl << endl;
	//inet_addr("127.0.0.1");

//binding
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));
	if (rtn != SOCKET_ERROR)
		printf("Socket bind Ok!\n");
	//listen
	rtn = listen(srvSocket, 5);
	if (rtn != SOCKET_ERROR)
		printf("Socket listen Ok!\n");

	//clientAddr.sin_family = AF_INET;
	addrLen = sizeof(clientAddr);
	char recvBuf[10240];

	u_long blockMode = 1;//将srvSock设为非阻塞模式以监听客户连接请求

	if ((rtn = ioctlsocket(srvSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
		cout << "ioctlsocket() failed with error!\n";
		return;
	}
	cout << "ioctlsocket() for server socket ok!	Waiting for client connection and data\n";

	//清空read,write描述符，对rfds和wfds进行了初始化，必须用FD_ZERO先清空，下面才能FD_SET
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);

	//设置等待客户连接请求
	FD_SET(srvSocket, &rfds);

	while (true) {
		//清空read,write描述符
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//设置等待客户连接请求
		FD_SET(srvSocket, &rfds);

		if (!first_connetion) {
			//设置等待会话SOKCET可接受数据或可发送数据
			FD_SET(sessionSocket, &rfds);
			FD_SET(sessionSocket, &wfds);

		}

		//开始等待
		int nTotal = select(0, &rfds, &wfds, NULL, NULL);

		//如果srvSock收到连接请求，接受客户连接请求
		if (FD_ISSET(srvSocket, &rfds)) {
			nTotal--;

			//产生会话SOCKET

			sessionSocket = accept(srvSocket, (LPSOCKADDR)&clientAddr, &addrLen);
			if (sessionSocket != INVALID_SOCKET)
				printf("Socket listen one client request!\n");

			//把会话SOCKET设为非阻塞模式
			if ((rtn = ioctlsocket(sessionSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
				cout << "ioctlsocket() failed with error!\n";
				return;
			}
			cout << "ioctlsocket() for session socket ok!	Waiting for client connection and data\n";

			//设置等待会话SOKCET可接受数据或可发送数据
			FD_SET(sessionSocket, &rfds);
			FD_SET(sessionSocket, &wfds);

			first_connetion = false;

		}

		/*提示响应报文*/
		const char* NOTFOUND = "HTTP/1.1 404 NOT FOUND\r\n";
		const char* REQERROR = "HTTP/1.1 400 Bad Request\r\n";
		const char* FORMERROR = "HTTP/1.1 400 Bad Request\r\n";

		//检查会话SOCKET是否有数据到来
		if (nTotal > 0) {
			//如果会话SOCKET有数据到来，则接受客户的数据
			if (FD_ISSET(sessionSocket, &rfds)) {
				//receiving data from client
				memset(recvBuf, '\0', 10240);
				rtn = recv(sessionSocket, recvBuf, 1024, 0);

				if (rtn > 0) {

					//开始解析http报文
					smatch m;  //结果文本
					regex s("([A-Za-z]+) +(.*) +(HTTP/[0-9][.][0-9])");//请求行
					string str(recvBuf);

					int matchnum = regex_search(str, m, s);

					if (FD_ISSET(sessionSocket, &wfds)) {//可写
						if (matchnum == 0) {//请求报文请求行不对
							cout << "requset error" << endl;
							send(sessionSocket, REQERROR, strlen(REQERROR), 0);
							closesocket(sessionSocket);
							return;
						}
						else {
							cout << "\nclient request line is :" << m[0] << endl << endl;
							cout << "client ip : " << inet_ntoa(clientAddr.sin_addr) << endl;
							cout << "client port :" << ntohs(clientAddr.sin_port) << endl << endl;
							string str_get = m[1];
							string str_url = m[2];
							smatch m2;
							regex s2("([/].*[.].*)");
							matchnum = regex_search(str_url, m2, s2);
							if (matchnum == 0) {//请求行文件位置不对
								cout << str_get << str_url << " format error" << endl;

								send(sessionSocket, FORMERROR, strlen(FORMERROR), 0);
								closesocket(sessionSocket);
								return;
							}
							else {

								ifstream f(fileposition + str_url, ios::binary);
								if (!f) {//未找到文件
									cout << str_url << " NOT FOUND" << endl;
									send(sessionSocket, NOTFOUND, strlen(NOTFOUND), 0);
									closesocket(sessionSocket);
									return;
								}
								else {
									smatch m3;  //结果文本
									regex s3("(/.*.html)");
									std::string Content_Type;
									matchnum = regex_search(str_url, m3, s3);
									if (matchnum != 0) {
										Content_Type = "text/html";
									}
									else
										Content_Type = "image/jpg";

									filebuf* temp = f.rdbuf();
									int size = temp->pubseekoff(0, f.end, f.in);
									temp->pubseekpos(0, f.in);

									char* buffer = new char[size];
									char* tail = buffer + size;
									temp->sgetn(buffer, size);
									f.close();
									cout << str_url << " return success" << endl<<endl;
									stringstream remsg;
									remsg << "HTTP/1.1 200 OK\r\n" << "Connection:close\r\n" << "Server:Macyrate\r\n" << "Content Length:" << size
										<< "\r\n" << "Content Type:" + Content_Type << "\r\n\r\n";
									string remsgstr = remsg.str();
									const char* remsgchar = remsgstr.c_str();
									int tmpsize = strlen(remsgchar);
									int sendre = send(sessionSocket, remsgchar, tmpsize, 0);
									while (buffer < tail) {
										sendre = send(sessionSocket, buffer, size, 0);
										buffer = buffer + sendre;
										size = size - sendre;
									}
								}


							}

						}
					}


				}


			}
		}
	}

}