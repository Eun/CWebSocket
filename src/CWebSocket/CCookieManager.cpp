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
#include "CCookieManager.h"


CCookieManager::CCookieManager(void)
{	
	nCookies = 0;
	cVar = NULL;
	cValue = NULL;
	cExpires = NULL;
	cPath = NULL;
	cDomain = NULL;
}

CCookieManager::~CCookieManager(void)
{
	cleanup();
}


int CCookieManager::CookieExists(char *varname)
{
	for (unsigned int i = 0; i < nCookies; ++i)
	{
		if (!stricmpn(varname, cVar[i], COOKIE_MAX_NAME))
		{
			return i;
		}
	}
	return -1;
}

bool CCookieManager::CookieAdd(char *name, char *value, char *expires, char *path, char *domain)
{
	int pos = 0;

	if (!name)
		return false;

	if (nCookies > 0  && (pos = CookieExists(name)) > -1)
	{
		// modify existing field
		strcpy(cValue[pos], value);

#ifndef COOKIE_SIMPLE
		if (expires)
			strcpy(cExpires[pos], expires);
		else if (cExpires[pos])
			free(cExpires[pos]);

		if (path)
			strcpy(cPath[pos], path);
		else if (cPath[pos])
			free(cPath[pos]);

		if (domain)
			strcpy(cDomain[pos], domain);
		else if (cDomain[pos])
			free(cDomain[pos]);
#endif

		return true;
	}
	else
	{
		// add a new field && resize
		pos = nCookies;
		++nCookies;
		// if we got no cookies create the arrays
		if (nCookies == 0)
		{
			if (((cVar = (char**)malloc(sizeof(char**))) == NULL) ||
				((cValue = (char**)malloc(sizeof(char**))) == NULL)
#ifndef COOKIE_SIMPLE
				||
				((cExpires = (char**)malloc(sizeof(char**))) == NULL) || 
				((cPath = (char**)malloc(sizeof(char**))) == NULL) || 
				((cDomain = (char**)malloc(sizeof(char**))) == NULL)
#endif
			   )
			{
				throw "Memory allocation failed!";
				return false;
			}
		}
		else
		{
			if (((cVar = (char**)realloc(cVar, sizeof(char**) * nCookies)) == NULL) ||
				((cValue = (char**)realloc(cValue, sizeof(char**) * nCookies)) == NULL)
#ifndef COOKIE_SIMPLE
				||
				((cExpires = (char**)realloc(cExpires, sizeof(char**) * nCookies)) == NULL) || 
				((cPath = (char**)realloc(cPath, sizeof(char**) * nCookies)) == NULL) || 
				((cDomain = (char**)realloc(cDomain, sizeof(char**) * nCookies)) == NULL)
#endif
			   )
			{
				throw "Memory allocation failed!";
				return false;
			}
		}


		if (((cVar[pos] = (char*)malloc(sizeof(char*) * COOKIE_MAX_NAME)) == NULL) ||
			((cValue[pos] = (char*)malloc(sizeof(char*) * COOKIE_MAX_VALUE)) == NULL)
#ifndef COOKIE_SIMPLE
			||
			((cExpires[pos] = (char*)malloc(sizeof(char*) * COOKIE_MAX_EXPIRES)) == NULL) || 
			((cPath[pos] = (char*)malloc(sizeof(char*) * COOKIE_MAX_PATH)) == NULL) || 
			((cDomain[pos] = (char*)malloc(sizeof(char*) * COOKIE_MAX_DOMAIN)) == NULL)
#endif
		   )
		{
			throw "Memory allocation failed!";
			return false;
		}


		// copy the data
		strcpy(cVar[pos], name);
		strcpy(cValue[pos], value);

#ifndef COOKIE_SIMPLE
		if (expires) strcpy(cExpires[pos], expires);
		else free(cExpires[pos]);
		if (path) strcpy(cPath[pos], path);
		else free(cPath[pos]);
		if (domain) strcpy(cDomain[pos], domain);
		else free(cDomain[pos]);
#endif
		return true;
	}
}

bool CCookieManager::CookieAddHTTP(char *data)
{
	bool isName = true;
	int start = 12;
	char *name; char *value; char *expires; char *path; char *domain;
	name = value = expires = path = domain = NULL;

	int pos = strpos(data, ";", start);
	while (pos > 0)
	{
		char *bump = substr(data, start, pos-start);

		int upos = strpos(bump, "=");

		if (upos > 0)
		{
			char *var = substr(bump, 0, upos);
			char *val = substr(bump, upos+1);

			if (isName)
			{
				if (((name = (char*)malloc(sizeof(char*) * COOKIE_MAX_NAME)) == NULL) ||
					((value = (char*)malloc(sizeof(char*) * COOKIE_MAX_VALUE)) == NULL))
				{
					throw "Memory allocation failed!";
					return false;
				}
				strcpy(name, var);
				strcpy(value, val);
				isName = false;

#ifdef COOKIE_SIMPLE
			free(var);
			free(val);
			break;
#endif

			}
#ifndef COOKIE_SIMPLE
			else if (!stricmpn(var, "expires", COOKIE_MAX_NAME))
			{
				expires = (char*)malloc(sizeof(char*) * COOKIE_MAX_EXPIRES);
				strcpy(expires, val);
			}
			else if (!stricmpn(var, "path", COOKIE_MAX_NAME))
			{
				path = (char*)malloc(sizeof(char*) * COOKIE_MAX_PATH);
				strcpy(path, val);
			}
			else if (!stricmpn(var, "domain", COOKIE_MAX_NAME))
			{
				domain = (char*)malloc(sizeof(char*) * COOKIE_MAX_DOMAIN);
				strcpy(domain, val);
			}
#endif

			free(var);
			free(val);
		}
	
		free(bump);
		start=pos+2;
		pos = strpos(data, ";", start);
	}
	
	bool retval = CookieAdd(name, value, expires, path, domain);
	free(name);
	free(value);
#ifndef COOKIE_SIMPLE
	free(expires);
	free(path);
	free(domain);
#endif
	return retval;
}

char *CCookieManager::BuildRequest(char *out)
{
	//memset(out, 0, sizeof(out));
	if (nCookies > 0)
	{
		strcat(out, "Cookie:");
		for (unsigned int i = 0; i < nCookies; ++i)
		{
			strcatf(out, " %s=%s%c", cVar[i], cValue[i] , (i < (nCookies-1)) ? ';' : '\0');
		}
	}
	return out;
}

unsigned int CCookieManager::GetCookieCount(void)
{
	return nCookies;
}

void CCookieManager::DebugOutput(void)
{
	if (nCookies > 0)
	{
		printf("CCookieManager: Cookies (%d):\n", nCookies);
		for (unsigned int i = 0; i < nCookies; ++i)
		{
			printf("CCookieManager: %d %s = %s\n", i, cVar[i], cValue[i]);
		}
	}
	else
	{
		printf("CCookieManager: No Cookies\n");
	}
}

void CCookieManager::cleanup(void)
{
// free all data
	for (unsigned int i = 0; i < nCookies; ++i)
	{
		free(cVar[i]);
		free(cValue[i]);
		#ifndef COOKIE_SIMPLE
			free(cExpires[i]);
			free(cPath[i]);
			free(cDomain[i]);;
		#endif
	}
	if (cVar != NULL)
	{
		free(cVar);
		free(cValue);
		cVar = cValue = NULL;
		#ifndef COOKIE_SIMPLE
			free(cExpires);
			free(cPath);
			free(cDomain); 
			cExpires = cPath = cDomain = NULL;
		#endif
	}
	nCookies = 0;
}
