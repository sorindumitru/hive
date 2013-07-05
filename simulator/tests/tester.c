#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, const char *argv[])
{
	int nr = atoi(argv[1]), i;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_port = htons(9090);
	inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
	connect(sock, (struct sockaddr *) &sin, sizeof(sin));

	/* Send startd */
	for (i = 0; i < nr; i++) {
		char *template = "{ \"load\" : { \"library\" : \"%s\", \"name\" : \"t%d\", \"nic\" : { \"type\" : \"%s\", \"address\" : \"00:00:00:11:22:33\" }, \"routing\" : { \"type\" : \"%s\" } } }";
		char data[2048] = {0};

		sprintf(data, template, argv[2], i + 1, argv[3], argv[4]);

		write(sock, data, strlen(data));
	}

	for (i = 0; i < nr; i++) {
		char data[2048] = {0};
		char *template = "{ \"start\" : { \"index\" : %d, \"count\" : 1} }";

		sprintf(data, template, i+1);
		write(sock, data, strlen(data));
	}

	return 0;
}
