#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <worker.hpp>

static command_handlers_t command_handlers;

static struct timeval keep_alive_tv = {
	.tv_sec = 60,
};

static void keep_alive_cb(int fd, short int event, void *arg)
{
}

worker::worker()
{
	m_ev_base = event_base_new();
	if (!m_ev_base)
		perror("Could not allocate new event base");
	m_keep_alive = event_new(m_ev_base, -1, EV_PERSIST,
				 keep_alive_cb, NULL);
}

worker::~worker()
{
}

void worker::addTimer(callback_t *callback, int timeout)
{
}

void worker::start()
{
	std::cout << "Starting worker " << sched_getcpu() << std::endl;
	evtimer_add(m_keep_alive, &keep_alive_tv);


	if (sched_getcpu() == 0)
		this->control();

	event_base_dispatch(m_ev_base);
}

void do_command(int fd, short event, void *arg)
{
	worker *w = static_cast<worker *>(arg);

	w->command(fd);
}

void worker::control()
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

	listen(m_control_sk, 64);
	m_control_event = event_new(m_ev_base, m_control_sk,
			EV_READ | EV_PERSIST, do_command, this);
	event_add(m_control_event, NULL);
	std::cout << "Started control thread" << std::endl;

	command_handlers["load "] = &worker::cmd_load;
}

void worker::cmd_load(const char * args)
{
	std::cout << args << std::endl;
}

void worker::command(int sock)
{
	char buffer[1024] = {0};

	read(m_control_sk, buffer, 1024);

	for (command_handlers_t::const_iterator i = command_handlers.begin(); i != command_handlers.end(); ++i) {
		size_t command_len = strlen(i->first);

		if (strlen(buffer) >= command_len && strncmp(buffer, i->first, command_len) == 0)
			(this->*i->second)(buffer + command_len);
	}
}

worker *workers[max_num_workers];

void *worker_init(void *_cpu)
{
	int cpu = (long)_cpu;
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);

	workers[cpu] = new worker();

	if (pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset))
		perror("Coult not set thread affinity");

	sleep(1);

	workers[cpu]->start();

	return NULL;
}
