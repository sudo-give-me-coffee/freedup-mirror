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
#ifndef WEB_H
#define WEB_H 1
/* freedup was written by AN <AN@freedup.org>  */
void dolisten(void);
const char*show_header(int http,int type);
int process(int out,const char*);

enum procreqtype {
     PROC_UNSET = 0,
     PROC_GET   = 1,
     PROC_PUT   = 2,
     PROC_POST  = 3
};

/* FORTESTINGONLY */
char* webtrans(char*input);
const char*answer(char*in);
/* old webinterface for inetd */
int weboffer(const char*inputfile);
int webconfig(const char*inputfile);
#endif /*WEB_H*/
