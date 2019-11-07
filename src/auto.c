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
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "auto.h"
#include "my.h"
#include "mp3.h"
#include "mp4.h"
#include "mpc.h"
#include "ogg.h"
#include "jpg.h"
#define	MAXFULLTESTBUFFER	32

extratab extra[] = {
     { "none",   mysize,   mystart,   myend,   mytest },
     { "auto", autosize, autostart, autoend, autotest },
     { "jpg",   jpgsize,  jpgstart,  jpgend,  jpgtest },
     { "mp3",   mp3size,  mp3start,  mp3end,  mp3test },
     { "mp4",   mp4size,  mp4start,  mp4end,  mp4test },
     { "mpc",   mpcsize,  mpcstart,  mpcend,  mpctest },
     { "ogg",   oggsize,  oggstart,  oggend,  oggtest },
/*  planned modules: for other multimedia data	*/
     {  NULL,  }	
};

char tag1char='B';	/* Begin */
char tag2char='E';	/* End */

const int autofulltest(const char*a)
{
    size_t count;
    unsigned char buffer[MAXFULLTESTBUFFER];
    FILE*A=fopen(a,"rb");
    if(A==NULL) return 0;
    count=fread(buffer,sizeof(char),MAXFULLTESTBUFFER,A);
    if( fclose(A)!=0 ) perror("Error closing file.");
    return autotest(buffer);
}

const int autotest(const unsigned char*buffer)
{
    int n;
    for(n=2;extra[n].type!=NULL;n++)
    {
	if( extra[n].test(buffer) !=0 )
	{
	    return n;
	}
    }
    return 0;
}

const unsigned long autostart(const char*a)
{
    return extra[autofulltest(a)].start(a);
}

const unsigned long autoend(const char*a)
{
    return extra[autofulltest(a)].end(a);
}

const unsigned long autosize(const char*a)
{
    int localextratyp=autofulltest(a);
    static struct stat astat;
    if(lstat(a,&astat) != 0)
    {
	perror("lstat() failed while reading file statistics");
	return(0);
    }
    return  (unsigned long)astat.st_size - (extra[localextratyp].start(a) + extra[localextratyp].end(a));
}

#if defined(AUTO_TEST)
int main(int a, char*v[])
{
    int cnt,end,typ;
    char command[256];
    if(a<2) { fprintf(stderr,"File Argument missing.\n"); exit(-1); }
    typ=autofulltest(v[1]);
    if(typ!=0) { fprintf(stderr,"File Argument of type %s.\n", extra[typ].type ); }
    cnt=autostart(v[1]);
    end=autoend(v[1]);
    if(a>1) fprintf(stderr,"%s: [ %ld ] %d %d \n",v[1],autosize(v[1]),cnt,end);
    strcpy(command,"./");
    strcat(command,extra[typ].type);
    strcat(command,"test ");
    strcat(command,v[1]);
    system(command);
    return 0;
}
#endif
