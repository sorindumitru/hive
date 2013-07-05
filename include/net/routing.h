#ifndef HIVE_ROUTING_H_
#define HIVE_ROUTING_H_

#include <net/utils.h>
#include <node.h>

#ifdef __cplusplus
extern "C" {
#endif

struct router {
	char			*name;
	struct list_head	list;
	void			(*start)(struct node *node);
	struct router		*(*clone)(struct node *node, struct router *router);
	struct address		*(*next_hop)(struct node *node, struct address *from, struct address *to);
	int			(*recv)(struct node *node, struct packet *packet);
	void			*priv;
};

extern struct list_head router_list;

void register_router(struct router *router);
void unregister_router(struct router *router);
struct router *router_get_by_name(const char *name);

static struct router *router_clone(struct node *node, const char *name)
{
	struct router *router = router_get_by_name(name);
	if (!name)
		return NULL;

	return router->clone(node, router);
}

extern struct address *route_next_hop(struct node *node, struct address *to);

#ifdef __cplusplus
}
#endif


#endif /* end of include guard: HIVE_ROUTING_H_ */
