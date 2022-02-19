#include <glad/glad.h>
#include "Application.hpp"

#include <stdexcept>
#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

Application::Application() :
	window(new Window(1280, 720, "Julia Sets")), canvas(nullptr)
{
	window->MakeContextCurrent();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("GLAD failed to initialize.");
	}

	glViewport(0, 0, 1280, 720);

	canvas = new Canvas();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	window->InitImGui();
	ImGui_ImplOpenGL3_Init("#version 460 core");

	ImGui::StyleColorsDark();

	data.lastMousePos = { 0.0, 0.0 };
	data.mouseDelta = { 0.0, 0.0 };
	data.wheel = { 0.0, 0.0 };

	GLFWwindow* nativeHandle = window->GetHandle();
	glfwSetWindowUserPointer(nativeHandle, (void*)&data);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(nativeHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);


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
		canvas->CalculateJuliaSet();

		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		canvas->Render();

		JuliaProperties& props = canvas->GetProperties();
		int width, height;
		window->GetWindowSize(width, height);
		props.aspectRatio = (float)height / (float)width;

		double mouseX, mouseY;
		glfwGetCursorPos(window->GetHandle(), &mouseX, &mouseY);

		data.mouseDelta.x = mouseX - data.lastMousePos.x;
		data.mouseDelta.y = mouseY - data.lastMousePos.y;

		data.lastMousePos.x = mouseX;
		data.lastMousePos.y = mouseY;

		float xSize = props.xBounds[1] - props.xBounds[0];

		if (glfwGetMouseButton(window->GetHandle(), GLFW_MOUSE_BUTTON_LEFT))
		{
			float stepSize = xSize / (float)width;

			props.xBounds[0] -= data.mouseDelta.x * stepSize;
			props.xBounds[1] -= data.mouseDelta.x * stepSize;

			props.yCenter += data.mouseDelta.y * stepSize;
		}

		if (data.wheel.y != 0.0)
		{
			props.xBounds[0] += data.wheel.y * (xSize / 10.0f);
			props.xBounds[1] -= data.wheel.y * (xSize / 10.0f);
		}

		data.wheel = { 0.0, 0.0 };

		const WorkProperties& workProps = canvas->GetWorkProperties();

		ImGui::Begin("Julia Set Properties");
		
		ImGui::SliderInt("max iterations", (int*)&props.maxIterations, 10, 1000);
		ImGui::SliderFloat("max color", &props.iterationColorCutoff, 10, 1000);
		ImGui::SliderInt("texture width", (int*)&props.textureWidth, 480, 2560);

		ImGui::SliderFloat2("c param", props.c, -1.5f, 1.5f);

		if (ImGui::Button(props.doublePrecision ? "Double Precision" : "Single Precision"))
			props.doublePrecision = !props.doublePrecision;

		ImGui::Separator();

		ImGui::Text("Max work groups - x: %i, y: %i, z: %i", workProps.groupCount[0], workProps.groupCount[1], workProps.groupCount[2]);
		ImGui::Text("Max group sizes - x: %i, y: %i, z: %i", workProps.groupSize[0], workProps.groupSize[1], workProps.groupSize[2]);
		ImGui::Text("Max invocations - %i", workProps.maxInvocations);

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window->Display();
	}
}
