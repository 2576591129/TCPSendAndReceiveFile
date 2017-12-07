//**********************************************  system head *****************************************
#include <iostream>
#include <WinSock2.h>
#include <fstream>
#include <string>
#include < io.h> 

using namespace std;
#pragma comment(lib,"Ws2_32.lib")


//*******************************************define data lengths****************************************
#define IP_LEN 20
#define RECV_LEN 4096
#define SEND_LEN 4096
#define FILE_NAME_LEN 50

//****************************************struct of file's information*************************************
struct FileInfo
{
	char file_path[MAX_PATH];
	char file_name[FILE_NAME_LEN];
	int file_size;
};

//************************************* Instances of the ServerTer class *********************************
class ServerTer;
ServerTer* instance = NULL;


class ServerTer
{
private:
	//----------------------------------------------------------------------------------------------------------------Constructed function	-------------------------------------------------------------
	ServerTer(int port,char * ip)
	{
		//------------------------------------------------------------Initialize member variable-----------------------------------------------------
		recv_num = 0;
		send_num = 0;
		ZeroMemory(recv_buf,RECV_LEN);
		ZeroMemory(send_buf,SEND_LEN);
		ZeroMemory(this->ip,IP_LEN);
		sck_listen = 0;
		sck_send_recv = 0;
		//------------------------------------------------------------------------------------------------------------------Initialize SOCKET-----------------------------------------------------------------------

		//******check the WSAData version************************************************
		WORD wVersionRequested = MAKEWORD(2,2);
		WSADATA WSAData;
		if(WSAStartup(wVersionRequested,&WSAData) != 0)return ;
		if(LOBYTE(WSAData.wVersion) != 2 || HIBYTE(WSAData.wVersion) != 2)return ;		

		//******build the listen socket*****************************************************
		sck_listen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(INVALID_SOCKET == sck_listen)return ;		

		//******Set IP and port************************************************************
		SetPortIP(port,ip);
	}
	//-----------------------------------------------------------------------------------------------------------------destructor function	-----------------------------------------------------------------
	~ServerTer()
	{
		WSACleanup();
		closesocket(sck_listen);
		closesocket(sck_send_recv);
	}

	//-------------------------------------------------------------------------------------------------------------------Set IP and Port-------------------------------------------------------------------------
	void SetPortIP(int port,char * ip)
	{
		//*****Save the IP and port into the class*****************************************
		this->port = port;
		strcpy_s(this->ip,IP_LEN,ip);

		//*****Bind socket to the specified IP and port***********************************
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(this->port);
		addr.sin_addr.S_un.S_addr = inet_addr(this->ip);//127.0.0.1   本地环网
		if(SOCKET_ERROR == bind(sck_listen,(const sockaddr*)&addr,sizeof(addr)))return;

		//*****Listen**********************************************************************
		if(SOCKET_ERROR == listen(sck_listen,SOMAXCONN))return;	

		//******Block there,tWait for client to connect************************************
		sck_send_recv = accept(sck_listen,NULL,NULL);
	}

	//-------------------------------------------------------------------------------------------------------------Get and save the file name--------------------------------------------------------
	void SaveFileName()
	{
		//******The file_path contains the file_name,we need to get it out of file_path****
		char* pTemp = info.file_path;
		pTemp = strrchr(pTemp,'\\');
		if(pTemp == NULL)return ;

		//******use memcpy,To prevent memory leaks*************************************
		memcpy(info.file_name,++pTemp,FILE_NAME_LEN);
	}

