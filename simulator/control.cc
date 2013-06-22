#include <dlfcn.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

#include <control.hpp>

extern event_base *m_plat_base[16];
static command_handlers_t command_handlers;

control::control()
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

typedef void *(*node_init_t)(void);
typedef void (*node_exit_t)(void *);

void control::cmd_load(char *args)
{
	char *name = strtok(args, " \n\t");
	char library[64] = {0};

	sprintf(library, "lib%s.so", name);

	void *handle = dlopen(library, RTLD_NOW);
	if (!handle) {
		std::cerr << dlerror();
		return;
	}

	char init_func[256] = {0};
	sprintf(init_func, "%s_init", name);
	node_init_t init = (node_init_t) dlsym(handle, init_func);
	if (!init) {
		std::cerr << dlerror();
		return;
	}
	void *priv = init();
}

void control::cmd_unload(char *args)
{
	char *name = strtok(args, " \n\t");
	char library[64] = {0};

	sprintf(library, "lib%s.so", name);

	void *handle = dlopen(library, RTLD_NOW);
	if (!handle) {
		std::cerr << dlerror();
		return;
	}

	char exit_func[256] = {0};
	sprintf(exit_func, "%s_exit", name);
	node_exit_t node_exit = (node_exit_t) dlsym(handle, exit_func);
	if (!node_exit) {
		std::cerr << dlerror();
		return;
	}
	node_exit(NULL);
}
