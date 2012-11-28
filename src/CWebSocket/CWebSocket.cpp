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
// VERSION: 1.7

// TODO: cookiefile support       
//       http 1.1 + unchunk
//		 auth
//		 HEADER output
// Changelog:
//  1.7:
//		* added openssl, nocookies preprocessor
//		* if openssl is disabled fallback to http
//		* fixed mem leak
//  1.6:
//		 * added https support via openssl
//		 * removed opt.HEADERS
//  1.5:
//       * changed COKIEFILE to COOKIEFILE
//		 * added cleanup method
//		 * changed internal constans
//		 * added cleanup for cookiemanager
//		 * added bind to interface
//  1.4:
//		 * changed from malloc to new
//  1.3:
//		 * fixed output overflow
//  1.2:
//		 * Added outputfile option
//       * using byte copys now 
//	1.1:
//       * Added Content-Length Header
#include "CWebSocket.h"

// ERRORCODES:
//  1:		Failed to init
//  2:		Failed to lookup host
//  3:		Failed to connect
//  4:		Failed to send
//  5:		Failed to open outputfile
//  6:		Failed to bind to interface
//  7:		SSL Context Failed
//  8:		SSL Handle Failed
//  9:		SSL Socket align failed
// 10:		SSL Connect Failed




CWebSocket::CWebSocket(void)
{	
	this->nErrorCode = 0;

#if ( defined (_WIN32) || defined (_WIN64) )
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if ( err != 0)
	{
		this->nErrorCode = 1;
	}
#endif

	// Set Default Settings
	memset(opt.URL, 0, sizeof(opt.URL));
	opt.RETURNTRANSFER = true;
	memset(opt.POSTDATA, 0, sizeof(opt.POSTDATA));
	opt.AUTOFOLLOWLOCATION = true;
	#ifndef NO_COOKIES
		memset(opt.COOKIEFILE, 0, sizeof(opt.COOKIEFILE));
	#endif
	memset(opt.USERAGENT, 0, sizeof(opt.USERAGENT));
	memset(opt.REFERER, 0, sizeof(opt.REFERER));
	memset(opt.LANGUAGE, 0, sizeof(opt.LANGUAGE));
	opt.TIMEOUT = 10000;
	#ifndef NO_COOKIES
		opt.ACCEPTCOOKIES = false;
		opt.Cookies = NULL;
	#endif
	memset(opt.OUTPUTFILE, 0, sizeof(opt.OUTPUTFILE));
	memset(opt.BINDIP, 0, sizeof(opt.BINDIP));
}


char *CWebSocket::GetHostIP(char *host)
{
	hostent *remoteHost = gethostbyname(host);
	if (remoteHost == NULL)
	{
		this->nErrorCode = 2;
		return NULL;
	}
	else if (remoteHost->h_addrtype == AF_INET)
	{
		struct in_addr addr;
		addr.s_addr = *(u_long *) remoteHost->h_addr;
		return inet_ntoa(addr);
	}
	this->nErrorCode = 2;
	return NULL;
}

void CWebSocket::SplitUrl(char *url, char *protocol, char *host, char *uri)
{
	char	*purl = url;
	unsigned char bhost = 0;

	memset(protocol, 0, sizeof(protocol));
	memset(host, 0, sizeof(host));
	memset(uri, 0, sizeof(uri));

	uri[0] = '/';
	
	// protocol
	int n = 0;
	strcpy(protocol, purl);
	do
	{
		if (*purl == ':')
		{
			bhost = 1;
			protocol[n] = 0;
		}
		else if (bhost == 1 && *purl == '/')
		{
			bhost = 2;
		}
		else if (bhost == 2 && *purl == '/')
		{
			*purl++;
			break;
		}
		n++;
	}
	while (*purl++);

	if (bhost < 2)
		purl = url;

	// host and uri
	strcpy(host, purl);
	bhost = false;
	n = 0;
	do
	{
		if (*purl == '/' && bhost == false)
		{
			host[n] = 0;
			bhost = true;
		}
		else if (bhost)
		{
			sprintf(strchr(uri, '\0'), "%c", *purl); 
		}
		n++;
	}
	while (*purl++);
}


void CWebSocket::GetPath(char *uri, char *path)
{
	strcpy(path, uri);
	char *pch = strrchr(path, '/');
	path[pch-path+1] = 0;

	// make sure there is at least a /
	if (!path[0])
	{
		path[0] = '/';
	}
}



