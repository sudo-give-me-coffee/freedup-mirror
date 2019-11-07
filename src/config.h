/*
This file is part of FreeDup.

    FreeDup -- search for duplicate files in one or more directory hierarchies
    Copyright (C) 2007,2008,2009 Andreas Neuper

    FreeDup is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FreeDup is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeDup.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef CONFIG_H
#define CONFIG_H 1
/* freedup was written by AN <AN@freedup.org>  */

#define  StrNCmp(x,y)		strncmp( x, y, strlen(y) )
#define  StrNCaseCmp(x,y)	strncasecmp( x, y, strlen(y) )
#define  StrNTCaseCmp(x,y)	strncasecmp( &x[strlen(x)-strlen(y)], y, strlen(y) )

extern struct option long_options[32];
extern int*setting[];
void writeconfig(const char*env,const int argc,char*const argv[]);
int readconfig(const char*env);
int readconfigline(char*env);
const char**getconfigs(void);
void listconfigs(const char*const fmt);
#if 1
void showconfig(FILE*file);
char* showconfigstr(char*buffer);
#endif
#endif /*CONFIG_H*/
