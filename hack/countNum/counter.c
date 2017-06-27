/*
While a new workpieces is coming, send UDP datagram to the server
如果一个工件到达（这里可以是一个摄像头在捕获此信号，之后摄像头设备发送UDP数据包到计件服务器，
通知服务器进行计件
*/

#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#define COUNTNUM 11
#define GETNUM 22


#define DESTIP "192.168.40.128"
#define DESTPORT 5005
#define BUFSIZE 1024

int main(){
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1){
		perror("socket error!");
	}

	struct sockaddr_in dest_addr;
	bzero(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr(DESTIP);
	dest_addr.sin_port = htons(DESTPORT);

	int addsize = sizeof(dest_addr);
	char buffer[BUFSIZE];

	bzero(buffer, BUFSIZE);
	int meta = htonl(COUNTNUM);
	memcpy(buffer, &meta, 4);

	//simulation
	while(1){
		int ret = sendto(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *)&dest_addr, addsize);

		if(ret < 0){
			if(errno == EINTR){
				printf("interrupted!\n");
			}else{
				perror("sendto error!");
			}
		}
		sleep(2);
	}

}