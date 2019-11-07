/*
    Copyright (C) 2003 Evan Buswell
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <time.h>
#include <netdb.h>
/*#include <usb.h>*/
#include "web.h"
#include "socket.h"
#include "nethelp.h"
#include "freedup.h"
#include "config.h"
#if defined(MSG_DONTWAIT) && defined(MSG_NOSIGNAL)
# define MSG_FREEDUP    MSG_OOB | MSG_DONTWAIT | MSG_NOSIGNAL
#else
# define MSG_FREEDUP    MSG_OOB
#endif
#ifndef ETHERMTU
# define ETHERMTU 1542
#else
# include <net/ethernet.h>
#endif


/* 
 * Write len bytes from buf to the socket.
 * Returns the return value from send()
 */
int sock_write_bytes(int sockfd, const unsigned char *buff, int len) {
    int t, n;

    for(t = 0 ; len > 0 ; ) {
	n = send(sockfd, (void *) (buff + t), len, MSG_NOSIGNAL);
	if (n < 0) {
	    return((t == 0) ? n : t);
	}
	t += n;
	len -= n;
    }
    return(t);
}

/* 
 * Read len bytes from buf to the socket.
 * Returns the return value from recv()
 */
int sock_read_bytes(int sockfd, const unsigned char *buff, int len) {
    int t, n;
    
    for(t = 0 ; len > 0 ; ) {
	n = recv(sockfd, (void *) (buff + t), len, MSG_NOSIGNAL);
	if (n < 0)
	    return((t == 0) ? n : t);
	t += n;
	len -= n;
    }
    return(t);
}


void l_listen(int*sock) {
    int i;
    int s;
    int connected=0;
    uid_t uid;
    int BUFFERSIZE=40960;
    int junk = 0;
    char *oob, *buffer=NULL;
    char *buffer1;
    struct timespec waittime;
    waittime.tv_sec = 0;
    waittime.tv_nsec = 250000000; /* a quarter second */

    oob = (char *) malloc(32);
    buffer  = buffer1 = (char *) malloc(BUFFERSIZE+4);

    /* drop priveleges */
    uid = getuid();
    seteuid(uid);
    
    listen(*sock, 1); /* We only get one connection on this port.
			 Everything else is refused. */
    while(1) {
	while((s = accept(*sock, NULL, NULL)) == -1) {
	    sleep(1);
	    /* retry after error.  Really bad errors shouldn't happen. */
	}
	if(!quietrun) fprintf(stderr, "Provider connected.\n");
	connected=1;
	while(connected) {
	    if((recv(s, oob, 32, MSG_FREEDUP) > 0)
	       && strncmp(oob, "flush", 5)) {
		    fprintf(stderr,"OUT-OF-BAND MESSAGE 1");
	    }

	    i = recv(s, buffer, BUFFERSIZE, MSG_NOSIGNAL);
	    if(i == -1 || i == 0) {
		if((i == -1) && (errno != EAGAIN) && (errno != EINTR)) {
		    if(junk != 0) {
			fprintf(stderr, "%d bytes\n", junk);
			junk = 0;
		    }
		    /* wait for a new connection */
		    perror("Lost provider connection");
		    close(s);
		    connected=0;
		}
		/* see if provider is still there */
		i = sock_write_bytes(s, (unsigned char*)"ping", 4);
		/* we get tcp acks, so there's no need to send
		       a pong from the provider */
		if((i == -1) && (errno != EINTR)) {
		    if(junk != 0) {
			fprintf(stderr, "%d bytes\n", junk);
			junk = 0;
		    }
		    /* wait for a new connection */
		    perror("Lost provider connection");
		    close(s);
		    connected=0;
		}
		nanosleep(&waittime, NULL);
	    } else {
#if FORTESTINGONLY
		const char*tosend=answer(buffer1);
		fprintf(stderr,"Buffer contains: »%s«\n",buffer1);
		send(s,tosend,strlen(tosend),0);
		memset(buffer1,0,BUFFERSIZE);
		close(s);
	        connected=0;
#else
		/*
		 * Some strange way of programming
		 * due to lack of experience with this.
		 *
		 * First round get the header
		 * second round get the body
		 * 
		 * each "GET" has no second round
		 *
		 * strange, but it works
		 */
	//	if((StrNCaseCmp(buffer,"GET") != 0)&&(buffer==buffer1))
		if( strstr(buffer1,"\n\n")==NULL && strstr(buffer1,"\n\r\n")==NULL )
		{
		    int bufferlength=strlen(buffer);
		    assert(bufferlength<BUFFERSIZE/2);
		    buffer=&buffer1[bufferlength];
	        } else
		{
		    process(s,buffer1);
		    buffer = buffer1;
		    memset(buffer1,0,BUFFERSIZE);
		    close(s);
		    connected=0;
		}
#endif
	    }
	}
    }
}

int*socket_init(void) {
    int *s;
    int on = 1;
    size_t mtu = ETHERMTU;
    struct sockaddr_in addr;
    
    /* allocate a socket to listen on */
    s = (int *) malloc(sizeof(int));
    if( s == NULL )
	return(NULL);

    /* locate socket */
    *s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( *s == -1 )
	return(NULL);

    /* set socket options */
    if( setsockopt(*s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int)) == -1)
	return(NULL);
    if( setsockopt(*s, SOL_SOCKET, SO_RCVBUF, &mtu, sizeof(size_t)) == -1)
	return(NULL);
    
    /* set socket essentials */
    struct servent*thiserv=getservbyname(PROGRAM,"tcp");
    addr.sin_family = AF_INET;
    addr.sin_port=(thiserv==NULL)?htons(80):thiserv->s_port;
    addr.sin_addr.s_addr = (u_int32_t) INADDR_ANY;
    if(!quietrun)
	fprintf(stderr, "Listening to all interfaces on port %d...\n", htons(addr.sin_port) );
    
    /* bind socket now */
    if(bind(*s, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1)
    {
	perror("Cannot bind to socket");
	return(NULL);
    }
    
    return(s);
}

void dolisten(void)
{
    int*s=(int*)NULL;
    if(!quietrun) printf("Server goes to listen mode now.\n");
    if( chdir(HOMEDIR) != 0 )
    {   perror("Cannot change to webdir.");
    }
    if( (s = socket_init()) != NULL)
    {
	while(1)
	{
	    l_listen(s);
	}
    }
}
