#include <glad/glad.h>
#include "Application.hpp"

#include <stdexcept>

Application::Application() :
	window(new Window(1280, 720, "Mandelbrot")), canvas(nullptr)
{
	window->MakeContextCurrent();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("GLAD failed to initialize.");
	}

	glViewport(0, 0, 1280, 720);

	canvas = new Canvas();
}

Application::~Application()
{
}

void Application::Launch()
{
	while (!window->ShouldClose())
	{
		glfwPollEvents();

		glClearColor(0.1f, 0.01f, 0.19f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		canvas->Render();

		window->Display();
	}
}
