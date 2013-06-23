#ifndef HIVE_SIM_CONTROL_HPP_
#define HIVE_SIM_CONTROL_HPP_

#include <event.h>
#include <jsoncpp/json/value.h>
#include <map>

#include <libmanager.hpp>

#include <net/utils.h>

struct node;

class control {
public:
	control();
	~control();

private:
	int m_control_sk;
	struct event *m_control_event;
	libmanager m_libmanager;

	static void do_command(int fs, short event, void *arg);
	void command(int sock);

	void cmd_load(Json::Value &);
	void cmd_unload(Json::Value &);
	void cmd_start(Json::Value &);
	void cmd_stop(Json::Value &);

	void start_node(unsigned index);
	void stop_node(unsigned index);
	void unload_node(unsigned index);

	typedef void *(*node_init_t)(void);
	typedef void (*node_exit_t)(void *);
	typedef void (*node_start_t)(void *);
	typedef void (*node_stop_t)(void *);

	struct node_data {
		void *dlhandle;
		struct node *node;
	};

	typedef std::map<unsigned, struct node_data> nodes_t;
	nodes_t m_nodes;
	unsigned m_node_index;

	unsigned add_node_data(void *dlhandle, struct node *node);
	const struct node_data *get_node_data(unsigned index) const;
	void del_node(unsigned index);

	typedef std::map<address, struct node*> nodes_by_address_t;
	nodes_by_address_t m_nodes_by_address;
};

typedef void(control::*command_handler)(Json::Value &);
typedef std::map<const char*, command_handler> command_handlers_t;

#endif /* end of include guard: HIVE_SIM_CONTROL_HPP_ */
