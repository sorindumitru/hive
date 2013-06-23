#include <dlfcn.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <string.h>

#include <control.hpp>

#include <node.h>
#include <net/routing.h>

extern event_base *m_plat_base[16];
static command_handlers_t command_handlers;

bool operator< (const address &left, const address &right)
{
	for (int i = 0; i < MAC_ADDRESS_LEN; i++) {
		if (left.mac[i] == right.mac[i])
			continue;
		return left.mac[i] < right.mac[i];
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

	command_handlers["load"] = &control::cmd_load;
	command_handlers["unload"] = &control::cmd_unload;
	command_handlers["start"] = &control::cmd_start;
	command_handlers["stop"] = &control::cmd_stop;

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
	char buffer[2048] = {0};

	read(m_control_sk, buffer, 1024);

	Json::Value root;
	Json::Reader reader;

	if (!reader.parse(buffer, root)) {
		std::cerr << "could not parse command" << std::endl;
		return;
	}

	for (command_handlers_t::const_iterator i = command_handlers.begin(); i != command_handlers.end(); ++i) {
		if (!root[i->first].isNull()) {
			(this->*i->second)(root[i->first]);
		}
	}
}

// Node command handling

void control::cmd_load(Json::Value &root)
{
	if (root["library"].isNull())
		return;
	
	std::string library = root["library"].asString();
	std::string name = root["name"].asString();

	struct node *node = node_find_by_name(name.c_str());
	if (node)
		return;

	void *handle = m_libmanager.load_node_library(library.c_str());
	if (handle == NULL)
		return;

	const struct node_library_t &node_lib = m_libmanager.get_node_library(handle);

	void *data = node_lib.init();
	node = (struct node *) malloc(sizeof(*node));

	Json::Value nic = root["nic"];

	if (nic.isNull())
		return;

	std::string json_addr = nic["address"].asString();

	node->nic = nic_clone(nic["type"].asString().c_str(),
			address_from_string(nic["address"].asString().c_str()));

	Json::Value router = root["routing"];

	node->router = router_get_by_name(router["type"].asString().c_str());

	node->index = add_node_data(handle, node);
	node->name = strdup(name.c_str());
	node->priv = data;
	node_add(node);

	std::cout << "created node " << node->index << std::endl;
}

void control::cmd_unload(Json::Value &root)
{
	Json::Value &index = root["index"];

	if (index.isNull())
		return;

	if (index.isArray()) {
		for (unsigned i = 0; i < index.size(); i++)
			unload_node(index[i].asInt());
	} else {
		unload_node(index.asInt());
	}
}

void control::unload_node(unsigned index)
{
	const struct node_data *node_data = get_node_data(index);
	if (node_data == NULL) {
		std::cerr << "cannot find node " << index << std::endl;
		return;
	}

	const struct node_library_t &node_lib = m_libmanager.get_node_library(node_data->dlhandle);

	node_lib.exit(node_data->node);
	del_node(index);

	std::cout << "removed node " << index << std::endl;
}

void control::cmd_start(Json::Value &root)
{
	Json::Value &index = root["index"];

	if (index.isNull())
		return;

	if (index.isArray()) {
		for (unsigned i = 0; i < index.size(); i++)
			start_node(index[i].asInt());
	} else {
		start_node(index.asInt());
	}
}

void control::start_node(unsigned index)
{
	const struct node_data *node_data = get_node_data(index);
	if (node_data == NULL) {
		std::cerr << "cannot find node " << index << std::endl;
		return;
	}

	const struct node_library_t &node_lib = m_libmanager.get_node_library(node_data->dlhandle);

	node_lib.start(node_data->node);

	std::cout << "started node " << index << std::endl;
}

void control::cmd_stop(Json::Value &root)
{
	Json::Value &index = root["index"];

	if (root["index"].isNull())
		return;
	
	if (index.isArray()) {
		for (unsigned i = 0; i < index.size(); i++)
			stop_node(index[i].asInt());
	} else {
		stop_node(index.asInt());
	}
}

void control::stop_node(unsigned index)
{
	const struct node_data *node_data = get_node_data(index);
	if (node_data == NULL) {
		std::cerr << "cannot find node " << index << std::endl;
		return;
	}

	const struct node_library_t &node_lib = m_libmanager.get_node_library(node_data->dlhandle);

	node_lib.stop(node_data->node);

	std::cout << "stopped node " << index << std::endl;
}

unsigned control::add_node_data(void *dlhandle, struct node* node)
{
	struct node_data &node_data = m_nodes[m_node_index];
	node_data.dlhandle = dlhandle;
	node_data.node = node;

	return m_node_index++;
}

const struct control::node_data *control::get_node_data(unsigned index) const
{
	nodes_t::const_iterator i = m_nodes.find(index);
	return i != m_nodes.end() ? &i->second : NULL;
}

void control::del_node(unsigned index)
{
	struct node *node = m_nodes[index].node;

	node_del(node);
	m_nodes.erase(index);

	free(node->name);
	free(node);
}
