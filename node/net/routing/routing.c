#include <net/routing.h>

LIST_HEAD(router_list);

struct address *route_next_hop(struct node *node, struct address *to)
{
	if (!node->router)
		return NULL;

	return node->router->next_hop(&node->nic->address, to);
}

void register_router(struct router *router)
{
	list_add(&router_list, &router->list);
}

void unregister_router(struct router *router)
{
	list_del(&router->list);
}
