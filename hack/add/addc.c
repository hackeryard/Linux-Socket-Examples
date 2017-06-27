/*
随机产生两个有符号整数，发送给服务器

network byte order:
	0x12345678

intel:
	0X78563412

using htonl or htons

*/
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<time.h>


#define PORT 5000
char buffer[8];
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
		
	/*32 bit -> 21亿多*/
	printf("%d\n", RAND_MAX);

	srand((int)time(NULL));

	bzero(buffer,sizeof(buffer));
	while(1){
		/*rand()%100*/
		int a = rand()/100;
		int b = rand()/100;
		int a1 = htonl(a);
		int b1 = htonl(b);
		printf("%d %d\n", a, b);
		memcpy(buffer, &a1, 4);
		memcpy(buffer+4, &b1, 4);

		write(csock,buffer,sizeof(buffer));
		
		bzero(buffer,sizeof(buffer));
		int nbytes = read(csock,buffer,sizeof(buffer));
		int raw;
		memcpy(&raw, buffer, 4);
		int res = ntohl(raw);
		printf("receive from server: %d\n", res);

		sleep(1);
		
	}
	
	close(csock);
	return 0;

}
