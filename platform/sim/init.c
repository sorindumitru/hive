#include <platform.h>

void platform_init(void)
{
	simplenic_init();
	broadcast_init();
	norouter_init();
	leach_init();
}
