#include <netinet/in.h> 
#include <stdio.h> 
#include <fcntl.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 

#define IP_PROTOCOL 0 
#define PORT_NO 15050 
#define NET_BUF_SIZE 32 
#define sendrecvflag 0 
#define nofile "File Not Found!" 

void clearBuf(char* b) {
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
}

//Read file content from storage to a buffer in user space using file read syscall
int sendFile(FILE * fp, char* buf, int s, int fd) {
	if (fp == NULL) {
		strcpy(buf, nofile);
		int len = strlen(nofile);
		buf[len] = EOF;
		return 1;
	}
	if (fd < 0) { perror("r1"); exit(1); }
	int len = read(fd, buf, s);
	if (len<=0){return 1;}
	return 0;
}

int main() {
	int sockfd, nBytes, fd, addrlen;
	struct msghdr message;
	struct sockaddr_in addr_con;
	struct iovec iov[1];
	char net_buf[NET_BUF_SIZE];
	FILE* fp;

	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(PORT_NO);
	addr_con.sin_addr.s_addr = INADDR_ANY;

	// socket
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);

	// bind
	if (bind(sockfd, (struct sockaddr*) & addr_con, sizeof(addr_con)) == 0)
		printf("\nSuccessfully binded!\n");
	else
		printf("\nBinding Failed!\n");

	while (1) {

		printf("\nWaiting for file name...\n");
		clearBuf(net_buf);
		addrlen = sizeof(addr_con);

		nBytes = recvfrom(sockfd, net_buf,
			NET_BUF_SIZE, sendrecvflag,
			(struct sockaddr*) & addr_con, &addrlen);

		fp = fopen(net_buf, "r");
		fd = open(net_buf, O_RDONLY);

		printf("\nFile Name Received: %s\n", net_buf);
		if (fp == NULL)
			printf("\nFile open failed!\n");
		else
			printf("\nFile Successfully opened!\n");

		iov[0].iov_base = net_buf;
		iov[0].iov_len = NET_BUF_SIZE;
		message.msg_name = (struct sockaddr*) & addr_con;
		message.msg_namelen = addrlen;
		message.msg_iov = iov;
		message.msg_iovlen = 1;
		message.msg_control = 0;
		message.msg_controllen = 0;

		while (1) {
			if (sendFile(fp, net_buf, NET_BUF_SIZE, fd)) {
				sendmsg(sockfd, &message, 0);
				clearBuf(net_buf);
				break;
			}
			sendmsg(sockfd, &message, 0);
			clearBuf(net_buf);
		}
		if (fp != NULL)
			fclose(fp);
	}
	return 0;
}
