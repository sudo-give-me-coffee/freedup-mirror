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
#include "sha1.h"
#include "freedup.h"
#include "helper.h"
#include "web.h"
#include "config.h"
#include <signal.h>
#include <assert.h>
#include <ctype.h>

/* Name this program was called with. */
char *program_name;
static void printhashchoice(void);
static void printxtrachoice(void);


void version(void)
{
#ifndef S_SPLINT_S
    printf( "\n\tfreedup: Version " VERSION FLAVOUR " \2512007-2008 by AN@freedup.org.\n"
		"\t sha1.c/h of sha-1.0.4 \2512001-2003 by Allan Saddi.\n\n");
#endif
    return;
}

/* SYNOPSIS */
void usage(void)
{
    printf( "\nUSAGE:\t%s [options] [<tree> ...]\n\n", program_name );
    printf( "-%c --%s\t   provide compatibility to freedups by William Stearns.[=-upg]\n",
	long_options[0].val,long_options[0].name);
    printf( "-%c --%s <path>     sets the current working directory to <path>.\n",
	long_options[1].val,long_options[1].name);
    printf( "-%c --%s\t   count file space savings per linked file.\n",
	long_options[2].val,long_options[2].name);
    printf( "-%c --%s\t   requires the modification time stamps to be equal.\n",
	long_options[3].val,long_options[3].name);
    printf( "-%c --%s <seconds> allow the modification time stamps to differ by maximum #.\n",
	long_options[4].val,long_options[4].name);
    printf( "-%c --%s <env>  load or store command line as an environment. There exist:\n\t\t",
	long_options[5].val,long_options[5].name); listconfigs("%s, "); myputs("");
    printf( "-%c --%s\t   requires the path-stripped file names to be equal.\n",
	long_options[6].val,long_options[6].name);
    printf( "-%c --%s\t   requires groups of the files to be equal before linking.\n",
	long_options[7].val,long_options[7].name);
    printf( "-%c --%s\t   shows this help. [other option are ignored]\n",
	long_options[8].val,long_options[8].name);
    printf( "-%c --%s\t   do treat hardlink files as duplicates and show them on -in\n",
	long_options[9].val,long_options[9].name);
    printf( "-%c --%s\t   Decide in interactive mode individually, what to do with them.\n",
	long_options[10].val,long_options[10].name);
    printf( "-%c --%s <key>    Decide linking direction by key. Valid are @,#,<,>,+ and -.\n",
	long_options[11].val,long_options[11].name);
    printf( "-%c --%s\t   only allow hardlinks. No symlinks are established.\n",
	long_options[12].val,long_options[12].name);
    printf( "-%c --%s <bytes>   only consider larger files. (preferred: -o \"-size +#\")\n",
	long_options[13].val,long_options[13].name);
    printf( "-%c --%s\t   do not really perform links [no action].\n",
	long_options[14].val,long_options[14].name);
    printf( "-%c --%s <opts> pass an option string to the initially called find command.\n",
	long_options[15].val,long_options[15].name);
    printf( "-%c --%s\t   requires file permissions to be equal.\n",
	long_options[16].val,long_options[16].name);
    printf( "-%c --%s <mask>    sets the octal mask for file permissions comparison.\n",
	long_options[17].val,long_options[17].name);
    printf( "-%c --%s\t   produces no output during the run (toggles -c and -v to off).\n",
	long_options[18].val,long_options[18].name);
    printf( "-%c --%s\t   generate symlinks although some given paths are relative.\n",
	long_options[19].val,long_options[19].name);
#ifdef HASHSUM
    printf( "-%c --%s <type>  select hash method: ",
	long_options[20].val,long_options[20].name);
    printhashchoice();
#endif
    printf( "-%c --%s\t   restore the time stamp of the directory after linking.\n",
	long_options[21].val,long_options[21].name);
    printf( "-%c --%s\t   requires users of the files to be equal before linking.\n",
	long_options[22].val,long_options[22].name);
    printf( "-%c --%s\t   display shell commands to perform linking [verbose mode].\n",
	long_options[23].val,long_options[23].name);
    printf( "-%c --%s\t   display current version.\n",
	long_options[24].val,long_options[24].name);
    printf( "-%c --%s\t   only weak symbolic links allthough hardlinks might be possible.\n",
	long_options[25].val,long_options[25].name);
    printf( "-%c --%s <style> select comparison style: ",
	long_options[26].val,long_options[26].name);
    printxtrachoice();
    printf( "-%c --%s\t   disable fast comparison using hash strings [0=no,1=classic,2=fast].\n",
	long_options[28].val,long_options[28].name);
    printf( "-%c --%s\t   disable linking of empty files i.e. files of size 0.\n",
	long_options[27].val,long_options[27].name);
    printf( "\t<tree>\t   any directory to scan for duplicate files recursively.\n\n"
            "\tOptions are toggle switches. Their final state applies.\n"
	    "\tLater <tree> entries are linked to the earlier ones.\n"
	    "\tIf no <tree> is given, filenames are expected from standard input.\n"
	    "\tWhen standard input is used the option -o has no effect."
         );
    version();
    return;
}
   
/* OPTION DEFAULTS  */
	int showsave=0;	/* -c */
	int sametime=0;	/* -d */
	int timediff=0;	/* -D */
	int samename=0;	/* -f */
	int samegrp =0;	/* -g */
	int showhard=0;	/* -H */
	int interact=0; /* -i */
	char globalkey='.'; /* -k */
	int hardlink=0;	/* -l */
	int minbytes=0;	/* -m */
	int testmode=0;	/* -n */
	int sameperm=0;	/* -p */
	int permmask=0;	/* -P */
	int quietrun=0;	/* -q */
	int dirmtime=0;	/* -T */
	int sameuser=0;	/* -u */
	int showmode=0;	/* -v */
	int weaklink=0;	/* -w */
	int nonzero=0;	/* -0 */
	int usehash=0;	/* -# */
	int extratyp=0; /* -x */
	int nosymlinks=0;	/* -s */
/*@null@*/char*findopta=NULL;	/* -o */
/*@null@*/char*pwd=NULL;	/* -b */
	int hashmethod=0;	/* -t */



/* DESCRIPTION

   main() handles the command line parameters
	  scans for files to consider
	  builds a list of all files to consider
	  qsort()s the list of files
   comparison_by_size() is called for two files by qsort()
	  for equally sized files compares hash signature
	  comparison_by_content() when the same size & hash
   comparison_by_content() is only called by comparison_by_size()
	  reads and compares both files block by block
	  determines priority, i.e. later args count less
	  calls link_files() for identical files
   link_files() replaces the second file by a link to the first
	  determines whether both files are on the same fs
	  uses inode hard links on the same device number
	  uses symlinks if device numbers differ

   Additionally various helper functions are deployed.
*/

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include <assert.h>
#include <ctype.h>

void perror(const char *s);
#include <errno.h>
int errno;

/* internal counters */
/* link actions performed */
int totalinked=0;
int symlinked=0;
int deleted=0;
int freedspace=0;
unsigned long long linksaved=0;

/* The number of regular files to scan. */
int frdinfosize=INITIALFINFOCOUNT;
int filecount=0;
int dupscount=0;

/* Time at start of execution.  */
time_t start_time;

/* Seconds between 00:00 1/1/70 and either one day before now
   (the default), or the start of today (if -daystart is given). */
time_t cur_day_start;

/* Status value to return to system. */
int exit_status;

/* The array pointer we use for sorting. */
frdinfo*file_info;

/* The array pointer we use for referencing duplicates in interactive mode. */
/*@null@*/
dinfo*dups_info=NULL;

#include "auto.h"

static void printxtrachoice(void)
{
    int n;
    unsigned char c=' ';
    for(n=1;extra[n].type!=NULL;n++)
    {
	    CHECK4KOMMA(c);
	    printf(" %s",extra[n].type);
     /* else perror("lstat() failed while reading file statistics"); */
    }
    printf("\n");
    return;
}

int selectextra(const char*type)
{
    int method=0;
    assert(type!=0);
    while( extra[method].type != NULL )
    {
	if( strncmp(extra[method].type,type,strlen(type)) == 0 )
	{
	    break;
	} else method++;
    }
    if( extra[method].type == NULL )
    {
	if(showmode!=0)
	    fprintf(stderr,
		"Cannot determine a extra comparison module called \"%s\".\n", type);
	return 0;
    }
    return method;
}

#ifdef HASHSUM
int hashreject=0;
int hashread=0;

