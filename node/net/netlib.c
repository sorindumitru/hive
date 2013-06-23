#include <net/routing.h>
#include <net/netlib.h>
#include <platform.h>

int hive_sendto(struct node *node, unsigned char *buf, size_t len, struct address *to)
{
	struct packet *packet;
	struct address *nexthop;
	int err = -ENOMEM;

	packet = plat_alloc(sizeof(*packet));
	if (!packet)
		goto out;

	packet->data = plat_alloc(len);
	if (!packet->data)
		goto free_packet;

	packet->len = len;
	plat_memcpy(packet->data, buf, len);

	/* Get next this packet from routing */
	nexthop = route_next_hop(node, to);

	/* TODO: Validate nexthop */

	node->nic->sendto(node, packet, nexthop);

	return len;

free_packet:
	plat_free(packet);
out:
	return -ENOMEM;
}

int hive_recv(struct node *node, unsigned char *buf, size_t *len)
{
	struct packet *packet = packet_dequeue(&node->nic->rcv_queue);

	if (!packet)
		return -EAGAIN;

	if (packet->len > *len)
		return -EMSGSIZE;

	plat_memcpy(buf, packet->data, packet->len);
	packet_free(packet);
	return packet->len;
}

int hive_recvfrom(struct node *node, unsigned char *buf, size_t *len, struct address *from)
{
	struct packet *packet = packet_dequeue(&node->nic->rcv_queue);

	if (!packet)
		return -EAGAIN;

	if (packet->len > *len)
		return -EMSGSIZE;

	/* TODO: Validate address */

	plat_memcpy(buf, packet->data, packet->len);
	packet_free(packet);
	return packet->len;
}
