#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>
#include <string>

using namespace std;

bool server = true;

#pragma comment(lib,"ws2_32.lib")

bool ends_with(const string& str, const string& suffix)
{
	if (str.length() < suffix.length()) {
		return false;
	}
	return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

// 根据文件扩展名返回对应的Content-Type  
string get_content_type(const string& file_path) {
	if (ends_with(file_path, ".html")) {
		return "text/html";
	}
	else if (ends_with(file_path, ".css")) {
		return "text/css";
	}
	else if (ends_with(file_path, ".js")) {
		return "application/javascript";

	}
	else if (ends_with(file_path, ".jpg") || ends_with(file_path, ".jpeg")) {
		return "image/jpeg";
	}
	else if (ends_with(file_path, ".png")) {
		return "image/png";
	}
	else
	{
		return "application/octet-stream"; // 默认二进制类型    
	}
}

void handle_request(SOCKET sessionSocket, const string& document_root, char recvBuf[], int rtn) {

	// 获取客户端的IP地址和端口号     
	sockaddr_in client_address;
	int addr_len = sizeof(client_address);
	getpeername(sessionSocket, (struct sockaddr*)&client_address, &addr_len);
	std::string client_ip = inet_ntoa(client_address.sin_addr);
	int client_port = ntohs(client_address.sin_port);


	// 解析HTTP请求
	istringstream request_stream(string(recvBuf, rtn));
	string request_line;
	getline(request_stream, request_line);
	istringstream request_line_stream(request_line);
	string method, url, http_version;
	request_line_stream >> method >> url >> http_version;

	// 输出请求来源信息到屏幕      	

	cout << "Request from: " << client_ip << ":" << client_port << endl;
	cout << "HTTP Request: " << request_line << endl;

	//打印
	cout << "Present request method: " << method << endl;
	cout << "Present request url: " << url << endl;
	cout << "Present request http_version: " << http_version << endl;

	if (url == "/") {
		url += "index.html";
	}

	string file_path = document_root + url;  	
	cout << "File_Path: " << file_path << endl;  	
	ifstream file_stream(file_path.c_str(), ios::binary);

	// 检查HTTP方法是否为GET
	if (method != "GET") {

		file_path = document_root + "/501.html"; 		
		ifstream file_stream(file_path.c_str(), ios::binary); 		
		//tradeoff 		
		ostringstream response_header; 	 		
		response_header << "HTTP/1.1 501 Not Implemented\r\n"; 	 		
		response_header << "Content-type: " << get_content_type(file_path) << "\r\n"; 	 		
		response_header << "Content-Length: "; 	 		
		file_stream.seekg(0, ios::end); 	 		
		response_header << file_stream.tellg() << "\r\n\r\n"; 	 		 		
		send(sessionSocket, response_header.str().c_str(), response_header.str().length(), 0);  	 		 		
		// 发送文件内容 	 		
		file_stream.seekg(0, ios::beg); 	 		
		char file_buffer[1024]; 	 		
		while (!file_stream.eof()) { 		 			
			file_stream.read(file_buffer, sizeof(file_buffer)); 		 			
			send(sessionSocket, file_buffer, file_stream.gcount(), 0);  		
		} 		

		//const char* response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
		//send(sessionSocket, response, strlen(response), 0);
		return;
	}


	//如果创建文件流失败
	if (!file_stream) {
		if (server == true) {
			file_path = document_root + "/404.html"; 	
			ifstream file_stream(file_path.c_str(), ios::binary); 	
			//tradeoff 	
			ostringstream response_header; 	 	
			response_header << "HTTP/1.1 404 Not Found\r\n"; 	
			response_header << "Content-type: " << get_content_type(file_path) << "\r\n"; 	 
			response_header << "Content-Length: "; 	 	
			file_stream.seekg(0, ios::end); 	 	
			response_header << file_stream.tellg() << "\r\n\r\n"; 	 		 
			send(sessionSocket, response_header.str().c_str(), response_header.str().length(), 0);  	 	
			// 发送文件内容 	 	
			file_stream.seekg(0, ios::beg); 	 	
			char file_buffer[1024]; 	 	
			while (!file_stream.eof()) { 		 		
				file_stream.read(file_buffer, sizeof(file_buffer)); 		 
				send(sessionSocket, file_buffer, file_stream.gcount(), 0);  	
			} 		
		}
		else {
			//测试返回404 						
			std::string content = "sldfljdfjhjfjkjdkfhg"; //随便什么 						
			std::string resp; 						
			resp.append("HTTP/1.1 404 Not Found\r\n");    //第一行状态吗和状态描述 						
			resp.append("Server: VerySimpleServer\r\n"); 						
			resp.append("Content-Type: text/html; charset=ISO-8859-1\r\n"); 						
			//resp.append("Content-Length: ").append(std::to_string(content.length())).append("\r\n"); 		
			resp.append("Content-Length: ").append("0").append("\r\n"); //即使返回404状态码，也要指定Content-Length，比如0 				
			resp.append("\r\n"); 					
			resp.append(content);  //由于Content-Length是0，所以content是什么已经不重要了

			send(sessionSocket, resp.c_str(), resp.length(), 0);
		}
	
		/*
		const char* data = "hng";
		const char* response = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\nContent-Length: 0\r\n\r\n";
		send(sessionSocket, response, strlen(response), 0);
		send(sessionSocket, data, strlen(data), 0);
		closesocket(sessionSocket);  //既然client离开了，就关闭sessionSocket
		*/



		return;
	}

	// 发送HTTP响应头
	ostringstream response_header;
	response_header << "HTTP/1.1 200 OK\r\n";
	response_header << "Content-type: " << get_content_type(file_path) << "\r\n";
	response_header << "Content-Length: ";
	file_stream.seekg(0, ios::end);
	response_header << file_stream.tellg() << "\r\n\r\n";
	send(sessionSocket, response_header.str().c_str(), response_header.str().length(), 0);

	// 发送文件内容
	file_stream.seekg(0, ios::beg);
	char file_buffer[1024];
	while (!file_stream.eof()) {
		file_stream.read(file_buffer, sizeof(file_buffer));
		send(sessionSocket, file_buffer, file_stream.gcount(), 0);
	}
	cout << "Send successfully!!!" << endl;
}

void main() {
	WSADATA wsaData;
	/*
		select()机制中提供的fd_set的数据结构，实际上是long类型的数组，
		每一个数组元素都能与一打开的文件句柄（不管是socket句柄，还是其他文件或命名管道或设备句柄）建立联系，建立联系的工作由程序员完成.
		当调用select()时，由内核根据IO状态修改fd_set的内容，由此来通知执行了select()的进程哪个socket或文件句柄发生了可读或可写事件。
	*/
	fd_set rfds;				//用于检查socket是否有数据到来的的文件描述符，用于socket非阻塞模式下等待网络事件通知（有数据到来）
	fd_set wfds;				//用于检查socket是否可以发送的文件描述符，用于socket非阻塞模式下等待网络事件通知（可以发送数据）
	bool first_connetion = true;
	bool request_over = true;

	//配置 Web 服务器的监听地址、监听端口和主目录
	FILE* config_file = fopen("I:/Desktop/VerySimpleServer/server_config.txt", "r"); // 使用fopen打开配置文件

	if (!config_file) {
		cerr << "Error: Could not open server_config.txt" << endl;
		return;
	}

	string listen_address;
	int port;
	string document_root;

	char line[256];
	while (fgets(line, sizeof(line), config_file)) {
		std::istringstream iss(line);
		std::string key, value;
		if (std::getline(iss, key, '=') && std::getline(iss, value)) {
			if (key == "listen_address") {
				listen_address = value;
			}
			else if (key == "port") {
				port = std::stoi(value);
			}
			else if (key == "document_root") {
				document_root = value;
			}
		}
	}

	fclose(config_file);

	// 打印读取的配置参数
	cout << "Listen Address: " << listen_address << endl;
	cout << "Port: " << port << std::endl;
	cout << "Document Root: " << document_root << endl;


	int nRc = WSAStartup(0x0202, &wsaData);

	if (nRc) {
		printf("Winsock  startup failed with error!\n");
	}

	if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
	}

	printf("Winsock  startup Ok!\n");


	//监听socket
	SOCKET srvSocket;

	//服务器地址和客户端地址
	sockaddr_in addr, clientAddr;

	//会话socket，负责和client进程通信
	SOCKET sessionSocket;

	//ip地址长度
	int addrLen;

	//创建监听socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");


	//设置服务器的端口和地址
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //主机上任意一块网卡的IP地址
	addr.sin_addr.S_un.S_addr = inet_addr(listen_address.c_str()); //主机上任意一块网卡的IP地址

	//binding
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));
	if (rtn != SOCKET_ERROR)
		printf("Socket bind Ok!\n");

	//监听
	rtn = listen(srvSocket, 5);
	if (rtn != SOCKET_ERROR)
		printf("Socket listen Ok!\n");

	clientAddr.sin_family = AF_INET;
	addrLen = sizeof(clientAddr);

	//设置接收缓冲区
	char recvBuf[4096];

	u_long blockMode = 1;//将srvSock设为非阻塞模式以监听客户连接请求

	//调用ioctlsocket，将srvSocket改为非阻塞模式，改成反复检查fd_set元素的状态，看每个元素对应的句柄是否可读或可写
	if ((rtn = ioctlsocket(srvSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO：允许或禁止套接口s的非阻塞模式。
		cout << "ioctlsocket() failed with error!\n";
		return;
	}
	cout << "ioctlsocket() for server socket ok!	Waiting for client connection and data\n";

	while (true) {

		//清空rfds和wfds数组
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//将srvSocket加入rfds数组
		//即：当客户端连接请求到来时，rfds数组里srvSocket对应的的状态为可读
		//因此这条语句的作用就是：设置等待客户连接请求
		FD_SET(srvSocket, &rfds);
		

		//如果first_connetion为true，sessionSocket还没有产生
		if (!first_connetion) {
			//将sessionSocket加入rfds数组和wfds数组
			//即：当客户端发送数据过来时，rfds数组里sessionSocket的对应的状态为可读；当可以发送数据到客户端时，wfds数组里sessionSocket的对应的状态为可写
			//因此下面二条语句的作用就是：
			//设置等待会话SOKCET可接受数据或可发送数据
			if (sessionSocket != INVALID_SOCKET) { //如果sessionSocket是有效的
				FD_SET(sessionSocket, &rfds);
				FD_SET(sessionSocket, &wfds);
			}

		}

		/*
			select工作原理：传入要监听的文件描述符集合（可读、可写，有异常）开始监听，select处于阻塞状态。
			当有可读写事件发生或设置的等待时间timeout到了就会返回，返回之前自动去除集合中无事件发生的文件描述符，返回时传出有事件发生的文件描述符集合。
			但select传出的集合并没有告诉用户集合中包括哪几个就绪的文件描述符，需要用户后续进行遍历操作(通过FD_ISSET检查每个句柄的状态)。
		*/
		//开始等待，等待rfds里是否有输入事件，wfds里是否有可写事件
		//The select function returns the total number of socket handles that are ready and contained in the fd_set structure
		//返回总共可以读或写的句柄个数

		int nTotal = select(0, &rfds, &wfds, NULL, NULL);

		//printf("request_over: %d\n", request_over);
		//如果srvSock收到连接请求，接受客户连接请求
		if (FD_ISSET(srvSocket, &rfds) && request_over) {

			nTotal--;   //因为客户端请求到来也算可读事件，因此-1，剩下的就是真正有可读事件的句柄个数（即有多少个socket收到了数据）

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
			//printf("hello\n");
			request_over = false;
		}

		//检查会话SOCKET是否有数据到来或者是否可以发送数据
		if (nTotal > 0) {
			//如果还有有可读事件，说明是会话socket有数据到来，则接受客户的数据
			if (FD_ISSET(sessionSocket, &rfds)) {
				//receiving data from client
				memset(recvBuf, '\0', 4096);
				rtn = recv(sessionSocket, recvBuf, 2048, 0);
				if (rtn > 0) {
					printf("Received %d bytes from client: \n%s\n", rtn, recvBuf);
					handle_request(sessionSocket, document_root, recvBuf, rtn);
					request_over = false;
				}
				else { //否则是收到了客户端断开连接的请求，也算可读事件。但rtn = 0
					printf("Client leaving ...\n");
					request_over = true;
					closesocket(sessionSocket);  //既然client离开了，就关闭sessionSocket
					nTotal--;	//因为客户端离开也属于可读事件，所以需要-1
					sessionSocket = INVALID_SOCKET; //把sessionSocket设为INVALID_SOCKET
				}
			}
			else {
				//printf("hello write\n");
				//request_over = true;
				continue;
			}
		}
	}

}