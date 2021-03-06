#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 

#define IP_PROTOCOL 0 
#define IP_ADDRESS "127.0.0.1" 
#define PORT_NO 15050 
#define NET_BUF_SIZE 32 
#define sendrecvflag 0 

void clearBuf(char* b) {
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
}

int recvFile(char* buf, int s) {
	int i;
	for (i = 0; i < s; i++) {
		char ch = buf[i];
		if (ch == EOF || ch=='\0')
			return 1;
		else
			printf("%c", ch);
	}
	return 0;
}

int main()
{
	int sockfd, nBytes;
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	char net_buf[NET_BUF_SIZE];
	FILE* fp;

	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(PORT_NO);
	addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);

	// socket
	sockfd = socket(AF_INET, SOCK_DGRAM,
		IP_PROTOCOL);

	while (1) {
		printf("\nPlease enter file name to receive:\n");
		scanf("%s", net_buf);
		sendto(sockfd, net_buf, NET_BUF_SIZE,
			sendrecvflag, (struct sockaddr*) & addr_con,
			addrlen);

		printf("\n---------Data Received---------\n");

		while (1) {
			clearBuf(net_buf);
			nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
				sendrecvflag, (struct sockaddr*) & addr_con,
				&addrlen);

			if (recvFile(net_buf, NET_BUF_SIZE)) {
				return 0;
			}
		}
		printf("\n-------------------------------\n");
	}
	return 0;
}