struct hashtabelle {
	const char *type;
	const char *program;
	const size_t length;
	const int   space1;
	const int   space2;
} hashme[] = {
     { "internal sha1","sha1",SHA1_HASH_SIZE, 40, 41 },
#if defined(SHA512SUM)
     { "sha512",       SHA512SUM,128,128,129 },
#endif
#if defined(SHA384SUM)
     { "sha384",       SHA384SUM, 96, 96, 97 },
#endif
#if defined(SHA256SUM)
     { "sha256",       SHA256SUM, 64, 64, 65 },
#endif
#if defined(SHA224SUM)
     { "sha224",       SHA224SUM, 56, 56, 57 },
#endif
     { "sha1",         SHA1SUM,   40, 40, 41 },
     { "md5",          MD5SUM,    32, 32, 33 },
/*   { "cksum",        CKSUM,     16, 16, 10 }, may not be used since
	variable length hash result is currently not supported */
     { "sum",          SUM,       11,  6,  8 }, /* not recommended */
     {  NULL,          NULL,       0,  0,  0 }
};

static void printhashchoice(void)
{
    int n;
    unsigned char c=' ';
    static struct stat xstat;
    for(n=1;hashme[n].type!=NULL;n++)
    {
	if( stat( hashme[n].program, &xstat) == 0)
	{
	    if( S_ISREG(xstat.st_mode) && ((S_IXOTH|S_IXGRP|S_IXUSR)&xstat.st_mode) )
	    {
		CHECK4KOMMA(c);
		printf("%s",hashme[n].type);
	    }
	}
     /* else perror("lstat() failed while reading file statistics"); */
    }
    printf("\n");
    return;
}

int hashed(const char*s)
{
    int returnval=atoi(s);

    if(returnval>2) returnval=2;
    if(returnval<0) returnval=0;
    return returnval;
}

static void gethash_internal(frdinfo*a)
{
    char buffer[BLOCK+2];
    FILE*file2hash;
    size_t size,rdsz=0;

    file2hash = fopen (a->name, "rb");
    if(file2hash == NULL)
    {
	/* perror("fopen() failed - file was probably deleted"); */
	return;
    }
    fseek( file2hash, extra[extratyp].start(((frdinfo*)a)->name), SEEK_SET );
    if(a->sha1pos == 0)
    {
	SHA1INIT(&(a->context));
    } else
    {
	fseek( file2hash, a->sha1pos, SEEK_SET );
    }
    rdsz = fread(buffer,1,(a->size<BLOCK)?a->size:BLOCK,file2hash);
    size = a->size - rdsz;
    do {
        if( ferror(file2hash) && ! feof(file2hash) )
        {
	    if( fclose(file2hash) ) perror("Error closing file2hash file.");
	    return;
        }
        SHA1UPDATE(&(a->context), buffer, rdsz);
	a->sha1pos = ftell(file2hash);
        rdsz = fread(buffer, 1, (size<BLOCK)?size:BLOCK, file2hash);
        size = size - rdsz;
    } while( (!feof(file2hash))&&(rdsz!=0) );
    if( fclose(file2hash) ) perror("Error closing file2hash file.");
    a->hash = calloc(SHA1_HASH_SIZE+2,sizeof(char));
    SHA1FINAL(&(a->context), (unsigned char*)a->hash);
    a->sha1pos = -1;
    return;
}

static void gethash(frdinfo*a)
{
    char buffer[MAXPATHLENGTH+132];
    FILE*file2hash;
    int n=0;
    hashread++;
    /*
     * this routine uses external shell commands
     */
    if( hashmethod==0 || extratyp!=0 )
    {
	gethash_internal(a);
	return;
    }
    sprintf(buffer,"%s \"%s\" 2>/dev/null",hashme[hashmethod].program, a->name);
    file2hash = popen (buffer, "r");
    if(file2hash == NULL)
    {
	perror(buffer);
	exit(-1);
    }
    fgets(buffer,MAXPATHLENGTH+132,file2hash);
    a->hash = (char*)calloc(hashme[hashmethod].length+2,sizeof(char));
    if( feof(file2hash) )
    {
	/*  hash was unsuccessful;
	 *  usually fails due to special characters in the file name
	 *  (this was at least the reason for redirecting errors)
	 *  write std dummy value;
	 */
        for(n=0;n<hashme[hashmethod].length;n++)
	   (a->hash)[n]='0';
	/*  it would be better (correct)
	 *  to link it to some temporary file,
	 *  to evaluate it, and to continue then
	 */
    } else
    {
	/*  if everything works fine
	 *  there should appear 32 characters
	 *  followed by two spaces and the file name
	 */
        assert(buffer[hashme[hashmethod].space1]==' ');
        assert(buffer[hashme[hashmethod].space2]==HASHCHAR);
        if(0) printf("HASH=%s\n", buffer);
	/* store the hash value to avoid recalculation */
        strncpy(a->hash,buffer,hashme[hashmethod].length);
    }
    pclose(file2hash);
    /* the string was already initialised with zero
     * since I am somewhat paranoid ...
     */
    a->hash[hashme[hashmethod].length]=0;
    return;
}

int hashprobe(const int method,const char*filename)
{
    static struct stat astat;
    char buffer[MAXPATHLENGTH+132];
    FILE*file2hash;

    if(stat(hashme[method].program, &astat) != 0)
    {
	/* the program does not even exist */
	sprintf(buffer,"%s: No hash program at %s",hashme[method].type,
		hashme[method].program);
	perror(buffer);
	return(-1);
    } else
    {
	/* hash program should be a/point to regular file */
	if( ! S_ISREG(astat.st_mode) ) return -2;
	/* a program should be executable by someone */
	if( ! ((astat.st_mode)&(S_IXUSR|S_IXGRP|S_IXOTH)) ) return -2;
    }
    /*
     * this routine uses external shell commands
     */
    sprintf(buffer,"%s \"%s\" 2>/dev/null",
	hashme[method].program, filename);
    file2hash = popen (buffer, "r");
    if(file2hash == NULL)
    {
	perror(buffer);
	return(-3);
    }
    fgets(buffer,MAXPATHLENGTH+132,file2hash);
/* tests showed that instead of the result, sometimes the command is returned */
/* reason is unclear - is a delay missing ?, use showmode to track it */
    pclose(file2hash);
    if( strlen(buffer) < hashme[method].length )
	return -4;
    if( buffer[hashme[method].space1] != ' ' 
    ||  buffer[hashme[method].space2] != HASHCHAR )
    {
	if(showmode!=0)
	    fprintf(stderr,
		"hash result is: \"%s\"\n"
		"%s: expected output format does not match template."
		" \"<#%d>%c%c<file>\"\n", buffer, hashme[method].type,
		hashme[method].length,hashme[method].space1,
		hashme[method].space2);
	return -5;
    }
    return 0;
}

int selecthash(const char*type)
{
    hashmethod=0;
    while(hashme[hashmethod].program!=NULL)
    {
	if( strncmp(hashme[hashmethod].type,type,strlen(type)) == 0)
	{
	    break;
	} else hashmethod++;
    }
    if( hashme[hashmethod].program==NULL)
    {
	if(showmode!=0)
	    fprintf(stderr,
		"Cannot determine a hash function called \"%s\".\n", type);
	hashmethod=-1;
	return hashmethod;
    }
    if( hashprobe(hashmethod,program_name) == 0)
    {
	return hashmethod;
    } else
    {
	if(showmode!=0)
	    fprintf(stderr,
		"Hash function \"%s\" oputput format not supported.\n",
		hashme[hashmethod].type);
	hashmethod=-1;
    }
    return hashmethod;
}

#endif /* HASHSUM */

void ttydirect(int on)
{
    static TTY new_io,old_io;
    int stty=(ECHO|ICANON);

    if(on==1)
    {
	if((tcgetattr(STDIN_FILENO, &old_io)) == EXIT_SUCCESS)
	{
	    new_io = old_io;
	    new_io.c_lflag = new_io.c_lflag & ~stty;
	    new_io.c_cc[VMIN] = 1; /* only one byte */
	    new_io.c_cc[VTIME]= 0; /*  immediately  */
	    tcsetattr(STDIN_FILENO, TCSANOW, &new_io);
	} else
	{
	    fprintf(stderr,"Could not switch to raw mode!\n");
	}
    } else
    {
	tcsetattr(STDIN_FILENO, TCSANOW, &old_io);
    }
}

int statcpy(frdinfo*a)
{
    static struct stat*astat;
    astat=&(a->info);
    if(lstat(a->name, astat) != 0)
    {
	perror("lstat() failed while reading file statistics");
	return(-1);
    }
    a->size = (extratyp==0) ? a->info.st_size : extra[extratyp].size(a->name);
    if(0)
    fprintf(stderr,">>%s>> %d - %d - %d - %d - %d\n",a->name,(int)(a->size) ,(int)(a->info.st_size),
	(int)(extra[extratyp].start(a->name)),(int)(extra[extratyp].end(a->name)),
	(int)(extra[extratyp].size(a->name)) );
    return 0;
}

