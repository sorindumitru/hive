#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <node.h>
#include <net/netlib.h>
#include <net/utils.h>

void network_start(void *arg)
{
	struct node *node = (struct node *) arg;

	printf("Starting %s\n", node->name);

	if (!strcmp(node->name, "receiver")) {
		unsigned char data[128] = {0};
		size_t len;

		/* Wait to receive packet */
		while (hive_recv(node, data, &len) != -EAGAIN);

		printf("Finally received packet %s\n", data);
	} else {
		unsigned char *data = "Hello, server, this is client!";
		struct address addr = address_from_string("00:00:00:22:33:44");

		hive_sendto(node, data, strlen(data), &addr);
	}
}

void network_stop(void *arg)
{
	struct node *node = (struct node *) arg;
	printf("Stopping %s\n", node->name);
}

void *network_init(void)
{
	return NULL;
}

void network_exit(void *_)
{

}
