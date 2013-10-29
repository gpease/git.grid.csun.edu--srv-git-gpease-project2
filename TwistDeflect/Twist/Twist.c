/*
Gregory Pease
Networking Project #2
10/28/2013

Currently implemented as a simple proxy server program.
I didn't finish the config file nor implement the filter,load balancer, nor caching service

Current usage: twist www.example.com/index.html
needs /filename for get request


Reference/Resources/Sampled:
http://www.tutorialspoint.com/unix_sockets/socket_server_example.htm  
http://stackoverflow.com/questions/17850749/get-requested-address-in-socket-programming-with-c
http://www.amiga.org/forums/archive/index.php/t-20341.html
http://www.lemoda.net/c/fetch-web-page/index.html
*/


#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    int sock, portnum, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[4096];

    if (argc < 2) {
        fprintf(stderr," current usage %s www.example.com/page.html  \n", argv[0]);
        exit(0);
    }
    portnum = 1492;//Columbus killed a bunch of dudes
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("err: opening socket");
        exit(1);
    }
    server = gethostbyname("ssh.csun.edu");//no fuss it works
    if (server == NULL) {
        fprintf(stderr,"err:, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
           (char *)&serv_addr.sin_addr.s_addr,
                server->h_length);
    serv_addr.sin_port = htons(portnum);

    
    if (connect(sock,&serv_addr,sizeof(serv_addr)) < 0) 
    {
         perror("err: connecting");
         exit(1);
    }	
    
	strcpy(buffer, argv[1]);
	
    n = write(sock,buffer,strlen(buffer));
    if (n < 0) 
    {
         perror("err: writing to socket");
         exit(1);
    }
    
	printf("Response:\n");
	while(1){
    bzero(buffer,4095);
    n = read(sock,buffer,4095);
    if (n < 0) 
    {
         perror("err: reading from socket");
         exit(1);
    }
	if(strlen(buffer)==0)
		break;
    printf("%s",buffer);
	}
    return 0;
}
