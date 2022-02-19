#include "Shader.hpp"

#include <stdexcept>
#include <glad/glad.h>

Shader::Shader() :
	program(0), vertexShader(0), fragmentShader(0), computeShader(0)
{
	program = glCreateProgram();
}

Shader::~Shader()
{
	if (vertexShader)
		glDeleteShader(vertexShader);

	if (fragmentShader)
		glDeleteShader(fragmentShader);

	if (computeShader)
		glDeleteShader(computeShader);

	glDeleteProgram(program);
}

void Shader::AttachVertexShader(const std::string& vertexSource)
{
	GLint result;
	char infoLog[512];

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* shaderSourceCString = vertexSource.c_str();
	glShaderSource(vertexShader, 1, &shaderSourceCString, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		glDeleteShader(vertexShader);
		throw std::runtime_error("Failed to compile vertex shader\n" + std::string(infoLog));
	}

	glAttachShader(program, vertexShader);
}

void Shader::AttachFragmentShader(const std::string& fragmentSource)
{
	GLint result;
	char infoLog[512];

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* shaderSourceCString = fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &shaderSourceCString, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		glDeleteShader(fragmentShader);
		throw std::runtime_error("Failed to compile fragment shader\n" + std::string(infoLog));
	}

	glAttachShader(program, fragmentShader);
}

void Shader::AttachComputeShader(const std::string& computeSource)
{
	GLint result;
	char infoLog[512];

	computeShader = glCreateShader(GL_COMPUTE_SHADER);
	const char* shaderSourceCString = computeSource.c_str();
	glShaderSource(computeShader, 1, &shaderSourceCString, NULL);
	glCompileShader(computeShader);

	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
		glDeleteShader(computeShader);
		throw std::runtime_error("Failed to compile compute shader\n" + std::string(infoLog));
	}

	glAttachShader(program, computeShader);
}

void Shader::Link()
{
	GLint result;
	char infoLog[512];

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		throw std::runtime_error("Failed to link shader program\n" + std::string(infoLog));
	}

	if (vertexShader)
		glDeleteShader(vertexShader);

	if (fragmentShader)
		glDeleteShader(fragmentShader);

	if (computeShader)
		glDeleteShader(computeShader);
}

void Shader::Use()
{
	glUseProgram(program);
}