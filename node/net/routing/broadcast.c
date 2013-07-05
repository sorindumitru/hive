#include <net/routing.h>

static struct address * broadcast_next_hop(struct node *node, struct address *from, struct address *to);
static int broadcast_recv(struct node *node, struct packet *packet);

struct router broadcast_router = {
	.name = "broadcast",
	.next_hop = broadcast_next_hop,
	.recv = broadcast_recv,
};

static struct address * broadcast_next_hop(struct node *node, struct address *from, struct address *to)
{
	struct address *broadcast = plat_alloc(sizeof(*broadcast));
	if (!broadcast)
		return NULL;

	memset(broadcast->mac, 0xFF, sizeof(broadcast->mac));

	return broadcast;
}

static int broadcast_recv(struct node *node, struct packet *packet)
{
	/* We don't do anything here */
	return 0;
}

void broadcast_init(void)
{
	register_router(&broadcast_router);
}

void broadcast_exit(void)
{
	unregister_router(&broadcast_router);
}
