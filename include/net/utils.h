#ifndef HIVE_NET_UTILS_H_
#define HIVE_NET_UTILS_H_

#include <list.h>
#include <platform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_ADDRESS_LEN		6

struct address {
	union {
		unsigned char mac_address[MAC_ADDRESS_LEN];
	};
};

struct packet {
	size_t			len;
	char			*data;
	struct list_head	list;
};

static inline void packet_free(struct packet *packet)
{
	plat_free(packet->data);
	plat_free(packet);
}

struct packet_queue {
	size_t			qlen;
	struct list_head	queue;
};

static inline struct packet *packet_dequeue(struct packet_queue *queue)
{
	struct packet *packet;
	if (list_empty(&queue->queue))
		return NULL;

	packet = list_entry(&queue->queue, struct packet, list);
	list_del(&packet->list);

	return packet;
}

#ifdef __cplusplus
}
#endif




#endif /* end of include guard: HIVE_NET_UTILS_H_ */
