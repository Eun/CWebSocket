#include <stdio.h>
#include "CWebSocket/CWebSocket.h"

void simplecall()
{
	CWebSocket web;
	strcpy(web.opt.URL, "http://ifconfig.me/ip");
	char *data = web.exec();
	
	if (data)
	{
		printf(data);
	}
	else
	{
		printf("Error #%d", web.nErrorCode);
	}
	web.cleanup();	
}

void httpscall()
{
	CWebSocket web;
	strcpy(web.opt.URL, "https://openssl.com/");
	char *data = web.exec();
	
	if (data)
	{
		printf(data);
	}
	else
	{
		printf("Error #%d", web.nErrorCode);
	}
	web.cleanup();	
}

int main(int argc, char **argv)
{
	httpscall();
	return 0;
}


