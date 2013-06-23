#include <net/nic.h>
#include <node.h>
#include <net/routing.h>
#include <net/utils.h>
#include <platform.h>
#include <stdio.h>
#include <string.h>

struct eth_hdr {
	struct address from;
	struct address to;
	unsigned short protocol;
};

struct nic *simplenic_clone(struct nic *nic, struct address *address);
int simplenic_sendto(struct node *node, struct packet *packet, struct address *address);
void simplenic_recv(struct node *node, struct packet *packet);

struct nic simplenic = {
	.name = "simplenic",
	.clone = simplenic_clone,
	.sendto = simplenic_sendto,
	.recv = simplenic_recv,
};

struct nic *simplenic_clone(struct nic *nic, struct address *address)
{
	struct nic *clone = malloc(sizeof(*clone));

	if (!clone)
		return NULL;

	memcpy(clone, nic, sizeof(*nic));
	memcpy(&clone->address, address, sizeof(*address));

	packet_queue_init(&clone->rcv_queue);

	return clone;
}

int simplenic_sendto(struct node *node, struct packet *packet, struct address *address)
{
	struct node *dest;
	struct eth_hdr *hdr;
	
	/* Build header */
	hdr = plat_alloc(sizeof(*hdr));
	memset(hdr, 0, sizeof(*hdr));
	memcpy(&hdr->from, &node->nic->address, sizeof(hdr->from));
	memcpy(&hdr->to, address, sizeof(hdr->to));
	hdr->protocol = packet->protocol ? packet->protocol : ETH_DATA;

	packet->header = (void *) hdr;

	list_for_each_entry(dest, &nodes, list) {
		if (address_match(&dest->nic->address, address) && dest != node)
			dest->nic->recv(dest, packet);
	}
}

void simplenic_recv(struct node *node, struct packet *packet)
{
	/* Check if it is for the router of the node */
	if (node->router->recv(packet))
		return;
	
	/* Sending to the application */
	packet_queue(packet, &node->nic->rcv_queue);
}

void simplenic_init(void)
{
	register_nic(&simplenic);
}

void simplenic_exit(void)
{
	unregister_nic(&simplenic);
}
