/*
根据命令行参数指定请求服务的类型，
可以通过shell并发测试服务器2servs的功能
*/
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>

#include <signal.h>

#define SERV1 1
#define SERV2 2

#define PORT 5000

static void sigpipe_handler(int signum){
	printf("sigpipe coming!\n");
}

int main(int argc, char **argv){
	signal(SIGPIPE, sigpipe_handler);

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
	
	char buffer[1024];
	bzero(buffer,sizeof(buffer));

	int type = atoi(argv[1]);
	type = htonl(type);
	memcpy(buffer, &type, 4);

	write(csock, buffer, 4);
	
	bzero(buffer,sizeof(buffer));
	int nbytes = read(csock,buffer,sizeof(buffer));
	//retuen -1: sigpipe
	//sleep(1);
	if(nbytes < 0){
		if(errno == EINTR){
			printf("interrupted!\n");
		}else{
			perror("read error!");
		}
	}else if(nbytes == 0){
		printf("server exit!\n");
	}else{
		printf("total %d\n", nbytes);
		buffer[nbytes] = '\0';
		printf("receive from server: %s",buffer);	
	}

	
	close(csock);
	return 0;

}
