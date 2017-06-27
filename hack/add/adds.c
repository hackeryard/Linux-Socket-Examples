/***
加法服务器，返回给客户端两个数的加法
如果需要更大的数的加法，考虑用long long
如果要求实现乘法，那么也可以用long long，不过需要自己转换网络字节序，用不成htonl
***/
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<netinet/in.h>

#define PORT 5000
#define BackLog 2
#define BufSize 8

int main(){
	char buf[BufSize];
	bzero(&buf,sizeof(buf)); //
	
	int ssock = socket(AF_INET,SOCK_STREAM,0);
	if(ssock == -1){
		perror("sock() error!");
		exit(1);
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

	if(listen(ssock,BackLog) == -1){
		perror("listen() error!");
		exit(1);
	} 

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
		bzero(buf,sizeof(buf)); 
		int nbytes = read(newfd,buf,sizeof(buf));

		int a;
		memcpy(&a, buf, 4);

		int b;
		memcpy(&b, buf+4, 4);
		
		a = ntohl(a);
		b = ntohl(b);
		printf("receive from client: %d %d\n", a, b);
		
		int res = a + b;
		res = htonl(res);
		memcpy(buf, &res, 4);
		write(newfd, buf, sizeof(buf));	
	}
		
	
	close(newfd);
	close(ssock);
	return 0;	
	

}
