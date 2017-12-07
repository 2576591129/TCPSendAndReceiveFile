
#include "server.h"

//void FileSize()
//{
//	char* filepath = "D:\\demo.txt"; 
//	FILE* file;
//	fopen_s(&file,filepath, "rb");  
//	if (file)  
//		int size = _filelength(_fileno(file));  
//	fclose(file);  
//}


int main()
{
	ServerTer * server = ServerTer::GetInstance(1234,"127.0.0.1");
	server->RecvFile();
	server->SendFile();
	


	system("pause");
	return 0;
}	

