#include <iostream>
#include "Application.hpp"

int main(int argc, char** argv)
{
	glfwInit();

	Application* app;
	try
	{
		app = new Application();
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << err.what() << std::endl;
		return -1;
	}

	app->Launch();

	return 0;
}
