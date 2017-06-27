/*
daemon process create
创建守护进程，预创建n个子进程，分别让他们ACCEPT客户端的连接请求，
并发脚本为con.sh，需要注意的是，一个子进程会循环ACCEPT请求并进行处理，并不是处理一个就退出，所以可以循环利用
但如果要求高并发环境，比如（同时）1000个请求，要想尽快处理，大致还是需要1000个子进程，虽然，
可能10个子进程就可以在可以接受的时间内handle完
*/
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>

#include <signal.h>

#define BACKLOG 5
#define BUFSIZE 1024
#define PIDNUM 10
#define PORT 5050

/*****
在这里，只是把accept的任务交给很多预创建的子进程，
可以不用设置sigcld信号处理，因为这些子服务器处于循环accept的模式，控制台使用中断即可回收所有子进程
****/
static void sigchld_handler(int signum){
	/*
	WNOHANG is non-blocking, quickly return if called
	Linux的信号机制是不排队的，假如在某一时间段多个子进程退出后都会发出SIGCHLD信号，但父进程来不及一个一个地响应，
	所以最后父进程实际上只执行了一次信号处理函数。但执行一次信号处理函数只等待一个子进程退出，所以最后会有一些子进程依然是僵尸进程。
	在这里不适用wait函数, 是因为wait调用会阻塞，那么父进程的后续逻辑可能无法执行
	*/
	printf("one child is dying...\n");
	int status;
	while(waitpid(-1, &status, WNOHANG) > 0);
}

static void handle_connect(int ssock){
	int csock;
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);

	//loop session
	while(1){
		printf("my pid: %d\n", getpid());
		csock = accept(ssock, (struct sockaddr *)&cliaddr, &len);
		if(csock == -1){
			perror("accept error!");
		}

		char buffer[BUFSIZE];
		bzero(buffer, BUFSIZE);
		//side strlen()
		int nbytes = recv(csock, buffer, sizeof(buffer), 0);
		if(nbytes < 0){
			if(errno == EINTR){
				printf("recv is interrupted!\n");
			}else{
				perror("recv error!");
			}
		}else if(nbytes == 0){
			printf("client leave!\n");
		}else{
			buffer[nbytes] = '\0';
			printf("recv from client: %s\n", buffer);
		}

		//send
		bzero(buffer, BUFSIZE);
		sprintf(buffer, "Hi client, I recved");
		send(csock, buffer, strlen(buffer), 0);
		close(csock);
	}

}

int main(){
	//create daemon
	pid_t dpid = fork();
	if(dpid < 0){
		perror("fork errro!");
	}else if(dpid > 0){
		exit(0);
	}

	setsid();
	umask(0);
	chdir("/");

	int index;
	for(index = 0; index < 1024; index++){
		close(index);
	}

	//socket
	struct sigaction act;
	act.sa_handler = sigchld_handler;

	sigaction(SIGCHLD, &act, NULL);

	int ssock = socket(AF_INET,SOCK_STREAM,0);
	if(ssock == -1){
		perror("sock() error!");
		exit(1);
	}

	/*
	many child process share the address with the father
	*/
	int set = 1;
	if(setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int)) == -1){
		perror("setsockopt error!");
	}
	
	struct sockaddr_in  servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(ssock,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
		perror("bind() error!");
		exit(1);
	}

	if(listen(ssock,BACKLOG) == -1){
		perror("listen() error!");
		exit(1);
	} 

	pid_t pid[PIDNUM];
	int i;
	for(i = 0; i < PIDNUM; i++){
		pid[i] = fork();
		if(pid[i] == 0){
			//child process wait new connection
			handle_connect(ssock);
			exit(0);//father process waitpid(-1,...)
		}
	}

	while(1){
		sleep(1);
		printf("my pid: %d\n", getpid());

		printf("I am sleeping...\n");
	}
	close(ssock);

	return 0;
}