	//-----------------------------------------------------------------------------------------------------------Get and save the file size-----------------------------------------------------------
	void SaveFileSize()
	{
		//*****Use the FILE class to open the file and get the file's size********************
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

	//----------------------------------------------------------------------------------------------------------------- member variable----------------------------------------------------------------------
private:
	char ip[IP_LEN];
	unsigned int port;
	FileInfo info;
public:
	int recv_num;
	int send_num;

	SOCKET sck_listen;
	SOCKET sck_send_recv;

	char recv_buf[RECV_LEN];
	char send_buf[SEND_LEN];
public:
	//**********************************************************************************

	//-------------------------------------------------------------------------------------------------USing the C++ Factory Pattern to create Instance--------------------------
	static ServerTer* GetInstance(int port,char * ip)
	{

		//*******Only one object exists in the same file*********************************
		if (instance == NULL)
		{
			instance = new ServerTer(port,ip);
			return instance ;
		}
		else
			return NULL;
	}

	//-------------------------------------------------------------------------------------------------Destroy the instance, release the space------------------------------------------------
	static void DestoryInstance()
	{
		delete instance;
		instance =NULL;
	}

	//------------------------------------------------------------------------------------------------Receives the specified file that sent from the client -----------------------------
	void RecvFile( char *path =0 ) 
	{
		//****************************************************************************************************************************
		//                                               If you do not specify a directory for your files, we will prompt you in the console.						 ||
		//																																																					     ||
		//													If your application doesn't have a console, fill in the correct path.												 ||				
		//****************************************************************************************************************************
		string s , input;
		//****There will send the file size and name firstly******************************
		recv_num = recv(this->sck_send_recv,(char*)&info,sizeof(info),0);
		if(recv_num<=0)return;	

		
		if (path == 0)
		{
			cout<<"Where would you like to be storage:";
			cin>>input;
		}
		// If the last byte you type forgot to fill out '\', we will help you to make up for it	
		s = path =(char *) input.c_str();
		char * temp = path;
		for (int i = 0; i <strlen(path); i++ &&temp++ );
		if(strcmp(temp,"\\"))
		{
			s +="\\";
			path =(char *) s.c_str();
		}

		//******Splicing the name to the back of path so that you can read and write files
		//******Use memcpy instead of strcpy**********************************************
		memcpy(info.file_path,path,MAX_PATH-FILE_NAME_LEN-1);
		strcat_s(info.file_path,FILE_NAME_LEN,info.file_name);

		//******Use the fstream class to open,read and write the file***********************
		fstream fs;
		fs.open(info.file_path,fstream::out | fstream::binary | fstream::trunc);
		int nSize = info.file_size;
		while(nSize)
		{
			recv_num = recv(sck_send_recv,recv_buf,RECV_LEN,0);
			if(recv_num > 0)
			{
				fs.write(recv_buf,recv_num);
				nSize -= recv_num;
			}
		}
		//关闭文件
		fs.close();
	}

	//**********************************************************************************

	//-----------------------------------------------------------------------------------------------------------------------Specify any file to be sent to the client -----------------------------
	void SendFile(char * szPathName = 0)
	{	
		//****************************************************************************************************************************
		//                                               If you do not specify a directory for your files, we will prompt you in the console.						 ||
		//																																																					     ||
		//													If your application doesn't have a console, fill in the correct path.												 ||				
		//****************************************************************************************************************************
		string input;
		if(szPathName == 0)
		{			
			cout<<"请输入文件名:";
			cin>>input;
			szPathName = (char*)input.c_str();
		}

		memcpy(info.file_path,szPathName,MAX_PATH);
		SaveFileInfo();

		//******It will send the file info first**********************************************
		send(this->sck_send_recv,(const char*)&info,sizeof(info),0);

		//******Use the fstream class to open,read and write the file********************
		fstream fs;
		fs.open(info.file_path,fstream::in | fstream::binary);
		while(1)
		{
			//*****Read 4096B to send_buf at one time, and send it ***********************
			fs.read(this->send_buf,RECV_LEN);
			int nrelreadsize = fs.gcount();
			if(nrelreadsize > 0)
				send(this->sck_send_recv,this->send_buf,nrelreadsize,0);
			else
				break;
		}
		fs.close();
	}
};
