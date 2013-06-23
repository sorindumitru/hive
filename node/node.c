#include <node.h>

LIST_HEAD(nodes);

void node_add(struct node *node)
{
	list_add(&node->list, &nodes);
}

void node_del(struct node *node)
{
	list_del(&node->list);
}

struct node *node_find_by_index(int index)
{
	struct node *node;

	list_for_each_entry(node, &nodes, list) {
		if (node->index == index)
			return node;
	}

	return NULL;
}

struct node *node_find_by_name(const char *name)
{
	struct node *node;

	list_for_each_entry(node, &nodes, list) {
		if (!strcmp(node->name, name))
			return node;
	}

	return NULL;
}
