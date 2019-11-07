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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include "auto.h"
#define	OGGTAILSIZE	1024
#define	OGGHEADSIZE	8192
/*
 * Information sources:
 *	http://en.wikipedia.org/wiki/Ogg
 * 	http://en.wikipedia.org/wiki/Comparison_of_container_formats
 */

const int    oggtest(const unsigned char*buffer)
{
    /* Range used: 0-3 */
    if( buffer[0]==((unsigned char)'O')  && buffer[1]==((unsigned char)'g')
     && buffer[2]==((unsigned char)'g')  && buffer[3]==((unsigned char)'S') ) return 1;
    return 0;
}

const int    oggfulltest(const char*a)
{
    size_t count;
    unsigned char buffer[32];
    FILE*A=fopen(a,"rb");
    assert(A!=NULL);
    count=fread(buffer,sizeof(char),10,A);
    if( fclose(A)!=0 ) perror("Error closing ogg file.");
    return oggtest(buffer);
}

const unsigned long oggstart(const char*a)
{
    size_t count,read=40;
    unsigned long n;
    char buffer[OGGHEADSIZE];
    FILE*A;
    A=fopen(a,"rb");
    if( (count=fread(buffer,sizeof(char),read,A)) < read ) return(0);
    tag1char='H';
    if( buffer[0]=='O' && buffer[1]=='g' && buffer[2]=='g' && buffer[3]=='S')
    {
        count+=fread(buffer,sizeof(char),OGGHEADSIZE,A);
        if( fclose(A)!=0 ) perror("Error closing ogg file.");
        do 
	    for(n=0; n<OGGHEADSIZE && !(buffer[n]=='\001' && buffer[n+1]=='\005');n++);
        while( strncmp(&buffer[n+2],"vorbis",6)==0 && strncmp(&buffer[n+9],"BCV",3)!=0 );
	return(n+40);
    }
    return 0;
}

const unsigned long oggend(const char*a)
{
    size_t rdsize=0;
    char buffer[OGGTAILSIZE],*ptr=NULL;
    FILE*A=NULL;
    A=fopen(a,"rb");
    if( A==NULL ) return(0);
    if( fseek(A, -OGGTAILSIZE, SEEK_END) != 0) return 0;
    tag1char='T';
    rdsize=fread(buffer,sizeof(char),OGGTAILSIZE,A);
    if( fclose(A)!=0 ) perror("Error closing ogg file.");
    ptr=buffer;
    while( (ptr=strchr(ptr,'O')) != NULL )
    {
	if( strncmp(ptr,"OggS",4)!=0 ) return((unsigned long)rdsize-(ptr-buffer));
    }
    return 0;
}

const unsigned long oggsize(const char*a)
{
    static struct stat xstat;
    if( stat( a, &xstat) == 0 )
    {
	return (  (unsigned long)xstat.st_size - (oggstart(a)+oggend(a)) );
    } else
    return 0;
}

#if defined(OGG_TEST)
char tag1char, tag2char;

int main(int a, char*v[])
{
    int n,cnt,end;
    char buffer[8200000];
    FILE*A,*B;
    cnt=oggstart(v[1]);
    end=oggend(v[1]);
    if(oggfulltest(v[1])!=1) { fprintf(stderr,"File Argument not of type ogg.\n"); exit(-1); }
    if(a>1) fprintf(stderr,"%s: %d %d\n",v[1],cnt,end);
    A=fopen(v[1],"rb");
    B=fopen("/tmp/test.ogg","wb");
    n=fread(buffer,sizeof(char),cnt,A);
    n=fread(buffer,sizeof(char),8192000,A);
    fwrite(buffer,sizeof(char),n,B);
    if( fclose(A)!=0 ) perror("Error closing ogg file.");
    if( fclose(B)!=0 ) perror("Error closing ogg file.");
    system("mpg321 /tmp/test.ogg");
    return 0;
}
#endif
