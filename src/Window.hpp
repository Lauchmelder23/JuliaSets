#pragma once 

#include <cstdint>
#include <string>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(uint32_t width, uint32_t height, const std::string& title);
	~Window();

	inline bool ShouldClose() { return glfwWindowShouldClose(handle); }
	inline void MakeContextCurrent() { glfwMakeContextCurrent(handle); }
	void Display();

private:
	GLFWwindow* handle;
};