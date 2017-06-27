/***
采用一次服务就结束的模式，
可以通过并发脚本con.sh测试
***/
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>

#include <signal.h>

#define PORT 5050

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
//	cliaddr.sin_addr.s_addr = htons("127.0.0.1");
	inet_aton("127.0.0.1",&cliaddr.sin_addr);
		
	if(connect(csock,(struct sockaddr *)&cliaddr,sizeof(cliaddr)) == -1){
		perror("connect() error!");
		close(csock);
		exit(1);
	}
	
	char buffer[1000];
	bzero(buffer,sizeof(buffer));

	sprintf(buffer, "Hi Server, I Recved!");
	
	write(csock,buffer,strlen(buffer));
	
	bzero(buffer,sizeof(buffer));
	int nbytes = read(csock,buffer,sizeof(buffer));
	buffer[nbytes] = '\0';
	printf("receive from server: %s",buffer);
		

	close(csock);
	return 0;

}
