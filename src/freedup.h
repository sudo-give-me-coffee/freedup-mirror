/*
    This file is part of FreeDup.

    FreeDup -- search for duplicate files in one or more directory hierarchies
    Copyright (C) 2007,2008,2009 Andreas Neuper

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
#ifndef FREEDUP_H
#define FREEDUP_H 1
/* freedup was written by AN <AN@freedup.org>  */
#include <sys/types.h>
#include <sys/stat.h>
#include "sha1.h"
/*
 * for the hash comparison #define MD5SUM
 */
#if defined(__CYGWIN__)
# include <cygwin/stat.h>
# define HASHCHAR	'*'
/*
 * the popen() is not supported in cygwin
 * for no hash comparison #undef MD5SUM
 */
#else
# if !defined(_AIX)
#  include <sys/stat.h>
# endif
# define HASHCHAR	' '
#endif
#if !defined(HASHSUM)
# define HASHSUM	1
#endif
# if defined(__OpenBSD__) || defined (__NetBSD__) || defined(__bsdi__) || defined(__APPLE__)
# endif
#  define SHA512SUM	"/usr/bin/sha512sum"
#  define SHA384SUM	"/usr/bin/sha384sum"
#  define SHA256SUM	"/usr/bin/sha256sum"
#  define SHA224SUM	"/usr/bin/sha224sum"
#  define SHA1SUM	"/usr/bin/sha1sum"
#  define MD5SUM	"/usr/bin/md5sum"
#  define CKSUM		"/usr/bin/cksum"
#  define SUM		"/usr/bin/sum"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <dirent.h>
#include <utime.h>
#if !defined(_AIX)
# define GNU_SOURCE
# include <getopt.h>
#endif
#include <time.h>
#ifndef loff_t
#define loff_t off_t
#endif
#define	EMPTYSTRING	"                                                                "
#define	FREEDUP		"freedup"
#define	PROGRAM		FREEDUP
#define	FREEDUPCONFIGNAME	"." FREEDUP ".cfg"
#define	PWDSIZE		8192
#define	HOMEDIR		"/usr/share/doc/packages/" FREEDUP "/html"
#define	WEBPAGE		"offer.html"

#define	SHA1INIT(x)		SHA1Init(x)
#define	SHA1UPDATE(x,y,z)	SHA1Update(x,y,z)
#define	SHA1FINAL(x,y)		SHA1Final(x,y)
#define	SHA1CONTEXT		SHA1Context

typedef
struct dupinfo
{
	/* keep the file stat as e.g. delivered by lstat() */
	loff_t          size;
	/* keep the file stat as e.g. delivered by lstat() */
	int		count;
	/* keep file names for later reference	*/
	char**name;
	/* keep file priorities for later reference	*/
	int*prio; /* this is the index BEFORE sorting	*/
	/*
		frdinfo[1].ident ----v	/----->	frdinfo[1].name
				 dupinfo[1]
		frdinfo[2].ident ----A	\----->	frdinfo[2].name
	 */
	struct dupinfo*replace;
} dinfo;

typedef
struct fileinfo
{
	/* remember the file name including path	*/
	char*name;
	/* keep the file stat as delivered by lstat()	*/
	struct stat info;
	/* size to compare, defaults to info.st_size	*/
	size_t size;
	/* keep the hash number for later comparison	*/
	char*hash;
	/* context during incomplete hash evaluation	*/
	SHA1CONTEXT context;
	long sha1pos;
	/* file system priority (= arg line position)	*/
	int  prio;
	/* referencing a reverse sorted structure	*/
	dinfo *ident;
} frdinfo;

typedef struct termios TTY;

#define basename(x)	((strrchr((x),'/')!=NULL)?strrchr((x),'/')+1:(x)) 
#define S_IRWXUGO       (S_IRWXU|S_IRWXG|S_IRWXO)
#ifndef ALLPERMS
#define ALLPERMS        (S_IRWXUGO|S_ISUID|S_ISGID|S_ISVTX)
#endif
#define S_PERM(mode)    ((mode) & (ALLPERMS))

#include "version.h"
#define	MAXPATHLENGTH		4096
#define BLOCK			4096
#define MAXANSWER		8192
#define INITIALFINFOCOUNT	20480
/* Here we define the maximum number of identical files in interactive mode, */
/* since link commands are indicated by different letters */
#define LETTERS			25

/* OPTIONS  */
extern	int showsave;
extern	int sametime;
extern	int timediff;
extern	int samename;
extern	int samegrp;
extern	int hardlink;
extern	int showhard;
extern	int minbytes;
extern	int testmode;
extern	int sameperm;
extern	int permmask;
extern	int quietrun;
extern	int sameuser;
extern	int showmode;
extern	int weaklink;
extern	int interact;
extern	char globalkey;
extern	int nonzero;
extern	int usehash;
extern	int extratyp;
extern	int nosymlinks;
/*@null@*/extern	char*findopta;
/*@null@*/extern	char*pwd;
extern	int hashmethod;
extern	int frdinfosize;
extern	int filecount;
extern	int dupscount;
extern	int dirmtime;
extern	frdinfo*file_info;

extern int readtree( frdinfo**, int*, const int, FILE*);
int selecthash(const char*type);
extern const char*getpwd(void);
#endif /*FREEDUP_H*/