const char*getpwd(void)
{
    size_t s=PWDSIZE;
    if( pwd==NULL )
    {
	pwd=calloc( s, sizeof(char) );
	getcwd( pwd, s );
    }
    assert( pwd!=NULL );
    return pwd;
}

const char*symname(const char*a,const char*b)
{
    static char*name=NULL;
    static char*dest=NULL;
    static const char*d=NULL, *ptr=NULL;
    int n,hierarchies=0;
    
    if(a[0]=='/') return a;	 	   /* full source Pathnames are kept */
    if(b[0]=='/')	  	/* source has relative, target has full path */
    {
	dest=calloc(strlen(pwd)+strlen(a)+2,sizeof(char));
	strcpy( dest, pwd );
	strcat( dest, "/" );
	strcat( dest,  a  );
	d=dest;
    } else d=a;			/* otherwise both pathnames are not complete */
    for( n=0; d[n]==b[n] && d[n]!=0 && b[n]!=0; n++); /*Filenames now differ */
    if( d[n]!=b[n] ) n--; 	/* avoid difficult situations with directory */
    for(;d[n]!='/' && n>0;n--);		      /* back to last directory mark */
    if(n!=0 && d[n]=='/')n++;			     /* should never be true */
    ptr=&b[n];			    /* upto here the pathnames are identical */
    for(hierarchies=0;ptr!=NULL;hierarchies++)
    {		/* determine how many dirlevels the target is located deeper */
	if( *ptr=='/' && ptr[1]=='/' )
	    hierarchies--;			  /* ignore multiple slashes */
	ptr=strchr(ptr,'/');
	if(ptr!=NULL && *ptr=='/')
	    ptr=ptr+1;				     /* avoids endless loops */
    }
    ptr=&d[n];	    /* symlink must point the determined hierarchy levels up */
    if(hierarchies>1)
    {
	if(name!=NULL) free(name);
	name=calloc(strlen(ptr)+3*hierarchies,sizeof(char));
	for(;hierarchies>1;hierarchies--) strcat(name,"../");
	ptr=strcat(name,ptr);			  /* before aiming to target */
    }
    return ptr;
}

void linkfilesecure(const char* aname,const char* bname,int(*lnk)(const char *, const char *))
{
    char tmpfilename[MAXPATHLENGTH+16];
    char*dirname=NULL,*ptr=NULL;
    const char*symaname=aname;
    static struct stat tstat,dstat;
    struct utimbuf utimecache;
    int gotdirtime=0;
    /* 
     * generate unique tmp file name
     * move file to tmp ;
     * later link it ;
     * finally remove tmp
     */
    if( testmode==0 )
    {
	strcpy(tmpfilename,bname);
	strcat(tmpfilename,".XXXXXX");
	/*
	 * Please note that the warning on mktemp() is acceptable. 
	 * Since we intend to move the file we cannot create it with mkstemp().
	 */
	if( mktemp(tmpfilename) == NULL )
	{
	    perror("There is no unique temporary file name.");
	}
	if( dirmtime!=0 )
	{
	    dirname = calloc(sizeof(char),strlen(bname)+2);
	    strcpy(dirname,bname);
	    ptr=strrchr(dirname,'/');
	    if( ptr!=NULL ) { *ptr='\0'; }
	    gotdirtime = !lstat(dirname,&dstat);
	    if( gotdirtime==0 )
		perror("Cannot read directory time stamp.");
	}
	if( lstat(tmpfilename,&tstat) != 0 )
	{
	    /*
	     * The error needs not to be catched, since it is wanted
	     * that no file exists with the target name
	     */
	    rename( bname, tmpfilename );
	} else
	{
	    // fprintf(stderr,"Not able to rename \"%s\" to \"%s\".",bname,tmpfilename);
	    errno = EEXIST;
	    perror("Not able to rename.");
	}
	if( lnk == symlink )
	{
	    symlinked++;
	    symaname=symname( aname, bname );
	}
	if( lnk( symaname, bname ) != 0 )
	{
	    // linking failed! 
	    // try to move original in place again and log that fact
	    //fprintf(stderr, "Linking failed. Trying rollback: \"%s\"", bname);
	    errno = ECANCELED;
	    perror("Linking failed - rollback appropriate");
	    if ( rename( tmpfilename, bname ) != 0 )
	    {
		// moving old file in place again failed.
		// check should be made to fail early, to pause or use noaction from now on
		//fprintf(stderr, "unable to roll back: \"%s\" left", tmpfilename);
		errno = ECANCELED;
		perror("Rollback failed");
	    }
	}
	else 
	    // unlink renamed original only of linking was successful
	    if( unlink( tmpfilename ) != 0 )
	    {
		// unlinking failed!
		fprintf(stderr, "Unlink failed: \"%s\"", tmpfilename);
	    }
	if( (dirmtime!=0) & (gotdirtime!=0) )
	{
	    utimecache.actime  = dstat.st_atime;
	    utimecache.modtime = dstat.st_mtime;
       	    if( utime( dirname, &utimecache) != 0)
		perror("Failed to restore directory timestamp.");
	    free( dirname );
	}
    }
    if( (showmode!=0) || (interact!=0) )
    {
	printf("ln %s \"%s\" \"%s\"\n",( lnk == symlink )?"-s":"-f",symaname,bname);
    }
    return;
}

void linkfiles(const char* a, const char* b, const struct stat*ainfo)
{
    struct stat *binfo,bstat;
    binfo = &bstat;
    if( lstat(b, binfo) != 0 ) return;
#ifdef DEBUG
    fprintf(stderr,"testmode=%d,showmode=%d,weaklink=%d,hardlink=%d,nosymlinks=%d,showsave=%d\n",
	testmode,showmode,weaklink,hardlink,nosymlinks,showsave);
#endif /*DEBUG*/
    if( (ainfo->st_ino == binfo->st_ino) 
     && (ainfo->st_dev == binfo->st_dev) )
    {
	/* since they are already linked
	 * do not spend more time on them
	 */
	return;
    }
    if( S_ISLNK(ainfo->st_mode) || S_ISLNK(binfo->st_mode) )
    {
        /* either one is already symlinked
	 * this avoids building up symlink chains (in case of a)
	 * or alternating symlinks during sort (in case of b)
	 */
	return;
    }
    if((nosymlinks!=0) && (ainfo->st_dev != binfo->st_dev) )
    {
        /* when on different devices and symlinks are not allowed
	 * do not link, i.e. no also not prepare to link.
	 * This needs to be catched before any execution.
	 */
	return;
    }
    if((nonzero!=0) && (ainfo->st_size == 0) )
    {
        /* when only non-empty files are allowed to be linked
	 * do not link, when they are identical and has size zero.
	 * This needs to be catched before any execution.
	 */
	return;
    }
    if((weaklink==0) && ( ainfo->st_dev == binfo->st_dev ) )
    {
	/* 
	 * either st_dev or st_ino are different,
	 * was checked earlier prior to call
	 */
	linkfilesecure(a,b,link);
    } else
    {
	assert( weaklink || !nosymlinks );
	if(hardlink==0 )
	{
	    linkfilesecure(a,b,symlink);
	}
    }
    if( showsave!=0 ) 
    {
	printf("Linking \"%s\" saves %d bytes\n",
		basename(b), 
		(int)(binfo)->st_size);
    }
    linksaved+=(int)(binfo)->st_size;
    totalinked++;
    return;
}

void realloc_dupinfo(dinfo*a)
{
	if(0) fprintf(stderr,"%s \t%d -> %d\n",a->name[a->count-1],(int)a->size, (int)(a->size)*2 );
	a->size+= (a->size); /* double size */
	a->name = realloc(a->name, sizeof(char**) * ( a->size + 1 ) );
	a->prio = realloc(a->prio, sizeof(int*) * ( a->size  + 1) );
	if( (a->name) == NULL || (a->prio) == NULL )
	{
	    perror("realloc() to extend dup info structure");
	    exit(-1);
	}
	memset(&(a->name[(a->size)>>1]),0,sizeof(char**)*((a->size)>>1)); 
	memset(&(a->prio[(a->size)>>1]),0,sizeof( int *)*((a->size)>>1)); 
	return;
}

