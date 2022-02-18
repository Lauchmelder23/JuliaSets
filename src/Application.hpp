#pragma once

#include "Window.hpp"
#include "Canvas.hpp"

class Application
{
public:
	Application();
	~Application();

	void Launch();

private:
	Window* window;
	Canvas* canvas;

	float backgroundColor[3];
};