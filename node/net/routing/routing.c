#include <net/routing.h>

LIST_HEAD(router_list);

struct address *route_next_hop(struct node *node, struct address *to)
{
	if (!node->router)
		return NULL;

	return node->router->next_hop(node, &node->nic->address, to);
}

void register_router(struct router *router)
{
	list_add(&router->list, &router_list);
}

void unregister_router(struct router *router)
{
	list_del(&router->list);
}

struct router *router_get_by_name(const char *name)
{
	struct router *router;

	list_for_each_entry(router, &router_list, list) {
		if (!strcmp(router->name, name))
			return router;
	}

	return NULL;
}
