#include "Canvas.hpp"

#include <string>
#include <stdexcept>

#include <glad/glad.h>

Canvas::Canvas() :
	vao(0), vbo(0), shader(0)
{
	CreateVertexArrayObject();
	CreateShaderProgram();
}

Canvas::~Canvas()
{
	if (shader)
		glDeleteProgram(shader);

	if (vbo)
		glDeleteBuffers(1, &vbo);

	if (vao)
		glDeleteVertexArrays(1, &vao);
}

void Canvas::Render()
{
	glUseProgram(shader);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Canvas::CreateVertexArrayObject()
{
	float vertices[4 * 2] = {
		-0.9f, -0.9f,
		-0.9f, 0.9f,
		0.9f, 0.9f,
		0.9f, -0.9f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (const void*)&vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);
	glEnableVertexAttribArray(0);
}

void Canvas::CreateShaderProgram()
{
	GLint result;
	char infoLog[512];

	shader = glCreateProgram();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexShaderSource = R"(
		#version 460 core

		layout (location = 0) in vec2 pos;

		void main()
		{
			gl_Position = vec4(pos, 0.0f, 1.0f);
		}
	)";
	const char* shaderSourceCString = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &shaderSourceCString, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		glDeleteShader(vertexShader);
		throw std::runtime_error("Failed to compile vertex shader\n" + std::string(infoLog));
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragmentShaderSource = R"(
		#version 460 core

		out vec4 FragColor;

		void main()
		{
			FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
		}
	)";
	shaderSourceCString = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &shaderSourceCString, NULL);
	glCompileShader(fragmentShader);

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		throw std::runtime_error("Failed to compile fragment shader\n" + std::string(infoLog));
	}

	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, 512, NULL, infoLog);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		throw std::runtime_error("Failed to link shader program\n" + std::string(infoLog));
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}
