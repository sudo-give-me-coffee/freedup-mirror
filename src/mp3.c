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
#include <sys/stat.h>
#include "auto.h"

const int    mp3test(const unsigned char*buffer)
{
    /* Range used: 0-2 */
    /* Tag Header */
    if( buffer[0]==((unsigned char)'I') && buffer[1]==((unsigned char)'D') 
     && buffer[2]==((unsigned char)'3') ) return 1;
    /* MP2/MP3 Header */
    if( buffer[0]==((unsigned char)'\377') && (buffer[1]&'\350')==((unsigned char)'\350') )    return 1;
    /* failed */
    return 0;
}

const int    mp3fulltest(const char*a)
{
    size_t count;
    unsigned char buffer[32];
    FILE*A=fopen(a,"rb");
    if(A==NULL) return 0;
    count=fread(buffer,sizeof(char),4,A);
    if( fclose(A)!=0 ) perror("Error closing mp3 file.");
    if(count<4) return 0;
    return mp3test(buffer);
}

const unsigned long mp3start(const char*a)
{
    unsigned long count=0;
    unsigned char buffer[10],version;
    FILE*A=fopen(a,"rb");
    if(A==NULL) return 0;
    count=fread(buffer,sizeof(char),10,A);
    /* later note: add really these 10 characters to the recoded value ? */
    /**/
    if( fclose(A)!=0 ) perror("Error closing mp3 file.");
    tag2char='2';
    if( buffer[0]==((unsigned char)'I') && buffer[1]==((unsigned char)'D') && buffer[2]==((unsigned char)'3') )
    {
        version= buffer[3];
	count += (unsigned long)(buffer[6]) << 21;
	count += (unsigned long)(buffer[7]) << 14; 
	count += (unsigned long)(buffer[8]) << 7; 
	count += (unsigned long)(buffer[9]); 
	return count;
    }
    return 0;
}

const unsigned long mp3end(const char*a)
{
    size_t n;
    unsigned char buffer[256];
    FILE*A=fopen(a,"rb");
    if(A==NULL) return 0;
    if( fseek(A, -128, SEEK_END)!=0 ) perror("Error positioning in mp3 file.");
    n=fread(buffer,sizeof(char),256,A);
    if( fclose(A)!=0 ) perror("Error closing mp3 file.");
    tag1char='1';
    if(n!=128) return 0;
    if( buffer[0] == ((unsigned char)'T') && buffer[1] == ((unsigned char)'A') 
     && buffer[2] == ((unsigned char)'G') )
    {   return  128;
    } else return 0;
}

const unsigned long mp3size(const char*a)
{
    static struct stat xstat;
    if( stat( a, &xstat) == 0 )
    {
	return ( (unsigned long)xstat.st_size - (mp3start(a)+mp3end(a)) );
    } else
    return 0;
}

#if defined(MP3_TEST)
#include <assert.h>
char tag1char, tag2char;

int main(int a, char*v[])
{
    int n,cnt,end;
    char buffer[204800];
    FILE*A;
    if(a<2) { fprintf(stderr,"File Argument missing.\n"); exit(-1); }
    if(mp3fulltest(v[1])!=1) { fprintf(stderr,"File Argument not of type mp3.\n"); exit(-1); }
    cnt=mp3start(v[1]);
    end=mp3end(v[1]);
    if(a>1) fprintf(stderr,"%s: [ %ld ] %d %d \n",v[1],mp3size(v[1]),cnt,end);
    A=fopen(v[1],"rb");
    assert(A!=NULL);
    if( fseek(A, cnt, SEEK_SET)!=0 ) perror("Error positioning in mp3 file.");
    n=fread(buffer,sizeof(char),81920,A);
    if( fclose(A)!=0 ) perror("Error closing mp3 file.");
    A=fopen("/tmp/test.mp3","wb");
    assert((A!=NULL));
    fwrite(buffer,sizeof(char),81920,A);
    if( fclose(A)!=0 ) perror("Error closing mp3 file.");
    system("file /tmp/test.mp3");
    return 0;
}
#endif
