#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include < io.h> 
using namespace std;
#pragma comment(lib,"Ws2_32.lib")

class ClientTer;

#define IP_LEN 20
#define RECV_LEN 4096
#define SEND_LEN 4096
#define FILE_NAME_LEN 50

struct FileInfo
{
	char file_path[MAX_PATH];
	char file_name[FILE_NAME_LEN];
	int file_size;
};

ClientTer *instance =NULL;

class ClientTer
{
private:
	ClientTer(int port,char * ip):recv_num(0),send_num(0)
	{
		info.file_size = 0;
		ZeroMemory(info.file_path,MAX_PATH);
		ZeroMemory(info.file_name,FILE_NAME_LEN);
		ZeroMemory(this->ip,IP_LEN);
		ZeroMemory(recv_buf,RECV_LEN);
		ZeroMemory(send_buf,SEND_LEN);

		WORD wVersionRequested = MAKEWORD(2,2);
		WSADATA WSAData;
		if(WSAStartup(wVersionRequested,&WSAData) != 0)return ;
		if(LOBYTE(WSAData.wVersion) != 2 || HIBYTE(WSAData.wVersion) != 2)return ;
		this->socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(INVALID_SOCKET == socket)return;
		SetPortIP(port,ip);
	}
	~ClientTer()
	{
		closesocket(socket);
		WSACleanup();
	}
private:
	unsigned int port;
	char ip[IP_LEN];
	FileInfo info;
	void SetPortIP(int port,char * ip)
	{
		this->port = port;
		strcpy_s(this->ip,IP_LEN,ip);
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(this->port);
		addr.sin_addr.S_un.S_addr = inet_addr(this->ip);//127.0.0.1   本地环网

		if(SOCKET_ERROR == connect(socket,(const sockaddr*)&addr,sizeof(addr)))return ;
	}
	void SaveFileName()
	{
		char* pTemp = info.file_path;
		pTemp = strrchr(pTemp,'\\');
		memcpy(info.file_name,++pTemp,FILE_NAME_LEN);
	}
	void SaveFileSize()
	{
		//获得文件大小
		FILE* file;
		fopen_s(&file,info.file_path, "rb");  
		if (!file)  return;
		info.file_size = _filelength(_fileno(file));  
		fclose(file);  
	}
	void SaveFileInfo()
	{
		SaveFileName();
		SaveFileSize();
	}

public:
	int recv_num ;
	int send_num;
	char recv_buf[RECV_LEN];
	char send_buf[SEND_LEN];
	SOCKET socket;

	void SendFile(char * szPathName = 0)
	{	
		if(szPathName == 0)
		{
			char input[260];
			cout<<"请输入文件名:";
			cin>>input;
			szPathName = input;
		}
		memcpy(info.file_path,szPathName,MAX_PATH);
		SaveFileInfo();
		send(this->socket,(const char*)&info,sizeof(info),0);
		fstream fs;
		fs.open(info.file_path,fstream::in | fstream::binary);
		while(1)
		{
			fs.read(this->send_buf,RECV_LEN);
			int nrelreadsize = fs.gcount();
			if(nrelreadsize > 0)
				send(this->socket,this->send_buf,nrelreadsize,0);
			else
				break;
		}
		fs.close();
	}
	void RecvFile( char *path = 0)
	{
		string s ;
		recv_num = recv(this->socket,(char*)&info,sizeof(info),0);
		if (path == 0)
		{
			char input[260];
			cout<<"请输入存放路径:";
			cin>>input;
			s = path = input;

			char * temp = path;
			for (int i = 0; i <strlen(path); i++ &&temp++ );
			if(strcmp(temp,"\\"))
			{
				s +="\\";
				path =(char *) s.c_str();
			}
		}
		memcpy(info.file_path,path,MAX_PATH-FILE_NAME_LEN-1);
		strcat_s(info.file_path,FILE_NAME_LEN,info.file_name);
		fstream fs;
		fs.open(info.file_path,fstream::out | fstream::binary | fstream::trunc);
		int nSize = info.file_size;
		while(nSize)
		{
			recv_num = recv(socket,recv_buf,RECV_LEN,0);
			if(recv_num > 0)
			{
				fs.write(recv_buf,recv_num);
				nSize -= recv_num;
			}
		}
		//关闭文件
		fs.close();
	}
	static ClientTer* GetInstance(int port,char * ip)
	{
		if (instance == NULL)
		{
			instance = new ClientTer(port,ip);
			return instance ;
		}
		else
			return NULL;
	}

	static void DestoryInstance()
	{
		delete instance;
		instance =NULL;
	}


};
