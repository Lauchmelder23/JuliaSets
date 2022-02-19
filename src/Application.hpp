#pragma once

#include "Window.hpp"
#include "Canvas.hpp"

struct WindowData
{
	struct
	{
		double x, y;
	} mouseDelta, lastMousePos, wheel;
};

class Application
{
public:
	Application();
	~Application();

	void Launch();

private:
	Window* window;
	Canvas* canvas;

	WindowData data;
};