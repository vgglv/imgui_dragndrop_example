cmake_minimum_required(VERSION 4.2)
project(sample_project)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "src")

add_subdirectory(third_party)

find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw imgui::imgui OpenGL::GL)

set(IMGUI_GLFW_SOURCES
	"${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.6/backends/imgui_impl_glfw.cpp"
	"${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.6/backends/imgui_impl_glfw.h")

set(IMGUI_OPENGL3_SOURCES
	"${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.6/backends/imgui_impl_opengl3.cpp"
	"${CMAKE_SOURCE_DIR}/third_party/imgui-1.92.6/backends/imgui_impl_opengl3.h")

target_sources(${PROJECT_NAME} PUBLIC ${IMGUI_GLFW_SOURCES} ${IMGUI_OPENGL3_SOURCES})