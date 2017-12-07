#include "client.h"

int main()
{
	ClientTer * client = ClientTer::GetInstance(1234,"127.0.0.1");
	client->SendFile();
	client->RecvFile();

	system("pause");
	return 0;
}




