#include <iostream>

#include "test.hpp"


static TestModule test_obj;;
Module *test_module = &test_obj;

void TestModule::init(void)
{
	std::cout << "We're starting!" << std::endl;
}