int build_dupinfo_list(frdinfo*a, frdinfo*b)
{
    int n=0,m=0;

    /* merge or create dups_info entries */
    if( (a->ident == NULL) && (b->ident == NULL) )
    {
	assert(LETTERS>3);	 /* LETTERS should be 26 or greater for production */
	dups_info[dupscount].size = LETTERS; /* originally designed for interaction */
	dups_info[dupscount].name = (char**)calloc(dups_info[dupscount].size+1,sizeof(char**));
	dups_info[dupscount].prio = (int*)calloc(dups_info[dupscount].size+1,sizeof(int*));
	dups_info[dupscount].name[0] = a->name;
	dups_info[dupscount].prio[0] = a->prio;
	dups_info[dupscount].name[1] = b->name;
	dups_info[dupscount].prio[1] = b->prio;
	dups_info[dupscount].name[2] = NULL;
	dups_info[dupscount].replace = NULL;
	/* other pointers are NULL, since calloc sets to zero */
	a->ident = b->ident = &dups_info[dupscount];
	dups_info[dupscount].count = 2;	/* redundant information, maybe retrieved from lists */
	dupscount++;
    } else
    {
	if( a->ident == NULL )
	{

	    /*clean up tangling references */
	    if( (b->ident->replace!=NULL) )
	    {
		if(!b->ident->replace->name)
		    b->ident->replace->name = b->ident->name;
		b->ident = b->ident->replace;
	    }
	    if( b->ident->name==NULL ) {
		b->ident->name = calloc(b->ident->size+1, sizeof(char**));
	    }
 	    a->ident = b->ident;
	    /* simply inherit reference list and append the new filename */
	    a->ident = b->ident;
	    for(n=0;b->ident->name[n]!=NULL;n++)
	    {
		if((n+1)==(b->ident->size)) realloc_dupinfo(b->ident);
	    }
	    b->ident->name[n+1] = NULL;
	    b->ident->name[n] = a->name;
	    b->ident->prio[n] = a->prio;
	    b->ident->count++;	/* redundant information, maybe retrieved from lists */
	} else
	if( b->ident == NULL )
	{
	    /*clean up tangling references */
	    if( (a->ident->replace!=NULL) )
	    {
		if(!a->ident->replace->name)
		    a->ident->replace->name = a->ident->name;
 		a->ident = a->ident->replace;
 	    }
	    if( a->ident->name==NULL ) {
		a->ident->name = calloc(a->ident->size+1, sizeof(char**));
	    }
	    /* simply inherit reference list and append the new filename */
	    b->ident = a->ident;
	    for(n=0; a->ident->name[n]!=NULL;n++)
	    {
		if((n+1)==(a->ident->size)) realloc_dupinfo(a->ident);
	    }
	    a->ident->name[n+1] = NULL;
	    a->ident->name[n] = b->name;
	    a->ident->prio[n] = b->prio;
	    a->ident->count++;	/* redundant information, maybe retrieved from lists */
	} else
	{
	    /*clean up tangling references */
	    while( ((a->ident->replace)!=NULL) )
	    {
		a->ident = a->ident->replace;
	    }
	    while( ((b->ident->replace)!=NULL) )
	    {
		b->ident = b->ident->replace;
	    }
	    /* merge the reference lists */
	    assert( a->ident->name!=NULL );
	    assert( b->ident->name!=NULL );
	    if( a->ident != b->ident )
	    {
		for(n=0;a->ident->name[n]!=NULL;n++)
		{
		    if(0) fprintf(stderr,"%d> %s\n",n,a->ident->name[n]);
		    if( (n+1)==(a->ident->size) ) 
			realloc_dupinfo(a->ident);
		}
		for(m=0;b->ident->name[m]!=NULL;m++) 
		{
		    if( (n+m+1)==(a->ident->size) ) 
			realloc_dupinfo(a->ident);
		    a->ident->name[n+m] = b->ident->name[m];
		    a->ident->prio[n+m] = b->ident->prio[m];
		    a->ident->count++;
		    if(0) fprintf(stderr,"%d# %s\n",n+m,a->ident->name[n+m]);
		}
		a->ident->name[n+m+1] = NULL;
		assert((n+m) < a->ident->size);
		free(b->ident->name);
		b->ident->name = NULL;
		b->ident->count= 0;
		/* avoid active search for tangling references */
		b->ident->replace = a->ident;
		b->ident = a->ident;
	    }
	}
    }
    return 0;
}

void dupinfo_global(dinfo*a,const char key)
{
    static struct stat *astat,*sstat;
    int n=-1,toppri=filecount,topsource=0,*adev,
	oldsource=0,newsource=0,oldest=0,newest=0,
	bigsource=0,tinysource=0,bigest=0,tinyest=0,
	maxsource=0,maxlink=0,source=0,printsource=0;
    static struct stat localstat,sourcestat;
    astat=&localstat;
    sstat=&sourcestat;
    adev= (int*)calloc(a->size+2,sizeof(int));
    if(adev==NULL)
    {
	perror("calloc() to extend adev help structure");
	exit(-1);
    }
    /* set the default to the first existing entry */
    do {
	n++; /* starts with 0 */
	if(a->name[n]==NULL) return;
    } while( lstat(a->name[n], astat) != 0 );
    toppri=a->prio[n];
    oldest=newest=astat->st_mtime;
    bigest=tinyest=astat->st_size;
    maxlink=astat->st_nlink;
    /* look for the best fitting values */
    for(   ; a->name[n]!=NULL; n++ )
    {
	if( lstat(a->name[n], astat) != 0 )
	{
	    /* ignore this file, probably deleted */
	    adev[n]=0;
	    continue;
	}
        adev[n]=astat->st_dev;
	if( toppri > a->prio[n] )
	{
	    topsource=n;
	    toppri=a->prio[n];
	}
	if( oldest > astat->st_mtime )
	{
	    oldsource=n;
	    oldest=astat->st_mtime;
	}
	if( newest < astat->st_mtime )
	{
	    newsource=n;
	    newest=astat->st_mtime;
	}
	if( maxlink < astat->st_nlink )
	{
	    maxsource=n;
	    maxlink=astat->st_nlink;
	}
	if( bigest < astat->st_size )
	{
	    bigsource=n;
	    bigest=astat->st_size;
	}
	if( tinyest > astat->st_size )
	{
	    tinysource=n;
	    tinyest=astat->st_size;
	}
    }
    switch(key)
    {
	case '@':  source=maxsource; break;
	case '#':  source=topsource; break;
	case '<':  source=oldsource; break;
	case '>':  source=newsource; break;
	case '+':  if(extratyp!=0) source=bigsource; else source=newsource; break;
	case '-':  if(extratyp!=0) source=tinysource; else source=oldsource; break;
	default :  source=0;         break;
    }
    lstat(a->name[source], sstat);
    printsource=totalinked;
    for( n=0; a->name[n]!=NULL; n++ )
    {
	if( testmode!=0 )
	{
	    if((showhard!=0) && n==0) myputs("");
	    if( (lstat( a->name[n], astat)==0))
	    {
		if( n!=source
		 && ((astat->st_dev!=sstat->st_dev)
		 ||  (astat->st_ino!=sstat->st_ino)))
		{
		    totalinked+=1;
		    linksaved+=astat->st_size;
		    if(totalinked==printsource+1 && !showhard)
			printf( "\n%s\n", a->name[source] );
		    printf( "%s\n", a->name[n] );
		} else
	        if(showhard!=0)
		    printf( "%s\n", a->name[n] );
	    }
        } else
	if( adev[n]!=0 && n!=source )
	    linkfiles(a->name[source],a->name[n],sstat);
    }
    return;
}

int dupinfo_notlinked(dinfo*a)
{
    int n=0;
    static struct stat *astat,*sstat;
    static struct stat localstat,sourcestat;
    astat=&localstat;
    sstat=&sourcestat;
    while( lstat(a->name[n], sstat) != 0 && a->name[n]!=NULL )
    {
	/* ignore this file, probably deleted */
	n++;
    }
    for( n++; a->name[n]!=NULL; n++ )
    {
	if( lstat(a->name[n], astat) != 0 )
	{
	    /* ignore this file, probably deleted */
	    continue;
	}
	if( (sstat->st_dev != astat->st_dev)
	||  (sstat->st_ino != astat->st_ino) )
	{
	    return 1;
	}
    }
    return 0;
}
	
