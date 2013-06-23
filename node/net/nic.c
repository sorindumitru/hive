#include <net/nic.h>

LIST_HEAD(drivers);

void register_nic(struct nic *nic)
{
	list_add(&nic->list, &drivers);
}

void unregister_nic(struct nic *nic)
{
	list_del(&nic->list);
}

struct nic *nic_find_by_name(const char *name)
{
	struct nic *nic;

	list_for_each_entry(nic, &drivers, list) {
		if (!strcmp(nic->name, name))
			return nic;
	}

	return NULL;
}
