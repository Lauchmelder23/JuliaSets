#pragma once

#include <string>

class Shader
{
public:
	Shader();
	~Shader();

	void AttachVertexShader(const std::string& vertexSource);
	void AttachFragmentShader(const std::string& fragmentSource);
	void AttachComputeShader(const std::string& computeSource);
	void Link();

	void Use();

private:
	int program;
	int vertexShader, fragmentShader, computeShader;
};