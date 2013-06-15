#ifndef HIVE_NETLIB_H
#define HIVE_NETLIB_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <list.h>
#include <node.h>
#include <platform.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int hive_sendto(struct node *node, unsigned char *buf, size_t len, struct address *to);
extern int hive_recv(struct node *node, unsigned char *buf, size_t *len);
extern int hive_recvfrom(struct node *node, unsigned char *buf, size_t *len, struct address *address);

#ifdef __cplusplus
}
#endif
#endif /* end of include guard: HIVE_NETLIB_H */
