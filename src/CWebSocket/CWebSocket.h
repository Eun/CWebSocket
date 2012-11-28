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
#ifndef INCLUDED_CWEBSOCKET
#define INCLUDED_CWEBSOCKET

#if ( defined (_WIN32) || defined (_WIN64) )
	#pragma comment(lib, "ws2_32.lib")
	#pragma warning (disable : 4996)
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef NO_COOKIES
	#include "CCookieManager.h"
	class CCookieManager;
#else
	typedef char CCookieManager;
#endif


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

#ifdef OPENSSL
	#if ( defined (_WIN32) || defined (_WIN64) )
		#ifdef _DEBUG
			#pragma comment(lib, "win32/lib/ssleay32MTd.lib")
			#pragma comment(lib, "win32/lib/libeay32MTd.lib")
		#else
			#pragma comment(lib, "win32/lib/ssleay32MT.lib")
			#pragma comment(lib, "win32/lib/libeay32MT.lib")
		#endif
	#endif
	#include <openssl/rand.h>
	#include <openssl/ssl.h>
	#include <openssl/err.h>
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

	#ifndef OPENSSL
		typedef void* SSL;
	#endif

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
	
	
	// helpers
	static char *strcatf(char *dst, char *format, ...)
	{
		va_list	argptr;
		char	temp_buffer[1024];
		int		len;
	 
	 
		va_start (argptr, format);
		vsprintf (temp_buffer, format,argptr);
		va_end (argptr);
	 
		if ((len = strlen(temp_buffer)) >= 1024)
		{
			return dst;
		}
	 
		strncat(dst, temp_buffer, len+1);
		return dst;
	}
			
	static int stricmpn (const char *s1, const char *s2, int n)
	{
		int		c1, c2;
		// bk001129 - moved in 1.17 fix not in id codebase
			if ( s1 == NULL ) {
			   if ( s2 == NULL )
				 return 0;
			   else
				 return -1;
			}
			else if ( s2==NULL )
			  return 1;	
		do {
			c1 = *s1++;
			c2 = *s2++;
			if (!n--) {
				return 0;		// strings are equal until end point
			}
			
			if (c1 != c2) {
				if (c1 >= 'a' && c1 <= 'z') {
					c1 -= ('a' - 'A');
				}
				if (c2 >= 'a' && c2 <= 'z') {
					c2 -= ('a' - 'A');
				}
				if (c1 != c2) {
					return c1 < c2 ? -1 : 1;
				}
			}
		} while (c1);

		return 0;		// strings are equal
	}



	static char *substr(const char *pstr, int start)
	{
		return substr(pstr, start, strlen(pstr) - start);
	}
	static char *substr(const char *pstr, int start, int numchars)
	{
		char *pnew = (char*)malloc(numchars+1);
		strncpy(pnew, pstr + start, numchars);
		pnew[numchars] = 0;
		return pnew;
	}

	static int strpos(char *str, char *target)
	{
	   char *res = strstr(str, target); 
	   if (res == NULL) return -1;
	   else             return res - str;
	}

	static int strpos(char *str, char *target, int start)
	{
	   char *tmp = str;
	   int x = 0;
	   while (x<start)
	   {
		   *tmp++;
		   x++;
	   }
	   char *res = strstr(tmp, target); 
	   if (res == NULL) return -1;
	   else             return res - str;
	}
	
};
#endif
