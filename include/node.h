#ifndef HIVE_NODE_H_
#define HIVE_NODE_H_

#include <net/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

struct router;

struct node {
	struct address address;	
	struct packet_queue *rcv_queue;

	struct router *router;
};

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: HIVE_NODE_H_ */
