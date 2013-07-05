#ifndef HIVE_NODE_H_
#define HIVE_NODE_H_

#include <net/nic.h>
#include <net/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

extern struct list_head nodes;
	
struct router;

struct node {
	char *name;
	unsigned int x;
	unsigned int y;
	int index;
	struct nic *nic;
	struct router *router;

	void *dlhandle;
	void *priv;

	struct list_head list;
};

extern void node_add(struct node *node);
extern void node_del(struct node *node);
extern struct node *node_find_by_name(const char *name);
extern struct node *node_find_by_index(int index);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: HIVE_NODE_H_ */
