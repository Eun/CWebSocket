/*
    CWebSocket
    Copyright (C) 2011  eun.su.am

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#if ( defined (_WIN32) || defined (_WIN64) )
	#pragma comment(lib, "ws2_32.lib")
	#pragma warning (disable : 4996)
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "CCookieManager.h"
#include "helpers.h"




#if ( defined (_WIN32) || defined (_WIN64) )
	#include <WinSock2.h>
#else
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	typedef int SOCKET;
	typedef const struct sockaddr SOCKADDR;
	#define INVALID_SOCKET -1
	#define closesocket close
#endif

#define CWEB_MAX_URL 265
#define CWEB_MAX_POSTDATA 265
#define CWEB_MAX_RECIVE 8192
#define CWEB_ALL 0
#define CWEB_KEEPCOOKIES 1

class CWebSocket
{
private:

	typedef struct
	{
		unsigned int status;
		char location[CWEB_MAX_URL];
		unsigned int length;
		//bool chunked;
	} headerInfoT;

	char sProtocol[128];
	char sHost[128];
	char sPath[128];
	char sUri[128];
	char sHostIP[128];


	char *GetHostIP(char *host);
	void SplitUrl(char *url, char *protocol, char *host, char *uri);
	void GetPath(char *uri, char *path);
	headerInfoT parse_headers(char *data, bool bSSL);
	int readfromsocket(SOCKET sock, SSL *sslHandle, char *buffer, int len);
	//void CWebSocket::unchunkHTTP11(char *data);

	char *pOutput;
	
public:
	CWebSocket(void);
	char *exec(void);
	void cleanup(void);
	void cleanup(int);
	
	typedef struct
	{
		char URL[CWEB_MAX_URL];
		bool RETURNTRANSFER;
		char POSTDATA[CWEB_MAX_POSTDATA];
		bool AUTOFOLLOWLOCATION;
		char COOKIEFILE[128];
		char USERAGENT[128];
		char REFERER[CWEB_MAX_URL];
		char LANGUAGE[6];
		unsigned int TIMEOUT;
		bool ACCEPTCOOKIES;	
		//bool UNCHUNK;
		CCookieManager *Cookies;
		char OUTPUTFILE[CWEB_MAX_URL];
		char BINDIP[16];
	} optT;

	

	typedef struct
	{
		char	 url[CWEB_MAX_URL];
		unsigned int status;
		unsigned int bytes;
	} returnT;

	optT opt;
	returnT ret;
	unsigned int nErrorCode;	
};