int dupinfo_entry(dinfo*a)
{
    static struct stat *astat,*sstat;
    static int n,m,min=0,max=0,cnt=0,source,key;
    int toppri=filecount,topsource=0,*adev,k=0,
	oldsource=0,newsource=0,oldest=0,newest=0,
	bigsource=0,tinysource=0,bigest=0,tinyest=0,
	maxsource=0,maxlink=0;
    static struct stat localstat,sourcestat;
    astat=&localstat;
    sstat=&sourcestat;
    adev= (int*)calloc(a->size+2,sizeof(int));
    if(adev==NULL)
    {
	perror("calloc() to extend adev help structure");
	exit(-1);
    }
    for( n=0; a->name[n]!=NULL && n<=('Z'-'A'); n++ )
    {
	/* commands are given by letters
	 * therefore the display is restricted to 26 lines
 	 */
	if( lstat(a->name[n], astat) != 0 )
	{
	    /* ignore this file, probably deleted */
	    continue;
	}
	if( newest==0 )
	{
	    toppri=a->prio[n];
	    oldest=newest=astat->st_mtime;
	    maxlink=astat->st_nlink;
	    bigest=tinyest=astat->st_size;
	}
	if( testmode!=0 )
	{
	    printf( "%s\n", a->name[n] );
	} else
	{
	    if( extratyp!=0 )
	    {
	    printf( " %d : %c [%9d:%04x:%06x:%04o:%01d:%c%c] %s\n",
		n, n+'A',
		(int)astat->st_size,
		(int)astat->st_dev,
		(int)astat->st_ino,
		(int)S_PERM(astat->st_mode),
		(int)astat->st_nlink,
		extra[extratyp].end(a->name[n])?tag1char:' ',
		extra[extratyp].start(a->name[n])?tag2char:' ',
		a->name[n]);
	    } else
	    printf( " %d : %c [%04x:%06x:%04o:%01d] %s\n",
		n, n+'A',
		(int)astat->st_dev,
		(int)astat->st_ino,
		(int)S_PERM(astat->st_mode),
		(int)astat->st_nlink,
		a->name[n]);
	    if(n==('Z'-'A'))
		printf( "WARNING: Full list cannot be processed interactively.\n" );
	}
        adev[n]=astat->st_dev;
	if( toppri > a->prio[n] )
	{
	    topsource=n;
	    toppri=a->prio[n];
	}
	if( oldest > astat->st_mtime )
	{
	    oldsource=n;
	    oldest=astat->st_mtime;
	}
	if( newest < astat->st_mtime )
	{
	    newsource=n;
	    newest=astat->st_mtime;
	}
	if( maxlink < astat->st_nlink )
	{
	    maxsource=n;
	    maxlink=astat->st_nlink;
	}
	if( bigest < astat->st_size )
	{
	    bigsource=n;
	    bigest=astat->st_size;
	}
	if( tinyest > astat->st_size )
	{
	    tinysource=n;
	    tinyest=astat->st_size;
	}
    }
    if( testmode==0 )
    {
        static char buffer[MAXPATHLENGTH],*target=NULL;
	/* 
	 * analyse and execute the answer
	 */
	target= (char*)calloc(a->size+2,sizeof(int));
	printf("Delete on number, Link on letter, Symlink on Capital (first is source)\t");
	strncpy(target,EMPTYSTRING,n);
	source=-1;
	if(target==NULL)
	{
	    perror("calloc() to extend target help structure");
	    exit(-1);
	}
	buffer[0]=buffer[1]=buffer[2]=buffer[3]=0;
	ttydirect(1);
	while(buffer[0]!='\n') {
	    fflush(stdout);
	    //fgets(buffer, MAXPATHLENGTH, stdin);
	    buffer[0]=getchar();
	    for(m=0;buffer[m]!=0 && m<MAXPATHLENGTH;m++)
	    {
		key=buffer[m];
		if( isalpha(key) && source==-1 )
		{   source=toupper(key)-'A';
		    if(source>=n) source=-1;
		} else {
		if( (key == '-') ) { source=(extratyp!=0)?tinysource:oldsource;
				     for(k=0;a->name[k]!=NULL;k++)
					target[k]=(adev[k]==adev[source])?'L':'S'; }
		if( (key == '+') ) { source=(extratyp!=0)? bigsource:newsource;
				     for(k=0;a->name[k]!=NULL;k++)
					target[k]=(adev[k]==adev[source])?'L':'S'; }
		if( (key == '@') ) { source=maxsource; 
				     for(k=0;a->name[k]!=NULL;k++)
					target[k]=(adev[k]==adev[source])?'L':'S'; }
		if( (key == '#') ) { source=topsource; 
				     for(k=0;a->name[k]!=NULL;k++)
					target[k]=(adev[k]==adev[source])?'L':'S'; }
		if( (key == '<') ) { source=oldsource; 
				     for(k=0;a->name[k]!=NULL;k++)
					target[k]=(adev[k]==adev[source])?'L':'S'; }
		if( (key == '>') ) { source=newsource; 
				     for(k=0;a->name[k]!=NULL;k++)
					target[k]=(adev[k]==adev[source])?'L':'S'; }
		if( isupper(key) ) { max++; if(key-'A'<n) target[(key-'A')]='S'; }
		if( islower(key) ) { min++; if(key-'a'<n) target[(key-'a')]='L'; } }
		if( isdigit(key) ) { cnt++; if(key-'0'<n) target[(key-'0')]='D'; }
		if( key=='\033'  ) { strncpy(target,EMPTYSTRING,n); source=-1;}
	    }
	    printf(EMPTYSTRING "\r  $ "); fflush(stdout);
	    printf(EMPTYSTRING "\r  $ ");
	    if( source!=-1 )
	    {
		if(!strncmp(EMPTYSTRING,target,n))
		    printf("\r <RETURN> to confirm. <ESC> to clear. All links will point to <%c>.",source+'a');
		target[source] = 'Q';
	    }
	    for(m=0;m<n;m++)
	    {
		switch(target[m])
		{
		    case 'D': printf("rm <%d>; ", m); break;
		    case 'S': printf("ln -s <%c> <%c>; ",source+'A', m+'A'); break;
		    case 'L': printf("ln <%c> <%c>; ",source+'a', m+'a'); break;
		}
	    }
	}
	ttydirect(0);
	if( (source==-1) && (max+min>0) ) return 1; /* all other cases are treated as valid */
        if( (max>0) && ((a->name[source])[0] != '/') )
	{
	    if(0) printf("DANGER: Resulting symlinks can be invalid due to relative pathname!");
	}
	/* 
	 * tell user what we will do
	 */
	myputs("");
	lstat(a->name[source],sstat);
	for( n=0; a->name[n]!=NULL; n++ )
	{
	    if( (lstat(a->name[n], astat) != 0) )
	    {
		/* ignore this file, probably deleted */
		continue;
	    }
	    if( astat->st_dev==sstat->st_dev && astat->st_ino==sstat->st_ino )
	    {
		/* ignore this file, it is already linked */
		if(n!=source)
		    printf( "\t$ #already linked %s\n", a->name[n] );
		continue;
	    }
	    switch(target[n])
	    {
		case 'S':  printf("\t$ ");
			   linkfilesecure(a->name[source],a->name[n],symlink);
			   linksaved+=(int)astat->st_size;
			   totalinked++;
			   break;
		case 'L':  printf("\t$ ");
			   linkfilesecure(a->name[source],a->name[n],link);
			   linksaved+=(int)astat->st_size;
			   totalinked++;
			   break;
		case 'D':  printf( "\t$ rm -f %s\n", a->name[n] );
			   unlink( a->name[n] );
			   freedspace+=(int)astat->st_size;
			   deleted++;
			   break;
		case 'Q':  break;
		case ' ':  printf( "\t$ #keep %s\n", a->name[n] );
			   break;
		default :  break;
	    }
	}
	free(target);
    } else
    {
	totalinked+=(n-1);
	linksaved+=((n-1)*astat->st_size);
    }
    return 0;
}


void print_dupinfo(void)
{
    int n=0;
    for(n=0;n<dupscount;n++)
    {
	if(dups_info[n].name!=NULL)
	{
	    if(testmode!=0)
	    {	dupinfo_global(&dups_info[n],globalkey);
	    } else
	    {
	        if( (showhard!=0) || dupinfo_notlinked(&dups_info[n]) )
		{
		    if(extratyp!=0)
			printf("\nDel:Lnk [filesize:devc:i-node:perm:L:tag] <filename>\n");
		    else
			printf("\nDel:Lnk [dev:i-node:perm:L] <filename>\n");
		    if( dupinfo_entry(&dups_info[n])!=0 ) n--;
		}
	    }
	}
    }
}

void exec_dupinfo(void)
{
    int n=0;
    for(n=0;n<dupscount;n++)
    {
	if(dups_info[n].name!=NULL)
	{
	    dupinfo_global(&dups_info[n],globalkey);
	}
    }
}

