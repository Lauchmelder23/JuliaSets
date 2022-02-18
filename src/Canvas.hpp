#pragma once

#include <cstdint>

class Canvas
{
public:
	Canvas();
	~Canvas();

	void Render();

private:
	void CreateVertexArrayObject();
	void CreateShaderProgram();

private:
	uint32_t vao, vbo;
	uint32_t shader;
};