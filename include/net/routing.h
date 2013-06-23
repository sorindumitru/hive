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
	struct address		*(*next_hop)(struct address *from, struct address *to);
	int			(*recv)(struct packet *packet);
};

extern struct list_head router_list;

void register_router(struct router *router);
void unregister_router(struct router *router);
struct router *router_get_by_name(const char *name);

extern struct address *route_next_hop(struct node *node, struct address *to);

#ifdef __cplusplus
}
#endif


#endif /* end of include guard: HIVE_ROUTING_H_ */
