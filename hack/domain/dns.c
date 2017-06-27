/*

*/

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
 
int main(int argc, char **argv){
	if(argc != 2){
		printf("[USAGE] +url\n");
		exit(1);
	}

	struct hostent * h;

	if((h = gethostbyname(argv[1])) == NULL){
		herror("gethostbyname error!");
		exit(1);
	}

	/*network order addr: _u32 	inet_ntoa() inet_ntop()
		struct sockaddr_in{
			short sin_family;
			unsigned short sin_port;
			struct in_addr sin_addr;
		}

		struct in_addr{
			_u32 s-addr;
		}

		Note: inet_ntop() inet_pton() inet_ntoa() all use <arpa/inet.h>

	*/
	printf("official name: %s\n", h->h_name);
	//print alias names
	int index = 0;
	while(h->h_aliases[index] != NULL){
		printf("alias name: %s\n", h->h_aliases[index]);
		index++;
	}
	printf("addr length: %d\n", h->h_length);
	//#define h_addr h_addr_list[0]
	printf("first resolved ip: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

	int i = 0;
	while(h->h_addr_list[i] != NULL){
		printf("resolved ip: %s\n", inet_ntoa(*((struct in_addr *)h->h_addr_list[i])));
		i++;
	}
}