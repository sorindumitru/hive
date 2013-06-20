#include <string.h>

#include <platform.h>

void *plat_alloc(size_t size)
{
	return malloc(size);
}

void plat_memcpy(void *to, void *from, size_t len)
{
	memcpy(to, from, len);
}

void plat_free(void *data)
{
	free(data);
}
