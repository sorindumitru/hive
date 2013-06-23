#ifndef HIVE_SIM_LIBMANAGER_HPP_
#define HIVE_SIM_LIBMANAGER_HPP_

#include <map>

typedef void *(*node_init_t)(void);
typedef void (*node_exit_t)(void *);
typedef void (*node_start_t)(void *);
typedef void (*node_stop_t)(void *);

struct node_library_t {
	node_init_t init;
	node_exit_t exit;
	node_start_t start;
	node_stop_t stop;
};

class libmanager {
public:
	void *load_node_library(const char *name);
	const struct node_library_t &get_node_library(void *handle) const;

private:
	typedef std::map<void*, struct node_library_t> node_library_map_t;
	node_library_map_t m_node_library_map;
};

#endif /* end of include guard: HIVE_SIM_LIBMANAGER_HPP_ */
