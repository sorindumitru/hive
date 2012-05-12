#ifndef PACKET_CSH1TJQG
#define PACKET_CSH1TJQG

struct packet {
	unsigned int len;
	char *mac;
	char *data;
	char *head;
};

struct packet *alloc_packet(unsigned int len);

#endif /* end of include guard: PACKET_CSH1TJQG */
