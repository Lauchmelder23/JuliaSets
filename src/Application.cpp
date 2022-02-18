#include <glad/glad.h>
#include "Application.hpp"

#include <stdexcept>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

Application::Application() :
	window(new Window(1280, 720, "Mandelbrot")), canvas(nullptr), backgroundColor{ 0.1f, 0.01f, 0.19f }
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
		glfwPollEvents();
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		canvas->Render();

		ImGui::Begin("Settings");
		ImGui::SliderFloat3("Background Color", backgroundColor, 0.0f, 1.0f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window->Display();
	}
}