CWebSocket::headerInfoT CWebSocket::parse_headers(char *data, bool bSSL)
{
	headerInfoT headerInfo;
	char *headers = data;

	// split the headers into single lines

	unsigned int crlf = 0;
	unsigned int n = 0;
	unsigned int l = 0;

	headerInfo.status = 0; 
	memset(headerInfo.location, 0, CWEB_MAX_URL);
	do
	{
		if (*headers == '\r')
			crlf = 1;
		else if (*headers == '\n' && crlf == 1)
		{
			if  (n-l-1 > 0)
			{
				char *headerline = substr(data, l, n-l-1);
				if (!strncmp(headerline, "HTTP/", 5))
				{
					char *tmp = substr(headerline, 9, 3);
					headerInfo.status = atoi(tmp);
					free(tmp);
				}
				#ifndef NO_COOKIES
					else if (!stricmpn(headerline, "set-cookie:", 11) && opt.ACCEPTCOOKIES)
					{
							if (opt.Cookies == NULL)
							{
								opt.Cookies = new CCookieManager();
							}
							opt.Cookies->CookieAddHTTP(headerline);
					}
				#endif
				else if (!stricmpn(headerline, "location:", 9) && opt.AUTOFOLLOWLOCATION)
				{
					char *tmp = substr(headerline, 10);
					if (
						!stricmpn(tmp, "http://", 7) 
						#ifdef OPENSSL
							|| !stricmpn(tmp, "https://", 8)
						#endif
						)
					{
						strcpy(headerInfo.location, tmp);
					}
					else
					{	
						#ifdef OPENSSL
						if (bSSL)
						{
							sprintf(headerInfo.location, "https://%s%s%s", sHost, sPath, tmp);
						}
						else
						#endif
						{
							sprintf(headerInfo.location, "http://%s%s%s", sHost, sPath, tmp);
						}
					}

					free(tmp);
				}
				else if (!stricmpn(headerline, "content-length:", 15))
				{
					char *tmp = substr(headerline, 16);
					headerInfo.length = atoi(tmp);
					free(tmp);
				}
				/*else if (!strnicmp(headerline, "transfer-encoding: chunked", 26) && opt.UNCHUNK)
				{
					headerInfo.chunked = true;
				}*/
				free(headerline);
				
				
				crlf = 0;
				l = n+1;
			}
		}
		else
		{
			crlf = 0;
		}
		n++;
	}
	while (*headers++);
	return headerInfo;
}

/*
void CWebSocket::unchunkHTTP11(char *data)
{
	int pos = strpos(data, "\r\n", 0);
	int start = 0;
	char *out = (char*)malloc(strlen(data) * sizeof(char*));
	memset(out, 0, sizeof(out));
	while (pos > -1)
	{
		long dec;
		char *buf = substr(data, start, pos - start);
		dec = strtol(buf, NULL, 16);
		start += strlen(buf) + dec + 4;
		free(buf);

		if (dec == 0) break;

		buf = substr(data, pos+2, dec);
		strcat(out, buf);
		free(buf);

		pos = strpos(data, "\r\n",  start);
	}
	strcpy(data, out);
	data[strlen(out)] = 0;
	free(out);
}
*/

