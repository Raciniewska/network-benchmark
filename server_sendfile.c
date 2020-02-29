#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <linux/limits.h>


int main(int argc, char** argv) {
	int port = 1234, sock, desc, fd, rc, addrlen;
	struct sockaddr_in addr, addr1;
	struct stat stat_buf;
	off_t offset = 0;
	char filename[PATH_MAX];
	/* check command line arguments, handling an optional port number */
	if (argc == 2) {
		port = atoi(argv[1]);
		if (port <= 0) {
			fprintf(stderr, "invalid port: %s\n", argv[1]);
			exit(1);
		}
	}
	else if (argc != 1) {
		fprintf(stderr, "usage: %s [port]\n", argv[0]);
		exit(1);
	}
	/* create Internet domain socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		fprintf(stderr, "unable to create socket: %s\n", strerror(errno));
		exit(1);
	}
	/* fill in socket structure */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	/* bind socket to the port */
	rc = bind(sock, (struct sockaddr*) & addr, sizeof(addr));
	if (rc == -1) {
		fprintf(stderr, "unable to bind to socket: %s\n", strerror(errno));
		exit(1);
	}
	/* listen for clients on the socket */
	rc = listen(sock, 1);
	if (rc == -1) {
		fprintf(stderr, "listen failed: %s\n", strerror(errno));
		exit(1);
	}

	while (1) {
		desc = accept(sock, (struct sockaddr*) & addr1, &addrlen);
		if (desc == -1) {
			fprintf(stderr, "accept failed: %s\n", strerror(errno));
			exit(1);
		}

		rc = recv(desc, filename, sizeof(filename), 0);
		if (rc == -1) {
			fprintf(stderr, "recv failed: %s\n", strerror(errno));
			exit(1);
		}
		filename[rc] = '\0';

		/* exit server if filename is "quit" */
		if (strcmp(filename, "quit") == 0) {
			fprintf(stderr, "quit command received, shutting down server\n");
			break;
		}

		fprintf(stderr, "received request to send file %s\n", filename);

		/* open the file to be sent */
		fd = open(filename, O_RDONLY);
		if (fd == -1) {
			fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
			exit(1);
		}

		/* get the size of the file to be sent */
		fstat(fd, &stat_buf);

		/* copy file using sendfile */
		offset = 0;
		rc = sendfile(desc, fd, &offset, stat_buf.st_size);
		if (rc == -1) {
			fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
			exit(1);
		}
		if (rc != stat_buf.st_size) {
			fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n",
				rc,
				(int)stat_buf.st_size);
			exit(1);
		}
		close(fd);
		close(desc);
	}
	close(sock);
	return 0;
}

