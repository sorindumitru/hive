#include <dlfcn.h>
#include <errno.h>
#include <iostream>

#include <module.hpp>

Module::Module()
{
}

Module::~Module()
{
}

ModuleFactory *ModuleFactory::m_instance = nullptr;

ModuleFactory *ModuleFactory::getInstance()
{
	if (!m_instance)
		m_instance = new ModuleFactory();
	return m_instance;
}

bool ModuleFactory::loadModule(const std::string &name)
{
	std::string libname = name + ".so";
	void *lib = dlopen(libname.c_str(), RTLD_LAZY);
	if (!lib) {
		std::cerr << "Error loading library " << libname
		          << dlerror() << std::endl;
		return false;
	}

	std::string symname = name + "_module";
	void *sym = dlsym(lib, symname.c_str());
	if (!sym) {
		std::cerr << "Error loading symbol " << symname
		          << std::endl;
		dlclose(lib);
		return false;
	}

	Module *module = *static_cast<Module **>(sym);
	module->m_lib = lib;

	module->init();

	modules[name] = module;
	
	return true;
}

bool ModuleFactory::unloadModule(const std::string &name)
{
	auto it = modules.find(name);
	if (it != modules.end()) {
		Module *module = it->second;
		dlclose(module->m_lib);
		return true;
	}

	return false;
}
