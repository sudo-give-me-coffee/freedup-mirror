/*
This file is part of FreeDup.

   FreeDup -- search for duplicate files in one or more directory hierarchies
   Copyright (C) 2007,2008 Andreas Neuper

   FreeDup is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with FreeDup.  If not, see <http://www.gnu.org/licenses/>.

*/
/* freedup was written by AN <AN@freedup.org>  */
#include <assert.h>
#include <ctype.h>
#include "freedup.h"
#include "config.h"
#include "web.h"
#define DEBUG 0

struct option long_options[32] =
{
	{ "freedups",   0, 0, 'a' }, /* 0 */
	{ "basedir",    1, 0, 'b' },
	{ "countsave",  0, 0, 'c' },
	{ "sametime",   0, 0, 'd' },
	{ "timediff",   0, 0, 'D' },
	{ "environment",1, 0, 'e' }, /* 5 */
	{ "samename",   0, 0, 'f' },
	{ "samegroup",  0, 0, 'g' },
	{ "help",       0, 0, 'h' },
	{ "showhard",   0, 0, 'H' },
	{ "interact",   0, 0, 'i' }, /* 10 */
	{ "globalkey",  1, 0, 'k' },
	{ "hardlink",   0, 0, 'l' },
	{ "minbytes",   1, 0, 'm' },
	{ "noaction",   0, 0, 'n' },
	{ "findoptions",0, 0, 'o' }, /* 15 */
	{ "sameperm",   0, 0, 'p' },
	{ "permmask",   0, 0, 'P' },
	{ "quietrun",   0, 0, 'q' },
	{ "symlinks",   0, 0, 's' },
	{ "hashmethod", 1, 0, 't' }, /* 20 */
	{ "dirmtime",	0, 0, 'T' },
	{ "sameuser",   0, 0, 'u' },
	{ "verbose",    0, 0, 'v' },
	{ "version",    0, 0, 'V' },
	{ "weaklinks",  0, 0, 'w' }, /* 25 */
	{ "extra",      1, 0, 'x' },
	{ "nonzero",    0, 0, '0' },
	{ "hash",       1, 0, '#' },
	{ "listenv",	0, 0, 'E' },
	{ NULL,         0, 0,  0  }  /* 30 */
};

int*setting[] = { 
	NULL,		/*  0 */
	NULL,
	&showsave,
	&sametime,
	&timediff,
	NULL,		/*  5 */
	&samename,
	&samegrp,
	NULL,
	&showhard,
	&interact,	/* 10 */
	NULL,
	&hardlink,
	&minbytes,
	&testmode,
	NULL,		/* 15 */
	&sameperm,
	&permmask,
	&quietrun,
	&nosymlinks,
	&hashmethod,	/* 20 */
	&dirmtime,
	&sameuser,
	&showmode,
	NULL,
	&weaklink,	/* 25 */
	&extratyp,
	&nonzero,
	&usehash,
	NULL,
	NULL,		/* 30 */
	NULL,
	NULL,
};

FILE* configfile(const char*const mode)
{
    FILE*cfg=NULL;
    char filename[2048];
    assert(getenv("HOME")!=NULL);
    assert(strlen(getenv("HOME"))+strlen(FREEDUPCONFIGNAME)<2048);
    strcpy(filename,getenv("HOME"));
    strcat(filename,"/");
    strcat(filename,FREEDUPCONFIGNAME);
    cfg=fopen(filename,mode);
    return cfg;
}

void writeconfig(const char*env,const int argc,char*const argv[])
{
    int n=0;
    FILE*cfg=configfile("a");
    if(cfg==NULL)
    {
	return;
    }
    fprintf(cfg,"[%s]\n",env);
    fprintf(cfg,"basedir=%s\n",pwd);
    if(findopta!=NULL)
	fprintf(cfg,"findoptions=%s\n",findopta);
    if(globalkey!=' ')
	fprintf(cfg,"globalkey=%c\n",globalkey);
    for(n=0;long_options[n].name!=NULL;n++)
    {
	if(setting[n]!=NULL)
	{
	    fprintf(cfg,"%s=%d\n",long_options[n].name,*setting[n]);
	}
    }
    for (n=0; n < argc; n++)
    {
	fprintf(cfg,"directory=%s\n",argv[n]);
    }
    fputs("\n",cfg);
    if( fclose(cfg) ) perror("Error closing config file.");
    return;
}


