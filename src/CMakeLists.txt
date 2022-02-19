# CMakeList.txt : CMake project for Mandelbrot, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Mandelbrot "main.cpp" "Window.cpp" "Application.cpp" "Canvas.cpp" "Shader.cpp")

target_sources(Mandelbrot PRIVATE
	${IMGUI_SOURCE_FILES}
)

target_include_directories(Mandelbrot PRIVATE
	glfw
	glad
	${IMGUI_INCLUDE_DIRS}
)

target_link_libraries(Mandelbrot
	glfw
	glad
)