int comparison_by_content_hash(const void*a,const  void*b)
{
    FILE*afile=NULL,
	*bfile=NULL;
    char ablock[BLOCK+2],
	 bblock[BLOCK+2];
    int  aread=0,
	 bread=0,
	 result=0;
    size_t asize=0, bsize=0;
    long   atell=0, btell=0;
    assert(a!=NULL);
    assert(b!=NULL);
    afile=fopen(((frdinfo*)a)->name,"r");
    if( afile == NULL )
    {
	/* perror("fopen() failed - file was probably deleted"); */
	return(-1);
    }
    bfile=fopen(((frdinfo*)b)->name,"r");
    if( bfile == NULL )
    {
	/* perror("fopen() failed - file was probably deleted"); */
	return(1);
    }
    fseek(afile,extra[extratyp].start(((frdinfo*)a)->name),SEEK_SET);
    fseek(bfile,extra[extratyp].start(((frdinfo*)b)->name),SEEK_SET);
    if(usehash>1)
    {
	if( ((frdinfo*)a)->sha1pos == 0)
	{
	    assert( (((frdinfo*)a)->hash==0) );
	    SHA1INIT(&(((frdinfo*)a)->context));
	    ((frdinfo*)a)->sha1pos = ftell(afile);
	}
	if( ((frdinfo*)b)->sha1pos == 0)
	{
	    assert( (((frdinfo*)b)->hash==0) );
	    SHA1INIT(&(((frdinfo*)b)->context));
	    ((frdinfo*)b)->sha1pos = ftell(bfile);
	}
    }
    asize=(((frdinfo*)a)->size);
    bsize=(((frdinfo*)b)->size);
    while(!(feof(afile)||feof(bfile)||asize==0||bsize==0||result!=0))
    {
        aread = fread(ablock,1,(asize<BLOCK)?asize:BLOCK,afile);
	atell = ftell(afile);
	if( ((frdinfo*)a)->sha1pos < atell )
	{
	    assert( atell-((frdinfo*)a)->sha1pos <= BLOCK );
	    SHA1UPDATE( &(((frdinfo*)a)->context),
		&ablock[aread-atell+((frdinfo*)a)->sha1pos],
			      atell-((frdinfo*)a)->sha1pos);
	    ((frdinfo*)a)->sha1pos = atell;
	}
        asize = asize - aread;
	assert((aread!=0)||feof(afile)||(asize==0));
        bread = fread(bblock,1,(bsize<BLOCK)?bsize:BLOCK,bfile);
	btell = ftell(bfile);
	if( ((frdinfo*)b)->sha1pos < btell )
	{
	    assert( btell-((frdinfo*)b)->sha1pos <= BLOCK );
	    SHA1UPDATE( &(((frdinfo*)b)->context),
		&bblock[bread-btell+((frdinfo*)b)->sha1pos],
			      btell-((frdinfo*)b)->sha1pos);
	    ((frdinfo*)b)->sha1pos = btell;
	}
        bsize = bsize - bread;
	assert((bread!=0)||feof(bfile)||(bsize==0));
	assert(aread==bread||feof(afile)||feof(bfile));
	result=memcmp(ablock,bblock,aread);
	if(0) fprintf(stderr,"%s|%s> %d/%ld\n",((frdinfo*)a)->name,((frdinfo*)b)->name,result,atell);
    }
    if( feof(afile) && (((frdinfo*)a)->sha1pos != -1) )
    {
	// assert(result==0); // not always correct!!!!
	((frdinfo*)a)->hash = calloc(SHA1_HASH_SIZE+2,sizeof(char));
	SHA1FINAL(&(((frdinfo*)a)->context), (unsigned char*)(((frdinfo*)a)->hash));
	((frdinfo*)a)->sha1pos = -1;
	hashread++;
    }
    if( feof(bfile) && (((frdinfo*)a)->sha1pos != -1) )
    {
	// assert(result==0); // not always correct!!!!
	((frdinfo*)b)->hash = calloc(SHA1_HASH_SIZE+2,sizeof(char));
	SHA1FINAL(&(((frdinfo*)b)->context), (unsigned char*)(((frdinfo*)b)->hash));
	((frdinfo*)b)->sha1pos = -1;
	hashread++;
    }
    if( fclose(afile) ) perror("Error closing hash comparison file A.");
    if( fclose(bfile) ) perror("Error closing hash comparison file B.");
    if( result!=0 )
    {
	/* this return is delayed. In case two files differ in the last byte,
	 * both files are read and hash calculation can be completed.
	 * If we would not delay the return, all bytes are read, but hash
	 * will remain incomplete.
	 */
	return(result);
    }
    build_dupinfo_list(((frdinfo*)a),((frdinfo*)b));
    return(0);
}

int comparison_by_content(const void*a,const  void*b)
{
    FILE*afile=NULL,
	*bfile=NULL;
    char ablock[BLOCK+2],
	 bblock[BLOCK+2];
    int  aread=0,
	 bread=0,
	 result=0;
    size_t asize=0, bsize=0;
    assert(a!=NULL);
    assert(b!=NULL);
    afile=fopen(((frdinfo*)a)->name,"r");
    if( afile == NULL )
    {
	/* perror("fopen() failed - file was probably deleted"); */
	return(-1);
    }
    bfile=fopen(((frdinfo*)b)->name,"r");
    if( bfile == NULL )
    {
	/* perror("fopen() failed - file was probably deleted"); */
	return(1);
    }
    fseek(afile,extra[extratyp].start(((frdinfo*)a)->name),SEEK_SET);
    fseek(bfile,extra[extratyp].start(((frdinfo*)b)->name),SEEK_SET);
    asize=(((frdinfo*)a)->size);
    bsize=(((frdinfo*)b)->size);
    while(!(feof(afile)||feof(bfile)||asize==0||bsize==0))
    {
        aread = fread(ablock,1,(asize<BLOCK)?asize:BLOCK,afile);
        asize = asize - aread;
	if(!((aread!=0)||feof(afile)||(asize==0)))
	{
	    // should not happen - indicates inconsistent filesystems (e.g. /sys)
	    // rarely matches some kind of race condition, i.e. file modified during access
	    fprintf(stderr, "WARNING: File \"%s\" not properly read.\n", (((frdinfo*)a)->name) );
	    result=-1;
	} else 
	{
	    bread = fread(bblock,1,(bsize<BLOCK)?bsize:BLOCK,bfile);
	    bsize = bsize - bread;
	    if(!((bread!=0)||feof(bfile)||(bsize==0)))
	    {
		// should not happen - indicates inconsistent filesystems (e.g. /sys)
		// rarely matches some kind of race condition, i.e. file modified during access
		fprintf(stderr, "WARNING: File \"%s\" not properly read.\n", (((frdinfo*)b)->name) );
		result=1;
	    } else
	    {
		assert(aread==bread||feof(afile)||feof(bfile));
	        result=memcmp(ablock,bblock,aread);
	    }
	}
	if( result!=0 )
	{
	    if( fclose(afile) ) perror("Error closing comparison file A file.");
	    if( fclose(bfile) ) perror("Error closing comparison file B file.");
	    return(result);
        }
    }
    if( fclose(afile) ) perror("Error closing comparison file A file.");
    if( fclose(bfile) ) perror("Error closing comparison file B file.");
    build_dupinfo_list(((frdinfo*)a),((frdinfo*)b));
    return(0);
}

