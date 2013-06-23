#ifndef HIVE_NET_UTILS_H_
#define HIVE_NET_UTILS_H_

#include <list.h>
#include <platform.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_ADDRESS_LEN		6

#define ETH_DATA		0x1337

struct address {
	union {
		unsigned char mac[MAC_ADDRESS_LEN];
	};
};

static inline struct address address_from_string(const char *source)
{
	struct address addr;
	int i, j;

	for (i = 0, j = 0; i < MAC_ADDRESS_LEN; i++, j+=2) {
		addr.mac[i] = source[j]*16 + source[j+1];
	}

	return addr;
}

static inline int addr_is_broadcast(struct address *addr)
{
	if (addr->mac[0] == 0xFF && addr->mac[1] == 0xFF && addr->mac[2] == 0xFF &&
		addr->mac[3] == 0xFF && addr->mac[4] == 0xFF && addr->mac[5] == 0xFF)
		return 1;

	return 0;
}

static inline int address_match(struct address *one, struct address *two)
{
	if (!memcmp(one, two, MAC_ADDRESS_LEN))
		return 1;
	else if (!addr_is_broadcast(two))
		return 1;

	return 0;
}

struct packet {
	size_t			len;
	unsigned short		protocol;
	struct address		*src;
	struct address		*dst;
	char			*header;
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

static inline void packet_queue_init(struct packet_queue *queue)
{
	queue->qlen = 0;
	INIT_LIST_HEAD(&queue->queue);
}

static inline void packet_queue(struct packet *packet, struct packet_queue *queue)
{
	queue->qlen++;
	list_add(&packet->list, &queue->queue);
}

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
