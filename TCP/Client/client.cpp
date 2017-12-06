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
int main ()
{
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
	SOCKET SocketClient = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(INVALID_SOCKET == SocketClient)
	{
		printf("socket Faild\n");
		WSACleanup();
		return 1;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if(SOCKET_ERROR == connect(SocketClient,(const sockaddr*)&addr,sizeof(addr)))
	{
		int n = GetLastError();
		printf("connect Faild\n");
		WSACleanup();
		closesocket(SocketClient);
		return 1;
	}

	char szPathName[260] = {0}; 
	cout<<"请输入内容："<<endl;
	cin>>szPathName;

	send(SocketClient,szPathName,260,0);
	

	closesocket(SocketClient);

	//8.卸载库
	WSACleanup();

	system("pause");
	return 0;

}
