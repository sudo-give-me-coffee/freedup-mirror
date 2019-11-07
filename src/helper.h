#include <stdio.h>

/* the following line is used in freedup.c only with the arg "" ==> could solve better */
#define myputs(a) {if( puts(a) == EOF ) perror(a);}
#define myputchar(a) {if( putchar((int)a) != ((int)a) ) perror("putchar error");}
#define CHECK4KOMMA(c)	{if(c==(unsigned char)','){if(putchar((int)c)!=((int)c)){perror("putchar() failed");};}else{c=',';}}
