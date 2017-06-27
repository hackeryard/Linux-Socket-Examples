/**********
Broadcast UDP Tester
可以向本地局域网内所有的主机发送UDP广播，端口是PORT
作为测试用
***********/

#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>

//tester port
#define PORT 5001

int main(){
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1){
		perror("socket errro!");
	}

	int set = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &set, sizeof(int)) == -1){
		perror("setsockopt error!");
	}

	struct sockaddr_in cliaddr;
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = inet_addr("192.168.40.255");
	cliaddr.sin_port = htons(PORT);

	while(1){
		char buffer[1000];
		bzero(buffer, sizeof(buffer));

		int addrsize = sizeof(struct sockaddr_in);
		scanf("%s", buffer);
		int ret = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, addrsize);
		if(ret < 0){
			perror("sendto error!");
		}
	}

}