#if 1
void showconfig(FILE*cfg)
{
    int n=0;
    fprintf(cfg,"basedir=%s\n",pwd);
    if(findopta!=NULL)
	fprintf(cfg,"findoptions=%s\n",findopta);
    if(globalkey!=' ')
	fprintf(cfg,"globalkey=%c\n",globalkey);
    for(n=0;long_options[n].name!=NULL;n++)
    {
	if(setting[n]!=NULL)
	{
	    fprintf(cfg,"%s=%d\n",long_options[n].name,*setting[n]);
	}
    }
    fputs("\n",cfg);
    return;
}

char* showconfigstr(char*nonb)
{
    int n=0,l=0;
    static char b[40960];
    sprintf(b,"basedir=%s\n",pwd);
#if 0
    fprintf(stderr,"basedir=%s\n%s",pwd,b);
#endif
    l=strlen(b);
    if(findopta!=NULL)
	sprintf(&(b[l]),"findoptions=%s\n",findopta);
    l=strlen(b);
    if(globalkey!=' ')
	sprintf(&(b[l]),"globalkey=%c\n",globalkey);
    for(n=0;long_options[n].name!=NULL;n++)
    {
	if(setting[n]!=NULL)
	{
	    l=strlen(b);
	    sprintf(&(b[l]),"%s=%d\n",long_options[n].name,*setting[n]);
	}
    }
    return b;
}
#endif

int whichoption(const char*name)
{
    int n=0;
    for(n=0;long_options[n].name!=NULL;n++)
    {
	if( StrNCmp(name,long_options[n].name) == 0 )
	{
	if(0) fprintf(stderr,"%s %d\n",name,n);
	    return n;
	}
    }
    return -1;
}

#define MAXENV	32
const char**getconfigs(void)
{
    char buffer[2048],*ptr=NULL;
    char**list;
    int count=0,max=MAXENV;
    FILE* cfg=configfile("r");
    if(cfg==NULL)
    {
	return NULL;
    }
    list=(char**)calloc( max, sizeof(char*) );
    do {
	fgets( buffer, 2048, cfg );
	if(feof(cfg))
	    return(const char**)list;
	if( (ptr=strchr( buffer, '['))!=NULL )
	{
	    while( isspace(*(ptr+1)) ) ptr++;
	    ptr++;
	    list[count]=calloc( strlen(ptr)+2, sizeof(char));
	    strcpy(list[count],ptr);
	    ptr=strchr( list[count], ']' );
	    *ptr=0;	/* to be on the save side */
	    while( isspace(*(ptr-1)) ) ptr--;
	    *ptr=0;
	    count++;
	}
	if(count==max)
	{
	    max=max<<1;
	    list=realloc( list, max*sizeof(char*) );
	    assert(list!=NULL);
	}
    } while( !feof(cfg) );
    return(const char**)list;
}

void listconfigs(const char*const fmt)
{
    int n=0;
    const char**list;
    list=getconfigs();
    if(list==NULL)
	return;
    while(list[n]!=NULL)
    {
	printf(fmt,list[n++]);
    }
    return;
}

int readconfigfile(FILE *cfg)
{
    char buffer[2048],pbuffer[2048],*ptr=NULL;
    int opt=0;
    do {
	fgets( buffer, 2048, cfg );
	if(feof(cfg)) 
	{
	    if( fclose(cfg) ) perror("Error closing config file.");
	    return 1;
	}
	ptr=strchr( buffer, '=' );
	if( ptr!=NULL )
	{
	    while( isspace(ptr[1]) && (ptr-buffer)<2000 ) ptr++;
	    ptr++;
	    opt=whichoption(buffer);
	    if( opt!=-1 && setting[opt]!=NULL )
	    {
	        if(0) fprintf(stderr,"%s %s [%d]\n",long_options[opt].name,ptr,opt);
		*(setting[opt])=atoi(ptr);
	    } else
	    {
	        if(ptr[strlen(ptr)-1]=='\n') ptr[strlen(ptr)-1]=0;
		if( StrNCmp(buffer,"globalkey") == 0 )
		{
		    globalkey = *ptr;
		} else
		if( StrNCmp(buffer,"findoptions") == 0 )
		{
		    findopta = calloc( strlen(ptr)+2, sizeof(char) );
		    strcpy( findopta, ptr );
		} else
		if( StrNCmp(buffer,"basedir") == 0 )
		{
		    if( pwd!=NULL ) free(pwd);
		    pwd = calloc( strlen(ptr)+2, sizeof(char) );
		    strcpy( pwd, ptr );
		    chdir(pwd);
		} else
		if( StrNCmp(buffer,"directory") == 0 )
		{
		    FILE*liste=NULL;
		    sprintf(pbuffer,"find \"%s\" -type f %s -print", ptr, (findopta==NULL)?"":findopta );
		    liste = popen (pbuffer, "r");
		    if(liste == 0)
		    {
			perror(pbuffer);
			return(-1);
		    }
		    filecount = readtree( &file_info, &frdinfosize, filecount, liste );
		    pclose(liste);
		}
	    }
	}
    } while( (strchr(buffer,'[')==0) );
    return 0;
}

