#ifndef HIVE_NIC_H_
#define HIVE_NIC_H_

#include <list.h>
#include <net/utils.h>

#ifdef __cplusplus
extern "C" {
#endif

struct nic;
struct node;

typedef struct nic *(*clone_nic_t)(struct nic *nic, struct address *address);
typedef void (*recv_t)(struct node *node, struct packet *packet);
typedef int (*sendto_t)(struct packet *packet, struct address *address);

struct nic {
	char *name;
	struct address address;
	struct packet_queue rcv_queue;
	void *priv;

	clone_nic_t clone;
	recv_t recv;
	sendto_t sendto;

	struct list_head list;
};


extern void register_nic(struct nic *nic);
extern void unregister_nic(struct nic *nic);
extern struct nic *nic_find_by_name(const char *name);

static inline struct nic *nic_clone(const char *name, struct address address)
{
	struct nic *source = nic_find_by_name(name);

	return source->clone(source, &address);
}

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: HIVE_NIC_H_ */
