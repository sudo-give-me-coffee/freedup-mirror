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

const int    jpgtest(const unsigned char*buffer)
{
    /* Range used: 0-1 */
    if( buffer[0]==(unsigned char)'\377' && buffer[1]==(unsigned char)'\330' )    return 1;
    return 0;
}

const int    jpgfulltest(const char*a)
{
    size_t count;
    unsigned char buffer[32];
    FILE*A=fopen(a,"rb");
    if(A==NULL) return 0;
    count=fread(buffer,sizeof(char),10,A);
    if( fclose(A)!=0 ) perror("Error closing jpg file.");
    if( buffer[0]==(unsigned char)'\377' && buffer[1]==(unsigned char)'\330' )    return 1;
    return 0;
}

const unsigned long jpgstart(const char*a)
{
    size_t count,read=2;
    unsigned char buffer[82000];
    FILE*A;
    A=fopen(a,"rb");
    if(A==NULL) return 0;
    if( (count=fread(buffer,sizeof(char),read+4,A)) < 6 ) return(0);
    tag1char='C';
    if( buffer[0]==(unsigned char)0xff && buffer[1]==(unsigned char)0xd8 )
    {	/*  0xd8 = SOI Start Of Image */
	while(feof(A)==0)
	{
	    read  = (((size_t)buffer[read+2])<<8) + (size_t)(buffer[read+3]) - 2;
	    count+= fread(buffer,sizeof(char),read+4,A);
	    if(0) fprintf(stderr, ">>ff %x %d\n", (unsigned int)(buffer[read+1]), (int)read);
	    if( feof(A)!=0 )
	    {
		fprintf(stderr, "File may be truncated. Not considered.");
	        if( fclose(A)!=0 ) perror("Error closing jpg file.");
		return 0;
	    }
	    if( buffer[read] == (unsigned char)0xff )
	    {
		/*
		 * In the following lines, the tags belong to one
		 * one out of two categories:
		 * - tags that might be added or deleted without changing the image
 		 * - markers that describe or are part of the image
		 * We skip those that belong to the first part and
		 * we try to keep the most relevant ones that hold the image.
		 * 
		 * Some programs like "wrjpgcom" put the comments past the DQT section
		 * hence we also skip this section, that is (an essential) part of the image.
		 */
		switch(buffer[read+1])
		{
		case (unsigned char)0xe0: /* APP0: JFIF tag */
		case (unsigned char)0xe1: /* APP1: EXIF tags */
		case (unsigned char)0xe2: /* APP2: general tag */
		case (unsigned char)0xe3: /* APP3: general tag */
		case (unsigned char)0xe4: /* APP4: general tag */
		case (unsigned char)0xe5: /* APP5: general tag */
		case (unsigned char)0xe6: /* APP6: general tag */
		case (unsigned char)0xe7: /* APP7: general tag */
		case (unsigned char)0xe8: /* APP8: general tag */
		case (unsigned char)0xe9: /* APP9: general tag */
		case (unsigned char)0xea: /* APP10: general tag */
		case (unsigned char)0xeb: /* APP11: general tag */
		case (unsigned char)0xec: /* APP12: general tag */
		case (unsigned char)0xed: /* APP13: general tag */
		case (unsigned char)0xee: /* APP14: Usually Copyright tag */
		case (unsigned char)0xef: /* APP15: general tag */
		case (unsigned char)0xfe: /* COM: Comments */
		case (unsigned char)0xdb: /* DQT: Definition of Quantization tables */
				 /*   needed, but skipped to pass comments */
			break;
		/* SOFn: Start of Frame Markers */
		case (unsigned char)0xc0: /* SOF0: Baseline DCT */
		case (unsigned char)0xc1: /* SOF1: Extended sequential DCT */
		case (unsigned char)0xc2: /* SOF2: Progressive DCT */
		case (unsigned char)0xc3: /* SOF3: Lossless (sequential) */
		case (unsigned char)0xc4: /* DHT: Definition der Huffman Tabellen */
		case (unsigned char)0xc5: /* SOF5: Differential sequential DCT */
		case (unsigned char)0xc6: /* SOF6: Differential progressive DCT */
		case (unsigned char)0xc7: /* SOF7: Differential lossless (sequential) */
		case (unsigned char)0xc8: /* JPG: reserviert für JPEG extensions */
		case (unsigned char)0xc9: /* SOF9: Extended sequential DCT */
		case (unsigned char)0xca: /* SOF10: Progressive DCT */
		case (unsigned char)0xcb: /* SOF11: Lossless (sequential) */
		case (unsigned char)0xcc: /* DAC: Definition der arithmetischen Codierung */
		case (unsigned char)0xcd: /* SOF13: Differential sequential DCT */
		case (unsigned char)0xce: /* SOF14: Differential progressive DCT */
		case (unsigned char)0xcf: /* SOF15: Differential lossless (sequential) */
		case (unsigned char)0xd9: /* EOI: End of Image */
		case (unsigned char)0xda: /* SOS: Start of Scan */
		default:  
			if( fclose(A)!=0 ) perror("Error closing jpg file.");
			return((unsigned long)count-4);
			/* break; */
		}
	    }
	}
    }
    if( fclose(A)!=0 ) perror("Error closing jpg file.");
    return 0;
}

const unsigned long jpgend(const char*a)
{
    if(a==NULL) return 0;
    /*
     * I did not find any trailing comments
     */
    tag2char=' ';
    return 0;
}

const unsigned long jpgsize(const char*a)
{
    static struct stat xstat;
    if( stat( a, &xstat) == 0 )
    {
	return (  (unsigned long)xstat.st_size - (jpgstart(a)+jpgend(a)) );
    } else
    return 0;
}

#if defined(JPG_TEST)
char tag1char, tag2char;

int main(int a, char*v[])
{
    size_t n,cnt,end;
    unsigned char buffer[8200000];
    FILE*A,*B;
    cnt=jpgstart(v[1]);
    end=jpgend(v[1]);
    if(jpgfulltest(v[1])!=1) { fprintf(stderr,"File Argument not of type jpeg.\n"); exit(-1); }
    if(a>1) fprintf(stderr,"%s: %d %d\n",v[1],cnt,end);
    A=fopen(v[1],"rb");
    B=fopen("/tmp/test.jpg","wb");
    n=fread(buffer,sizeof(char),cnt,A);
    fwrite(buffer,sizeof(char),2,B);
    n=fread(buffer,sizeof(char),8192000,A);
    fwrite(buffer,sizeof(char),n,B);
    if( fclose(A)!=0 ) perror("Error closing jpg file.");
    if( fclose(B)!=0 ) perror("Error closing jpg file.");
    system("xv /tmp/test.jpg");
    return 0;
}
#endif
