/*
    part of CWebSocket
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
#include "helpers.h"
// helper functions
char *strcatf(char *dst, char *format, ...)
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
        
int stricmpn (const char *s1, const char *s2, int n)
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



char *substr(const char *pstr, int start)
{
	return substr(pstr, start, strlen(pstr) - start);
}
char *substr(const char *pstr, int start, int numchars)
{
	char *pnew = (char*)malloc(numchars+1);
	strncpy(pnew, pstr + start, numchars);
	pnew[numchars] = 0;
	return pnew;
}

int strpos(char *str, char *target)
{
   char *res = strstr(str, target); 
   if (res == NULL) return -1;
   else             return res - str;
}

int strpos(char *str, char *target, int start)
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
