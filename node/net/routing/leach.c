#include <stdlib.h>

#include <timer.h>
#include <net/routing.h>

static struct router *leach_clone(struct node *node, struct router *router);
static struct address *leach_next_hop(struct address *from, struct address *to);
static int leach_recv(struct node *node, struct packet *packet);

struct router leach_router = {
	.name = "LEACH",
	.clone = leach_clone,
	.next_hop = leach_next_hop,
	.recv = leach_recv,
};

#define PERCENTAGE_CLUSTER_HEADS  (0.1)

struct leach_private {
	int	round;
};

#define LEACH_PROTO 0xEEAC

struct leach_adv {
#define LEACH_ADV	0xEADF
	unsigned int type;
	unsigned int id;
	unsigned int x;
	unsigned int y;
};

static struct router *leach_clone(struct node *node, struct router *router)
{
	float probability, threshold;
	struct router *clone = (struct router *) plat_alloc(sizeof(*clone));
	if (!router)
		return NULL;

	/* Determine the probability that this node
	 * will be a cluster head */
	probability = rand()/RAND_MAX;

	threshold = PERCENTAGE_CLUSTER_HEADS;

	if (probability < threshold) {
		/* I'm a cluster head, lets notify everyone else */
		struct address to = address_from_string("ff:ff:ff:ff:ff:ff");
		struct packet *packet = plat_alloc(sizeof(*packet));
		struct leach_adv *advhdr;

		if (!packet)
			return NULL;

		packet->data = plat_alloc(sizeof(struct leach_adv));
		if (!packet->data) {
			plat_free(packet);
			return NULL;
		}

		packet->protocol = LEACH_PROTO;
		advhdr = (struct leach_adv*) packet->data;
		advhdr->type = LEACH_ADV;
		advhdr->id = node->index;
		advhdr->x = node->x;
		advhdr->y = node->y;

		node->nic->sendto(node, packet, &to);
	}
}

static struct address *leach_next_hop(struct address *from, struct address *to)
{
		
}

static int leach_adv_rcv(struct node *node, struct packet *packet)
{

}

static int leach_recv(struct node *node, struct packet *packet)
{
	unsigned int *type = (unsigned int *) packet->data;

	if (packet->protocol != LEACH_PROTO)
		return 0;

	/* Only LEACH packets, hopefully */

	switch (*type) {
	case LEACH_ADV:
		leach_adv_rcv(node, packet);
		break;
	default:
		printf("UNKNOWN LEACH PACKET DATA\n");
	}

	return 1;
}

void leach_init(void)
{
	register_router(&leach_router);
}
