#include <net/routing.h>

static struct address * norouter_next_hop(struct node *node, struct address *from, struct address *to);
static int norouter_recv(struct node *node, struct packet *packet);

struct router norouter_router = {
	.name = "norouter",
	.next_hop = norouter_next_hop,
	.recv = norouter_recv,
};

static struct address * norouter_next_hop(struct node *node, struct address *from, struct address *to)
{
	/* Simply return destination address */
	return to;
}

static int norouter_recv(struct node *node, struct packet *packet)
{
	/* We don't do anything here */
	return 0;
}

void norouter_init(void)
{
	register_router(&norouter_router);
}

void norouter_exit(void)
{
	unregister_router(&norouter_router);
}
