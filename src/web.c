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
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "web.h"
#include "socket.h"
#include "nethelp.h"
#include "freedup.h"
#include "config.h"
char*homedir=HOMEDIR;

char*webtrans(char*ask)
{
    int n=0,repl=0,length=0;
    char trans[6],*in,*nin;
    in=strdup(ask);
    nin=strchr(in,'%');
    length=strlen(in);
    strcpy(trans,"0xxx");
    while( nin!=NULL )
    {
	n = nin-in;
        strncpy(&trans[2],nin+1,2);
        in[n]=(unsigned char)strtol(trans,(char**)NULL,16);
	strcpy(&in[n+1],&nin[3]);
	nin=strchr(&in[n+1],'%');
	repl+=2;
    }
    in[length-repl+3]=0;
    return in;
}

const char*show_header(int http,int type)
{
    char*content=NULL;
    static char buffer[1024];
    time_t now;

    switch(type&0xf)
    {
    case 1: content="\nContent-Type: text/html; charset=ISO-8859-1"; break;
    case 2: content="\nContent-Type: text/plain; charset=ISO-8859-1"; break;
    case 3: content="\nContent-Type: image/gif"; break;
    case 4: content="\nContent-Type: image/jpg"; break;
    default: content=""; break;
    }
    now=time(NULL);
    sprintf(buffer, "HTTP/1.0 %d Answer\nServer: freedup\n"
		    "Connection: close%s\nDate: %s\n\n",
		    http,content,ctime(&now));
    return(buffer);
}

#ifdef FORTESTINGONLY
const char*answer(char*in)
{
    char*ptr=NULL,*end=NULL;
    int type=0;

    static char out[MAXANSWER+2];
    memset(out,0,MAXANSWER);

    if( strncasecmp("GET ",in,4) == 0 )
    {
        type=1;
        ptr=&in[4];
    } else
    if( strncasecmp("POST ",in,5) == 0 )
    {
        type=1;
        ptr=&in[5];
    } else
    {
        type=1;
        ptr=&in[0];
    }
    end=strchr(ptr,' ');
    assert( ((end-ptr<1024),"filename buffer is defined to 1024 chars only") );
    *end=0;
    if( strncasecmp(FREEDUP,ptr+1,strlen(FREEDUP)) == 0 )
    {   /* just strip the standard header */
        ptr+=1+strlen(FREEDUP);
    }
    strcpy(out,show_header(200,1));
    strcat(out,"\n\n<HTML><HEAD><TITLE>TEST</TITLE></HEAD><BODY><H1>TEST</H1></BODY></HTML>\n");
    strcat(out,ptr);
    return(out);
}
#endif /* FORTESTINGONLY */

#ifdef OLDWEB

int weboffer(const char*inputfile)
{
    char buffer[20480];
    FILE*out=stdout,*infile=fopen(inputfile,"r");
    if(infile==NULL)
    {
	perror("Cannot open html file");
    } else
    {
	while(!feof(infile))
	{
	    fgets( buffer, 20480, infile );
	    fprintf(out,"%s",buffer);
	}
	if( fclose(infile) ) perror("Error closing infile file.");
    }
    exit(0);
    return 0;
}


int webconfig(const char*inputfile)
{
    char buffer[20480],*ptr=NULL;
    FILE*in=stdin,*out=stdout;
    FILE*cfs=tmpfile();
    memset(buffer,0,20480);
    do {
	fgets( buffer, 20480, in );
	if( !StrNCaseCmp( buffer, "GET" ) )
	{
	    weboffer(inputfile);
	} else
	{
	    fcntl (0, F_SETFL, O_NONBLOCK);
	}
    } while( strlen( buffer ) >= 3 );
    fgets( buffer, 20480, in );
    for( ptr=strchr(buffer,'&'); ptr!=NULL; ptr=strchr(ptr+1,'&') ){ *ptr='\n'; }
    fprintf( cfs,"%s\n", buffer );
    fflush( cfs );
    if( fseek( cfs, 0L, SEEK_SET)!=0 ) perror("Error positioning in config file.");
    readconfigfile( cfs );
    fprintf(out,"HTTP/1.1 200 OK\nContent-Type: text/plain; charset=ISO-8859-1\n\n");
    fflush(out);
    if(feof(in)) 
    {
	if( fclose(in) ) perror("Error closing infile file.");
	return 1;
    }
    return 0;
}

