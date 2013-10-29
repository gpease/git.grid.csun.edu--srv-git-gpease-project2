/*
Gregory Pease
Networking Project #2
10/28/2013

Currently implemented as a simple proxy server program.
I didn't finish the config file nor implement the filter,load balancer, nor caching service

Current usage: Deflect

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
int main( int argc, char *argv[] )
{
    int sock,Osock, newsock, portnum,Oportnum, clilen;
    char buffer[4096];
	char hostname[4096];
	char filepath[4096];
	char temp[4096];
	char GET[4096];
    struct sockaddr_in serv_addr, cli_addr, out_addr;
    int  n,pid,www,filestart, i;
	struct hostent *Oserver;

    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("err: opening socket");
        exit(1);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portnum = 1492;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnum);
	
 
    /* AND IN THE DARKNESS BINDS THEM!!!*/
    if (bind(sock, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("err: on binding");
         exit(1);
    }
    
    listen(sock,5);
    clilen = sizeof(cli_addr);
    while (1) 
    {
        newsock = accept(sock, 
                (struct sockaddr *) &cli_addr, &clilen);
        if (newsock < 0){
            perror("err: on accept");
            exit(1);
        }
        
        pid = fork();
        if (pid < 0){
            perror:("err: on fork");
	    exit(1);
        }
        if (pid == 0){
            
			n = read(newsock,buffer,4095);
			if (n < 0){
				perror("err: reading from socket");
				exit(1);
			}
			
					//Out Socket
			Osock = socket(AF_INET, SOCK_STREAM, 0);
			if (Osock < 0) 
			{
				perror("err: opening socket");
				exit(1);
			}
			
			//buffer string processing
			if(buffer[0]!='w')
				strcpy(hostname,buffer+7);
			else
				strcpy(hostname,buffer);
				
			strcpy(filepath,strchr(hostname,'/'));
			
			strcpy(GET,"GET ");
			strcat(GET,filepath);
			strcat(GET," HTTP/1.1");
			strcat(GET,"\r\nhost: ");
			strncat(GET,hostname,strlen(hostname)-strlen(filepath));
			strcat(GET,"\r\n\r\n");
			
			printf("GET REQ:\n%s %d",GET,strlen(GET));
			
			strncat(temp,hostname,strlen(hostname)-strlen(filepath));
			//insert filter code here
			/*You cannot pass! I am a servant of the Secret Fire, wielder of the Flame of Anor.
			The dark fire will not avail you, Flame of Udun! Go back to the shadow. You shall not pass!
			*/
			Oserver = gethostbyname(temp);//hostname be good now yo
			if (Oserver == NULL) {
				fprintf(stderr,"err:, Server is missing, maybe he went out or something\n");
				exit(0);
			}
			
			Oportnum = 80;
			bzero((char *) &out_addr, sizeof(out_addr));
			out_addr.sin_family = AF_INET;
			out_addr.sin_port = htons(Oportnum);
			bcopy((char *)Oserver->h_addr,(char *)&out_addr.sin_addr.s_addr,Oserver->h_length);
				
			
			
			
			if (connect(Osock, (struct sockaddr *)&out_addr, sizeof(out_addr)) < 0)
				perror("err: connecting to GET server");
			
			
			n=write(Osock,GET,strlen(GET));//send GET request
			if (n < 0){
				perror("err: writing GET");
				exit(1);
			}
			
			
			while(1){
				bzero(buffer,4095);
				n = read(Osock,buffer,4095);
				if (n < 0) 
				{
					perror("err: reading from GET");
					exit(1);
				}
				if(strlen(buffer)==0){
					break;
					}
				else{
					printf("%s",buffer);
				n = write(newsock,buffer,4095);
				if (n < 0){
					perror("err: writing to sock");
					exit(1);
					}
				}
			}
			close(Osock);
            close(sock);
            exit(0);
        }
        else{
            close(newsock);
        }
    } 
}
