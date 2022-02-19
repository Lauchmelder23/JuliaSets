#include <glad/glad.h>
#include "Application.hpp"

#include <stdexcept>
#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

Application::Application() :
	window(new Window(1280, 720, "Julia Sets")), canvas(nullptr)
{
	// Make the window's context the current one
	window->MakeContextCurrent();

	// Load OpenGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("GLAD failed to initialize.");
	}

	// Set viewport
	glViewport(0, 0, 1280, 720);

	canvas = new Canvas();

	// Set up ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	window->InitImGui();
	ImGui_ImplOpenGL3_Init("#version 460 core");

	ImGui::StyleColorsDark();

	// Set UserData
	data.lastMousePos = { 0.0, 0.0 };
	data.mouseDelta = { 0.0, 0.0 };
	data.wheel = { 0.0, 0.0 };

	// set user data pointer and install callbacks
	GLFWwindow* nativeHandle = window->GetHandle();
	glfwSetWindowUserPointer(nativeHandle, (void*)&data);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(nativeHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	// Just store scroll wheel info in the user pointer
	glfwSetScrollCallback(nativeHandle,
		[] (GLFWwindow* window, double x, double y)
		{
			WindowData* data = (WindowData*)glfwGetWindowUserPointer(window);

			data->wheel.x = x;
			data->wheel.y = y;
		}
	);
}

Application::~Application()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Application::Launch()
{
	while (!window->ShouldClose())
	{
		// Recalculate the julia set
		canvas->CalculateJuliaSet();

		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Render julia set
		canvas->Render();

		// Get julia set properties & recalculate aspect ratio
		JuliaProperties& props = canvas->GetProperties();
		int width, height;
		window->GetWindowSize(width, height);
		props.aspectRatio = (float)height / (float)width;

		// Properties of the GPUs work capabilities
		const WorkProperties& workProps = canvas->GetWorkProperties();

		// Render ImGui window
		ImGui::Begin("Julia Set Properties");

		ImGui::SliderInt("Max Iterations", (int*)&props.maxIterations, 10, 1000);
		ImGui::SliderFloat("Color Threshold", &props.iterationColorCutoff, 10, 1000);
		ImGui::SliderInt("Texture Width", (int*)&props.textureWidth, 480, 2560);

		ImGui::SliderFloat2("c", props.c, -1.5f, 1.5f);

		if (ImGui::Button(props.doublePrecision ? "Double Precision" : "Single Precision"))
			props.doublePrecision = !props.doublePrecision;

		ImGui::Separator();

		ImGui::Text("Max work groups - x: %i, y: %i, z: %i", workProps.groupCount[0], workProps.groupCount[1], workProps.groupCount[2]);
		ImGui::Text("Max group sizes - x: %i, y: %i, z: %i", workProps.groupSize[0], workProps.groupSize[1], workProps.groupSize[2]);
		ImGui::Text("Max invocations - %i", workProps.maxInvocations);

		// calculate mouse delta
		double mouseX, mouseY;
		glfwGetCursorPos(window->GetHandle(), &mouseX, &mouseY);

		data.mouseDelta.x = mouseX - data.lastMousePos.x;
		data.mouseDelta.y = mouseY - data.lastMousePos.y;

		data.lastMousePos.x = mouseX;
		data.lastMousePos.y = mouseY;

		// Size of the domain (x direction)
		float xSize = props.xBounds[1] - props.xBounds[0];

		// Camera panning handling
		ImVec2 min = ImGui::GetWindowPos();
		ImVec2 max = { min.x + ImGui::GetWindowWidth(), min.y + ImGui::GetWindowHeight() };
		if (!ImGui::IsMouseHoveringRect(min, max) && glfwGetMouseButton(window->GetHandle(), GLFW_MOUSE_BUTTON_LEFT))
		{
			float stepSize = xSize / (float)width;

			props.xBounds[0] -= data.mouseDelta.x * stepSize;
			props.xBounds[1] -= data.mouseDelta.x * stepSize;

			props.yCenter += data.mouseDelta.y * stepSize;
		}

		// Zooming
		if (data.wheel.y != 0.0)
		{
			props.xBounds[0] += data.wheel.y * (xSize / 10.0f);
			props.xBounds[1] -= data.wheel.y * (xSize / 10.0f);
		}

		data.wheel = { 0.0, 0.0 };


		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window->Display();
	}
}