#endif	/* OLDWEB */

int mysendfile(int out,const char*inputfile)
{
    const char*external;
	  char*buffer;
    struct stat inputstat;
    FILE*infile=fopen(inputfile,"r");

    if( stat( inputfile, &inputstat) != 0)
    {
        perror(inputfile);
        return(-1);
    }
    buffer = malloc(inputstat.st_size+100);

    if(infile==NULL)
    {   perror("Cannot open html file");
	free(buffer);
    } else
    if(buffer==NULL)
    {   perror("Cannot alloc web file serving memory.");
	free(buffer);
    } else
    {   
	external = show_header(200,1);
	send(out,external,strlen(external),0);
	/* could have provided size too - i am too lazy for */
	while(!feof(infile))
	{   /* should run only once */
	    fgets( buffer, inputstat.st_size+100, infile );
	    send(out,buffer,strlen(buffer),0);
	}
	fprintf(stderr,"\nINPUT SIZE: %d\n", ((int)inputstat.st_size) );
	if( fclose(infile) ) perror("Error closing infile file.");
    }
    free(buffer);
    return 0;
}

int process(int out,const char*b)
{
    char buffer[MAXANSWER];
    char*ptr=NULL,*page=NULL;
    char*external=NULL,*lt=NULL;
    enum procreqtype request=PROC_UNSET;
    int lgth=strlen(b);

    if( StrNCaseCmp(b,"GET")  == 0 ) request=PROC_GET;
    if( StrNCaseCmp(b,"PUT")  == 0 ) request=PROC_PUT;
    if( StrNCaseCmp(b,"POST") == 0 ) request=PROC_POST;
    page = strchr(b,' ')+1;
    ptr = strchr(page,' '); *ptr=0; ptr++; /* terminate page name */
#if 0
fprintf(stderr,"--%s--%s-->>%s<<%s>>",(request==PROC_GET)?"GET":"POST",page,ptr,buffer);
#endif
    if( strchr(page,'?')==NULL && (request==PROC_GET) )
    {
	mysendfile(out,(strlen(page)<2)?WEBPAGE:(page+1));
    } else /* should check for POST */
    if( (StrNCaseCmp(page,"free")==0) 
    ||  (StrNCaseCmp(page,"/free")==0) )
    {
        external=show_header(200,2);
	send(out,external,strlen(external),0);
	/* */;
	lt=strstr(ptr,"\n\n");
             if( lt == NULL )	{lt=strstr(ptr,"\n\r\n");}
   	     if( lt == NULL )	{external= NULL; }
	else if( lt[1]=='\n' )	{external=&lt[2];}
	else if( lt[2]=='\n' )	{external=&lt[3];}
        else			{external= NULL; }
	if( external!=NULL && (external-b)<lgth )
	{
	    readconfigline( external );
	    /* send(out,ptr,strlen(ptr),0); */
            if( StrNTCaseCmp(page,"test")==0 ) 
	    {
	        external=showconfigstr(buffer);
	        send(out,external,strlen(external),0);
	    } else
	    {
		/* execute */
		/* system("freedup -e 200902222222"); */
		;
	    }
	} else
	{
	    fprintf(stderr,"+++\?\?\?> %s \n\n",(ptr==NULL)?"NO-STRING":ptr);
	}
    } else
    {
        strcpy(buffer,show_header(200,2));
	/*not ready yet*/;
	send(out,buffer,strlen(buffer),0);
    }
    return 0;
}