static int comparison_by_size(const void*a, const void*b)
{
    static int sizediff;
    int hashdifference=0;
    assert(a!=NULL);
    assert(b!=NULL);
    sizediff = (( (((frdinfo*)a)->size) 
                - (((frdinfo*)b)->size) ));
    /*
     * avoid work when not desired
     * follow the minbytes directive
     * "-m" does not work on payload
     */
    if((minbytes > (((frdinfo*)a)->info.st_size))
    || (minbytes > (((frdinfo*)b)->info.st_size)))
      {
        return(sizediff);
      }
    /*
     * up to here it is obvious:
     * - different size different file
     * - same size -> differs file?
     */
    if(sizediff == 0)
    {
	/* exclude weaklinks from linking, must not result in endless link loops */
	if( (((frdinfo*)a)->info.st_mode & S_IFMT) != S_IFREG 
	 || (((frdinfo*)b)->info.st_mode & S_IFMT) != S_IFREG )
	    /* same size but no linking allowed */
	    return 0;
	/* compare the path-less file names
	 * if the samename directive (-f) requires this */
	if( strcmp(basename(((frdinfo*)a)->name),basename(((frdinfo*)b)->name)) 
	 && (samename==1) )
	    /* different names do not allow linking */
	    return strlen(basename(((frdinfo*)a)->name))-strlen(basename(((frdinfo*)b)->name));
	/* check whether the user entries match (-u) */
	if( ((((frdinfo*)a)->info.st_uid)!=(((frdinfo*)b)->info.st_uid)) 
	 &&  (sameuser==1) )
	    /* different user ids do not allow linking */
	    return ((((frdinfo*)a)->info.st_uid)-(((frdinfo*)b)->info.st_uid));
	/* check whether the group entries match (-g) */
	if( ((((frdinfo*)a)->info.st_gid)!=(((frdinfo*)b)->info.st_gid)) 
	 &&  (samegrp ==1) )
	    /* different group ids do not allow linking */
	    return ((((frdinfo*)a)->info.st_gid)-(((frdinfo*)b)->info.st_gid));
	/* check whether the permissions match (-P / -p) */
	if( (((((frdinfo*)a)->info.st_mode)&(ALLPERMS)&permmask)
	   !=((((frdinfo*)b)->info.st_mode)&(ALLPERMS)&permmask)) 
	 && (sameperm==1) )
	    /* different permissions do not allow linking */
	    return 0;
	/* check whether the modification dates differ within range (-D / -d) */
	if( (abs((((frdinfo*)a)->info.st_mtime)-(((frdinfo*)b)->info.st_mtime)) > timediff) 
	 && (sametime==1) )		// maybe we should use labs() instead of abs()
	    /* different modification time do not allow linking */
	    return((((frdinfo*)a)->info.st_mtime)-(((frdinfo*)b)->info.st_mtime)); 
	/* do check whether already hardlinked */
	if( ((((frdinfo*)a)->info.st_ino == ((frdinfo*)b)->info.st_ino))
	 && ((((frdinfo*)a)->info.st_dev == ((frdinfo*)b)->info.st_dev)) )
	{
	    build_dupinfo_list(((frdinfo*)a),((frdinfo*)b));
	    return 0;
	}
#ifdef HASHSUM
	    if( usehash == 1 )
	    {
		if((((frdinfo*)a)->hash)== NULL) gethash((frdinfo*)a);
		if((((frdinfo*)b)->hash)== NULL) gethash((frdinfo*)b);
		if((((frdinfo*)a)->hash == NULL)
		|| (((frdinfo*)b)->hash == NULL))
		    return -1; /* most like means file is deleted or moved */
		hashdifference= strncmp( (((frdinfo*)a)->hash),
			     (((frdinfo*)b)->hash), hashme[hashmethod].length );
		if( hashdifference != 0 )
		{
		    hashreject++;
		    return( hashdifference );
		} else
		{
		    return comparison_by_content(a,b);
		}
	    } else
	    if( usehash == 2 )
	    {
		if( (((frdinfo*)a)->hash)!=NULL && (((frdinfo*)b)->hash)!=NULL )
		{
		    hashdifference= strncmp( (((frdinfo*)a)->hash),
				 (((frdinfo*)b)->hash), hashme[hashmethod].length );
		    if( hashdifference != 0 )
		    {
			hashreject++;
			return( hashdifference );
		    }
		}
		return comparison_by_content_hash(a,b);
	    } else
#endif
	    {
		return comparison_by_content(a,b);
	    }
    }
    return(sizediff);
}

void print_infolist(void)
{
    int n;
    for(n=0;n<filecount;n++)
    {
        printf("%9d %x:%05d %s\n",
	  (int)file_info[n].info.st_size,
	  (int)file_info[n].info.st_dev,
	  (int)file_info[n].info.st_ino,
	  file_info[n].name);
    }
}

void print_duplist(void)
{
    int n,m,identicalcount=0,alreadylinked=0;
    for(n=0;n<filecount;n++)
    {
        for(m=1;n+m<filecount && (file_info[n].size==file_info[n+m].size);m++)
        {
	    identicalcount=m+1;
	    if( (file_info[n].info.st_dev  == file_info[n+m].info.st_dev)
	    &&  (file_info[n].info.st_ino  == file_info[n+m].info.st_ino) )
	    {
		alreadylinked+=1;
	    }
if(0)	    printf("%01d\t%01d\t",identicalcount,alreadylinked);
	}
	if( (identicalcount==m) && (identicalcount==alreadylinked+1) )
	{
	    identicalcount=0;
	} 
 	alreadylinked=0;
	{
	printf("%9d %x:%05d %s\n",
	  (int)file_info[n].info.st_size,
	  (int)file_info[n].info.st_dev,
	  (int)file_info[n].info.st_ino,
	  file_info[n].name);
	    identicalcount--;
	}
    }
}

void printstats(void)
{
    myputs("");
    if(deleted!=0)
	fprintf(stderr,"%d Files (%d Bytes) have been%s deleted\n",
		    deleted,freedspace,(testmode!=0)?" marked to be":"");
    fprintf(stderr,"%d file%s of %d %sreplaced by links.\nThe total size of replac%s files %s %lld bytes.\n",
	totalinked,(totalinked==1)?"":"s",filecount,(testmode!=0)?"will be ":"",
	(testmode!=0)?"eable":"ed", (testmode!=0)?"is":"was", linksaved);
//    if( (testmode!=0) && (interact!=0) && (extratyp!=0) )
//	fprintf(stderr,"Byte count might not be accurate, for a reliable prospect do not use '-i'.\n");
#ifdef HASHSUM
    if( usehash!=0 )
    fprintf(stderr,"%s hash algorithm had to read %d files to avoid %d file comparisons.\n",
	hashme[hashmethod].type,hashread,hashreject);
#endif /* HASHSUM */
}

#define SCALE 1000
int readdirectory(frdinfo**fin, int*total, const int filecount, char*dir)
{
    int  count=filecount;
    char buffer[MAXPATHLENGTH];
    DIR *pwd;
    struct dirent *dirinfo;
    static int progress = 0;
    static char pindicator[] = "-\\|/.oOo";

    if (!(pwd=opendir(dir))) {
	perror(dir);
	return 0;
    }

    while ((dirinfo = readdir(pwd)) != NULL) 
    {
	if( !strcmp(dirinfo->d_name, ".") 
	 || !strcmp(dirinfo->d_name, "..") ) 
	{
	    continue;
	}
	/*
	 *
	 */
	if( (strlen(dirinfo->d_name)>0 )
	 && (strlen(dir)>0) )
	{
	    strcpy(buffer,dir);
	    if(buffer[strlen(buffer)-1]!='/')
		strcat(buffer,"/");
	} else continue;
	strcat(buffer,dirinfo->d_name);
	/*
	 *
	 */
	if (quietrun==0)
	{
	    if( (progress+1)/SCALE != (progress)/SCALE )
	        fprintf(stderr, "\rReading directory tree %c ",
	    	    pindicator[progress/SCALE+((extratyp)?4:0)]);
	    progress = (progress + 1) % (4*SCALE);
	}
	/*
	 *
	 */
	(*fin)[count].prio=count;
	(*fin)[count].name=(char*)malloc(strlen(buffer)+2);
	if( (*fin)[count].name == NULL )
	{
	    perror("malloc() for buffer in file info structure");
	    closedir(pwd);
	    exit(-1);
	}
	strcpy((*fin)[count].name,buffer);
	(*fin)[count].ident=NULL;
	/*
	 * if reading statistics fails,
	 * assume that the file has been deleted.
	 */
	if( (statcpy(&((*fin)[count]))!=0)  )
	{
	    free((*fin)[count].name);
	    continue;
	}
	if( S_ISDIR((*fin)[count].info.st_mode) )
	{ 
	    count = readdirectory(fin, total, count, buffer );
	    continue;
	} else
	if( S_ISREG((*fin)[count].info.st_mode) )
	{
	    if( (nonzero!=0) && ((*fin)[count].info.st_size==0) )
	    {
	        free((*fin)[count].name);
	        continue;
	    }
	} else
	{
	    /*
	     * ignore all other types like pipes, sockets, 
	     * links, character and block devices.
	     */
	    free((*fin)[count].name);
	    continue;
	}
	if( count > (*total)-2 )
	{   /*
	     * expand file_info array to take more members
	     */
	    (*total) = (*total) + INITIALFINFOCOUNT;
	    (*fin) = realloc((*fin), sizeof(frdinfo) * ( (*total) +4 ) );
	    if( (*fin) == NULL )
	    {
		perror("realloc() to extend file info structure");
		closedir(pwd);
		exit(-1);
	    }
	}
	if(0) fprintf(stderr,"%d:\t%s\n",count,(*fin)[count].name);
	count++;
    }
    closedir(pwd);
    return count;
}

