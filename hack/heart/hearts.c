/***
首先服务器设置一个3s的计时器，之后阻塞在read调用，如果在计时器超时之前read正常返回，
那么再次循环，并且继续设置3s的计时器，如果超时到达read仍然没有结果，则read被中断，
我们就可以把这个newfd关掉然后break了。
***/
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>

#include <signal.h>

#define PORT 5000
#define BackLog 1
#define BufSize 1024

void sigalrm_handler(int signum){
	printf("alrm handler\n\tTime Out: client heart beat timeout!");

}

int main(){
	char buf[BufSize];
	bzero(&buf,sizeof(buf)); //
	
	int ssock = socket(AF_INET,SOCK_STREAM,0);
	if(ssock == -1){
		perror("sock() error!");
		exit(1);
	}
	
	struct sigaction act;
	act.sa_handler = sigalrm_handler;
	act.sa_flags = SA_NOMASK;

	sigaction(SIGALRM, &act, NULL);

	struct sockaddr_in  servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int set = 1;
	if(setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int)) == -1){
		perror("setsockopt error!");
	}
	
	if(bind(ssock,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
		perror("bind() error!");
		exit(1);
	}

	if(listen(ssock,BackLog) == -1){
		perror("listen() error!");
		exit(1);
	} 
	printf("Listen here!\n");

	struct sockaddr_in  cliaddr;
	bzero(&cliaddr,sizeof(cliaddr)); 
	int addrsize = sizeof(cliaddr);
	int newfd = accept(ssock,(struct sockaddr *)&cliaddr,&addrsize);
	if(newfd == -1){
		perror("accept() error!");
		close(newfd);
		exit(1);
	}
	
	//server receive message from client: hello server!
	while(1){
		//set 3s timer
		printf("Begin Timer!\n");
		alarm(3);
		bzero(buf,sizeof(buf)); 
		int nbytes = read(newfd,buf,sizeof(buf));
		if(nbytes < 0){
			if(errno == EINTR){
				/*if timeout coming, close the socket*/
				printf("interrupted, 3s timeout! \n");
				close(newfd);
				break;
			}else{
				perror("read error!");
			}
		}else if(nbytes == 0){
			/**server window overload
			recv win = 0
			will tell client not to send data**/

			/*
			FIN packet coming(TCP)
			*/

			printf("client leave!\n");
			close(newfd);
			exit(-1);
		}else{
			alarm(0);//cancel the timer
			buf[nbytes] = '\0';
			printf("receive from client: %s",buf);
		}

	}
		
	printf("do something at last\n");
	close(newfd);
	close(ssock);
	return 0;	
	

}
