#include <stdlib.h>
#include <errno.h>

#include <net/basic.h>
#include <net/packet.h>

struct packet *basic_alloc_packet(unsigned int len)
{
	struct packet *packet = alloc_packet(len + sizeof(struct basic_header));
	struct basic_header *header = (struct basic_header *) packet->data;

	if (!packet)
		return NULL;

	packet->head = (char *)(header + 1);

	return packet;
}

struct basic_socket *basic_socket_alloc(void)
{
	struct basic_socket *socket = malloc(sizeof(*socket));

	if (!socket)
		return NULL;

	socket->to = 0;
	socket->from = 0;

	return socket;
}

int basic_socket_bind(struct basic_socket *sk, int from, int to)
{
	sk->from = from;
	sk->to = to;
}

int basic_send(struct basic_socket *sk, char *buf, unsigned int len)
{
	struct packet *packet = basic_alloc_packet(len);
	struct basic_header *header = (struct basic_header *) packet->mac;

	if (!packet)
		return -ENOMEM;

	header->to = sk->to;
	header->from = sk->from;

	/* TODO: send to device*/
	
	return 0;
}

int basic_recv(struct basic_socket *sk, char *buf, unsigned int len)
{
	/* TODO: Recv packet from socket queue */
	return 0;
}
