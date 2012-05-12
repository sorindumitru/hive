#ifndef BASIC_L7M8L24G
#define BASIC_L7M8L24G

struct basic_header {
	unsigned int from;
	unsigned int to;
};

struct basic_socket {
	unsigned int from;
	unsigned int to;
};

struct packet *basic_alloc_packet(unsigned int len);
struct basic_socket *basic_socket_alloc(void);
int basic_socket_bind(struct basic_socket *sk, int from, int to);
int basic_send(struct basic_socket *sk, char *buff, unsigned int len);

#endif /* end of include guard: BASIC_L7M8L24G */
