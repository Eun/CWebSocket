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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if ( defined (_WIN32) || defined (_WIN64) )
	#pragma warning (disable : 4996)
#endif

char *strcatf(char *dst, char *format, ...);
int stricmpn (const char *s1, const char *s2, int n);
char *substr(const char *pstr, int start);
char *substr(const char *pstr, int start, int numchars);
int strpos(char *str, char *target);
int strpos(char *str, char *target, int start);
