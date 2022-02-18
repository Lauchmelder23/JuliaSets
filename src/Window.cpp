#include <glad/glad.h>
#include "Window.hpp"

#include <stdexcept>

Window::Window(uint32_t width, uint32_t height, const std::string& title) :
	handle(nullptr)
{
	handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (handle == nullptr)
	{
		const char* buffer;
		int result = glfwGetError(&buffer);

		throw std::runtime_error("Failed to initialize GLFWwindow (" + std::to_string(result) + ") " + std::string(buffer));
	}

	glfwSetWindowSizeLimits(handle, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

	glfwSetFramebufferSizeCallback(handle, 
		[](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		}
	);
}

Window::~Window()
{
	glfwDestroyWindow(handle);
}

void Window::Display()
{
	glfwSwapBuffers(handle);
}
