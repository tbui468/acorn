cmake_minimum_required(VERSION 3.20)
project(apple)

set(CMAKE_CXX_STANDARD 17)

include_directories(vendor/imgui/examples/libs/glfw/include)
include_directories(vendor/imgui)
include_directories(vendor/imgui/backends)
include_directories(vendor/glad/include)
file(GLOB GLAD_SOURCES vendor/glad/glad.c)
file(GLOB IMGUI_SOURCES vendor/imgui/*.cpp vendor/imgui/backends/imgui_impl_glfw.cpp vendor/imgui/backends/imgui_impl_opengl3.cpp)
link_directories(vendor/glfw/lib-vc2019)

add_subdirectory(src)
