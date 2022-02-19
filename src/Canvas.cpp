#include "Canvas.hpp"

#include <string>
#include <stdexcept>
#include <complex>
#include <regex>

#include <glad/glad.h>

double Map(double fromMin, double fromMax, double toMin, double toMax, double val)
{
	return (val - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
}

Canvas::Canvas() :
	vao(0), vbo(0), texture(0)
{
	// Default Julia properties
	properties.xBounds[0] = -2.5f;
	properties.xBounds[1] = 2.5f;
	properties.yCenter = 0.0f;
	properties.aspectRatio = 9.0f / 16.0f;
	properties.textureWidth = 1920;
	properties.maxIterations = 100;
	properties.iterationColorCutoff = 100.0f;
	properties.c[0] = -0.835;
	properties.c[1] = -0.2321;
	properties.doublePrecision = false;
	properties.isPolar = false;

	CreateVertexArrayObject();
	CreateShaderProgram();
	CreateTexture();

	CreateCompueShader();
}

Canvas::~Canvas()
{
	if (texture)
		glDeleteTextures(1, &texture);

	if (vbo)
		glDeleteBuffers(1, &vbo);

	if (vao)
		glDeleteVertexArrays(1, &vao);
}

void Canvas::Render()
{
	// Wait for compute shader to finish
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Render texture to screen
	shader.Use();
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Canvas::CalculateJuliaSet()
{
	// Wait for previous calculation to finish
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// width and height of the target texture
	int width = properties.textureWidth;
	int height = properties.textureWidth * properties.aspectRatio;

	// domain in y direction
	float yLength = (properties.xBounds[1] - properties.xBounds[0]) * properties.aspectRatio;
	float yMin = properties.yCenter - 0.5f * yLength;
	float yMax = properties.yCenter + 0.5f * yLength;

	// Bind our texture object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set texture properties (linear filtering)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Re-create empty texture with right dimensions
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Prepare texture for use in the compute shader
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Decide whether to use single- or double precision shader
	if (properties.doublePrecision)
		doubleComputeShader.Use();
	else
		computeShader.Use();

	// Set uniforms for shader
	float c[2];
	if (!properties.isPolar)
	{
		c[0] = properties.c[0];
		c[1] = properties.c[1];
	}
	else
	{
		c[0] = properties.c[0] * cosf(properties.c[1]);
		c[1] = properties.c[0] * sinf(properties.c[1]);
	}

	glUniform2f(1, properties.xBounds[0], properties.xBounds[1]);
	glUniform2f(2, yMin, yMax);
	glUniform2f(3, c[0], c[1]);
	glUniform1i(4, properties.maxIterations);
	glUniform1f(5, properties.iterationColorCutoff);

	// Calculate Julia set
	glDispatchCompute(width, height, 1);
}

void Canvas::CreateVertexArrayObject()
{
	// Create simple quad
	float vertices[4 * (2 + 2)] = {
		-1.0f, -1.0f,	0.0f, 0.0f,
		-1.0f,  1.0f,	0.0f, 1.0f,
		 1.0f,  1.0f,	1.0f, 1.0f,
		 1.0f, -1.0f,	1.0f, 0.0f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (const void*)&vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Canvas::CreateShaderProgram()
{
	// Create the render shader
	// It simply renders the texture to a quad
	std::string vertexShaderSource = R"(
		#version 460 core

		layout (location = 0) in vec2 pos;
		layout (location = 1) in vec2 uv;

		out vec2 uvCoord;

		void main()
		{
			uvCoord = uv;
			gl_Position = vec4(pos, 0.0f, 1.0f);
		}
	)";
	shader.AttachVertexShader(vertexShaderSource);

	std::string fragmentShaderSource = R"(
		#version 460 core

		in vec2 uvCoord;
		out vec4 FragColor;

		uniform sampler2D canvas;

		void main()
		{
			FragColor = texture(canvas, uvCoord);
		}
	)";
	shader.AttachFragmentShader(fragmentShaderSource);

	shader.Link();
}

void Canvas::CreateCompueShader()
{
	QueryWorkGroupInfo();

	// Create compute shader
	std::string shaderSource = R"(
		#version 460 core

		layout(local_size_x = 1, local_size_y = 1) in;
		layout(rgba32f, binding = 0) uniform image2D img_out;
		layout(location = 1) uniform vec2 xDomain;
		layout(location = 2) uniform vec2 yDomain;
		layout(location = 3) uniform vec2 c;
		layout(location = 4) uniform int maxIterations;
		layout(location = 5) uniform float iterationColorCutoff;

		double map(double fromMin, double fromMax, double toMin, double toMax, double val)
		{
			return (val - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
		}

		dvec2 complexMul(dvec2 a, dvec2 b)
		{
			return dvec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
		}

		void main()
		{
			vec4 pixel = vec4(0.0f, 0.05f, 0.2f, 1.0f);

			ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
			ivec2 image_size = ivec2(gl_NumWorkGroups.xy);

			double threshold = 0.5f * (sqrt(4 * length(c) + 1) + 1);
	
			dvec2 z = dvec2(
				map(0, image_size.x, xDomain.x, xDomain.y, pixel_coords.x),
				map(0, image_size.y, yDomain.x, yDomain.y, pixel_coords.y)
			);
	
			for(int i = 0; i < maxIterations; i++)
			{
				if(length(z) > threshold)
				{
					pixel.x = float(i) / iterationColorCutoff;
					break;
				}
		
				z = complexMul(z, z) + c;
			}
	
			imageStore(img_out, pixel_coords, pixel);
		}
	)";
	doubleComputeShader.AttachComputeShader(shaderSource);
	doubleComputeShader.Link();

	// Single precision shader is the exact same, except different datatypes
	shaderSource = std::regex_replace(shaderSource, std::regex("double"), "float");
	shaderSource = std::regex_replace(shaderSource, std::regex("dvec2"), "vec2");
	computeShader.AttachComputeShader(shaderSource);
	computeShader.Link();
}

void Canvas::CreateTexture()
{
	glGenTextures(1, &texture);
}

void Canvas::QueryWorkGroupInfo()
{
	// Get infor about the GPUs work group props
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workProperties.groupCount[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workProperties.groupCount[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workProperties.groupCount[2]);

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workProperties.groupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workProperties.groupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workProperties.groupSize[2]);

	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workProperties.maxInvocations);
}
