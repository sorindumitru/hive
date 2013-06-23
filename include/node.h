#ifndef HIVE_NODE_H_
#define HIVE_NODE_H_

#include <net/nic.h>
#include <net/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

struct router;

struct node {
	int index;
	struct nic *nic;
	struct router *router;

	void *dlhandle;

	struct list_head list;
};

extern void node_add(struct node *node);
extern void node_del(struct node *node);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: HIVE_NODE_H_ */