int readtree( frdinfo**fin, int*total, const int filecount, FILE*in)
/*
 * "total" will be modified
 * "fin" might be modified
 * "in" is read until EOF
 */
{
    char buffer[MAXPATHLENGTH];
    int  count;

    for(count=filecount;!feof(in);count++)
    {
	fgets(buffer,MAXPATHLENGTH,in);
	if( !feof(in) && (strlen(buffer)>1) )
	{
	    (*fin)[count].prio=count;
	    (*fin)[count].name=(char*)malloc(strlen(buffer)+2);
	    if( (*fin)[count].name == NULL )
	    {
		perror("malloc() for buffer in file info structure");
		exit(-1);
	    }
	    assert(buffer[strlen(buffer)-1]=='\n');
	    /* fails if MAXPATHLENGTH is exceeded */
	    buffer[strlen(buffer)-1]=0;
	    strcpy((*fin)[count].name,buffer);
	    (*fin)[count].ident=NULL;
	    if( statcpy(&((*fin)[count]))!=0
	    /*
	     * Drop symbolic links and devices here
	     */
	    ||  !S_ISREG((*fin)[count].info.st_mode)
	    || ((nonzero!=0) && ((*fin)[count].info.st_size==0)) )
	    {
		free((*fin)[count].name);
		count--;
		continue;
	    }
	    if( count > (*total)-2 )
	    {   /*
		 * expand file_info array to take more members
		 */
		(*total) = (*total) + INITIALFINFOCOUNT;
		(*fin) = realloc((*fin), sizeof(frdinfo) * ( (*total) +4 ) );
		if( (*fin) == NULL )
		{
		    perror("realloc() to extend file info structure");
		    exit(-1);
		}
	    }
	} else count--;
    }
    return count;
}

void finalize(void)
{
    if(interact!=0)  print_dupinfo();
    else	  exec_dupinfo();
    if(quietrun==0) printstats();
	          return;
}

void finalsig(int sig)
{
    signal(SIGINT,SIG_DFL);
    fprintf(stderr,"Signal received!\nLinking files found so far.");
    exit(0);
}

int
main (int argc, char *argv[])
{
    int i,rdcfg=0;
    FILE*liste;
    char c,buffer[MAXPATHLENGTH],stopornot[128];
    char*environment=NULL;
    program_name = argv[0];
    getpwd();
#ifdef HAVE_SETLOCALE
  setlocale (LC_ALL, "");
#endif
#ifdef DEBUG
  /* Locale is not used in current implementation
   * Time is not used in current implementation
   */
  start_time = time (NULL);
  if(quietrun==0) printf ("cur_day_start = %s", ctime (&cur_day_start));
#endif /* DEBUG */
    exit_status = 0;

    file_info = (frdinfo*)calloc(frdinfosize+4,sizeof(frdinfo));
    if( file_info == NULL )
    {
        perror("calloc() to provide file info structure");
        return(-1);
    }
    filecount=0;
    while ((c = getopt_long(argc, argv, "ab:cdD:e:fghHik:lm:Nno:pP:qst:TuvVwWx:#:0",long_options,NULL)) != (char)EOF) 
    {
        switch (c)
	{
	  case 'a': sameperm	= sameuser = samegrp = 1; break;
	  case 'b': if(pwd!=NULL) free(pwd);
		    pwd = calloc( strlen(optarg)+2, sizeof(char) );
		    strcpy( pwd, optarg );     
		    chdir( pwd );	 	break;
	  case 'c': showsave	= 1-showsave;	break;
	  case 'd': sametime	= 1-sametime;
		    timediff	= sametime?timediff:0;
						break;
	  case 'D': timediff	= atoi(optarg);
		    sametime	= 1;		break;
	  case 'e': if(environment!=NULL) free(environment);
		    environment= calloc( strlen(optarg)+2, sizeof(char) );
		    strcpy( environment, optarg );
		    rdcfg=readconfig(environment);break;
	  case 'f': samename	= 1-samename;	break;
	  case 'g': samegrp	= 1-samegrp ;	break;
	  case '?':
	  case 'h': usage();                  exit(0);
	  case 'H': showhard	= 1-showhard;	break;
	  case 'i': interact	= 1-interact;	break;
	  case 'I': /*INETD*  for(;webconfig(HOMEDIR "/" WEBPAGE););*/ break;
	  case 'k': globalkey	= optarg[0];	break;
	  case 'l': hardlink	= 1-hardlink;	break;
	  case 'm': minbytes	= atoi(optarg);	break;
	  case 'N': testmode	= 1;		break;
	  case 'n': testmode	= 1-testmode;	break;
	  case 'o': if(findopta!=NULL) free(findopta);
		    findopta	= calloc( strlen(optarg)+2, sizeof(char) );
		    strcpy( findopta, optarg );	break;
	  case 'p': sameperm	= 1-sameperm;	break;
	  case 'P': permmask	= (int)strtol(optarg,NULL,8);
		    sameperm	= 1;		break;
	  case 'q': quietrun	= 1-quietrun;
		    showsave	= showmode = 0;	break;
	  case 's': nosymlinks	= 1-nosymlinks;	break;
#ifdef HASHSUM
	  case 't': hashmethod	= selecthash(optarg);
						break;
#endif
	  case 'T': dirmtime	= 1-dirmtime;	break;
	  case 'u': sameuser	= 1-sameuser;	break;
	  case 'v': showmode	= 1-showmode;	break;
	  case 'V': version();                exit(0);
	  case 'w': weaklink	= 1-weaklink;	break;
	  case 'W': dolisten();			break;
	  case 'x': extratyp	= selectextra(optarg);
						break;
#ifdef HASHSUM
	  case '#': usehash	= hashed(optarg);break;
#endif
	  case '0': nonzero	= 1-nonzero;	break;
	  default:
		fprintf(stderr,"wrong option: \"%c\"\n",c);
		usage();
		exit(-1);
	}
    }
    if( (sameperm!=0) && (permmask==0)) permmask=07777;
    if( environment!=NULL )
    {
	if(rdcfg==0) writeconfig(environment,argc-optind,&(argv[optind]));
	// if(rdcfg==0) writeconfig(environment);
    }
  
    if( (usehash!=0) && (hashmethod!=0) )
    {
	if( hashme[hashmethod].program != NULL )
	{
	    if( hashprobe(hashmethod,program_name) != 0 )
	    {
		hashmethod++;
	    } /* else break;*/
	    usehash=1; /* usehash==2 would disable external hash sums */
	}
	if( hashme[hashmethod].program == NULL )
	{
	    usehash=0;
	    fprintf(stderr,
		    "No working hashmethod found.\n"
		    "--> Use of hash methods is disabled.\n\n");
	}
    }
    if( 1==0 && (testmode==0) && (extratyp!=0) ) {
	fprintf(stderr,
		"Extra Style comparisons are not thouroughly tested yet.\n"
		"You may loose header information, if you press <return>.\n"
		"Use CTRL-C to stop.");
	scanf("%s",stopornot); }
    /* 
     * Collect files to scan.
     * Read them and fill in stat now
     */
    if( (argc == optind) && (filecount==0) )
    {	/*
	 * Read file names (not dir names) from stdin
	 * if no directories where read from environment
	 */
	if(quietrun==0) fprintf(stderr,"Taking file names from stdin\n\n");
	filecount = readtree( &file_info, &frdinfosize, 0, stdin );
	/*
	 * Now we will skip the following for-loop
	 */
    } else
    {
        for (i=optind;i < argc;i++)
	{
	    if(findopta!=NULL)
	    {
		sprintf(buffer,"find \"%s\" -type f %s -print", argv[i],(findopta==NULL)?"":findopta);
		liste = popen (buffer, "r");
		if(liste == 0)
		{
		    perror(buffer);
		    return(-1);
		}
		filecount = readtree( &file_info, &frdinfosize, filecount, liste );
		pclose(liste);
	    } else
	    {
		filecount = readdirectory( &file_info, &frdinfosize, filecount, argv[i] );
	    }
	}
    }
    /* worst case estimate is that all files are duplicate */
    /* but the worst case is that all files are identical */
    dups_info = (dinfo*)calloc( (filecount)+2, sizeof(dinfo) );
    if( dups_info == NULL )
    {
	perror("alloc() does not provide info structure for duplicate files");
	exit(-1);
    }
    if(quietrun==0) fprintf(stderr,"\n%d files to investigate\n\n",filecount);
    if ( atexit(finalize) != 0 )
    {
	fprintf(stderr, "\nCannot set finalize() as exit function\n");
    } else
    {
	signal(SIGINT,finalsig);
        qsort( file_info, filecount, sizeof(frdinfo), comparison_by_size );
    }
/*  if(interact!=0)  print_dupinfo();
    else	  exec_dupinfo();
    if(quietrun==0) printstats(); 	*/
    exit(exit_status);
}
