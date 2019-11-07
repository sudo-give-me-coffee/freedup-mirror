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

const int    mpctest(const unsigned char*buffer)
{
    /* Range used: 0-8 */
    /* MousePack Header */
    if( buffer[0]==((unsigned char)'M') 
     && buffer[1]==((unsigned char)'P') 
     && buffer[2]==((unsigned char)'+') ) return 1;
    /* failed */
    return 0;
}

const int    mpcfulltest(const char*a)
{
    size_t count;
    unsigned char buffer[32];
    FILE*A = fopen(a,"rb");
    if(A==NULL) return 0;
    count = fread(buffer,sizeof(char),32,A);
    if( fclose(A)!=0 ) perror("Error closing mpc file.");
    return mpctest(buffer);
}

const unsigned long mpcstart(const char*a)
{
    if(a==NULL) return 0;
    /* no mouse pack format with initial header known */
    tag2char=' ';
    return 0;
}

const unsigned long mpcend(const char*a)
{
    size_t n;
    unsigned char buffer[256];
    FILE*A=fopen(a,"rb");
    if(A==NULL) return 0;
    tag1char='A';
    /* try APETAGEX footer */
    if( fseek(A, -32, SEEK_END)!=0 ) perror("Error positioning in mpc file.");
    n = fread( buffer, sizeof(char), 32, A );
    if( fclose(A)!=0 ) perror("Error closing mpc file.");
    if(n!=32) return 0;
    if( strncmp((const char*)buffer, "APETAGEX", 8) == 0 )
    {   return  (32+(unsigned long)((buffer[12]+(buffer[13]<<8)+(buffer[14]<<16)+(buffer[15]<<24))));
    } else return 0;
}

const unsigned long mpcsize(const char*a)
{
    static struct stat xstat;
    if( stat( a, &xstat) == 0 )
    {
	return (  (unsigned long)xstat.st_size - (mpcstart(a)+mpcend(a)) );
    } else
    return 0;
}

#if defined(MPC_TEST)
#include <assert.h>
char tag1char, tag2char;

int main(int a, char*v[])
{
    size_t n,cnt,end;
    char buffer[204800];
    FILE*A;
    if(a<2) { fprintf(stderr,"File Argument missing.\n"); exit(-1); }
    if(mpcfulltest(v[1])!=1) { fprintf(stderr,"File Argument not of type mpc.\n"); exit(-1); }
    cnt=mpcstart(v[1]);
    end=mpcend(v[1]);
    if(a>1) fprintf(stderr,"%s: [ %ld ] %d %d \n",v[1],mpcsize(v[1]),cnt,end);
    A=fopen(v[1],"rb");
    assert(A!=NULL);
    if( fseek(A, cnt, SEEK_SET)!=0 ) perror("Error positioning in mpc file.");
    n=fread(buffer,sizeof(char),81920,A);
    if( fclose(A)!=0 ) perror("Error closing mpc file.");
    A=fopen("/tmp/test.mpc","wb");
    assert((A!=NULL));
    fwrite(buffer,sizeof(char),81920,A);
    if( fclose(A)!=0 ) perror("Error closing mpc file.");
    system("file /tmp/test.mpc");
    return 0;
}
#endif
