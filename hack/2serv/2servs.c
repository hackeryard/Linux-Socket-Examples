/*
many process reuse the tcp addr, so set the option of SO_REUSEADDR
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
#define BackLog 10
#define BufSize 1024

static void sigchld_handler(int signum){
	printf("one child is dying...\n");
	sleep(4);
	int status;
	while(waitpid(-1, &status, NULL) > 0);
}

int main(){
	struct sigaction act;
	bzero(&act, sizeof(act));
	act.sa_handler = sigchld_handler;
	act.sa_flags = SA_NOMASK | SA_RESTART;

	sigaction(SIGCHLD, &act, NULL);

	char buf[BufSize];
	bzero(&buf,sizeof(buf)); //
	
	int ssock = socket(AF_INET,SOCK_STREAM,0);
	if(ssock == -1){
		perror("sock() error!");
		exit(1);
	}

	int set = 1;
	if(setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int)) == -1){
		perror("setsockopt error!");
	}
	
	struct sockaddr_in  servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
		perror("bind() error!");
		exit(1);
	}

	if(listen(ssock,BackLog) == -1){
		perror("listen() error!");
		exit(1);
	} 

	//accept is in loop, every time server can handler only one client
	//so read from newfd is no problem(TCP流量是没有边界的)
	while(1){
		struct sockaddr_in  cliaddr;
		bzero(&cliaddr,sizeof(cliaddr)); 
		int addrsize = sizeof(cliaddr);
		int newfd = accept(ssock, (struct sockaddr *)&cliaddr, &addrsize);
		//restart and continue
		if(newfd == -1){
			if(errno == EINTR){
				continue;
			}else{
				perror("accept() error!");
				close(newfd);
				exit(1);	
			}
		}

		//serv1 or serv2
		bzero(buf,sizeof(buf)); 
		int nbytes = read(newfd, buf, sizeof(buf));
		if(nbytes < 0){
			if(errno == EINTR){
				printf("interrupted!\n");
			}else{
				perror("read error!");
			}
		}else if(nbytes == 0){
			printf("client exit!\n");
		}else{
			int type;
			memcpy(&type, buf, 4);
			type = ntohl(type);

			if(type == SERV1){

				//debug:
				printf("ser 1 \n");
				//while(1){
					bzero(buf, sizeof(buf));
					char * temp = "Wahaha, Serv1 is down!\n";
					//amd64 pointer 64
					memcpy(buf, temp, strlen(temp));
					//write content but client can not retrive so 
					//client can only in listen queue but not accepted
					write(newfd, buf, strlen(buf));
					close(newfd);
				//}

			}else if(type == SERV2){
				//debug:
				printf("ser 2\n");

				pid_t pid = fork();

				if(pid < 0){
					perror("fork error!");
				}else if(pid == 0){
					close(ssock);
					sleep(5);
					char * temp = "Wahaha, Serv2 is down!\n";
					memcpy(buf, temp, strlen(temp));
					write(newfd, buf, strlen(buf));		
					exit(0);

				}else{
					close(newfd);
				}
			}else{
				printf("Unknown service type!\n");
			}//end if type

		}//end if nbytes

	}//end while

	return 0;	

}
