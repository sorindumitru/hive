#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <node.h>
#include <timer.h>
#include <net/netlib.h>
#include <net/utils.h>

enum {
	kMsgADV = 1,
	kMsgREQ,
	kMsgDATA
};

struct metadata_header_t {
	unsigned char type;
	unsigned short origin_node;
	unsigned short sending_node;
	unsigned short seq_no;
};

struct metadata_t {
	struct metadata_header_t header;

	unsigned char *data;
	size_t data_len;

	struct list_head list;
};

struct counters_t {
	unsigned adv_recv;
	unsigned adv_sent;
	unsigned req_recv;
	unsigned req_sent;
	unsigned data_recv;
	unsigned data_sent;
};

struct node_data_t {
	struct list_head send_metadata_list;
	struct list_head recv_metadata_list;

	struct timer *recv_timer;
	struct counters_t counters;
};

static unsigned short s_seq_no = 1;

struct node_data_t *get_node_data(struct node *node)
{
	return (struct node_data_t *)node->priv;
}

void send_adv(struct node *node, struct metadata_t *metadata)
{
//	printf("%s: %s\n", __FUNCTION__, node->name);

	metadata->header.type = kMsgADV;
	metadata->header.sending_node = node->index;

	struct address addr;
	plat_memset(&addr, 0xFF, sizeof(addr));
	hive_sendto(node, (unsigned char *)&metadata->header, sizeof(metadata->header), &addr);

	get_node_data(node)->counters.adv_sent++;
}

void send_req(struct node *node, struct metadata_t *metadata, struct address *address)
{
//	printf("%s: %s\n", __FUNCTION__, node->name);

	metadata->header.type = kMsgREQ;
	metadata->header.sending_node = node->index;

	hive_sendto(node, (unsigned char *)&metadata->header, sizeof(metadata->header), address);

	get_node_data(node)->counters.req_sent++;
}

void send_data(struct node *node, struct metadata_t *metadata, struct address *address)
{
//	printf("%s: %s\n", __FUNCTION__, node->name);

	metadata->header.type = kMsgDATA;

	size_t buf_size = sizeof(metadata->header) + metadata->data_len;
	unsigned char *buf = (unsigned char *)plat_alloc(buf_size);
	if (!buf)
		return;

	plat_memcpy(buf, &metadata->header, sizeof(metadata->header));
	plat_memcpy(buf + sizeof(metadata->header), metadata->data, metadata->data_len);

	hive_sendto(node, buf, buf_size, address);

	get_node_data(node)->counters.data_sent++;

	plat_free(buf);
}

void spin_send(struct node *node, unsigned char *data, size_t data_len)
{
	struct metadata_t *metadata = (struct metadata_t *)plat_alloc(sizeof(*metadata));
	plat_memset(metadata, 0, sizeof(*metadata));

//	printf("%s: %s\n", __FUNCTION__, node->name);

	metadata->header.origin_node = node->index;
	metadata->header.seq_no = s_seq_no++;

	metadata->data = plat_alloc(data_len);
	metadata->data_len = data_len;
	plat_memcpy(metadata->data, data, data_len);

	INIT_LIST_HEAD(&metadata->list);

	struct node_data_t *node_data = (struct node_data_t *)node->priv;
	list_add_tail(&metadata->list, &node_data->send_metadata_list);

	printf("node '%s' sending data: %s\n", node->name, (char *)metadata->data);

	send_adv(node, metadata);
}

int needs_data(struct node *node, struct metadata_header_t *metadata_header)
{
	struct metadata_t *entry;
	list_for_each_entry(entry, &get_node_data(node)->recv_metadata_list, list) {
		if (entry->header.origin_node == metadata_header->origin_node && entry->header.seq_no == metadata_header->seq_no)
			return 0;
	}
	return 1;
}

struct metadata_t *get_metadata(struct list_head * list, unsigned short origin_node, unsigned short seq_no)
{
	struct metadata_t *entry;
	list_for_each_entry(entry, list, list) {
		if (entry->header.origin_node == origin_node && entry->header.seq_no == seq_no)
			return entry;
	}
	return NULL;
}

void recv_adv(struct node *node, unsigned char *data, size_t data_len, struct address *address)
{
	struct metadata_header_t *hdr = (struct metadata_header_t *)data;

//	printf("%s: %s\n", __FUNCTION__, node->name);
	get_node_data(node)->counters.adv_recv++;

	if (hdr->origin_node == node->index || hdr->sending_node == node->index)
		return;

	if (!needs_data(node, hdr))
		return;

	struct metadata_t *metadata = (struct metadata_t *)plat_alloc(sizeof(*metadata));
	plat_memset(metadata, 0, sizeof(*metadata));

	metadata->header = *hdr;
	INIT_LIST_HEAD(&metadata->list);

	struct node_data_t *node_data = (struct node_data_t *)node->priv;
	list_add_tail(&metadata->list, &node_data->recv_metadata_list);

	send_req(node, metadata, address);
}

