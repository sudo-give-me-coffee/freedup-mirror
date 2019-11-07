/*
This file is part of FreeDup.

    FreeDup -- search for duplicate files in one or more directory hierarchies
    Copyright (C) 2007,2008 Andreas Neuper

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
/* freedup was written by AN <AN@freedup.org>  */
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

const int mytest(const unsigned char*a)
{
    if(a==NULL) return 0;
    return 0;
}

const unsigned long mystart(const char*a)
{
    if(a==NULL) return 0;
    return 0;
}

const unsigned long myend(const char*a)
{
    if(a==NULL) return 0;
    return 0;
}

const unsigned long mysize(const char*a)
{
    static struct stat astat;
    if(lstat(a,&astat) != 0)
    {
	perror("lstat() failed while reading file statistics");
	return(0);
    }
    return  (unsigned long)astat.st_size;
}
