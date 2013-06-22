#ifndef HIVE_SIM_CONTROL_HPP_
#define HIVE_SIM_CONTROL_HPP_

#include <event.h>
#include <map>

class control {
public:
	control();
	~control();

private:
	int m_control_sk;
	struct event *m_control_event;

	static void do_command(int fs, short event, void *arg);
	void command(int sock);

	struct module_data {
		void *handle;
		void *data;
	};

	typedef std::map<std::string, module_data> module_map_t;
	module_map_t module_map;

	void cmd_load(char *);
	void cmd_unload(char *);
};

typedef void(control::*command_handler)(char*);
typedef std::map<const char*, command_handler> command_handlers_t;

#endif /* end of include guard: HIVE_SIM_CONTROL_HPP_ */
