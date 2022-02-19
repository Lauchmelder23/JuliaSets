#pragma once

#include <cstdint>
#include "Shader.hpp"

struct JuliaProperties
{
	float xBounds[2];
	float yCenter;
	float aspectRatio;
	uint32_t maxIterations;
	float iterationColorCutoff;
	uint32_t textureWidth;
	float c[2];
	bool doublePrecision;
	bool isPolar;
};

struct WorkProperties
{
	int groupCount[3];
	int groupSize[3];
	int maxInvocations;
};

class Canvas
{
public:
	Canvas();
	~Canvas();

	void Render();
	void CalculateJuliaSet();
	inline JuliaProperties& GetProperties() { return properties; }
	inline const WorkProperties& GetWorkProperties() { return workProperties; }

private:
	void CreateVertexArrayObject();
	void CreateShaderProgram();
	void CreateCompueShader();
	void CreateTexture();

	void QueryWorkGroupInfo();

private:
	uint32_t vao, vbo;
	Shader shader, computeShader, doubleComputeShader;
	uint32_t texture;

	JuliaProperties properties;
	WorkProperties workProperties;
};