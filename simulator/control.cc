#include <dlfcn.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

#include <control.hpp>

#include <node.h>

extern event_base *m_plat_base[16];
static command_handlers_t command_handlers;

bool operator< (const address &left, const address &right)
{
	for (int i = 0; i < MAC_ADDRESS_LEN; i++) {
		if (left.mac_address[i] == right.mac_address[i])
			continue;
		return left.mac_address[i] < right.mac_address[i];
	}

	return false;
}

control::control()
	: m_node_index(1)
{
	m_control_sk = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in sin;
	int one = 1;

	if (m_control_sk < 0)
		perror("Could not create control socket");

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(9090);
	if (bind (m_control_sk, (struct sockaddr *) &sin, sizeof(sin)) < 0 )
		perror("Could not bind control socket");

	if (setsockopt(m_control_sk, SOL_SOCKET, SO_REUSEADDR,
				&one, sizeof(one)) < 0)
		perror("Could not set so reuseaddr on control socket");

	command_handlers["load "] = &control::cmd_load;
	command_handlers["unload "] = &control::cmd_unload;
	command_handlers["start "] = &control::cmd_start;
	command_handlers["stop "] = &control::cmd_stop;

	listen(m_control_sk, 64);
	m_control_event = event_new(m_plat_base[sched_getcpu()],
			m_control_sk, EV_READ | EV_PERSIST, do_command, this);
	event_add(m_control_event, NULL);
	std::cout << "Started control thread" << std::endl;
}

control::~control()
{
	evtimer_del(m_control_event);
	event_free(m_control_event);

	close(m_control_sk);
}

void control::do_command(int fd, short event, void *arg)
{
	control *c = static_cast<control *>(arg);
	c->command(fd);
}

void control::command(int sock)
{
	char buffer[1024] = {0};

	read(m_control_sk, buffer, 1024);

	for (command_handlers_t::const_iterator i = command_handlers.begin(); i != command_handlers.end(); ++i) {
		size_t command_len = strlen(i->first);

		if (strlen(buffer) >= command_len && strncmp(buffer, i->first, command_len) == 0)
			(this->*i->second)(buffer + command_len);
	}
}

// Node command handling

void control::cmd_load(char *args)
{
	char *name = strtok(args, " \n\t");

	void *handle = add_library(name);
	if (handle == NULL)
		return;

	const struct lib_functions &lib_funcs = get_lib_functions(handle);

	void *data = lib_funcs.node_init();
	struct node *node = lib_funcs.node_getnode(data);

	unsigned index = add_node_data(handle, data, node);

	std::cout << "created node " << index << std::endl;
}

void control::cmd_unload(char *args)
{
	char *arg = strtok(args, " \n\t");
	if (arg == NULL) {
		std::cerr << "usage: unload node_index" << std::endl;
		return;
	}

	unsigned index = atoi(arg);
	
	const struct node_data *node_data = get_node_data(index);
	if (node_data == NULL) {
		std::cerr << "cannot find node " << index << std::endl;
		return;
	}

	const struct lib_functions &lib_funcs = get_lib_functions(node_data->dlhandle);

	lib_funcs.node_exit(node_data->data);
	del_node(index);

	std::cout << "removed node " << index << std::endl;
}

void control::cmd_start(char *args)
{
	char *arg = strtok(args, " \n\t");
	if (arg == NULL) {
		std::cerr << "usage: start node_index" << std::endl;
		return;
	}

	unsigned index = atoi(arg);

	const struct node_data *node_data = get_node_data(index);
	if (node_data == NULL) {
		std::cerr << "cannot find node " << index << std::endl;
		return;
	}

	const struct lib_functions &lib_funcs = get_lib_functions(node_data->dlhandle);

	lib_funcs.node_start(node_data->data);

	std::cout << "started node " << index << std::endl;
}

void control::cmd_stop(char *args)
{
	char *arg = strtok(args, " \n\t");
	if (arg == NULL) {
		std::cerr << "usage: stop node_index" << std::endl;
		return;
	}

	unsigned index = atoi(arg);

	const struct node_data *node_data = get_node_data(index);
	if (node_data == NULL) {
		std::cerr << "cannot find node " << index << std::endl;
		return;
	}

	const struct lib_functions &lib_funcs = get_lib_functions(node_data->dlhandle);

	lib_funcs.node_stop(node_data->data);

	std::cout << "stopped node " << index << std::endl;
}

unsigned control::add_node_data(void *dlhandle, void *data, struct node* node)
{
	struct node_data &node_data = m_nodes[m_node_index];
	node_data.dlhandle = dlhandle;
	node_data.data = data;
	node_data.node = node;

	// TODO: Check for duplicate addresses
	m_nodes_by_address[node->address] = node;

	return m_node_index++;
}

const struct control::node_data *control::get_node_data(unsigned index) const
{
	nodes_t::const_iterator i = m_nodes.find(index);
	return i != m_nodes.end() ? &i->second : NULL;
}

void control::del_node(unsigned index)
{
	m_nodes_by_address.erase(m_nodes[index].node->address);

	m_nodes.erase(index);
}

void *control::add_library(const char *name)
{
	// Add mappings between library name and node functions

	char library[64] = {0};
	sprintf(library, "lib%s.so", name);

	void *handle = dlopen(library, RTLD_NOW);
	if (!handle) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	if (m_lib_functions_map.find(handle) != m_lib_functions_map.end())
		return handle;

	char func_name[256] = {0};
	lib_functions lib_funcs;

	sprintf(func_name, "%s_init", name);
	lib_funcs.node_init = (node_init_t) dlsym(handle, func_name);
	if (!lib_funcs.node_init) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	sprintf(func_name, "%s_getnode", name);
	lib_funcs.node_getnode = (node_getnode_t) dlsym(handle, func_name);
	if (!lib_funcs.node_getnode) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	sprintf(func_name, "%s_exit", name);
	lib_funcs.node_exit = (node_exit_t) dlsym(handle, func_name);
	if (!lib_funcs.node_exit) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	sprintf(func_name, "%s_start", name);
	lib_funcs.node_start = (node_start_t) dlsym(handle, func_name);
	if (!lib_funcs.node_start) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	sprintf(func_name, "%s_stop", name);
	lib_funcs.node_stop = (node_stop_t) dlsym(handle, func_name);
	if (!lib_funcs.node_stop) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	m_lib_functions_map[handle] = lib_funcs;

	return handle;
}

const struct control::lib_functions &control::get_lib_functions(void *dlhandle) const
{
	lib_functions_map_t::const_iterator i = m_lib_functions_map.find(dlhandle);
	if (i == m_lib_functions_map.end())
		perror("cannot find library functions");
	return i->second;
}
