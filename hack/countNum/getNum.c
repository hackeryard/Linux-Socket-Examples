/*
using UDP to retrive the data from remote server
ͨ��UDPȡ��Զ�̼Ƽ������������ݣ�ע�������ֽ����ת��
������ϵ�ṹ���ֽ���
	��ˣ�Motorola 6800 and 68k, Xilinx Microblaze, IBM POWER, system/360, System/370
	С�ˣ�intel x86��Z80 (including Z180, eZ80 etc.), MCS-48, 8051, DEC Alpha, Altera Nios, Atmel AVR, SuperH, VAX, �� PDP-11
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
	int meta = htonl(GETNUM);
	memcpy(buffer, &meta, 4);

	int ret = sendto(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *)&dest_addr, addsize);

	if(ret < 0){
		if(errno == EINTR){
			printf("interrupted!\n");
		}else{
			perror("sendto error!");
		}
	}

	ret = recvfrom(sockfd, buffer, BUFSIZE, 0, NULL, NULL);
	if(ret < 0){
		if(errno == EINTR){
			printf("interrupted!\n");
		}else{
			perror("recvfrom error!");
		}
	}

	int data = 0;
	memcpy(&data, buffer, 4);
	data = ntohl(data);

	printf("recv data is: %d\n", data);
}