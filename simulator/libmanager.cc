#include <dlfcn.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

#include <libmanager.hpp>

void *libmanager::load_node_library(const char *name)
{
	// Add mappings between library name and node functions

	char library[64] = {0};
	sprintf(library, "lib%s.so", name);

	void *handle = dlopen(library, RTLD_NOW);
	if (!handle) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	if (m_node_library_map.find(handle) != m_node_library_map.end())
		return handle;

	char func_name[256] = {0};
	struct node_library_t node_lib;

	sprintf(func_name, "%s_init", name);
	node_lib.init = (node_init_t) dlsym(handle, func_name);
	if (!node_lib.init) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	sprintf(func_name, "%s_exit", name);
	node_lib.exit = (node_exit_t) dlsym(handle, func_name);
	if (!node_lib.exit) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	sprintf(func_name, "%s_start", name);
	node_lib.start = (node_start_t) dlsym(handle, func_name);
	if (!node_lib.start) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	sprintf(func_name, "%s_stop", name);
	node_lib.stop = (node_stop_t) dlsym(handle, func_name);
	if (!node_lib.stop) {
		std::cerr << dlerror() << std::endl;
		return NULL;
	}

	m_node_library_map[handle] = node_lib;

	return handle;
}

const struct node_library_t &libmanager::get_node_library(void *dlhandle) const
{
	node_library_map_t::const_iterator i = m_node_library_map.find(dlhandle);
	if (i == m_node_library_map.end())
		perror("node library not loaded");
	return i->second;
}
