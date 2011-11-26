#ifndef HIVE_SIM_MODULE_HPP_
#define HIVE_SIM_MODULE_HPP_

#include <map>
#include <string>

class Module {
public:
	Module();
	virtual ~Module();

	virtual void init(void) = 0;

protected:
	void *m_lib;
	friend class ModuleFactory;
};

class ModuleFactory {
public:
	static ModuleFactory *getInstance();
	~ModuleFactory();

	bool loadModule(const std::string &name);
	bool unloadModule(const std::string &name);
private:
	ModuleFactory() {};
	ModuleFactory(const ModuleFactory &factory);
	std::map<std::string, Module*> modules;

	static ModuleFactory *m_instance;
};

#endif /* HIVE_SIM_MODULE_HPP_ */
