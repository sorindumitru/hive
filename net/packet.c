#include <stdlib.h>

#include <net/packet.h>

struct packet *alloc_packet(unsigned int len)
{
	struct packet *packet = malloc(sizeof(*packet));

	if (!packet)
		return NULL;

	packet->data = malloc(len);

	if (!packet->data)
		goto out;

	packet->len = len;
	packet->head = packet->data;

	return packet;
out:
	free(packet);
	return NULL;
}
