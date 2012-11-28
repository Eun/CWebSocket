/*
    CCookieManager part of CWebSocket
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
#ifndef INCLUDED_CCOOKIEMANAGER
#define INCLUDED_CCOOKIEMANAGER
#ifndef NO_COOKIES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CWebSocket.h"

#define COOKIE_SIMPLE
#define COOKIE_MAX_BYTES	768
#define COOKIE_MAX_NAME		128
#define COOKIE_MAX_VALUE	128
#define COOKIE_MAX_EXPIRES	128
#define COOKIE_MAX_PATH		128
#define COOKIE_MAX_DOMAIN	128

class CCookieManager
{
private:
	char **cVar;
	char **cValue;
	char **cExpires;
	char **cPath;
	char **cDomain;
	unsigned int nCookies;
	int CookieExists(char *varname);
public:
	CCookieManager(void);
	~CCookieManager(void);
	bool CookieAdd(char *name, char *value, char *expires, char *path, char *domain);
	bool CookieAddHTTP(char *data);
	char *BuildRequest(char *out);
	unsigned int GetCookieCount(void);
	void DebugOutput(void);
	void cleanup(void);
};
#endif
#endif
