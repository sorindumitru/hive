#include <stdlib.h>

#include <timer.h>
#include <net/routing.h>

static void leach_start(struct node *node);
static struct router *leach_clone(struct node *node, struct router *router);
static struct address *leach_next_hop(struct node *node, struct address *from, struct address *to);
static int leach_recv(struct node *node, struct packet *packet);

struct router leach_router = {
	.name = "LEACH",
	.start = leach_start,
	.clone = leach_clone,
	.next_hop = leach_next_hop,
	.recv = leach_recv,
};

#define PERCENTAGE_CLUSTER_HEADS  (0.06)

struct leach_private {
	int cluster_head;
	unsigned int distance;
	struct address nexthop;
};

#define LEACH_PROTO 0xEEAC

struct leach_adv {
#define LEACH_ADV	0xEADF
	unsigned int type;
	unsigned int id;
	unsigned int x;
	unsigned int y;
	struct address address;
};

static struct address broadcast;

static struct router *leach_clone(struct node *node, struct router *router)
{
	float probability, threshold;
	struct leach_private *priv;
	struct router *clone = (struct router *) plat_alloc(sizeof(*clone));
	if (!clone)
		return NULL;

	memcpy(clone, router, sizeof(*router));

	clone->priv = plat_alloc(sizeof(struct leach_private));
	if (!clone->priv) {
		plat_free(clone);
		return NULL;
	}

	priv = (struct leach_private *) clone->priv;

	/* Determine the probability that this node
	 * will be a cluster head */
	probability = ((float)rand())/RAND_MAX;

	threshold = PERCENTAGE_CLUSTER_HEADS;
	
	if (probability < threshold)
		priv->cluster_head = 1;
	else
		priv->cluster_head = 0;
	priv->nexthop = broadcast;
	priv->distance = -1;

	if (probability < threshold)
		printf("Node %d is a cluster\n", node->index);
	else
		printf("Node %d is not a cluster\n", node->index);

	return clone;
}

static void leach_start(struct node *node)
{
	struct packet *packet = plat_alloc(sizeof(*packet));
	struct leach_adv *advhdr;
	struct leach_private *priv = (struct leach_private *) node->router->priv;

	if (!packet)
		return;

	if (!priv->cluster_head)
		return;

	packet->data = plat_alloc(sizeof(struct leach_adv));
	if (!packet->data) {
		plat_free(packet);
		return;
	}

	packet->protocol = LEACH_PROTO;
	packet->len = sizeof(*advhdr);
	advhdr = (struct leach_adv*) packet->data;
	advhdr->type = LEACH_ADV;
	advhdr->id = node->index;
	advhdr->x = node->x;
	advhdr->y = node->y;
	advhdr->address = node->nic->address;

	node->nic->sendto(node, packet, &broadcast);
}

static struct address *leach_next_hop(struct node *node, struct address *from, struct address *to)
{
	struct leach_private *priv = (struct leach_private *) node->router->priv;

	return &priv->nexthop;
}

static inline unsigned int distance_squared(int x1, int y1, int x2, int y2)
{
	return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
}

static int leach_adv_rcv(struct node *node, struct packet *packet)
{
	struct leach_private *priv = (struct leach_private *) node->router->priv;
	struct leach_adv *adv = (struct leach_adv *) packet->data;
	int distance = distance_squared(node->x, node->y, adv->x, adv->y);	

	if (distance < priv->distance) {
		priv->distance = distance;
		priv->nexthop = adv->address;
		printf("My new nexthop is ");
		address_print(&priv->nexthop);
		printf("\n");
	}
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
	int i = 0;

	for (i = 0; i < MAC_ADDRESS_LEN; i++)
		broadcast.mac[i] = 0xFF;
	register_router(&leach_router);
}
