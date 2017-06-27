#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>

#define PORT 5000

int main(){
	int csock = socket(AF_INET,SOCK_STREAM,0);
	if(csock == -1){
		perror("sock() error!");
		exit(1);
	}
	
	struct sockaddr_in  cliaddr;
	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(PORT);
	inet_aton("127.0.0.1",&cliaddr.sin_addr);
		
	if(connect(csock,(struct sockaddr *)&cliaddr,sizeof(cliaddr)) == -1){
		perror("connect() error!");
		close(csock);
		exit(1);
	}
	
	char buffer[1000];
	bzero(buffer,sizeof(buffer));
	memcpy(buffer, "asdfghjkl\n", sizeof(buffer));
	int count = 0;
	while(1){
		/*every second send server a heart beat packet*/
		sleep(1);
		write(csock,buffer,sizeof(buffer));
		count++;
		
		// bzero(buffer,sizeof(buffer));
		// int nbytes = read(csock,buffer,sizeof(buffer));
		// buffer[nbytes] = '\0';
		// printf("receive from server: %s",buffer);
		if(count > 2){
			sleep(10);
		}
		
	}
	
	close(csock);
	return 0;

}
