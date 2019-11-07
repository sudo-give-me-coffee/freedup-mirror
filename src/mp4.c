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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "auto.h"

const int    mp4test(const unsigned char*buffer)
{
    /* Range used: 4-8 */
    /* MP4 Header */
    if( buffer[4]==((unsigned char)'f') && buffer[5]==((unsigned char)'t') 
     && buffer[6]==((unsigned char)'y') && buffer[7]==((unsigned char)'p') ) return 1;
    if( buffer[4]==((unsigned char)'m') && buffer[5]==((unsigned char)'o') 
     && buffer[6]==((unsigned char)'o') && buffer[7]==((unsigned char)'v') ) return 1;
    /* failed */
    return 0;
}

const int    mp4fulltest(const char*a)
{
    size_t count;
    unsigned char buffer[32];
    FILE*A = fopen(a,"rb");
    if(A==NULL) return 0;
    count = fread(buffer,sizeof(char),32,A);
    if( fclose(A)!=0 ) perror("Error closing mp4 file.");
    return mp4test(buffer);
}

const unsigned long mp4start(const char*a)
{
    size_t retval,extend=8;
    unsigned long size,count=0;
    unsigned char buffer[32];
    FILE*A=NULL;
    if( (A=fopen(a,"rb"))==NULL ) return 0;
    retval = fread( buffer, sizeof(char), extend, A );
    tag1char='S';
    if( (retval<extend) || mp4test(buffer)==0 ) return 0;
    while( strncmp( (char*)&buffer[4], "ftyp", 4 )==0
	|| strncmp( (char*)&buffer[4], "moov", 4 )==0
	|| strncmp( (char*)&buffer[4], "free", 4 )==0
	|| strncmp( (char*)&buffer[4], "wide", 4 )==0
	|| strncmp( (char*)&buffer[4], "meta", 4 )==0 )
	/* "mdat" contains the encoded song */
    {
if(0) fprintf(stderr,"%s> %d %d\n",a,(int)extend,(int)retval);
	size = (unsigned long)((buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3]); 
	if( fseek( A, (long)size-8L, SEEK_CUR ) || (size<=8) ) return 0;
	count+=size;
    	retval=fread(buffer,sizeof(char),8,A);
   	if( retval < 8 ) return 0; /* header might end at eof */
    }
    if( fclose(A)!=0 ) perror("Error closing mp4 file.");
    return count;
}

const unsigned long mp4end(const char*a)
{
    static struct stat xstat;
    size_t retsval,extend=8;
    unsigned long size,retval=0;
    unsigned char buffer[24];
    FILE*A=NULL;
    buffer[8]=((unsigned char)0);
    if( stat( a, &xstat) != 0 ) return 0;
    if( (A=fopen(a,"rb"))==NULL ) return 0;
    tag2char='F';
    retsval = fread( buffer, sizeof(char), extend, A );
    if( (retsval<extend) || mp4test(buffer)==0 ) return 0;
    while(feof(A)==0)
    {
	size = (unsigned long)((buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3]); 
#if defined(MP4_TEST)
	{   /* debugging */
	    static char text[8];
            strncpy( text, (char*)&buffer[4], 4 );
	    text[4]=0;
	    fprintf(stderr,"%s:%8ld\n",text,size);
	}
#endif
	if( strncmp((char*)&buffer[4],"mdat",4)==0 )
	{
	    retval=((unsigned long)ftell(A))+size-8UL;
	}
	if( fseek( A, (long)size-8L, SEEK_CUR )!=0 || (size<=8UL) )
	{
	    if( fclose(A)!=0 ) perror("Error closing mp4 file.");
	    return 0;
	}
        if( fread(buffer,sizeof(char),8,A)<8 )
	{
	    if( fclose(A)!=0 ) perror("Error closing mp4 file.");
	    return ((unsigned long)xstat.st_size - retval);
	}
    }
    if( fclose(A)!=0 ) perror("Error closing mp4 file.");
    return ( (unsigned long)xstat.st_size-retval);
}

const unsigned long mp4size(const char*a)
{
    static struct stat xstat;
    if( stat( a, &xstat) == 0 )
    {
	return(  (unsigned long)xstat.st_size - (mp4start(a)+mp4end(a)) );
    } else
    return 0;
}

#if defined(MP4_TEST)
#include <assert.h>
char tag1char, tag2char;

int main(int a, char*v[])
{
    size_t n,cnt,end;
    char buffer[204800];
    FILE*A;
    if(a<2) { fprintf(stderr,"File Argument missing.\n"); exit(-1); }
    if(mp4fulltest(v[1])!=1) { fprintf(stderr,"File Argument not of type mp4.\n"); exit(-1); }
    cnt=mp4start(v[1]);
    end=mp4end(v[1]);
    if(a>1) fprintf(stderr,"%s: [ %ld ] %d %d \n",v[1],mp4size(v[1]),cnt,end);
    A=fopen(v[1],"rb");
    assert(A!=NULL);
    if( fseek(A, cnt, SEEK_SET)!=0 ) perror("Error positioning in mp4 file.");
    n=fread(buffer,sizeof(char),81920,A);
    if( fclose(A)!=0 ) perror("Error closing mp4 file.");
    A=fopen("/tmp/test.mp4","wb");
    assert((A!=NULL));
    fwrite(buffer,sizeof(char),81920,A);
    if( fclose(A)!=0 ) perror("Error closing mp4 file.");
    system("file /tmp/test.mp4");
    return 0;
}
#endif
