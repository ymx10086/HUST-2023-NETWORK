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

// �����ļ���չ�����ض�Ӧ��Content-Type  
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
		return "application/octet-stream"; // Ĭ�϶���������    
	}
}

void handle_request(SOCKET sessionSocket, const string& document_root, char recvBuf[], int rtn) {

	// ��ȡ�ͻ��˵�IP��ַ�Ͷ˿ں�     
	sockaddr_in client_address;
	int addr_len = sizeof(client_address);
	getpeername(sessionSocket, (struct sockaddr*)&client_address, &addr_len);
	std::string client_ip = inet_ntoa(client_address.sin_addr);
	int client_port = ntohs(client_address.sin_port);


	// ����HTTP����
	istringstream request_stream(string(recvBuf, rtn));
	string request_line;
	getline(request_stream, request_line);
	istringstream request_line_stream(request_line);
	string method, url, http_version;
	request_line_stream >> method >> url >> http_version;

	// ���������Դ��Ϣ����Ļ      	

	cout << "Request from: " << client_ip << ":" << client_port << endl;
	cout << "HTTP Request: " << request_line << endl;

	//��ӡ
	cout << "Present request method: " << method << endl;
	cout << "Present request url: " << url << endl;
	cout << "Present request http_version: " << http_version << endl;

	if (url == "/") {
		url += "index.html";
	}

	string file_path = document_root + url;  	
	cout << "File_Path: " << file_path << endl;  	
	ifstream file_stream(file_path.c_str(), ios::binary);

	// ���HTTP�����Ƿ�ΪGET
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
		// �����ļ����� 	 		
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


	//��������ļ���ʧ��
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
			// �����ļ����� 	 	
			file_stream.seekg(0, ios::beg); 	 	
			char file_buffer[1024]; 	 	
			while (!file_stream.eof()) { 		 		
				file_stream.read(file_buffer, sizeof(file_buffer)); 		 
				send(sessionSocket, file_buffer, file_stream.gcount(), 0);  	
			} 		
		}
		else {
			//���Է���404 						
			std::string content = "sldfljdfjhjfjkjdkfhg"; //���ʲô 						
			std::string resp; 						
			resp.append("HTTP/1.1 404 Not Found\r\n");    //��һ��״̬���״̬���� 						
			resp.append("Server: VerySimpleServer\r\n"); 						
			resp.append("Content-Type: text/html; charset=ISO-8859-1\r\n"); 						
			//resp.append("Content-Length: ").append(std::to_string(content.length())).append("\r\n"); 		
			resp.append("Content-Length: ").append("0").append("\r\n"); //��ʹ����404״̬�룬ҲҪָ��Content-Length������0 				
			resp.append("\r\n"); 					
			resp.append(content);  //����Content-Length��0������content��ʲô�Ѿ�����Ҫ��

			send(sessionSocket, resp.c_str(), resp.length(), 0);
		}
	
		/*
		const char* data = "hng";
		const char* response = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\nContent-Length: 0\r\n\r\n";
		send(sessionSocket, response, strlen(response), 0);
		send(sessionSocket, data, strlen(data), 0);
		closesocket(sessionSocket);  //��Ȼclient�뿪�ˣ��͹ر�sessionSocket
		*/



		return;
	}

	// ����HTTP��Ӧͷ
	ostringstream response_header;
	response_header << "HTTP/1.1 200 OK\r\n";
	response_header << "Content-type: " << get_content_type(file_path) << "\r\n";
	response_header << "Content-Length: ";
	file_stream.seekg(0, ios::end);
	response_header << file_stream.tellg() << "\r\n\r\n";
	send(sessionSocket, response_header.str().c_str(), response_header.str().length(), 0);

	// �����ļ�����
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
		select()�������ṩ��fd_set�����ݽṹ��ʵ������long���͵����飬
		ÿһ������Ԫ�ض�����һ�򿪵��ļ������������socket��������������ļ��������ܵ����豸�����������ϵ��������ϵ�Ĺ����ɳ���Ա���.
		������select()ʱ�����ں˸���IO״̬�޸�fd_set�����ݣ��ɴ���ִ֪ͨ����select()�Ľ����ĸ�socket���ļ���������˿ɶ����д�¼���
	*/
	fd_set rfds;				//���ڼ��socket�Ƿ������ݵ����ĵ��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�������ݵ�����
	fd_set wfds;				//���ڼ��socket�Ƿ���Է��͵��ļ�������������socket������ģʽ�µȴ������¼�֪ͨ�����Է������ݣ�
	bool first_connetion = true;
	bool request_over = true;

	//���� Web �������ļ�����ַ�������˿ں���Ŀ¼
	FILE* config_file = fopen("I:/Desktop/VerySimpleServer/server_config.txt", "r"); // ʹ��fopen�������ļ�

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

	// ��ӡ��ȡ�����ò���
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


	//����socket
	SOCKET srvSocket;

	//��������ַ�Ϳͻ��˵�ַ
	sockaddr_in addr, clientAddr;

	//�Ựsocket�������client����ͨ��
	SOCKET sessionSocket;

	//ip��ַ����
	int addrLen;

	//��������socket
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n");


	//���÷������Ķ˿ں͵�ַ
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //����������һ��������IP��ַ
	addr.sin_addr.S_un.S_addr = inet_addr(listen_address.c_str()); //����������һ��������IP��ַ

	//binding
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));
	if (rtn != SOCKET_ERROR)
		printf("Socket bind Ok!\n");

	//����
	rtn = listen(srvSocket, 5);
	if (rtn != SOCKET_ERROR)
		printf("Socket listen Ok!\n");

	clientAddr.sin_family = AF_INET;
	addrLen = sizeof(clientAddr);

	//���ý��ջ�����
	char recvBuf[4096];

	u_long blockMode = 1;//��srvSock��Ϊ������ģʽ�Լ����ͻ���������

	//����ioctlsocket����srvSocket��Ϊ������ģʽ���ĳɷ������fd_setԪ�ص�״̬����ÿ��Ԫ�ض�Ӧ�ľ���Ƿ�ɶ����д
	if ((rtn = ioctlsocket(srvSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ��
		cout << "ioctlsocket() failed with error!\n";
		return;
	}
	cout << "ioctlsocket() for server socket ok!	Waiting for client connection and data\n";

	while (true) {

		//���rfds��wfds����
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//��srvSocket����rfds����
		//�������ͻ�������������ʱ��rfds������srvSocket��Ӧ�ĵ�״̬Ϊ�ɶ�
		//��������������þ��ǣ����õȴ��ͻ���������
		FD_SET(srvSocket, &rfds);
		

		//���first_connetionΪtrue��sessionSocket��û�в���
		if (!first_connetion) {
			//��sessionSocket����rfds�����wfds����
			//�������ͻ��˷������ݹ���ʱ��rfds������sessionSocket�Ķ�Ӧ��״̬Ϊ�ɶ��������Է������ݵ��ͻ���ʱ��wfds������sessionSocket�Ķ�Ӧ��״̬Ϊ��д
			//�����������������þ��ǣ�
			//���õȴ��ỰSOKCET�ɽ������ݻ�ɷ�������
			if (sessionSocket != INVALID_SOCKET) { //���sessionSocket����Ч��
				FD_SET(sessionSocket, &rfds);
				FD_SET(sessionSocket, &wfds);
			}

		}

		/*
			select����ԭ������Ҫ�������ļ����������ϣ��ɶ�����д�����쳣����ʼ������select��������״̬��
			���пɶ�д�¼����������õĵȴ�ʱ��timeout���˾ͻ᷵�أ�����֮ǰ�Զ�ȥ�����������¼��������ļ�������������ʱ�������¼��������ļ����������ϡ�
			��select�����ļ��ϲ�û�и����û������а����ļ����������ļ�����������Ҫ�û��������б�������(ͨ��FD_ISSET���ÿ�������״̬)��
		*/
		//��ʼ�ȴ����ȴ�rfds���Ƿ��������¼���wfds���Ƿ��п�д�¼�
		//The select function returns the total number of socket handles that are ready and contained in the fd_set structure
		//�����ܹ����Զ���д�ľ������

		int nTotal = select(0, &rfds, &wfds, NULL, NULL);

		//printf("request_over: %d\n", request_over);
		//���srvSock�յ��������󣬽��ܿͻ���������
		if (FD_ISSET(srvSocket, &rfds) && request_over) {

			nTotal--;   //��Ϊ�ͻ���������Ҳ��ɶ��¼������-1��ʣ�µľ��������пɶ��¼��ľ�����������ж��ٸ�socket�յ������ݣ�

			//�����ỰSOCKET
			sessionSocket = accept(srvSocket, (LPSOCKADDR)&clientAddr, &addrLen);
			if (sessionSocket != INVALID_SOCKET)
				printf("Socket listen one client request!\n");

			//�ѻỰSOCKET��Ϊ������ģʽ
			if ((rtn = ioctlsocket(sessionSocket, FIONBIO, &blockMode) == SOCKET_ERROR)) { //FIONBIO��������ֹ�׽ӿ�s�ķ�����ģʽ��
				cout << "ioctlsocket() failed with error!\n";
				return;
			}
			cout << "ioctlsocket() for session socket ok!	Waiting for client connection and data\n";

			//���õȴ��ỰSOKCET�ɽ������ݻ�ɷ�������
			FD_SET(sessionSocket, &rfds);
			FD_SET(sessionSocket, &wfds);

			first_connetion = false;
			//printf("hello\n");
			request_over = false;
		}

		//���ỰSOCKET�Ƿ������ݵ��������Ƿ���Է�������
		if (nTotal > 0) {
			//��������пɶ��¼���˵���ǻỰsocket�����ݵ���������ܿͻ�������
			if (FD_ISSET(sessionSocket, &rfds)) {
				//receiving data from client
				memset(recvBuf, '\0', 4096);
				rtn = recv(sessionSocket, recvBuf, 2048, 0);
				if (rtn > 0) {
					printf("Received %d bytes from client: \n%s\n", rtn, recvBuf);
					handle_request(sessionSocket, document_root, recvBuf, rtn);
					request_over = false;
				}
				else { //�������յ��˿ͻ��˶Ͽ����ӵ�����Ҳ��ɶ��¼�����rtn = 0
					printf("Client leaving ...\n");
					request_over = true;
					closesocket(sessionSocket);  //��Ȼclient�뿪�ˣ��͹ر�sessionSocket
					nTotal--;	//��Ϊ�ͻ����뿪Ҳ���ڿɶ��¼���������Ҫ-1
					sessionSocket = INVALID_SOCKET; //��sessionSocket��ΪINVALID_SOCKET
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