void recv_req(struct node *node, unsigned char *data, size_t data_len, struct address *address)
{
	struct metadata_t *metadata;
	struct metadata_header_t *hdr = (struct metadata_header_t *)data;

//	printf("%s: %s\n", __FUNCTION__, node->name);
	get_node_data(node)->counters.req_recv++;

	metadata = get_metadata(&get_node_data(node)->recv_metadata_list, hdr->origin_node, hdr->seq_no);

	if (!metadata)
		metadata = get_metadata(&get_node_data(node)->send_metadata_list, hdr->origin_node, hdr->seq_no);

	if (!metadata) {
		printf("received req for an unknown data origin_node=%u, seq_no=%u\n", hdr->origin_node, hdr->seq_no);
		return;
	}

	if (metadata->data_len == 0) {
		printf("received req but have no data for origin_node=%u, seq_no=%u\n", hdr->origin_node, hdr->seq_no);
		return;
	}

	send_data(node, metadata, address);
}

void recv_data(struct node *node, unsigned char *data, size_t data_len)
{
	struct metadata_t *metadata;
	struct metadata_header_t *hdr = (struct metadata_header_t *)data;

//	printf("%s: %s\n", __FUNCTION__, node->name);
	get_node_data(node)->counters.data_recv++;

	metadata = get_metadata(&get_node_data(node)->recv_metadata_list, hdr->origin_node, hdr->seq_no);

	if (!metadata) {
		printf("received data for an unknown request origin_node=%u, seq_no=%u\n", hdr->origin_node, hdr->seq_no);
		return;
	}

	metadata->data = (unsigned char *)plat_alloc(data_len);
	metadata->data_len = data_len - sizeof(struct metadata_header_t);
	plat_memcpy(metadata->data, data + sizeof(struct metadata_header_t), metadata->data_len);

	printf("node '%s' received data: %s\n", node->name, (char *)metadata->data);

	send_adv(node, metadata);
}

void spin_recv(struct node *node, unsigned char *data, size_t data_len, struct address *address)
{
//	printf("%s: %s\n", __FUNCTION__, node->name);

	if (data_len < sizeof(struct metadata_header_t)) {
		printf("short packet received, got %zu, expected at least %lu\n", data_len, sizeof(struct metadata_header_t));
		return;
	}

	struct metadata_header_t *hdr = (struct metadata_header_t *)data;

	if (data_len == sizeof(struct metadata_header_t)) {
		if (hdr->type == kMsgADV)
			recv_adv(node, data, data_len, address);
		else if (hdr->type == kMsgREQ)
			recv_req(node, data, data_len, address);
	} else if (hdr->type == kMsgDATA) {
		recv_data(node, data, data_len);
	} else {
		printf("received packet with invalid message type %u\n", hdr->type);
	}
}

void recv_callback(void *arg)
{
	struct node *node = (struct node *)arg;

	unsigned char data[128] = {0};
	size_t len = sizeof(data);
	int recv_len = 0;
	struct address from;

	while ((recv_len = hive_recvfrom(node, data, &len, &from)) != -EAGAIN)
		spin_recv(node, data, recv_len, &from);
}

void spin_start(void *arg)
{
	struct node *node = (struct node *) arg;
	struct node_data_t *node_data = (struct node_data_t *)node->priv;

	node_data->recv_timer->arg = node;
	timer_add(node_data->recv_timer);

	printf("Starting %s\n", node->name);

	if (strncmp(node->name, "sender", 6) == 0) {
		char data[128];
		snprintf(data, sizeof(data) - 1, "Hello from %s!", node->name);
		spin_send(node, data, strlen(data) + 1);
	}
}

void clear_metadata_list(struct list_head *list)
{
	while (!list_empty(list)) {
		struct metadata_t *metadata = list_entry(list->next, struct metadata_t, list);
		plat_free(metadata->data);
		list_del(&metadata->list);
		plat_free(metadata);
	}
}

void spin_stop(void *arg)
{
	struct node *node = (struct node *) arg;
	struct node_data_t *node_data = (struct node_data_t *)node->priv;

	printf("Stopping %s\n", node->name);

	timer_del(node_data->recv_timer);

	printf("\tADV  recv: %u\n", node_data->counters.adv_recv);
	printf("\tADV  sent: %u\n", node_data->counters.adv_sent);
	printf("\tREQ  recv: %u\n", node_data->counters.req_recv);
	printf("\tREQ  sent: %u\n", node_data->counters.req_sent);
	printf("\tDATA recv: %u\n", node_data->counters.data_recv);
	printf("\tDATA sent: %u\n", node_data->counters.data_sent);

	clear_metadata_list(&node_data->send_metadata_list);
	clear_metadata_list(&node_data->recv_metadata_list);

	plat_memset(&node_data->counters, 0, sizeof(node_data->counters));
}

void *spin_init(void)
{
	struct node_data_t *node_data = (struct node_data_t *)plat_alloc(sizeof(*node_data));
	plat_memset(node_data, 0, sizeof(*node_data));

	INIT_LIST_HEAD(&node_data->send_metadata_list);
	INIT_LIST_HEAD(&node_data->recv_metadata_list);

	node_data->recv_timer = timer_new_recursive(recv_callback, 1);

	return node_data;
}

void spin_exit(void *arg)
{
	struct node *node = (struct node *) arg;
	struct node_data_t *node_data = (struct node_data_t *)node->priv;

	timer_free(node_data->recv_timer);

	plat_free(node->priv);
}
