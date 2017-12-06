#include <iostream>
#include <WinSock2.h>
#include <fstream>
using namespace std;
#pragma comment(lib,"Ws2_32.lib")

struct FileHead
{
	char szFileName[260];
	int nFileSize;
};

int main()
{
	//1.加载库
	WORD wVersionRequested = MAKEWORD(2,2);
	WSADATA WSAData;
	int err;
	err = WSAStartup(wVersionRequested,&WSAData);
	if(err != 0)
	{
		printf("WSAStartup Faild\n");
		return 1;
	}

	if(LOBYTE(WSAData.wVersion) != 2 || HIBYTE(WSAData.wVersion) != 2)
	{
		printf("WSAStartup Faild\n");
		WSACleanup();
		return 1;
	}

	//2.创建socket
	SOCKET SocketListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(INVALID_SOCKET == SocketListen)
	{
		printf("socket Faild\n");
		WSACleanup();
		return 1;
	}

	//3.绑定
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//127.0.0.1   本地环网
	if(SOCKET_ERROR == bind(SocketListen,(const sockaddr*)&addr,sizeof(addr)))
	{
		int n = GetLastError();
		printf("bind Faild\n");
		WSACleanup();
		closesocket(SocketListen);
		return 1;
	}

	//4.监听
	if(SOCKET_ERROR == listen(SocketListen,SOMAXCONN))
	{
		printf("listen Faild\n");
		WSACleanup();
		closesocket(SocketListen);
		return 1;
	}


	SOCKET SocketWaiter = accept(SocketListen,NULL,NULL);
	if(INVALID_SOCKET == SocketWaiter)
	{
		printf("accept Faild\n");
		WSACleanup();
		closesocket(SocketListen);
		return 1;
	}
	cout<<"服务器已经启动!"<<endl;
	int nres;
	char szbuf[4096] = {0};
	nres = recv(SocketWaiter,szbuf,4096,0);
	if (nres > 0)
	{
		cout<<szbuf<<endl;
	}
	
	system("pause");
	return 0;
}
