#include <stdio.h>
#include "CWebSocket/CWebSocket.h"

void request(char *url)
{
	CWebSocket web;
	strcpy(web.opt.URL, url);
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

void post(char *url, char *postdata)
{
	CWebSocket web;
	strcpy(web.opt.URL, url);
	strcpy(web.opt.POSTDATA, postdata);
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
	printf("(HTTP)  IP: ");
	request("http://ifconfig.me/ip");
	printf("\n(HTTPS/POST) Result: ");
	post("https://httpbin.org/post", "var1=val1&var2=val2");
	printf("\nPress a key to exit");
	 
	getchar();
	return 0;
}


