/*
    SymHarden -- replaces a symlinked file by a hardlink to its target
    		 in the case the target exists and is not a directory
    Copyright (C) 2009 Andreas Neuper

    SymHarden is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SymHarden is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SymHarden.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#define	VERSION	"1.0.3"
/**********************VERSION HISTORY**********************
1.0.1 initial version
1.0.2 test for existence of temp-file
1.0.3 added some (unused) return values to avoid warnings
***********************************************************/

void usage(char*v)
{
    char*a=strrchr(v,'/');
    if(a!=NULL)a++;
    else a=v;
    fprintf(stderr,
	"%s Version " VERSION "\n\nSYNTAX:\t%s <SYMLINK>\n"
	"\treplace symlink by hardlink if possible\n"
#if 0
	"\treturn\t0 on success,\n"
	"\t\t1 on directories or plain files,\n"
	"\t\t2 on looped links,\n"
	"\t\t3 on nonexistent path or reference,\n"
	"\t\t4 on missing permissions,\n"
	"\t\t-1 on missing target\n"
#endif
	"\n",v,a);
    exit(-1);
    return;
}

char*temp=NULL;	/* must be global to allow atexit(restore) */
void restore(void)
{
    char*a=strdup(temp);
    a[strlen(a)-10]=0;
    rename(temp,a);
    /* might complain if executed in vain */
    /* added for security reasons only */
    free(temp);
}

char*mkmytemp(const char*v)
{
    struct stat a;
    do {
	temp=malloc(strlen(v)+10);
	strcpy(temp,v);
	strcat(temp,".s2hXXXXXX");
	mktemp(temp);
    } while(!lstat(temp,&a));
    atexit(restore);
    return temp;
}

char*linkto(const char*path)
{
    char*buf=NULL;
    size_t lgth=1024,lsz,ret;
    do {
        lgth<<=1;
	if(buf==NULL){free(buf);}
	buf=malloc(lgth);
        lsz=readlink(path,buf,lgth);
    }while( (lgth==lsz) );
    /* if buffer too short errno==ENAMETOOLONG condition would not fail! */
    if(errno!=0)
    {
	perror(path);
	switch(errno)
	{ case EINVAL:	ret=1;	break;
	  case ELOOP:	ret=2;	break;
	  case EACCES:
	  case EPERM:	ret=4;	break;
	  case ENOENT:
	  case ENOTDIR:	ret=3;	break;
	  default:	ret=-1;
	  /* for some reason these error codes are not correct in most cases */
	}
	exit(ret);
    } else
    buf[lsz]=0; /* is not terminated by default*/
    return buf;
}

int
main(int n , char*v[])
{
    char*target=NULL;

    if(n!=2) usage(v[0]);
    target=linkto(v[1]);
    mkmytemp(v[1]);
    rename(v[1],temp);
    if(link(target,v[1]) == 0)
    {
	unlink(temp);
        if(0) fprintf(stderr,"\nln \"%s\" \"%s\"\n",target,v[1]);
    } else
    {
	rename(temp,v[1]);
    }
    exit(0);
}
/*
 * Tested for
 *	early exit line --> atexit(restore)
 *	inexecutable link()
 *	intended temporary file name
 *	usage name handling
 * 	correct behaviour of restore()
 *	accepted warning for mktemp (mkstemp is no alternative)
 *	freeing allocated space of target (not allowed to)
 */