int readconfig(const char*env)
{
    char buffer[2048],*ptr=NULL;
    FILE* cfg=configfile("r");
    if(cfg==NULL)
    {
	return 0;
    }
    do {
	fgets( buffer, 2048, cfg );
	if(feof(cfg)) return 0;
	if( (ptr=strchr( buffer, '['))!=NULL )
	{
	    while( isspace(*(ptr+1)) ) ptr++;
	    ptr++;
	}
    } while( ptr==NULL || StrNCmp(ptr,env) );
    readconfigfile(cfg);
    if( fclose(cfg) ) perror("Error closing config file.");
    return 1;
}


int readconfigline(char *buffer)
{
    char *token,pbuffer[2048],*ptr=NULL;
    int opt=0;
    token=strtok( buffer, "&?" );
    while( token!=NULL )
    {
	if(DEBUG)fprintf(stderr,"TOKEN: %s \n",token);
	/*
	 *  |<   TOKEN   >|
	 *   token
	 *   |
	 *   v            0
	 *   keyword=value&keyword=value&...
	 *          A
	 *          |
	 *          ptr
	 */
	ptr=strchr( token, '=' );

	if( ptr!=NULL )
	{
	    while( isspace(ptr[1]) && (ptr-token)<2000 ) ptr++; ptr++;
	    opt=whichoption(token);
	    if( opt!=-1 && setting[opt]!=NULL )
	    {
	        if(DEBUG) 
		  fprintf(stderr,"%s %s [%d]\n",long_options[opt].name,ptr,opt);
		*(setting[opt])=atoi(ptr);
	    } else
	    {
	        if(ptr[strlen(ptr)-1]=='\n') ptr[strlen(ptr)-1]=0;
		if( StrNCmp(token,"globalkey") == 0 )
		{
		    globalkey = *webtrans(ptr);
		} else
		if( StrNCmp(token,"findoptions") == 0 )
		{
		    findopta = calloc( strlen(ptr)+2, sizeof(char) );
		    strcpy( findopta, webtrans(ptr) );
		} else
		if( StrNCmp(token,"prghash") == 0 )
		{
#if 0
/* incomplete test failed - reason unclear - the result was */
/* */
/* #######sha1######							*/
/* hash result is: "/usr/bin/sha1sum "./freedup" 2>/dev/null"		*/
/* sha1: expected output format does not match template. "<#40>()<file>"*/
/* Hash function "sha1" oputput format not supported.			*/
/* */
showmode=1;
fprintf(stderr,"\n#######%s######\n",ptr);
#endif
		    hashmethod=selecthash(ptr);
		} else
		if( StrNCmp(token,"action") == 0 )
		{   switch(atoi(ptr))
		    { /* must be a radio button to avoid colliding settings */
			case 0:	testmode=1;	
				weaklink=0;
				hardlink=0;
				break;
			case 1:	testmode=0;	/* normal */
				weaklink=0;
				hardlink=0;
				break;
			case 2:	testmode=0;	
				weaklink=0;
				hardlink=1;
				break;
			case 3:	testmode=0;	
				weaklink=1;
				hardlink=0;
				break;
			default:testmode=1;
				weaklink=0;
				hardlink=0;
				break;
		    } /* this post definition solved a historic heir */
		} else
		if( StrNCmp(token,"basedir") == 0 )
		{
		    if( pwd!=NULL ) free(pwd);
		    pwd = calloc( strlen(ptr)+2, sizeof(char) );
		    strcpy( pwd, webtrans(ptr) );
		    chdir(pwd);
		} else
		if( StrNCmp(token,"directory") == 0 )
		{
		    FILE*liste=NULL;
		    if(strlen(ptr)>0)
		    {
			sprintf(pbuffer,"find \"%s\" -type f %s -print", 
				webtrans(ptr), (findopta==NULL)?"":findopta );
			liste = popen (pbuffer, "r");
			if(liste == 0)
			{
			    perror(pbuffer);
			    return(-1);
			}
			filecount = readtree( &file_info, &frdinfosize, filecount, liste );
			pclose(liste);
		    }
		}
	    }
	}
        token=strtok( NULL, "&" );
    }
    return 0;
}