char* CWebSocket::exec(void)
{
	if (this->nErrorCode == 0)
	{
		pOutput = NULL;
		//char ContentLength[128] = "";
		char request[512] = "";
		bool bSSL;
		
		FILE * pFile;
		if (opt.OUTPUTFILE[0])
		{
			pFile = fopen (opt.OUTPUTFILE,"wb+");
			if (pFile==NULL)
			{
				this->nErrorCode = 5;
				return false;
			}
		}


		SplitUrl(opt.URL, sProtocol, sHost, sUri);
		GetPath(sUri, sPath);
				
		#ifdef OPENSSL
			if (!stricmp(sProtocol, "https"))
				bSSL = true;
			else
				bSSL = false;
		#else
			bSSL = false;
		#endif
				
		

		memset(sHostIP, 0, sizeof(sHostIP));

		char *pHostIP = GetHostIP(sHost);
		if (!pHostIP)
		{
			this->nErrorCode = 2;
			return false;
		}

		strcpy(sHostIP, pHostIP);
		if (opt.POSTDATA[0])
			sprintf(request, "POST %s HTTP/1.0\r\n", sUri);
		else
			sprintf(request, "GET %s HTTP/1.0\r\n", sUri);

			
		strcatf(request, "Host: %s\r\n", sHost);
		
		if (opt.REFERER[0])
			strcatf(request, "Referer: %s\r\n", opt.REFERER); 
			
		if (opt.USERAGENT[0])
			strcatf(request, "User-Agent: %s\r\n", opt.USERAGENT);

		if (opt.LANGUAGE[0])
			strcatf(request, "Accept-Language: %s\r\n", opt.LANGUAGE);

		if (opt.TIMEOUT == 0)
			opt.TIMEOUT = 1;

	
		
		// load cookie array if needed
		#ifndef NO_COOKIES
			if (opt.Cookies && opt.Cookies->GetCookieCount() > 0)
			{
				opt.Cookies->BuildRequest(request);
			}
		#endif

		if (opt.POSTDATA[0])
		{
			
			strcat(request, "Content-Type: application/x-www-form-urlencoded\r\n");
			strcatf(request, "Content-length: %d\r\n\r\n%s", strlen(opt.POSTDATA), opt.POSTDATA);
		}
		else
			strcat(request, "Connection: close\r\n\r\n");



		SSL *sslHandle = NULL;
		#ifdef OPENSSL
			SSL_CTX *sslContext = NULL;
		#endif
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		
		struct sockaddr_in serv_addr;

		if (sock == INVALID_SOCKET)
		{
			this->nErrorCode = 1;
			return false;
		}

			
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;

		
#if ( defined (_WIN32) || defined (_WIN64) )
		serv_addr.sin_addr.S_un.S_addr = inet_addr(sHostIP);
#else
		inet_aton(sHostIP, &serv_addr.sin_addr);	
#endif

		#ifdef OPENSSL
			if (bSSL)
				serv_addr.sin_port = htons(443);	
			else
				serv_addr.sin_port = htons(80);

		#else
			serv_addr.sin_port = htons(80);
		#endif	
		
		if (strlen(opt.BINDIP) > 0)
		{
			struct sockaddr_in local_addr;
			
			memset(&local_addr, 0, sizeof(local_addr));	
			local_addr.sin_family = AF_INET;
		
			#if ( defined (_WIN32) || defined (_WIN64) )
				local_addr.sin_addr.S_un.S_addr = inet_addr(opt.BINDIP);
			#else
				inet_aton(opt.BINDIP, &local_addr.sin_addr);	
			#endif
			
			if (bind(sock, (SOCKADDR*)&local_addr, sizeof(local_addr)) < 0)
			{
				this->nErrorCode = 6;
				return false;		
			}
		}

		if (connect(sock, (SOCKADDR*)&serv_addr, sizeof(serv_addr)) < 0) 
		{
			this->nErrorCode = 3;
			return false;
		}

		#ifdef OPENSSL
		if (bSSL)
		{
			SSL_load_error_strings();
			SSL_library_init();
			sslContext = SSL_CTX_new (SSLv23_client_method());
			if (sslContext == NULL)
			{
				this->nErrorCode = 7;
				closesocket(sock);
				return false;
			}
			sslHandle = SSL_new (sslContext);
			if (sslHandle == NULL)
			{
				if (sslContext)
				{
					SSL_CTX_free (sslContext);
				}
				this->nErrorCode = 8;
				closesocket(sock);
				return false;
			}
			if (!SSL_set_fd (sslHandle, sock))
			{
				closesocket(sock);
				if (sslHandle)
				{
					SSL_shutdown(sslHandle);
					SSL_free(sslHandle);
				}
				if (sslContext)
				{
					SSL_CTX_free (sslContext);
				}
				this->nErrorCode = 9;
				return false;
			}
			
			if (SSL_connect (sslHandle) != 1)
			{
				closesocket(sock);
				if (sslHandle)
				{
					SSL_shutdown(sslHandle);
					SSL_free(sslHandle);
				}
				if (sslContext)
				{
					SSL_CTX_free (sslContext);
				}
				this->nErrorCode = 10;
				return false;
			}
			int bytes_send = SSL_write(sslHandle, request, strlen(request));
			if (bytes_send < 0)
			{
				closesocket(sock);
				if (sslHandle)
				{
					SSL_shutdown(sslHandle);
					SSL_free(sslHandle);
				}
				if (sslContext)
				{
					SSL_CTX_free (sslContext);
				}
				this->nErrorCode = 4;
				return false;
			}
		}
		else
		#endif
		{
			int bytes_send = send(sock, request, strlen(request), 0);
			if (bytes_send < 0)
			{
				closesocket(sock);
				this->nErrorCode = 4;
				return false;
			}
		}
		int bytes_read;

		char buffer[CWEB_MAX_RECIVE]="";
		char *out = NULL;
		int outsize = 0;
		

		bool bHeadersPassed = false;
		char *sHeaderData;
		int headerPos;
		headerInfoT headerInfo;
		int iOptVal = this->opt.TIMEOUT;
		int iOptLen = sizeof(int);

		headerInfo.status = 0;
		//headerInfo.chunked = false;

		


		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iOptVal, iOptLen);
		while ((bytes_read = readfromsocket(sock, sslHandle, buffer, CWEB_MAX_RECIVE-1)) > 0)
		{
			if (iOptVal > 1)
			{
				iOptVal = 10;
				setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&iOptVal, iOptLen);
			}
			char *newBuffer;
			newBuffer = new char[bytes_read + outsize + 1];

			memset(newBuffer, 0, bytes_read + outsize + 1);
			if (out != NULL)
			{
				memcpy(newBuffer, out, outsize);
				delete [] out;
			}
			out = newBuffer;
			//

			memcpy(out + outsize, buffer, bytes_read);
			outsize+=bytes_read;

	
			if (!bHeadersPassed)
			{
				headerPos = strpos(out,"\r\n\r\n");
				if (headerPos > 0)
				{
					headerPos += 4;
					sHeaderData = (char*) malloc(sizeof(char) * (headerPos + 1));
					strncpy(sHeaderData, out, headerPos);
					sHeaderData[headerPos] = 0;
					bHeadersPassed = true;
					headerInfo = parse_headers(sHeaderData, bSSL);

					if (opt.AUTOFOLLOWLOCATION && headerInfo.location[0])
					{
						strcpy(opt.REFERER, opt.URL);
						strcpy(opt.URL, headerInfo.location);
						
						closesocket(sock);
						#ifdef OPENSSL
							if (sslHandle)
							{
							  SSL_shutdown (sslHandle);
							  SSL_free (sslHandle);
							}
							if (sslContext)
							{
								SSL_CTX_free (sslContext);
							}
						#endif
						if (out)
						{
							delete [] out;
						}
						return this->exec();
					}
					else
					{
						strcpy(headerInfo.location, opt.URL);
					}
					free(sHeaderData);		

					// copy the new content
					memcpy(out, out + headerPos, bytes_read - headerPos);
					bytes_read = bytes_read - headerPos;
					outsize = bytes_read;
				}
			}
			
		
			if (opt.OUTPUTFILE[0])
			{
				
				fwrite(out, bytes_read, 1, pFile);
				delete [] out;
				out = NULL;
				outsize = 0;
			}

			memset(buffer, 0, CWEB_MAX_RECIVE);
		}

		closesocket(sock);
		#ifdef OPENSSL
			if (sslHandle)
			{
				SSL_shutdown (sslHandle);
				SSL_free (sslHandle);
			}
			if (sslContext)
			{
				SSL_CTX_free (sslContext);
			}
		#endif

		ret.bytes = outsize;
		ret.status = headerInfo.status;
		strcpy(ret.url, headerInfo.location);

		if (opt.OUTPUTFILE[0])
		{
			fclose(pFile);
		}

		if (opt.RETURNTRANSFER && !opt.OUTPUTFILE[0])
		{
			// null terminate
			if (out)
			{
				out[outsize] = 0;
			}
			pOutput = out;
			return out;
		}
		else
		{
			if (out)
			{
				delete [] out;
			}
			return false;
		}
	}
	return false;
}

int CWebSocket::readfromsocket(SOCKET sock, SSL *sslHandle, char *buffer, int len)
{
	#ifdef OPENSSL
		if (sslHandle != NULL)
			return SSL_read (sslHandle, buffer, CWEB_MAX_RECIVE-1);
		else
			return recv(sock, buffer, CWEB_MAX_RECIVE-1, 0);
	#else
		return recv(sock, buffer, CWEB_MAX_RECIVE-1, 0);
	#endif
}

void CWebSocket::cleanup(void)
{
	cleanup(CWEB_ALL);
}
void CWebSocket::cleanup(int flags)
{
	if (pOutput != NULL)
	{
		delete [] pOutput;
		pOutput = NULL;
	}
	#ifndef NO_COOKIES
		if (flags != CWEB_KEEPCOOKIES)
		{
			if (opt.Cookies != NULL)
			{
				delete [] opt.Cookies;
				opt.Cookies = NULL;
			}
		}
	#endif
}
