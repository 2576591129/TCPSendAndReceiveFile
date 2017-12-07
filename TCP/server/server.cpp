#include "server.h"
int main()
{
	ServerTer * server = ServerTer::GetInstance(1234,"127.0.0.1");
	server->RecvFile();
	server->SendFile();
	system("pause");
	return 0;
}	

