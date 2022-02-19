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
	inline void SetShouldClose(int value) { return glfwSetWindowShouldClose(handle, value); }
	inline void MakeContextCurrent() { glfwMakeContextCurrent(handle); }
	inline void GetWindowSize(int& width, int& height) { glfwGetWindowSize(handle, &width, &height); }

	void Display();
	void InitImGui();

	inline GLFWwindow* GetHandle() { return handle; }

private:
	GLFWwindow* handle;
};