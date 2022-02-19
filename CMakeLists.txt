# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Julia")

set(IMGUI_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/imgui")

set(IMGUI_INCLUDE_DIRS ${IMGUI_SOURCE_DIR})
set(IMGUI_SOURCE_FILES 
	"${IMGUI_SOURCE_DIR}/imgui.cpp" 
	"${IMGUI_SOURCE_DIR}/imgui_demo.cpp" 
	"${IMGUI_SOURCE_DIR}/imgui_draw.cpp"
	"${IMGUI_SOURCE_DIR}/imgui_tables.cpp"
	"${IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
	"${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
	"${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
)

# Include sub-projects.
add_subdirectory ("vendor/glfw")
add_subdirectory ("vendor/glad")
add_subdirectory ("src")
