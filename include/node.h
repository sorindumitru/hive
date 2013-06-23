#ifndef HIVE_NODE_H_
#define HIVE_NODE_H_

#include <net/nic.h>
#include <net/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

struct router;

struct node {
	struct nic *nic;
	struct router *router;

	void *dlhandle;
};
#ifdef __cplusplus
}
#endif

#endif /* end of include guard: HIVE_NODE_H_ */
