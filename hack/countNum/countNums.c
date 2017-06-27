/*
信号驱动IO，需要进行以下三个步骤：
SIGNAL DRIVEN IO Using SIGIO to count the number of workpieces
1, set signal() or sigaction()
2, set F_SETOWN
3, set F_SETFL -> FASYNC

本次采用前4字节作为控制信息，判断是摄像头计件信号，还是远程客户端进行的数据请求
使用全局变量count作为计件数
*/
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include <fcntl.h>
#include <signal.h>

#include <sys/wait.h>
#include <sys/time.h>

#define COUNTNUM 11
#define GETNUM 22

#define PORT 5005
#define BUFSIZE 1024

#define COUNT count++
static int count;
char buffer[BUFSIZE];

static int usockfd;

static void udp_io_handler(int signum){
	printf("UDP SIGIO Coming...\n");

	int addrsize = sizeof(struct sockaddr_in);
	struct sockaddr_in dest_addr;

	int num = recvfrom(usockfd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&dest_addr, &addrsize);
	if(num < 0){
		perror("[ERROR]	 recv error!");
		exit(-1);
	}

	int meta = 0;
	memcpy(&meta, buffer, 4);
	meta = ntohl(meta);
	if(meta == COUNTNUM){
		//count number
		COUNT;
		printf("current count is: %d\n", count);

	}else if(meta == GETNUM){
		//get global var:count and send
		int data = count;
		data = htonl(data);
		memcpy(buffer, &data, 4);

		num = sendto(usockfd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&dest_addr, addrsize);
		if(num < 0){
			if(errno == EINTR){
				printf("interrupted!\n");
			}else{
				perror("sendto error!");
			}
		}

	}
}

int main(){
	//start the udp server
	usockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(usockfd < 0){
		perror("udp server socket() error");
	}

	struct sockaddr_in usaddr;
	usaddr.sin_family = AF_INET;
	usaddr.sin_addr.s_addr = htonl(INADDR_ANY);//
	usaddr.sin_port = htons(PORT);

	int set = 1;
	if(setsockopt(usockfd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int)) == -1){
		perror("setsockopt error!");
	}

	//udp server bind addr
	if(bind(usockfd, (struct sockaddr *)&usaddr, sizeof(usaddr)) < 0){
		perror("udp server bind error!");
		exit(1);
	}

	signal(SIGIO, udp_io_handler);

	if (fcntl(usockfd, F_SETOWN, getpid()) < 0){
		perror("fcntl F_SETOWN");
		exit(1);
	}


	int oflags = fcntl(usockfd, F_GETFL);
	printf("%d\n", oflags);
	if (fcntl(usockfd, F_SETFL, FASYNC) < 0){
		perror("fcntl F_SETFL, FASYNC");
		exit(1);
	}

	oflags = fcntl(usockfd, F_GETFL);
	printf("%d\n", oflags);

	while(1);
}