#ifndef HIVE_PLATFORM_H
#define HIVE_PLATFORM_H

#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*callback_t)(void *arg);

extern void *plat_alloc(size_t size);
extern void plat_memcpy(void *to, void *from, size_t len);
extern void plat_free(void *data);
extern void plat_add_timer(callback_t *callback, int timeout);

struct packet;
struct address;

extern void plat_sendto(struct packet *packet, struct address *to);


#ifdef __cplusplus
}
#endif

#endif /* end of include guard: HIVE_PLATFORM_H */

