#ifndef HIVE_NET_UTILS_H_
#define HIVE_NET_UTILS_H_

#include <list.h>
#include <platform.h>
#include <stdio.h>
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

static inline char hex_to_int(char hex)
{
	if (hex >= '0' && hex <= '9')
		return hex - '0';
	if (hex >= 'a' && hex <= 'f')
		return hex - 'a';
	if (hex >= 'A' && hex <= 'F')
		return hex - 'A';

	return 0;
}

static inline struct address address_from_string(const char *source)
{
	struct address addr;
	int i, j;

	for (i = 0, j = 0; i < MAC_ADDRESS_LEN; i++, j+=3) {
		addr.mac[i] = hex_to_int(source[j])*16 + hex_to_int(source[j+1]);
	}

	return addr;
}

static inline void address_print(struct address *address)
{
	printf("%hx:%hx:%hx:%hx:%hx:%hx",
				address->mac[0],
				address->mac[1],
				address->mac[2],
				address->mac[3],
				address->mac[4],
				address->mac[5]);
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
	
	if (addr_is_broadcast(two))
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
	plat_free(packet->header);
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
	list_add_tail(&packet->list, &queue->queue);
}

static inline struct packet *packet_dequeue(struct packet_queue *queue)
{
	struct packet *packet;
	if (list_empty(&queue->queue))
		return NULL;

	packet = list_entry(&queue->queue, struct packet, list);
	list_del(&packet->list);
	queue->qlen--;

	return packet;
}

#ifdef __cplusplus
}
#endif




#endif /* end of include guard: HIVE_NET_UTILS_H_ */
